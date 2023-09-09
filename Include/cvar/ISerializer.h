// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: ISerializer.h - serializer interfaces header file
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/Api.h>
#include <cvar/CVarTypes.h>
#include <exception>
#include <cstring>
#include <ostream>
#include <iostream>
#include <optional>
#include <unordered_map>

namespace CVar {
    
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
            std::unordered_map<String, Value> m_root;

        protected:
            template <typename U>
            std::optional<U> _TokenizeString(bool _bExpectQuot = true) {
                std::optional<U> str = std::nullopt;
                std::string stdStr;

                if (_bExpectQuot) {
                    char cQuot = static_cast<char>(m_stream.peek());
                    if (cQuot == '"' || cQuot == '\'') {
                        static_cast<void>(m_stream.get());

                        while (m_stream.peek() != -1 && m_stream.peek() != cQuot)
                            stdStr += static_cast<char>(m_stream.get());

                        str = U(stdStr);

                        if (!m_stream.eof())
                            static_cast<void>(m_stream.get());
                    }
                } else {
                    while (m_stream.peek() != -1 && !_Contains(m_stream.peek(), " \t\n\r", 4))
                        stdStr += static_cast<char>(m_stream.get());

                    str = U(stdStr);
                }

                return str;
            }

            std::optional<Int> _TokenizeInt() {
                std::optional<Int> iVal = std::nullopt;
                std::string sInt;

                size_t uReverseBytes = 0;
                while (m_stream.peek() != -1 && ((m_stream.peek() <= '9' && m_stream.peek() >= '0') || m_stream.peek() == '-')) {
                    sInt += static_cast<char>(m_stream.get());
                    uReverseBytes++;
                }

                if (m_stream.peek() != '.' && !sInt.empty())
                    iVal = static_cast<Int>(std::stoi(sInt));
                else {
                    for (size_t i = 0; i < uReverseBytes; i++)
                        m_stream.unget();
                }

                return iVal;
            }

            std::optional<Float> _TokenizeFloat() {
                std::optional<Float> fVal = std::nullopt;
            
                std::string sFloat;
                size_t uReverseBytes = 0;
                while (m_stream.peek() != -1 && _Contains(m_stream.peek(), "123456789e-.", 12)) {
                    sFloat += static_cast<char>(m_stream.get());
                    uReverseBytes++;
                }

                if (!sFloat.empty())
                    fVal = static_cast<Float>(std::stof(sFloat));
                else {
                    for (size_t i = 0; i < uReverseBytes; i++)
                        m_stream.unget();
                }

                return fVal;
            }

            std::optional<Bool> _TokenizeBool() {
                char buf[sizeof("false")] = {};

                // true
                size_t i = 0;
                for (i = 0; i < sizeof("true")-1 && m_stream.peek() != -1; i++)
                    buf[i] = static_cast<char>(m_stream.get());

                if (!std::strcmp(buf, "true"))
                    return true;

                for (size_t j = 0; j < i; j++)
                    m_stream.unget();
                m_stream.clear();

                // false
                for (i = 0; i < sizeof("false")-1 && m_stream.peek() != -1; i++)
                    buf[i] = static_cast<char>(m_stream.get());

                if (!std::strcmp(buf, "false"))
                    return false;

                for (size_t j = 0; j < i; j++)
                    m_stream.unget();
                m_stream.clear();

                return std::nullopt;
            }

            bool _Contains(char _c, const char* _szFilter, size_t _uSize) {
                for (size_t i = 0; i < _uSize; i++) {
                    if (_szFilter[i] == _c) return true;
                }

                return false;
            }

        public:
            IUnserializer(std::istream& _stream) :
                m_stream(_stream) {}

            inline T&& Get() {
                return std::move(m_root);
            }
    };


    class SyntaxErrorException : public std::exception {
        private:
            std::string m_sWhatMessage;

        public:
            SyntaxErrorException(const std::string& _sWhat = "Unknown exception") :
                m_sWhatMessage(_sWhat) {}

            const char* what() const noexcept override {
                return m_sWhatMessage.c_str();
            }
    };


    class UnexpectedEOFException : public std::exception {
        private:
            std::string m_sWhatMessage;

        public:
            UnexpectedEOFException(const std::string& _sWhat = "Unknown exception") :
                m_sWhatMessage(_sWhat) {}

            const char* what() const noexcept override {
                return m_sWhatMessage.c_str();
            }
    };
}
