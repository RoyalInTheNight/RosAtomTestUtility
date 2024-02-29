#include "../include/socket.h"
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <ios>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

tp::bit inet_domain_socket::get_logging() const {
    return this->logging_status;
}

_socket::sock_bus_t inet_domain_socket::get_socket_bus() const {
    if (this->socket_type == sk_inet::type::client)
        return this->bus;

    return this->_bus;
}

_socket::sock_len_t inet_domain_socket::get_socket_len() const {
    return this->len;
}

_socket::sock_inet_t inet_domain_socket::get_socket() const {
    return this->inet;
}

inet_domain_socket& inet_domain_socket::get_inet() const {
    return (inet_domain_socket&)*this;
}

tp::addr inet_domain_socket::set_thread_status(const thread_status &_thread) {
    this->_thread_status = _thread;
}

inet_domain_socket::inet_domain_socket() 
        : logging_status{false} {
    this->debug.address = (tp::u64)this;
    this->debug.offset  = (tp::u32)sizeof *this;
}

inet_domain_socket::inet_domain_socket(tp::bit log_status) 
        : logging_status{log_status} {
    this->debug.address = (tp::u64)this;
    this->debug.offset  = (tp::u32)sizeof *this;

    if (log_status) {
        std::string filename = "log" 
                    + std::to_string((tp::s32)this->debug.address)
                    + ".log";

        log_file = filename.c_str();
    }
}

inet_domain_socket::inet_domain_socket(const inet_domain_socket& _this) {
    this->socket_type    = std::move(_this.socket_type);
    this->bus            = _this.bus;
    this->len            = _this.len;
    this->inet           = std::move(_this.inet);
    this->logging_status = _this.logging_status;

    this->debug.address = (tp::u64)this;
    this->debug.offset  = (tp::u32)sizeof *this;

    if (this->logging_status) {
        std::string filename = "log"
                    + std::to_string((tp::s32)this->debug.address)
                    + ".log";

        log_file = filename.c_str();
    }
}

inet_domain_socket::~inet_domain_socket() {
    shutdown(bus, SHUT_RDWR);
    memset(&inet, 0, sizeof inet);

    if (this->logging_status)
        logging("[ INFO ]Called destructor, shutdown socket...\n");
}

tp::addr inet_domain_socket::set_address(const char *address) {
    if (address     == nullptr && 
        socket_type == sk_inet::type::client) {
        if (this->logging_status)
            logging("[FAILED]Set client address failed\n\tcalled: logging(nullptr)");

        return;
    }

    else if (address     == nullptr && 
             socket_type == sk_inet::type::server) {
        this->inet.sin_addr.s_addr = htonl(INADDR_ANY);

        if (this->logging_status)
            logging("[  OK  ]Set server address success\n");

        return;
    }

    this->inet.sin_addr.s_addr = inet_addr(address);
}

tp::addr inet_domain_socket::set_port(const char *port) {
    if (port == nullptr) {
        if (this->logging_status)
            logging("[FAILED]Set port failed\n\tcalled: logging(nullptr)");

        return;
    }

    std::string stoi_conv = port;

    if (std::stoi(stoi_conv) > __INT16_MAX__ || 
        std::stoi(stoi_conv) < 0x0) {
        if (this->logging_status)
            logging("[FAILED]Set port failed\n\tport > __INT16_MAX__ || port < 0\n");

        return;
    }

    tp::u16 set_port_value = std::stoi(stoi_conv);

    this->inet.sin_port = htons(set_port_value);
}

tp::addr inet_domain_socket::set_type(const sk_inet::type &_type) {
    socket_type = std::move(_type);
}

tp::addr inet_domain_socket::socket_create() {
    this->bus = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (this->bus < 0) {
        this->socket_status = sk::status::c_sock_err;

        if (this->logging_status)
            logging("[FAILED]Socket create failed\n");

        return;
    }

    if (this->logging_status)
        logging("[  OK  ]Socket create success\n");

    this->inet.sin_family = AF_INET;
    this->len             = sizeof this->inet;
}

tp::addr inet_domain_socket::bind_socket() {
    if (this->bus < 0) {
        this->socket_status = sk::status::b_sock_err;

        if (this->logging_status)
            logging("[FAILED]Socket don't cofigured\n");

        return;
    }

    tp::s32 result = 0;

    switch (this->socket_type) {
        case sk_inet::type::client:
            result = ::connect(this->bus, 
                               (const sockaddr *)&this->inet, 
                               this->len);

            if (result < 0) {
                this->socket_status = sk::status::b_sock_err;

                if (this->logging_status)
                    logging("[FAILED]Socket bind failed\n\tconnect error\n");
                
                return;
            }

            else
                if (this->logging_status)
                    logging("[  OK  ]Socket bind success\n\tconnect success\n");

            return;

        case sk_inet::type::server:
            result = ::bind(this->bus,
                            (const sockaddr *)&this->inet, 
                            this->len);

            if (result < 0) {
                this->socket_status = sk::status::b_sock_err;

                if (this->logging_status)
                    logging("[FAILED]Socket bind failed\n\tbind error\n");

                return;
            }

            else
                if (this->logging_status)
                    logging("[  OK  ]Socket bind success\n\tbind success\n");

            result = ::listen(this->bus, 512);

            if (result < 0) {
                this->socket_status = sk::status::l_sock_err;

                if (this->logging_status)
                    logging("[FAILED]Socket listen failed\n");

                return;
            }

            else 
                if (this->logging_status)
                    logging("[  OK  ]Socket listen success\n");

            return;

        default:
            this->socket_status = sk::status::b_sock_err;

            if (this->logging_status)
                logging("[FAILED]Bind socket failed\n\tunknown type\n");

            return;
    }
}

