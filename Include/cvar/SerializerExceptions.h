// cvar: Console variable systems support library
// license: Apache, see LICENCE file
// file: SerializerExceptions.h - serializer exception types header file
// author: Karl-Mihkel Ott

#pragma once

#include <string>
#include <exception>

namespace cvar {
    

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