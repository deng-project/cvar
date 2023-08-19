// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: CVarSystem.h - CVar system class header file
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/Api.h>
#include <cvar/CVarTypes.h>
#include <fstream>

namespace CVar {

    class CVAR_API CVarSystem {
        private:
            std::unordered_map<String, ValueDescriptor> m_root;

        private:
            CVarSystem() = default;
            std::vector<String> _HashKeyWords(const std::string& _key);
            ValueDescriptor* _FindNode(const std::string& _key);

        public:
            static CVarSystem& GetInstance();

            template <typename T>
            void Serialize(const std::string& _sFileName) {
                std::ofstream stream(_sFileName);
                T serializer(stream, m_root);
                stream.close();
            }


            template <typename T>
            void Unserialize(const std::string& _sFileName) {
                std::ifstream stream(_sFileName);
                T unserializer(_sFileName);
                m_root = std::move(unserializer.GetRoot());
            }

            inline String* GetDescription(const std::string& _key) {
                ValueDescriptor* pDesc = _FindNode(_key);
                if (pDesc)
                    return &pDesc->description;

                return nullptr;
            }

            inline Value* GetValue(const std::string& _key) {
                ValueDescriptor* pDesc = _FindNode(_key);
                if (pDesc)
                    return &pDesc->val;
                return nullptr;
            }

            template <typename T>
            inline T* Get(const std::string& _key) {
                ValueDescriptor* pDesc = _FindNode(_key);
                if (pDesc)
                    return std::get_if<T>(&pDesc->val);
            }

            template <typename T>
            bool Set(const String& _key, const String& _description, const T& _val) {
                std::vector<String> cvarStrings = _HashKeyWords(_key);
                std::unordered_map<String, ValueDescriptor>* pTable = &m_root;

                for (size_t i = 0; i < cvarStrings.size() - 1; i++) {
                    if (pTable->find(cvarStrings[i]) == pTable->end()) {
                        pTable->insert(std::make_pair(cvarStrings[i], ValueDescriptor{ 
                            String(),
                            std::make_shared<Object>()
                            }));
                    }
                    auto pObject = std::get_if<std::shared_ptr<Object>>(&pTable->find(cvarStrings[i])->second.val);
                    if (!pObject) return false;
                    pTable = &pObject->get()->GetContents();
                }

                pTable->insert_or_assign(cvarStrings.back(), ValueDescriptor{ _description, _val });
                return true;
            }
    };
}
