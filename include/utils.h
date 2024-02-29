#ifndef PLATFORM_UTILS
#define PLATFORM_UTILS

#include <cstddef>
#include <cstdlib>
#include "core.h"

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <fstream>
#include <string>

namespace tp = core::c_types;

class platform_utils {
public:
    typedef nlohmann::json json;
    typedef std::string    path;

    tp::addr memcpy(void *, const void *, size_t);
    tp::addr memset(void *, tp::s32, size_t);
    
    [[nodiscard]] size_t strlen(const char *);
};

namespace ADASIS {
    typedef struct {
        tp::s32 msgType;
        tp::s32 cyclicCounter;
        tp::s32 pathIndex;
        tp::s32 offset;
        tp::s32 positionIndex;
        tp::s32 positionAge;
        tp::s32 speed;
        tp::s32 relativeHeading;
        tp::s32 positionProbability;
        tp::s32 positionConfidence;
        tp::s32 currentLane;
    } ADASIS_position;

    typedef struct {
        tp::s32 msgType;
        tp::s32 cyclicCounter;
        tp::s32 retransmission;
        tp::s32 pathIndex;
        tp::s32 offset;
        tp::s32 update;
        tp::s32 functionalRoadClass;
        tp::s32 formOfWay;
        tp::s32 effectiveSpeedLimit;
        tp::s32 effectiveSpeedLimitType;
        tp::s32 numberOfIanesInDrivingDirection;
        tp::s32 numberOfIanesInOppositeDirection;
        tp::s32 tunnel;
        tp::s32 bridge;
        tp::s32 dividedRoad;
        tp::s32 builtUpArea;
        tp::s32 complexIntersection;
        tp::s32 relativeProbability;
        tp::s32 partOfCalculateRoute;
    } ADASIS_segment;

    typedef struct {
        tp::s32 msgType;
        tp::s32 cyclicCounter;
        tp::s32 pathIndex;
        tp::s32 retransmission;
        tp::s32 offset;
        tp::s32 update;
        tp::s32 subPathIndex;
        tp::s32 turnAngle;
        tp::s32 relativeProbability;
        tp::s32 functionalRoadClass;
        tp::s32 formOfWay;
        tp::s32 numberOfIanesInDrivingDirection;
        tp::s32 numberOfIanesInOppositeDirection;
        tp::s32 complexIntersection;
        tp::s32 rightOfWay;
        tp::s32 partOfCalculateRoute;
        tp::s32 lastStubAtOffset;
    } ADASIS_stub;

    typedef struct {
        tp::s32 msgType;
        tp::s32 cyclicCounter;
        tp::s32 retransmission;
        tp::s32 pathIndex;
        tp::s32 offset;
        tp::s32 update;
        tp::s32 profileType;
        tp::s32 controlPoint;
        tp::s32 value0;
        tp::s32 distance1;
        tp::s32 value1;
        tp::s32 accuracy;
    } ADASIS_profshort;

    typedef struct {
        tp::s32 msgType;
        tp::s32 cyclicCounter;
        tp::s32 retransmission;
        tp::s32 pathIndex;
        tp::s32 offset;
        tp::s32 update;
        tp::s32 profileType;
        tp::s32 controlPoint;
        tp::u32 value;
    } ADASIS_proflong;

    typedef struct {
        tp::s32 msgType;
        tp::s32 cyclicCounter;
        tp::s32 countryCode;
        tp::s32 regionCode;
        tp::s32 drivingSide;
        tp::s32 speedUnits;
        tp::s32 majorProtocolVersion;
        tp::s32 minorProtocolVersion;
        tp::s32 minorProtocolSubVersion;
        tp::s32 hardwareVersion;
        tp::s32 mapProvider;
        tp::s32 mapVersionYear;
        tp::s32 mapVersionYearQuarter;
    } ADASIS_metadata;

    class ADASIS_utils {
    public:
        tp::s32 dataSendIntervalMilliseconds = 1000;

        std::string ADASIS_Parsing(const std::string&, char* data);
    };
}

typedef unsigned long long __Sz;

template<class __Tp> class alloc {
private:
    typedef __Tp *__Pt;

public:
    static __Pt allocator(__Sz size = 0) {
        size = size <= 0 ? 1 : size;

        __Pt ptr = (__Pt)malloc(size);

        for (__Sz i = 0; i < size; i++)
            *(ptr + i) = __Tp();
    }

    static __Pt add_memory(__Pt ptr, __Sz size, __Sz new_size) {
        __Pt tmp_ptr = allocator(new_size);

        size = size > new_size ? new_size : size;

        for (__Sz i = 0; i < size; i++)
            *(tmp_ptr + i) = *(ptr + i);

        size = new_size;

        delete[] ptr;
        return tmp_ptr;
    }
};

