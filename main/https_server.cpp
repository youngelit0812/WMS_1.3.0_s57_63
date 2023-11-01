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
#include <csignal>
#include <queue>
#include <signal.h>

#include "Json.hpp"
#include "MainApp.h"

#if defined(__WXGTK__) || defined(__WXQT__)
#include <gtk/gtk.h>
#endif

#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif

#define MAIN_PARAM_COUNT		7
#define RESTART_MARK_STR		"!"
#define TERMINATE_MARK_STR		"exit"
#define RENDER_SPLIT_MARK		'|'

bool g_bUserInput;
bool g_isRunning;
int g_nCSignal;

bool g_bMessageLoop;
std::queue<std::string> g_messageQueue;
std::mutex g_mtx;
std::condition_variable g_cv;

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
		envConfig.sIMGDirPath = jsonData.value("IMGDirPath", "");		
	}
	catch (nlohmann::json::parse_error & e)
	{
		std::cerr << "Error parsing environment file " << szEnvFilePath << " with error : " << e.what() << std::endl;
	}

	json_file.close();
}

void signal_handler(int signal) 
{
	g_isRunning = false; // Set the global flag to stop the server	
	g_bMessageLoop = false;
}

void c_signal_handler(int signal)
{
	g_nCSignal = signal;
	g_bMessageLoop = false;
}

#ifdef _WIN32
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType) {
	case CTRL_C_EVENT:	
		return TRUE;
	default:
		return FALSE;
	}
}
#else
void IgnoreInterruptSignal(int signal) {
}
#endif

void UserInputThreadProc() {
	std::string sInput;

#ifdef _WIN32	
	SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
#else	
	std::signal(SIGINT, IgnoreInterruptSignal);
#endif

	while (g_bUserInput) {
		std::getline(std::cin, sInput);

		if (sInput == RESTART_MARK_STR || sInput == TERMINATE_MARK_STR) {
			std::unique_lock<std::mutex> lock(g_mtx);
			lock.unlock();
			g_messageQueue.push(sInput);
			g_cv.notify_one();
#if defined(_WIN32) || defined(_WIN64)				
			::_sleep(10);
#else
			usleep(10 * 1000);
#endif
			lock.lock();
		}

#if defined(_WIN32) || defined(_WIN64)				
		::_sleep(0);
#else
		sleep(0);
#endif		
	}
}

int main(int argc, char** argv)
{
	std::string sConfigPath = "./env/config.json";
	g_bMessageLoop = true;
	g_nCSignal = 0;
	g_isRunning = true;
	g_bUserInput = true;

	std::thread userInputThread(UserInputThreadProc);

	signal(SIGINT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGTERM, signal_handler);	
	signal(SIGABRT, signal_handler);
#if defined(_WIN32) || defined(_WIN64)
	signal(SIGBREAK, signal_handler);
	signal(SIGABRT_COMPAT, signal_handler);
#endif

	std::signal(SIGINT, c_signal_handler);

	Environments envConfig;
	loadEnvironment((char*)sConfigPath.c_str(), envConfig);

	std::string www = "/wms";
	wxInitializer initializer(argc, argv);
#ifdef __linux__
	gtk_init(&argc, &argv);
#endif

	MainApp* pAppForService = new MainApp();
	if (!pAppForService->OnInit(envConfig.sENCDirPath, envConfig.rebuildCharts, envConfig.sIMGDirPath)) {
		pAppForService->OnExit(envConfig.sIMGDirPath);
		delete pAppForService;

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
	serverHTTPS->SetEnvironment((void*)& envConfig);
	serverHTTPS->AddStaticContent(www, &envConfig, "/api");

	// Start the server
	std::cout << "Server starting..." << std::endl;
	serverHTTPS->Start();	

	//HTTP server
	int nPortForHTTP = envConfig.nPortForHTTP;
	std::cout << "HTTP server port: " << nPortForHTTP << std::endl;
	
	// Create a new HTTP server
	auto serverHTTP = std::make_shared<HTTPCacheServer>(service, nPortForHTTP);
	serverHTTP->SetEnvironment((void*)&envConfig);
	serverHTTP->AddStaticContent(www, &envConfig, "/api");

	// Start the server
	std::cout << "Server starting..." << std::endl;
	serverHTTP->Start();

	printf("Press 'exit' to stop the server or '!' to restart the server...\n");
    // Perform text input
    std::string line;
	while (1) {
		if (!g_isRunning || g_nCSignal > 0) break;
		
		if (g_bMessageLoop) {
			try {
				std::unique_lock<std::mutex> lock(g_mtx);
				g_cv.wait(lock, [] { return !g_messageQueue.empty(); });				

				std::string message = g_messageQueue.front();
				g_messageQueue.pop();

				if (message == RESTART_MARK_STR) {
					printf("Server restarting...\n");
					serverHTTP->Restart();
					serverHTTPS->Restart();
					printf("Done!\n");					
				}
				else if (message == TERMINATE_MARK_STR) {
					break;
				} else if (message.at(0) == RENDER_SPLIT_MARK){
					std::vector<std::string> substrings;

					std::string delimiter("|");
					size_t pos = 0;
					std::string token;
					while ((pos = message.find(delimiter)) != std::string::npos) {
						token = message.substr(0, pos);
						substrings.push_back(token);
						message.erase(0, pos + delimiter.length());
					}
					substrings.push_back(message);

					if (substrings.size() == MAIN_PARAM_COUNT) {
						pAppForService->UpdateFrameCanvas(substrings.at(1), std::stoi(substrings.at(2)), std::stoi(substrings.at(3)),
							substrings.at(4), substrings.at(5), substrings.at(6) == "1" ? true : false);
					}
				}
			}
			catch (std::exception& e) {
				continue;
			}
		}
#if defined(_WIN32) || defined(_WIN64)
		::Sleep(0);
#else
		sleep(0);
#endif
    }

	g_bUserInput = false;
#if defined(_WIN32) || defined(_WIN64)
	::Sleep(0);
#else
	sleep(0);
#endif
    // Stop the server
	std::cout << "Server stopping..." << std::endl;
	if (pAppForService->OnExit(envConfig.sIMGDirPath)) delete pAppForService;

	serverHTTPS->Stop();
	serverHTTP->Stop();			
    std::cout << "Done!" << std::endl;

    // Stop the Asio service
    std::cout << "Asio service stopping..." << std::endl;
    service->Stop();
    std::cout << "Done!" << std::endl;
	
	userInputThread.join();

	return 0;
}
