#include "server/http/https_session.h"
#include "server/http/https_server.h"
#include "MainApp.h"

namespace CppServer {
namespace HTTP {

HTTPSSession::HTTPSSession(const std::shared_ptr<HTTPSServer>& server)
	: Asio::SSLSession(server),
	_cache(server->cache())
{
	m_pApp = server->GetMainApp();
	m_pConfig = server->GetConfig();
}

void HTTPSSession::onReceived(const void* buffer, size_t size)
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

void HTTPSSession::onDisconnected()
{
    // Receive HTTP request body
    if (_request.IsPendingBody())
    {
        onReceivedRequestInternal(_request);
        _request.Clear();
        return;
    }
}

void HTTPSSession::onReceivedRequestInternal(const HTTPRequest& request)
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

		printf("\nonReceivedRequestInternal: extracted URL:%s\n", sTrippedURL.c_str());
		if (!sTrippedURL.empty()) {
			Environments* pConfig = (Environments*)m_pConfig;
			MainApp* pMainApp = (MainApp*)m_pApp;
			try {
				std::string sImageFormat = umParameters.at("format");

				if (sImageFormat.empty()) {
					SendResponseAsync(response().MakeGetMapResponse("", false));
					return;
				}

				if ((sImageFormat.find(PNG_FILE_EXTENSION) == std::string::npos && sImageFormat.find(JPEG_FILE_EXTENSION) == std::string::npos) || pConfig->sIMGFilePath.empty()) {
					SendResponseAsync(response().MakeGetMapResponse("", false));
					return;
				}

				bool bPNGImageFlag = true;
				if (sImageFormat.find(JPEG_FILE_EXTENSION) != std::string::npos) bPNGImageFlag = false;

				std::string sIMGFilePath = bPNGImageFlag ? pConfig->sIMGFilePath + PNG_FILE_EXTENSION : pConfig->sIMGFilePath + JPEG_FILE_EXTENSION;
				if (pMainApp->UpdateFrameCanvas(umParameters.at("bbox"), std::stoi(umParameters.at("width")), std::stoi(umParameters.at("height")), umParameters.at("layers"), sIMGFilePath, bPNGImageFlag)) {
					SendResponseAsync(response().MakeGetMapResponse(sIMGFilePath, bPNGImageFlag));
				}
				else {
					SendResponseAsync(response().MakeGetMapResponse("", false));
				}
			} catch (std::exception const& ex) {
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
