/*!
    \file https_server.cpp
    \brief HTTPS server example
    \author Ivan Shynkarenka
    \date 30.04.2019
    \copyright MIT License
*/
#include "config.h"

#ifdef __MINGW32__
#undef IPV6STRICT  // mingw FTBS fix:  missing struct ip_mreq
#include <windows.h>
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/app.h>
#endif  // precompiled headers
#ifdef __WXMSW__
//#include "c:\\Program Files\\visual leak detector\\include\\vld.h"
#endif

#include "asio_service.h"

#include "server/http/http_server.h"
#include "server/http/https_server.h"
#include "string/string_utils.h"
#include "utility/singleton.h"

#include <iostream>
#include <map>
#include <mutex>

#include "Json.hpp"
#include "MainApp.h"

#if defined(__WXGTK__) || defined(__WXQT__)
#include <gtk/gtk.h>
#endif

class Cache : public CppCommon::Singleton<Cache>
{
   friend CppCommon::Singleton<Cache>;

public:
    std::string GetAllCache()
    {
        std::scoped_lock locker(_cache_lock);
        std::string result;
        result += "[\n";
        for (const auto& item : _cache)
        {
            result += "  {\n";
            result += "    \"key\": \"" + item.first + "\",\n";
            result += "    \"value\": \"" + item.second + "\",\n";
            result += "  },\n";
        }
        result += "]\n";
        return result;
    }

    bool GetCacheValue(std::string_view key, std::string& value)
    {
        std::scoped_lock locker(_cache_lock);
        auto it = _cache.find(key);
        if (it != _cache.end())
        {
            value = it->second;
            return true;
        }
        else
            return false;
    }

    void PutCacheValue(std::string_view key, std::string_view value)
    {
        std::scoped_lock locker(_cache_lock);
        auto it = _cache.emplace(key, value);
        if (!it.second)
            it.first->second = value;
    }

    bool DeleteCacheValue(std::string_view key, std::string& value)
    {
        std::scoped_lock locker(_cache_lock);
        auto it = _cache.find(key);
        if (it != _cache.end())
        {
            value = it->second;
            _cache.erase(it);
            return true;
        }
        else
            return false;
    }

private:
    std::mutex _cache_lock;
    std::map<std::string, std::string, std::less<>> _cache;
};

class HTTPCacheSession : public CppServer::HTTP::HTTPSession
{
public:
	using CppServer::HTTP::HTTPSession::HTTPSession;

protected:
	void onReceivedRequest(const CppServer::HTTP::HTTPRequest& request) override
	{
		// Show HTTP request content
		std::cout << std::endl << request;

		// Process HTTP request methods
		if (request.method() == "HEAD") SendResponseAsync(response().MakeHeadResponse());
		else if (request.method() == "GET")
		{
			std::string key(request.url());
			std::string value;

			// Decode the key value
			key = CppCommon::Encoding::URLDecode(key);
			CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
			CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");

			if (key.empty())
			{
				// Response with all cache values
				SendResponseAsync(response().MakeGetResponse(Cache::GetInstance().GetAllCache(), "application/json; charset=UTF-8"));
			}
			// Get the cache value by the given key
			else if (Cache::GetInstance().GetCacheValue(key, value))
			{
				// Response with the cache value
				SendResponseAsync(response().MakeGetResponse(value));
			}
			else
				SendResponseAsync(response().MakeErrorResponse(404, "Required URL doesn't exist (404 Page not found) : " + key));
		}		
		else
			SendResponseAsync(response().MakeErrorResponse("Unsupported HTTP method: " + std::string(request.method())));
	}

	void onReceivedRequestError(const CppServer::HTTP::HTTPRequest & request, const std::string & error) override
	{
		std::cout << "Request error: " << error << std::endl;
	}

	void onError(int error, const std::string & category, const std::string & message) override
	{
		std::cout << "HTTP session caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
	}
};

class HTTPCacheServer : public CppServer::HTTP::HTTPServer
{
public:
	using CppServer::HTTP::HTTPServer::HTTPServer;

protected:
	std::shared_ptr<CppServer::Asio::TCPSession> CreateSession(const std::shared_ptr<CppServer::Asio::TCPServer>& server) override
	{
		return std::make_shared<HTTPCacheSession>(std::dynamic_pointer_cast<CppServer::HTTP::HTTPServer>(server));
	}

protected:
	void onError(int error, const std::string& category, const std::string& message) override
	{
		std::cout << "HTTP server caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
	}
};

class HTTPSCacheSession : public CppServer::HTTP::HTTPSSession
{
public:
    using CppServer::HTTP::HTTPSSession::HTTPSSession;

protected:
    void onReceivedRequest(const CppServer::HTTP::HTTPRequest& request) override
    {
		printf("onReceivedRequest 1 \n");
        // Show HTTP request content
        std::cout << std::endl << request;

        // Process HTTP GET request methods
        if (request.method() == "GET")
        {
            std::string key(request.url());
            std::string value;

			// Decode the key value
            key = CppCommon::Encoding::URLDecode(key);			
            CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
            CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");

            if (key.empty())
            {
                // Response with all cache values
                SendResponseAsync(response().MakeGetResponse(Cache::GetInstance().GetAllCache(), "application/json; charset=UTF-8"));
            }
            // Get the cache value by the given key
            else if (Cache::GetInstance().GetCacheValue(key, value))
            {
                // Response with the cache value
                SendResponseAsync(response().MakeGetResponse(value));
            }
            else
                SendResponseAsync(response().MakeErrorResponse(404, "Required URL doesn't exist (404 Page not found) : " + key));
        }        
        else
            SendResponseAsync(response().MakeErrorResponse("Unsupported HTTP method: " + std::string(request.method())));
    }

