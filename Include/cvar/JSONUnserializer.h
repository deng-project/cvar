// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: JSONUnserializer.h - JSON unserializer class header
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/Api.h>
#include <cvar/ISerializer.h>
#include <limits>
#include <unordered_set>

namespace CVar {

    struct JSONNull {};

    class CVAR_API JSONUnserializer : public IUnserializer {
        private:
            std::vector<std::variant<char, String, Float, Int, Bool, JSONNull>> m_tokens;
            const char m_szJsonSyntax[8] = { '{', '}', '[', ']', ',', ':', '\"', '\'' };
            const char m_szJsonWhitespace[4] = { ' ', '\n', '\r', '\t' };

        private:
            std::variant<std::monostate, String> _LexString();
            std::variant<std::monostate, Int> _LexInt();
            std::variant<std::monostate, Float> _LexFloat();
            std::variant<std::monostate, Bool> _LexBool();
            std::variant<std::monostate, JSONNull> _LexNull();

            bool _Contains(char _c, const char* _szFilter, size_t _uSize);

            void _Lex();
            void _Parse();

        public:
            JSONUnserializer(std::istream& _stream);
    };
}
