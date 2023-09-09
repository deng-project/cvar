// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: JSONUnserializer.h - JSON unserializer class header
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/Api.h>
#include <cvar/ISerializer.h>
#include <optional>
#include <limits>
#include <queue>
#include <stack>

namespace CVar {

    struct JSONNull {};

    struct JSONToken {
        JSONToken() = default;
        JSONToken(const std::variant<std::monostate, char, String, Float, Int, Bool, JSONNull>& _token, uint32_t _uLine) :
            token(_token),
            uLine(_uLine) {}

        std::variant<std::monostate, char, String, Float, Int, Bool, JSONNull> token;
        uint32_t uLine = 1;
    };

    // for streaming variants
    std::ostream& operator<<(std::ostream& _stream, std::variant<std::monostate, char, String, Float, Int, Bool, JSONNull>& _token);

    enum JSONTokenIndex {
        JSONTokenIndex_Unknown,
        JSONTokenIndex_Char,
        JSONTokenIndex_String,
        JSONTokenIndex_Float,
        JSONTokenIndex_Int,
        JSONTokenIndex_Bool,
        JSONTokenIndex_JSONNull
    };

    class CVAR_API JSONUnserializer : public IUnserializer<std::unordered_map<String, Value>> {
        private:
            JSONToken m_token = JSONToken(std::monostate{}, 1);
            const char m_szJsonSyntax[8] = { '{', '}', '[', ']', ',', ':', '\"', '\'' };
            const char m_szJsonWhitespace[4] = { ' ', '\n', '\r', '\t' };
            uint32_t m_uLineCounter = 1;

        private:
            // poll a next token
            bool _NextToken();
            void _NextTokenEx();

            std::optional<JSONNull> _TokenizeNull();

            template <typename T>
            bool _TryValueTokenization(const std::optional<T>& _jsonValue) {
                if (_jsonValue.has_value()) {
                    m_token.token = _jsonValue.value();
                    m_token.uLine = m_uLineCounter;
                    return true;
                }

                return false;
            }

            List _ParseList();
            void _ParseObject(std::unordered_map<String, Value>* _pRootObject);
            void _Parse();

        public:
            JSONUnserializer(std::istream& _stream);
    };
}
