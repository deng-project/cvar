// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: ISerializer.h - serializer interface header file
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/Api.h>
#include <cvar/CVarTypes.h>
#include <ostream>
#include <istream>
#include <unordered_map>

namespace CVar {

    class CVAR_API ISerializer {
        protected:
            std::ostream& m_stream;
            std::unordered_map<String, Value>& m_root;

        public:
            ISerializer(std::ostream& _stream, std::unordered_map<String, Value>& _root) :
                m_stream(_stream),
                m_root(_root) {}
            virtual void Serialize(bool bBeautified = true) = 0;
    };


    class CVAR_API IUnserializer {
        protected:
            std::istream& m_stream;
            std::unordered_map<String, Value> m_root;

        public:
            IUnserializer(std::istream& _stream) :
                m_stream(_stream) {}

            inline std::unordered_map<String, Value>&& Get() {
                return std::move(m_root);
            }
    };
}
