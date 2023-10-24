/*!
    \file https_server.cpp
    \brief HTTPS server implementation
    \author Ivan Shynkarenka
    \date 30.04.2019
    \copyright MIT License
*/

#include "server/http/https_server.h"

#include "string/format.h"

namespace CppServer {
namespace HTTP {

void HTTPSServer::SetEnvironment(void *pConfig) {	
	m_pConfig = pConfig;
}

void HTTPSServer::AddStaticContent(const CppCommon::Path& path, void *pConfig, const std::string& prefix, const CppCommon::Timespan& timeout)
{
    auto hanlder = [](CppCommon::FileCache & cache, const std::string& key, void *pConfig, const CppCommon::Timespan& timespan)
    {
		std::unordered_map<std::string, std::string> smParameters;
		auto response = HTTPResponse();
		
		response.SetBegin(404);
		response.SetContentType("text/plain");
		response.SetBody("404 Page Not Found.");

        return cache.insert(key, response.cache(), timespan);
    };

    cache().insert_path(path, pConfig, prefix, timeout, hanlder);
}

} // namespace HTTP
} // namespace CppServer
