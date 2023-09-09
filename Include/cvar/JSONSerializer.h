// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: JSONSerializer.h - JSON serializer class header
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/Api.h>
#include <cvar/ISerializer.h>

namespace CVar {

    class CVAR_API JSONSerializer : public ISerializer<std::unordered_map<String, Value>> {
        private:
            void _SerializeBeautified();
            void _SerializeCompact();

        public:
            JSONSerializer(std::ostream& _stream, std::unordered_map<String, Value>& _root);
            virtual void Serialize(bool bBeautified = true) override;
    };
}
