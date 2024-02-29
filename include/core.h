#ifndef PLATFORM_CORE
#define PLATFORM_CORE

namespace core {
    namespace c_types {
        typedef char                s8;
        typedef unsigned char       u8;
        typedef short              s16;
        typedef unsigned short     u16;
        typedef int                s32;
        typedef unsigned int       u32;
        typedef long long          s64;
        typedef unsigned long long u64;
        typedef bool               bit;
        typedef void              addr;

        namespace ptr {
            typedef s8   *  _s8;
            typedef u8   *  _u8;
            typedef s16  * _s16;
            typedef u16  * _u16;
            typedef s32  * _s32;
            typedef u32  * _u32;
            typedef s64  * _s64;
            typedef u64  * _u64;
            typedef addr *_addr;
        }
    }

    namespace _socket {
        enum class status : core::c_types::u8 {
            c_sock_err = 1,
            b_sock_err = 2,
            l_sock_err = 3,
            a_sock_err = 4,
            s_sock_err = 5,
            r_sock_err = 6
        };

        namespace unix_domain {
            enum class type : core::c_types::u8 {
                server = 0xfe,
                client = 0xff
            };
        }

        namespace inet_domain {
            enum class type : core::c_types::u8 {
                server = 0xee,
                client = 0xef
            };
        }
    }

    namespace _spi {

    }
}

extern core::c_types::bit KAMAz_stopped;

#endif // PLATFORM_API_KAMAz