template<class __Tp> class cxx_allocator {
private:
    typedef __Tp *__Pt;

public:
    inline __Pt allocator(__Sz size = 0) {
        size = size <= 0 ? 1 : size;

        __Pt ptr = new __Tp[size];

        return ptr;
    }

    inline __Pt add_memory(__Pt  ptr, __Sz size, __Sz new_size) {
        __Pt tmp_ptr = allocator(new_size);

        size = size > new_size ? new_size : size;

        for (__Sz i = 0; i < size; i++)
            *(tmp_ptr + i) = *(ptr + i);
        
        size = new_size;

        delete[] ptr;
        return tmp_ptr;
    }
};

template<class __Ch, 
         class __Allocator = cxx_allocator<__Ch>> class base_buffer
        : public __Allocator {
private:
    typedef __Ch *__PCh;
    typedef bool  __Bit;
    typedef int    __It;
    typedef base_buffer<__Ch, __Allocator> buffer;

    __PCh  ptr;
    __Sz psize;
    __It  ferr;
    __It *refc;

    inline __Sz ptrlen(const __PCh ptr) {
        __PCh s;

        for (s = ptr; *s; s++);
        return s - ptr;
    }

public:
    base_buffer(buffer& cpy) : ptr{cpy.ptr}, psize{cpy.psize}, refc{cpy.refc} {++(*refc);}
    base_buffer(__Sz size) : ptr{this->allocator(size)}, psize{size}, refc{new tp::s32()} {++(*refc);}
    base_buffer() : psize{0}, refc{new tp::s32()} {++(*refc);}
    base_buffer(__PCh cpy) : refc(new tp::s32()) {
        ++(*refc);

        psize = ptrlen(cpy);
        ptr   = this->allocator(psize);

        for (__Sz i = 0; i < psize; i++)
            *(ptr + i) = *(cpy + i); 
    }

    inline void update_size() {
        __Sz rsize = ptrlen(ptr);

        if (rsize != psize)
            ptr = this->add_memory(ptr, psize, rsize);
    }

    inline __It lastError() const {return ferr;}
    inline __Bit isEmpty() const {return psize <= 0 ? true : false;}
    inline __Sz size() const {return psize;}
    inline __Sz length() const {return psize <= 0 ? 0 : psize - 1;}
    inline __PCh data() {update_size(); return ptr;}

    inline __Ch& at(__Sz pos = 0) {
        if (pos >= psize) {
            ferr = 0x21;

            return (__Ch&)__Ch();
        }

        return *(ptr + pos);
    }

    inline void push_back(const __Ch& value) {
        if (psize == 0) {
            ptr = this->allocator();
            psize++;
        }

        else
            ptr = this->add_memory(ptr, psize, psize++);

        if (ptr == nullptr)
            ferr = 0x22;

        *(ptr + this->length()) = (__Ch)value;                
    }

    inline void pop_back() {
        if (psize == 0)
            return;

        --psize;
    }

    inline buffer& operator=(const buffer& cpy) {
        if (cpy.isEmpty())
            return (buffer&)*this;
        
        if (cpy.refc) {
            delete[] ptr;

            ptr   = cpy.ptr;
            psize = cpy.psize;
            refc  = cpy.refc;

            ++(*refc);
        }

        return (buffer&)*this;
    }

    inline buffer& operator=(const __PCh cpy) {
        if (cpy == nullptr)
            return (buffer&)*this;
        

        __Sz ptr_size = ptrlen(cpy);

        ptr = this->add_memory(ptr, psize, ptr_size);

        for (__Sz i = 0; i < ptr_size; i++)
            *(ptr + i) = *(cpy + i);

        return (buffer&)*this;
    }

    inline buffer& operator+(const buffer& cpy) {
        if (cpy.isEmpty())
            return (buffer&)*this;
        
        __Sz new_size = psize + cpy.psize;

        ptr = this->add_memory(ptr, psize, new_size);

        for (__Sz i = 0; i < cpy.psize; i++)
            *(ptr + (i + psize)) = cpy.at(i);

        psize = new_size;
        
        return (buffer&)*this;
    }

    inline buffer& operator+(const __PCh cpy) {
        if (cpy == nullptr)
            return (buffer&)*this;

        __Sz cpy_size = ptrlen(cpy);
        __Sz new_size = psize + cpy_size;

        ptr = this->add_memory(ptr, psize, new_size);

        for (__Sz i = 0; i < cpy_size; i++)
            *(ptr + (i + psize)) = *(cpy + i);

        psize = new_size;
        
        return (buffer&)*this;
    }

    inline buffer& operator+=(const buffer& cpy) {
        return (buffer&)*this + cpy;
    }

    inline buffer& operator+=(const __PCh cpy) {
        return (buffer&)*this + cpy;
    }

    ~base_buffer() {
        if (refc) {
            --(*refc);

            if (!(*refc)) {
                if (psize > 0)
                    delete[] ptr;

                delete refc;
            }
        }
    }
};

#endif // PLATFORM_UTILS