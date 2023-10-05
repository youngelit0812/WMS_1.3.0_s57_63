/*!
    \file https_server.h
    \brief HTTPS server definition
    \author Ivan Shynkarenka
    \date 30.04.2019
    \copyright MIT License
*/

#ifndef CPPSERVER_HTTP_HTTPS_SERVER_H
#define CPPSERVER_HTTP_HTTPS_SERVER_H

#include "https_session.h"

#include "cache/filecache.h"
#include "server/asio/ssl_server.h"

namespace CppServer {
namespace HTTP {

//! HTTPS server
/*!
    HTTPS server is used to create secured HTTPS Web server and
    communicate with clients using secure HTTPS protocol.
    It allows to receive GET, POST, PUT, DELETE requests and
    send HTTP responses.

    Thread-safe.
*/
class HTTPSServer : public Asio::SSLServer
{
public:
    using SSLServer::SSLServer;

    HTTPSServer(const HTTPSServer&) = delete;
    HTTPSServer(HTTPSServer&&) = delete;
    virtual ~HTTPSServer() = default;

    HTTPSServer& operator=(const HTTPSServer&) = delete;
    HTTPSServer& operator=(HTTPSServer&&) = delete;

    //! Get the static content cache
    CppCommon::FileCache& cache() noexcept { return _cache; }
    const CppCommon::FileCache& cache() const noexcept { return _cache; }

    //! Add static content cache
    /*!
        \param path - Static content path
        \param prefix - Cache prefix (default is "/")
        \param timeout - Refresh cache timeout (default is 1 hour)
    */
	void SetDBFlag(void *pAPP, void *pConfig);
	
	void* GetMainApp() { return m_pApp; }
	void* GetConfig() { return m_pConfig; }

    void AddStaticContent(const CppCommon::Path& path, void *pConfig, const std::string& prefix = "/", const CppCommon::Timespan& timeout = CppCommon::Timespan::hours(1));
    //! Remove static content cache
    /*!
        \param path - Static content path
    */
    void RemoveStaticContent(const CppCommon::Path& path) { _cache.remove_path(path); }
    //! Clear static content cache
    void ClearStaticContent() { _cache.clear(); }

    //! Watchdog the static content cache
    void Watchdog(const CppCommon::UtcTimestamp& utc = CppCommon::UtcTimestamp()) { _cache.watchdog(utc); }

protected:
    std::shared_ptr<Asio::SSLSession> CreateSession(const std::shared_ptr<Asio::SSLServer>& server) override { return std::make_shared<HTTPSSession>(std::dynamic_pointer_cast<HTTPSServer>(server)); }

private:
    // Static content cache
    CppCommon::FileCache _cache;	
	void*				 m_pApp;
	void*				 m_pConfig;
};

/*! \example https_server.cpp HTTPS server example */

} // namespace HTTP
} // namespace CppServer

#endif // CPPSERVER_HTTP_HTTPS_SERVER_H
