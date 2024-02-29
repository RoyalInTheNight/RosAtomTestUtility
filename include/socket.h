#ifndef PLATFORM_SOCKET
#define PLATFORM_SOCKET

#include <cstdlib>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "core.h"
#include "utils.h"

#include <unistd.h>
#include <string>
#include <fstream>

#include <functional>

namespace tp      = core::c_types;
namespace sk      = core::_socket;
namespace sk_inet = core::_socket::inet_domain;
namespace sk_unix = core::_socket::unix_domain;

namespace _socket {
    typedef tp::s32      sock_bus_t;
    typedef socklen_t    sock_len_t;
    typedef sockaddr_in sock_inet_t;
    typedef sockaddr_un sock_unix_t;
    typedef std::function<tp::addr()> 
                      net_handler_t;
    
    typedef std::function<tp::addr(tp::s32)>
                   thread_handler_t;

    enum class thread_status : tp::u8 {
        multi_thread  = 0xfe,
        single_thread = 0xff
    };
}

using namespace _socket;

class inet_domain_socket {
protected:
    sk_inet::type    socket_type;
    sk::status     socket_status;
    thread_status _thread_status;

    sock_bus_t   bus;
    sock_bus_t  _bus;
    sock_len_t   len;
    sock_inet_t inet;

    union debug_info {
        tp::u64 address;
        tp::u32  offset;
    };

    debug_info    debug;

    tp::bit logging_status;

    std::string log_file;

    tp::addr logging(const char *);

    net_handler_t           handler;
    thread_handler_t thread_handler;

public:
    [[nodiscard]] sock_bus_t  get_socket_bus()   const;
    [[nodiscard]] sock_len_t  get_socket_len()   const;
    [[nodiscard]] sock_inet_t get_socket()       const;
    [[nodiscard]] inet_domain_socket& get_inet() const;
    [[nodiscard]] net_handler_t get_handler()    const;
    [[nodiscard]] sk::status    get_status()     const;
    [[nodiscard]] tp::bit       get_logging()    const;

    inet_domain_socket();
    inet_domain_socket(tp::bit);
    inet_domain_socket(const inet_domain_socket&);
    ~inet_domain_socket();

    tp::addr set_address(const char *);
    tp::addr set_port(const char *);
    tp::addr set_type(const sk_inet::type&);
    tp::addr set_handler(net_handler_t);
    tp::addr set_thread_status(const thread_status&);
    tp::addr set_logging(tp::bit);

    tp::addr socket_create();
    tp::addr bind_socket();

    tp::addr socket_write(const char *);
    tp::s32 socket_read(char *, tp::u32);

    tp::addr listen_connection();

    tp::addr close_handle();
    tp::addr close_socket();
};

class unix_domain_socket {
protected:
    sk_unix::type socket_type;
    sk::status  socket_status;

    sock_bus_t    bus;
    sock_bus_t   _bus;
    sock_len_t    len;
    sock_unix_t _unix;

    tp::s32 recv_unix_result;

    union debug_info {
        tp::u64 address;
        tp::u32  offset;
    };

    debug_info    debug;

    tp::bit logging_status;

    std::string log_file;

    tp::addr logging(const char *);

    net_handler_t handler;

public:
    [[nodiscard]] sock_bus_t  get_socket_bus()   const;
    [[nodiscard]] sock_len_t  get_socket_len()   const;
    [[nodiscard]] sock_unix_t get_socket()       const;
    [[nodiscard]] unix_domain_socket& get_unix() const;
    [[nodiscard]] net_handler_t get_handler()    const;
    [[nodiscard]] sk::status    get_status()     const;
    [[nodiscard]] tp::s32      get_recv_result() const;
    [[nodiscard]] tp::bit       get_logging()    const;

    unix_domain_socket();
    unix_domain_socket(tp::bit);
    unix_domain_socket(const unix_domain_socket&);
    ~unix_domain_socket();

    tp::addr set_path(const char *);
    tp::addr set_type(const sk_unix::type&);
    tp::addr set_handler(net_handler_t);
    tp::addr set_logging(tp::bit);

    tp::addr socket_create();
    tp::addr bind_socket();

    tp::addr socket_write(const char *);
    tp::addr socket_write(const char *, tp::u32);
    tp::s32  socket_read(char *, tp::s32);

    tp::addr listen_connection();

    tp::addr close_handle();
    tp::addr close_socket();
};

#endif // PLATFORM_SOCKET