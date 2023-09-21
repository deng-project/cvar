// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: ISerializer.h - serializer interfaces header file
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/Api.h>
#include <cvar/CVarTypes.h>
#include <cstring>
#include <istream>
#include <optional>
#include <unordered_map>

namespace cvar {
    
    template <typename T>
    class CVAR_API ISerializer {
        protected:
            std::ostream& m_stream;
            T& m_root;

        public:
            ISerializer(std::ostream& _stream, T& _root) :
                m_stream(_stream),
                m_root(_root) {}
            virtual void Serialize(bool bBeautified = true) = 0;
    };


    template <typename T>
    class CVAR_API IUnserializer {
        protected:
            std::istream& m_stream;
            T m_root;

        public:
            IUnserializer(std::istream& _stream) :
                m_stream(_stream) {}

            inline T&& Get() {
                return std::move(m_root);
            }
    };


    template<typename T>
    class CVAR_API IPlainTextUnserializer : public IUnserializer<T> {
        public:
            IPlainTextUnserializer(std::istream& _stream) :
                IUnserializer<T>(_stream) {}

        protected:
            template <typename U>
            std::optional<U> _TokenizeString(bool _bExpectQuot = true) {
                std::optional<U> str = std::nullopt;
                std::string stdStr;

                if (_bExpectQuot) {
                    char cQuot = static_cast<char>(this->m_stream.peek());
                    if (cQuot == '"' || cQuot == '\'') {
                        static_cast<void>(this->m_stream.get());

                        while (this->m_stream.peek() != -1 && this->m_stream.peek() != cQuot)
                            stdStr += static_cast<char>(this->m_stream.get());

                        str = U(stdStr);

                        if (!this->m_stream.eof())
                            static_cast<void>(this->m_stream.get());
                    }
                } else {
                    char cPeek = static_cast<char>(this->m_stream.peek());
                    while (cPeek != -1 && !_Contains(cPeek, " \t\n\r", 4)) {
                        stdStr += static_cast<char>(this->m_stream.get());
                        cPeek = static_cast<char>(this->m_stream.peek());
                    }

                    str = U(stdStr);
                }

                return str;
            }

            std::optional<Int> _TokenizeInt(const std::string& _str) {
                if (_str.empty())
                    return std::nullopt;
                
                bool bInvalid = false;
                for (size_t i = 0; i < _str.size(); i++) {
                    if ((_str[i] < '0' || _str[i] > '9') && _str[i] != '-') {
                        bInvalid = true;
                        break;
                    }
                }

                if (bInvalid) return std::nullopt;
                return static_cast<Int>(std::stoi(_str));
            }

            std::optional<Float> _TokenizeFloat(const std::string& _str) {
                if (_str.empty())
                    return std::nullopt;

                bool bInvalid = false;
                for (size_t i = 0; i < _str.size(); i++) {
                    if ((_str[i] < '0' || _str[i] > '9') && _str[i] != '.' && _str[i] != 'e' && _str[i] != '-') {
                        bInvalid = true;
                        break;
                    }
                }

                if (bInvalid)
                    return std::nullopt;

                return static_cast<Float>(std::stof(_str));
            }

            std::optional<Bool> _TokenizeBool(const std::string& _str) {
                if (_str == "true")
                    return true;
                else if (_str == "false")
                    return false;

                return std::nullopt;
            }

            bool _Contains(char _c, const char* _szFilter, size_t _uSize) {
                for (size_t i = 0; i < _uSize; i++) {
                    if (_szFilter[i] == _c) return true;
                }

                return false;
            }
    };
}
