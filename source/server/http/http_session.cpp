#include "server/http/http_session.h"
#include "server/http/http_server.h"
#include "MainApp.h"
#include <queue>

extern std::queue<std::string> g_messageQueue;
extern std::mutex g_mtx;
extern std::condition_variable g_cv;

namespace CppServer {
namespace HTTP {

HTTPSession::HTTPSession(const std::shared_ptr<HTTPServer>& server)
    : Asio::TCPSession(server),
      _cache(server->cache())
{
	m_pConfig = server->GetConfig();
}

void HTTPSession::onReceived(const void* buffer, size_t size)
{
    // Receive HTTP request header
    if (_request.IsPendingHeader())
    {
        if (_request.ReceiveHeader(buffer, size))
            onReceivedRequestHeader(_request);

        size = 0;
    }

    // Check for HTTP request error
    if (_request.error())
    {
        onReceivedRequestError(_request, "Invalid HTTP request!");
        _request.Clear();
        Disconnect();
        return;
    }

    // Receive HTTP request body
    if (_request.ReceiveBody(buffer, size))
    {
        onReceivedRequestInternal(_request);
        _request.Clear();
        return;
    }

    // Check for HTTP request error
    if (_request.error())
    {
        onReceivedRequestError(_request, "Invalid HTTP request!");
        _request.Clear();
        Disconnect();
        return;
    }
}

void HTTPSession::onDisconnected()
{
    // Receive HTTP request body
    if (_request.IsPendingBody())
    {
        onReceivedRequestInternal(_request);
        _request.Clear();
        return;
    }
}

void HTTPSession::onReceivedRequestInternal(const HTTPRequest& request)
{
    // Try to get the cached response
    if (request.method() == "GET")
    {
        std::string_view url = request.url();
		std::unordered_map<std::string, std::string> umParameters;
		std::string sTrippedURL = CppCommon::Encoding::URLParser(url, umParameters);

		for (auto itemParam : umParameters) {
			printf("parameter key : %s, value : %s\n", itemParam.first.c_str(), itemParam.second.c_str());
		}
				
		if (!sTrippedURL.empty()) {
			try {
				Environments* pConfig = (Environments*)m_pConfig;		

				std::string sImageFormat = umParameters.at("format");

				if (sImageFormat.empty()) {
					SendResponseAsync(response().MakeGetMapResponse("", false));
					return;
				}

				if ((sImageFormat.find(PNG_FILE_EXTENSION) == std::string::npos && sImageFormat.find(JPEG_FILE_EXTENSION) == std::string::npos) || pConfig->sIMGDirPath.empty()) {
					SendResponseAsync(response().MakeGetMapResponse("", false));
					return;
				}

				bool bPNGImageFlag = true;
				if (sImageFormat.find(JPEG_FILE_EXTENSION) != std::string::npos) bPNGImageFlag = false;

				std::string sIMGFilePathPrefix = pConfig->sIMGDirPath + getCurrentDateTimeMicrosecond();
				std::string sIMGFilePath = bPNGImageFlag ? sIMGFilePathPrefix + PNG_FILE_EXTENSION : sIMGFilePathPrefix + JPEG_FILE_EXTENSION;

				std::string sMessage("");				
				sMessage += "|" + umParameters.at("bbox") + "|" + umParameters.at("width") + "|" + umParameters.at("height") 
						 + "|" + umParameters.at("layers") + "|" + sIMGFilePath + "|" + (bPNGImageFlag ? "1" : "0");

				g_messageQueue.push(sMessage);				
				g_cv.notify_one();

#if defined(_WIN32) || defined(_WIN64)				
				::_sleep(1);
#else
				usleep(1 * 1000);
#endif
				std::unique_lock<std::mutex> lockNotify(g_mtx);        
				SendResponseAsync(response().MakeGetMapResponse(sIMGFilePath, bPNGImageFlag));
			}
			catch (std::exception const& ex) {
				SendResponseAsync(response().MakeGetMapResponse("", false));
			}

			return;
		}
    }

    // Process the request
    onReceivedRequest(request);
}

} // namespace HTTP
} // namespace CppServer
