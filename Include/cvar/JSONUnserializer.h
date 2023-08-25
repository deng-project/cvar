// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: JSONUnserializer.h - JSON unserializer class header
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/Api.h>
#include <cvar/ISerializer.h>
#include <limits>
#include <queue>
#include <stack>

namespace CVar {

    struct JSONNull {};

    struct JSONToken {
        JSONToken() = default;
        JSONToken(const std::variant<char, String, Float, Int, Bool, JSONNull>& _token, uint32_t _uLine) :
            token(_token),
            uLine(_uLine) {}

        std::variant<char, String, Float, Int, Bool, JSONNull> token;
        uint32_t uLine = 1;
    };

    // for streaming variants
    std::ostream& operator<<(std::ostream& _stream, std::variant<char, String, Float, Int, Bool, JSONNull>& _token);

    enum JSONTokenIndex {
        JSONTokenIndex_Char,
        JSONTokenIndex_String,
        JSONTokenIndex_Float,
        JSONTokenIndex_Int,
        JSONTokenIndex_Bool,
        JSONTokenIndex_JSONNull
    };

    class CVAR_API JSONUnserializer : public IUnserializer {
        private:
            std::queue<JSONToken> m_qTokens;
            const char m_szJsonSyntax[8] = { '{', '}', '[', ']', ',', ':', '\"', '\'' };
            const char m_szJsonWhitespace[4] = { ' ', '\n', '\r', '\t' };
            uint32_t m_uLineCounter = 1;

        private:
            std::variant<std::monostate, String> _LexString();
            std::variant<std::monostate, Int> _LexInt();
            std::variant<std::monostate, Float> _LexFloat();
            std::variant<std::monostate, Bool> _LexBool();
            std::variant<std::monostate, JSONNull> _LexNull();

            bool _Contains(char _c, const char* _szFilter, size_t _uSize);

            void _Lex();

            inline bool _EofError() {
                if (m_qTokens.empty()) {
                    m_errStream << "Unexpected EOF";
                    return true;
                }
                return false;
            }

            List _ParseList();
            void _ParseObject(std::unordered_map<String, Value>* _pRootObject);
            void _Parse();

        public:
            JSONUnserializer(std::istream& _stream, std::ostream& _errStream = std::cerr);
    };
}
