// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: CVarTypes.cpp - CVar types definition source file
// author: Karl-Mihkel Ott

#include <stack>
#include <ostream>
#include <cvar/CVarTypes.h>

namespace CVar {
    std::ostream& operator<<(std::ostream& _stream, List& _list) {
        std::stack<List*> stckList;
		stckList.push(&_list);

		while (!stckList.empty()) {
			_stream << '[';
			auto pList = stckList.top();

			for (auto it = pList->Begin(); it != pList->End(); it++) {
				switch (it->index()) {
                    case Type_Bool:
						_stream << (std::get<Type_Bool>(*it) ? "true" : "false");
						break;

					case Type_Float:
						_stream << std::get<Type_Float>(*it);
						break;

					case Type_Int:
						_stream << std::get<Type_Int>(*it);
						break;

					case Type_List:
						_stream << '[';
						stckList.push(std::get<Type_List>(*it).get());
						break;

					case Type_Object:
						_stream << *std::get<Type_Object>(*it).get();
						break;

					case Type_String:
						_stream << '\"' << std::get<Type_String>(*it) << '\"';
						break;

					default:
						break;
				}

				if (it != (_list.End() - 1))
					_stream << ", ";
				else _stream << ']';
			}

			stckList.pop();
			if (!stckList.empty())
				_stream << ", ";
		}

		return _stream;
    }

    std::ostream& operator<<(std::ostream& _stream, Object& _obj) {
        std::stack<std::pair<Object*, std::unordered_map<String, ValueDescriptor>::iterator>> stckObjects;
		stckObjects.push(std::make_pair(&_obj, _obj.GetContents().begin()));

		_stream << '{';
		while (!stckObjects.empty()) {
			BEGIN:
			auto& obj = stckObjects.top();

			for (auto it = obj.second; it != obj.first->GetContents().end(); it++) {
				_stream << '\"' << it->first << "\": ";
				switch (it->second.val.index()) {
					case Type_Bool:
						_stream << (std::get<Type_Bool>(it->second.val) ? "true" : "false");
						break;

					case Type_Float:
						_stream << std::get<Type_Float>(it->second.val);
						break;

					case Type_Int:
						_stream << std::get<Type_Int>(it->second.val);
						break;

					case Type_List:
						_stream << std::get<Type_List>(it->second.val);
						break;

					case Type_Object:
					{
						_stream << '{';
						auto pObject = std::get<Type_Object>(it->second.val).get();
						it++;
						obj.second = it;
						stckObjects.push(std::make_pair(pObject, pObject->GetContents().begin()));
						goto BEGIN;
					}

					case Type_String:
						_stream << '\"' << std::get<Type_String>(it->second.val) << '\"';
						break;

					default:
						break;
				}

				auto it2 = obj.first->GetContents().end();
				if (std::next(it) != it2)
					_stream << ", ";
			}
			
			stckObjects.pop();

            if (stckObjects.size() >= 1 && stckObjects.top().second != stckObjects.top().first->GetContents().end())
                _stream << "}, ";
            else _stream << '}';

		}

		return _stream;
    }
}