tp::addr inet_domain_socket::listen_connection() {
    if (this->bus < 0) {
        if (this->logging_status)
            logging("[FAILED]Socket broken\n");

        return;
    }

    if (this->_thread_status == thread_status::multi_thread) {
        if (this->socket_type == sk_inet::type::server) {
            tp::s32 thr_bus = ::accept(this->bus,
                                      (sockaddr *)&this->inet, 
                                      &this->len);

            if (thr_bus < 0) {
                this->socket_status = sk::status::a_sock_err;

                if (this->logging_status) {
                    logging("[FAILED]Socket accept failed\n\tsocket broken\n");

                    return;
                }
            }

            this->thread_handler(thr_bus);
            
            close(thr_bus);
        }
    }

    else if (this->_thread_status == thread_status::single_thread) {
        if (this->socket_type == sk_inet::type::server) {
            this->_bus = ::accept(this->bus,
                                 (sockaddr *)&this->inet, 
                                 &this->len);

            if (this->_bus < 0) {
                this->socket_status = sk::status::a_sock_err;

                if (this->logging_status) {
                    logging("[FAILED]Socket accept failed\n\tsocket broken\n");

                    return;
                }
            }
        }
    }
}

tp::addr inet_domain_socket::socket_write(const char *data) {
    if (this->bus < 0) {
        if (this->logging_status)
            logging("[FAILED]Socket write failed\n\tsocket broken\n");

        return;       
    }

    platform_utils utils;

    if (data == nullptr)
        data = new char('0');
    
    tp::s32 result = 0;

    if (this->socket_type == sk_inet::type::client) {
        result = ::send(this->bus, data, 
                                       utils.strlen(data), 0);

        if (result < 0) {
            if (this->logging_status)
                logging("[FAILED]Socket write failed\n\tsend error\n");

            return;
        }
        

        else
            if (this->logging_status)
                logging("[  OK  ]Socket write success\n");
    }

    else if (this->socket_type == sk_inet::type::server) {
        result = ::send(this->_bus, data, 
                                         utils.strlen(data), 0);

        if (result < 0) {
            if (this->logging_status)
                logging("[FAILED]Socket server write failed\n\tsend error\n");

            return;
        }

        else
            if (this->logging_status)
                logging("[  OK  ]Socket server write success\n");
    }
}

tp::s32 inet_domain_socket::socket_read(char *data, tp::u32 size_data) {
    if (this->bus < 0) {
        if (this->logging_status)
            logging("[FAILED]Socket read failed\n\tsocket broken\n");

        return -2;
    }

    tp::s32 result = 0;

    platform_utils utils;

    if (data == nullptr)
        data = new char[size_data];

    else
        utils.memset(data, 0, utils.strlen(data));

    
    if (this->socket_type == sk_inet::type::client) {
        result = ::recv(this->bus, data, size_data, 0);

        if (result < 0) {
            if (this->logging_status)
                logging("[FAILED]Socket read failed\n\trecv error\n");

            return -1;
        }

        else 
            if (this->logging_status)
                logging("[  OK  ]Socket read success\n");
    }

    else if (this->socket_type == sk_inet::type::server) {
        result = ::recv(this->_bus, data, size_data, 0);

        if (result < 0) {
            if (this->logging_status)
                logging("[FAILED]Socket server read failed\n\trecv error\n");

            return -1;
        }

        else
            if (this->logging_status)
                logging("[  OK  ]Socket server read success\n");
    }

    return result;
}

tp::addr inet_domain_socket::close_handle() {
    if (this->socket_type == sk_inet::type::client)
        shutdown(this->bus, SHUT_RDWR);

    else if (this->socket_type == sk_inet::type::server) {
        shutdown(this->bus,  SHUT_RDWR);
        shutdown(this->_bus, SHUT_RDWR);
    }
}

tp::addr inet_domain_socket::close_socket() {
    if (this->socket_type == sk_inet::type::client)
        close(this->bus);

    else if (this->socket_type == sk_inet::type::server)
        close(this->_bus);
}

tp::addr inet_domain_socket::logging(const char *log) {
    if (log == nullptr)
        return;

    platform_utils utils;

    std::ofstream file(log_file, std::ios_base::binary | 
                                         std::ios_base::app);

    if (file.is_open()) {
        file.write(log, utils.strlen(log));
        file.close();
    }

    else 
        file.close();
}

_socket::net_handler_t inet_domain_socket::get_handler() const {
    return this->handler;
}

tp::addr inet_domain_socket::set_handler(net_handler_t _net) {
    this->handler = _net;
}

sk::status inet_domain_socket::get_status() const {
    return this->socket_status;
}

tp::addr inet_domain_socket::set_logging(tp::bit log_status) {
    this->logging_status = log_status;
}