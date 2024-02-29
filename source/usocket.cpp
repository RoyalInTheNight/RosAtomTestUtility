#include "../include/socket.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <ios>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

tp::bit unix_domain_socket::get_logging() const {
    return this->logging_status;
}

unix_domain_socket::unix_domain_socket() 
        : logging_status{false} {
    this->debug.address = (tp::u64)this;
    this->debug.offset  = (tp::u32)sizeof *this;
}

unix_domain_socket::unix_domain_socket(tp::bit log_status) 
        : logging_status{log_status} {
    this->debug.address = (tp::u64)this;
    this->debug.offset  = (tp::u32)sizeof *this;

    if (this->logging_status) {
        std::string filename = "log"
                             + std::to_string((tp::s32)this->debug.address)
                             + ".log";
        
        log_file = filename.c_str();
    }
}

unix_domain_socket::unix_domain_socket(const unix_domain_socket& _this) {
    this->socket_type    = std::move(_this.socket_type);
    this->bus            = _this.bus;
    this->len            = _this.len;
    this->_unix          = std::move(_this._unix);
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

unix_domain_socket::~unix_domain_socket() {
    shutdown(this->bus, SHUT_RDWR);
    memset(&_unix, 0, sizeof _unix);

    if (this->logging_status)
        logging("[ INFO ]Called destructor, shutdown unix socket...\n");
}

tp::addr unix_domain_socket::set_path(const char *path) {
    platform_utils utils;

    if (utils.strlen(path) > sizeof(this->_unix.sun_path)) {
        if (this->logging_status)
            logging("[FAILED]Set unix path failed\n\tsize path > _unix.sun_path\n");

        return;
    }

    strcpy(this->_unix.sun_path, path);
}

tp::addr unix_domain_socket::set_type(const sk_unix::type &_type) {
    this->socket_type = std::move(_type);
}

tp::addr unix_domain_socket::socket_create() {
    this->bus = ::socket(AF_UNIX, SOCK_STREAM, 0);

    if (this->bus < 0) {
        this->socket_status = sk::status::c_sock_err;

        if (this->logging_status)
            logging("[FAILED]Socket unix create failed\n");

        return;
    }

    if (this->logging_status)
        logging("[  OK  ]Socket unix create success\n");

    this->_unix.sun_family = AF_UNIX;
    this->len              = sizeof this->_unix;
}

tp::addr unix_domain_socket::bind_socket() {
    if (this->bus < 0) {
        this->socket_status = sk::status::b_sock_err;

        if (this->logging_status)
            logging("[FAILED]Socket don't configured\n");

        return;
    }

    tp::s32 result = 0;

    switch (this->socket_type) {
        case sk_unix::type::client:
            result = ::connect(this->bus, 
                               (const sockaddr *)&this->_unix, 
                               this->len);

            if (result < 0) {
                this->socket_status = sk::status::b_sock_err;

                if (this->logging_status)
                    logging("[FAILED]Socket unix bind failed\n\tconnect error\n");

                return;
            }

            else
                if (this->logging_status)
                    logging("[  OK  ]Socket unix bind success\n");

            return;

        case sk_unix::type::server:
            result = ::bind(this->bus,
                            (const sockaddr *)&this->_unix,
                            this->len);

            if (result < 0) {
                this->socket_status = sk::status::b_sock_err;

                if (this->logging_status)
                    logging("[FAILED]Socket unix bind failed\n\tbind error\n");

                return;
            }

            else
                if (this->logging_status)
                    logging("[  OK  ]Socket unix bind success\n");

            result = ::listen(this->bus, 2);

            if (result < 0) {
                this->socket_status = sk::status::l_sock_err;

                if (this->logging_status)
                    logging("[FAILED]Socket unix listen failed\n\tlisten error\n");

                return;
            }          

            else
                if (this->logging_status)
                    logging("[  OK  ]Socket unix listen success\n");

            return; 

        default:
            this->socket_status = sk::status::b_sock_err;

            if (this->logging_status)
                logging("[FAILED]Bind unix socket failed\n\tunknown type\n");

            return;
    }
}

tp::addr unix_domain_socket::listen_connection() {
    if (this->bus < 0) {
        if (this->logging_status)
            logging("[FAILED]Socket broken\n");

        return;
    }

    if (this->socket_type == sk_unix::type::server) {
        this->_bus = ::accept(this->bus, 
                             (sockaddr *)&this->_unix, 
                             &this->len);

        if (this->_bus < 0) {
            this->socket_status = sk::status::a_sock_err;

            if (this->logging_status)
                logging("[FAILED]Socket unix accept failed\n\tsocket broken\n");

            return;
        }
    }
}

tp::addr unix_domain_socket::socket_write(const char *data) {
    if (this->bus < 0) {
        if (this->logging_status)
            logging("[FAILED]Socket write failed\n\tsocket broken\n");

        return;
    }

    platform_utils utils;

    tp::s32 result = 0;

    if (this->socket_type == sk_unix::type::client) {
        result = ::send(this->bus, data, 
                                        utils.strlen(data), 0);

        if (result < 0) {
            this->socket_status = sk::status::s_sock_err;

            if (this->logging_status)
                logging("[FAILED]Socket unix write failed\n\tsend error\n");

            return;
        }

        else
            if (this->logging_status)
                logging("[  OK  ]Socket unix write success\n");
    }

    else if (this->socket_type == sk_unix::type::server) {
        result = ::send(this->_bus, data, 
                                        utils.strlen(data), 0);

        if (result < 0) {
            this->socket_status = sk::status::s_sock_err;

            if (this->logging_status)
                logging("[FAILED]Socket unix server write failed\n\tsend error\n");
            
            return;
        }

        else
            if (this->logging_status)
                logging("[  OK  ]Socket unix server write success\n");
    }
}

tp::addr unix_domain_socket::socket_write(const char *data, tp::u32 data_size) {
    if (this->bus < 0) {
        if (this->logging_status)
            logging("[FAILED]Socket write failed\n\tsocket broken\n");

        return;
    }

    //platform_utils utils;

    tp::s32 result = 0;

    if (this->socket_type == sk_unix::type::client) {
        result = ::send(this->bus, data, data_size, 0);

        if (result < 0) {
            this->socket_status = sk::status::s_sock_err;

            if (this->logging_status)
                logging("[FAILED]Socket unix write failed\n\tsend error\n");

            return;
        }

        else
            if (this->logging_status)
                logging("[  OK  ]Socket unix write success\n");
    }

    else if (this->socket_type == sk_unix::type::server) {
        result = ::send(this->_bus, data, data_size, 0);

        if (result < 0) {
            this->socket_status = sk::status::s_sock_err;

            if (this->logging_status)
                logging("[FAILED]Socket unix server write failed\n\tsend error\n");

            return;
        }

        else
            if (this->logging_status)
                logging("[  OK  ]Socket unix server write success\n");
    }
}

tp::s32 unix_domain_socket::socket_read(char *data, tp::s32 size) {
    if (this->bus < 0) {
        if (this->logging_status)
            logging("[FAILED]Socket read failed\n\tsocket broken\n");

        return -1;
    }

    platform_utils utils;

    if (data == nullptr)
        data = new char[size];

    else
        utils.memset(data, 0, utils.strlen(data));

    if (this->socket_type == sk_unix::type::client) {
        this->recv_unix_result = ::recv(this->bus, data, size, 0);

        if (this->recv_unix_result < 0) {
            this->socket_status = sk::status::r_sock_err;

            if (this->logging_status)
                logging("[FAILED]Socket read failed\n\trecv error\n");

            return -1;
        }

        else
            if (this->logging_status)
                logging("[  OK  ]Socket read success\n");
    }

    else if (this->socket_type == sk_unix::type::server) {
        this->recv_unix_result = ::recv(this->_bus, data, size, 0);

        if (this->recv_unix_result < 0) {
            this->socket_status = sk::status::r_sock_err;

            if (this->logging_status)
                logging("[FAILED]Socket unix server read failed\n\trecv error\n");
            
            return -1;
        }

        else
            if (this->logging_status)
                logging("[  OK  ]Socket unix server read success\n");
    }

    return this->recv_unix_result;
}

tp::addr unix_domain_socket::close_handle() {
    if (this->socket_type == sk_unix::type::client) {
        shutdown(this->bus, SHUT_RDWR);
        unlink(this->_unix.sun_path);
    }

    else if (this->socket_type == sk_unix::type::server) {
        shutdown(this->bus,  SHUT_RDWR);
        shutdown(this->_bus, SHUT_RDWR);
        unlink(this->_unix.sun_path);
    }
}

tp::addr unix_domain_socket::close_socket() {
    if (this->socket_type == sk_unix::type::client)
        close(this->bus);

    else if (this->socket_type == sk_unix::type::server)
        close(this->_bus);
}

tp::addr unix_domain_socket::logging(const char *log) {
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

_socket::sock_unix_t unix_domain_socket::get_socket() const {
    return this->_unix;
}

_socket::sock_bus_t unix_domain_socket::get_socket_bus() const {
    if (this->socket_type == sk_unix::type::server)
        return this->_bus;

    return this->bus;
}

tp::addr unix_domain_socket::set_handler(net_handler_t _net) {
    this->handler = _net;
}

net_handler_t unix_domain_socket::get_handler() const {
    return this->handler;
}

sk::status unix_domain_socket::get_status() const {
    return this->socket_status;
}

tp::s32 unix_domain_socket::get_recv_result() const {
    return this->recv_unix_result;
}

tp::addr unix_domain_socket::set_logging(tp::bit log_mode) {
    this->logging_status = log_mode;
}