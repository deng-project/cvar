// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: SID.h - Runtime string hashing implementation
// author: Karl-Mihkel Ott

#include <cstring>
#include <cvar/SID.h>

namespace CVar {
    #if defined(ENV32)
	uint32_t RuntimeCrc32(const std::string& _str) {
		uint32_t uHash = 0;
		for (size_t i = 0; i < _str.size(); i++) {
			uHash = (uHash >> 8) ^ crc32_table[(uHash ^ _str[i]) & 0xff];
		}

		return uHash ^ 0xffffffff;
	}

    uint32_t RuntimeCrc32(const char* const _szData) {
        uint32_t uHash = 0;
        for (size_t i = 0; i < std::strlen(_szData); i++) {
            uHash = (uHash >> 8) ^ crc32_table[(uHash ^ _szData[i]) & 0xff];
        }

        return uHash ^ 0xffffffff;
    }
    #elif defined (ENV64)
	uint64_t RuntimeCrc64(const std::string& _str) {
		uint64_t uHash = 0;
		for (size_t i = 0; i < _str.size(); i++) {
			uHash = (uHash >> 8) ^ crc64_table[(uHash ^ _str[i]) & 0xff];
		}

		return uHash ^ 0xffffffffffffffff;
	}

    uint64_t RuntimeCrc64(const char* const _szData) {
        uint64_t uHash = 0;
        for (size_t i = 0; i < std::strlen(_szData); i++) {
            uHash = (uHash >> 8) ^ crc64_table[(uHash ^ _szData[i]) & 0xff];
        }

        return uHash ^ 0xffffffffffffffff;
    }
    #endif
}
