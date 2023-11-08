#include "server/http/http_server.h"

#include "string/format.h"

namespace CppServer {
    namespace HTTP {

        void HTTPServer::SetEnvironment(void* pConfig) {
            m_pConfig = pConfig;
        }

        void HTTPServer::AddStaticContent(const CppCommon::Path& path, void* pConfig, const std::string& prefix, const CppCommon::Timespan& timeout)
        {
            auto hanlder = [](CppCommon::FileCache& cache, const std::string& key, void* pConfig, const CppCommon::Timespan& timespan)
                {
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
