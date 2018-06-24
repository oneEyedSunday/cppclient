#include <cpprestsdk/http_client.h>
#include <cpprestsdk/ppltasks.h>
#include <cpprestsdk/json.h>


class Member {
	public:
		int id;
		std::wstring name, sport;

		void Display(){
			std::wcout << id << L", " << name << L", " << sport << std::endl;
		}
};

enum FieldValue {id, name, sport};

class MemberGenerator {
	std::map<std::wstring, FieldValue> fieldMap; // key is a wstring
	// value is of yype enum FieldValue
	Member member;

	public:
		MemberGenerator(){
			// constructor
			fieldMap[L"id"] = FieldValue::id;
			fieldMap[L"name"] = FieldValue::name;
			fieldMap[L"sport"] = FieldValue::sport;
		}


		void setField(std::wstring name, json::value value){
			switch(fieldMap[name]){
				case FieldValue::id:
					member.id = value.as_integer();
					break;
				case FieldValue::name:
					member.name = value.as_string();
					break;
				case FieldValue::sport:
					member.sport = value.as_string();
					break;
			}
		}

		Member getMemberFromJson(json::value jsonValue){
			for(auto iter = jsonValue.cbegin(); iter != jsonValue.cend(); ++iter){
				const json::value &propertyName = iter->first;
				const json::value &propertyValue = iter->second;

				setField(propertyName.as_string(), propertyValue);
			}

			return member
		}
};