    void onReceivedRequestError(const CppServer::HTTP::HTTPRequest& request, const std::string& error) override
    {
        std::cout << "Request error: " << error << std::endl;
    }

    void onError(int error, const std::string& category, const std::string& message) override
    {
        std::cout << "HTTPS session caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
    }
};

class HTTPSCacheServer : public CppServer::HTTP::HTTPSServer
{
public:
    using CppServer::HTTP::HTTPSServer::HTTPSServer;

protected:
    std::shared_ptr<CppServer::Asio::SSLSession> CreateSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server) override
    {
        return std::make_shared<HTTPSCacheSession>(std::dynamic_pointer_cast<CppServer::HTTP::HTTPSServer>(server));
    }

protected:
    void onError(int error, const std::string& category, const std::string& message) override
    {
        std::cout << "HTTPS server caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
    }
};

void loadEnvironment(char* szEnvFilePath, Environments& envConfig)
{
	std::ifstream json_file(szEnvFilePath, std::ios::binary);

	try
	{
		nlohmann::json jsonData = nlohmann::json::parse(json_file);

		envConfig.nPortForHTTP = jsonData.value("PortForHTTP", 0);
		envConfig.nPortForHTTPS = jsonData.value("PortForHTTPS", 0);
		envConfig.rebuildCharts = jsonData.value("rebuildCharts", 0) == 0 ? false : true;
		envConfig.sENCDirPath = jsonData.value("ENCDirPath", "");
		envConfig.sIMGFilePath = jsonData.value("IMGFilePath", "");
	}
	catch (nlohmann::json::parse_error & e)
	{
		std::cerr << "Error parsing environment file " << szEnvFilePath << " with error : " << e.what() << std::endl;
	}

	json_file.close();
}

int main(int argc, char** argv)
{
	std::string sConfigPath = "./env/config.json";

	Environments envConfig;
	loadEnvironment((char*)sConfigPath.c_str(), envConfig);

	std::string www = "/wms";
	wxInitializer initializer(argc, argv);
#ifdef __linux__
	gtk_init(&argc, &argv);
#endif
	MainApp* app = new MainApp();
	if (!app->OnInit(envConfig.sENCDirPath, envConfig.rebuildCharts)) {
		delete app;
		return 1;
	}

	// Create a new Asio service
	auto service = std::make_shared<AsioService>();

	// Start the Asio service
	std::cout << "Asio service starting...";
	service->Start();
	std::cout << "Done!" << std::endl;

	// HTTPS server port
	int nPortForHTTPS = envConfig.nPortForHTTPS;
	// HTTPS server content path    
	std::cout << "HTTPS server port: " << nPortForHTTPS << std::endl;

	// Create and prepare a new SSL server context
	auto context = std::make_shared<CppServer::Asio::SSLContext>(asio::ssl::context::tlsv13);
	context->set_password_callback([](size_t max_length, asio::ssl::context::password_purpose purpose) -> std::string { return "qwerty"; });
	context->use_certificate_chain_file("./tools/certificates/server.pem");
	context->use_private_key_file("./tools/certificates/server.pem", asio::ssl::context::pem);
	context->use_tmp_dh_file("./tools/certificates/dh4096.pem");

	// Create a new HTTPS server
	auto serverHTTPS = std::make_shared<HTTPSCacheServer>(service, context, nPortForHTTPS);
	serverHTTPS->SetDBFlag((void*)app, (void*)& envConfig);
	serverHTTPS->AddStaticContent(www, &envConfig, "/api");

	// Start the server
	std::cout << "Server starting..." << std::endl;
	serverHTTPS->Start();

	//HTTP server
	int nPortForHTTP = envConfig.nPortForHTTP;
	std::cout << "HTTP server port: " << nPortForHTTP << std::endl;

	// Create a new HTTP server
	auto serverHTTP = std::make_shared<HTTPCacheServer>(service, nPortForHTTP);
	serverHTTP->SetDBFlag((void*)app, (void*)&envConfig);
	serverHTTP->AddStaticContent(www, &envConfig, "/api");

	// Start the server
	std::cout << "Server starting..." << std::endl;
	serverHTTP->Start();

    std::cout << "Press Enter to stop the server or '!' to restart the server..." << std::endl;

    // Perform text input
    std::string line;
    while (getline(std::cin, line))
    {
        if (line.empty())
            break;

        // Restart the server
        if (line == "!")
        {
            std::cout << "Server restarting..." << std::endl;			
			serverHTTP->Restart();
			serverHTTPS->Restart();
            std::cout << "Done!" << std::endl;
            continue;
        }
    }

    // Stop the server
	std::cout << "Server stopping..." << std::endl;
	if (app->OnExit()) delete app;

	serverHTTPS->Stop();
	serverHTTP->Stop();			
    std::cout << "Done!" << std::endl;

    // Stop the Asio service
    std::cout << "Asio service stopping..." << std::endl;
    service->Stop();
    std::cout << "Done!" << std::endl;

	return 0;
}
