#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
// #include <string>


using namespace web; // for json
using namespace web::http; // for status_codes
using namespace web::http::client;

class Member {
	public:
		int id;
		std::string name, sport;

		void Display(){
			std::cout << id << ", " << name << ", " << sport << std::endl;
		}
};

enum FieldValue {id, name, sport};

class MemberGenerator {
	std::map<std::string, FieldValue> fieldMap; // key is a wstring
	// value is of yype enum FieldValue
	Member member;

	public:
		MemberGenerator(){
			// constructor
			fieldMap["id"] = FieldValue::id;
			fieldMap["name"] = FieldValue::name;
			fieldMap["sport"] = FieldValue::sport;
		}

		void setField(std::string name, json::value& value){
			switch(fieldMap[name]){
				case FieldValue::id:
					member.id = value.as_integer();
					break;
				case FieldValue::sport:
					member.sport = value.as_string();
					break;
				case FieldValue::name:
					member.name = value.as_string();
					break;
			}
		}

		Member getMemberFromJson(json::object obj){
			for (auto iterInner = obj.cbegin(); iterInner != obj.cend(); ++iterInner){
				auto propertyName = iterInner->first;
				auto propertyValue = iterInner->second;
				setField(propertyName, propertyValue);
			}
			return member;
		}
};


http_client myClient(U("http://localhost:3000"));
uri_builder builder(U("/api/values"));

pplx::task<void> GetAll();
pplx::task<void> Get(int);
pplx::task<int> Post();
pplx::task<void> Put(int);
pplx::task<void> Delete(int);

int main(){
	
	GetAll().then([]{
		std::cout << "Before calling Post()" << std::endl;
		Post().then([](int newId){
			std::cout << "After calling Post()" << std::endl;
			std:: cout << newId << std::endl;
			Get(newId).then([newId]{
				Put(newId).then([]{
					GetAll().then([]{
						std::cout << "All done!" << std::endl;
					}).wait();
				}).wait();
			}).wait();
		}).wait();
	}).wait();
	return 0;
}

pplx::task<void> GetAll()
{
	return pplx::create_task([]{
		
		// make a request

		// build request URI and start the request
		// uri_builder builder(U("/api/values"));
		auto reqBuilder = builder; //copy
		return myClient.request(methods::GET, reqBuilder.to_string());
	})
	.then([](http_response response){
		if(response.status_code() == status_codes::OK){
			// success
			std::cout << "Success" << std::endl;
			return response.extract_json();
		}

		// else?
		// error?
		// will this return a is_null that gets checked for later
		std::cout << "Failure?:" << std::endl;
		// apparently this returns a null value
		return pplx::create_task([]{ return json::value(); });
	}).then([](json::value value){
		// do checks for stuff like 404 errors 
		// receiving json object not arrays of objects
		// perform a check for what value is array or just object
		json::array jsonArray = value.as_array();
		// jsonValue is an array of values representing Memeber objects
		if(jsonArray.size() < 1) return;

		MemberGenerator generator;
		for(int i=0; i < jsonArray.size(); i++){
			auto data = jsonArray[i];
			auto dataObj = data.as_object();
			auto member = generator.getMemberFromJson(dataObj);
			member.Display();
		}
	});
}


pplx::task<void> Get(int id){
	return pplx::create_task([id]{
		uri uid(std::to_string(id));
		auto reqBuilder = builder;
		reqBuilder.append(uid);
		return myClient.request(methods::GET, reqBuilder.to_string());
	}).then([](http_response response){
		if(response.status_code() == status_codes::OK){
			return response.extract_json();
		// extract json is asycn
		}

		return pplx::create_task([]{ return json::value(); });
	})
	.then([](json::value jsonValue){
		if(jsonValue.is_null()) return;

		MemberGenerator generator;
		auto member = generator.getMemberFromJson(jsonValue.as_object());
		member.Display();
	});
}

pplx::task<int> Post(){
	// include support for interaction
	return pplx::create_task([]{
		auto reqBuilder = builder;
		json::value postData;
		postData["name"] = json::value::string("Joe Smith");
		postData["sport"] = json::value::string("Baseball");

		return myClient.request(methods::POST, reqBuilder.to_string(), postData.serialize().c_str(), "application/json");
	}).then([](http_response response){
		if (response.status_code() == status_codes::OK){
			auto body = response.extract_string();
			std::cout << "Added new Id: " << body.get().c_str() << std::endl;
			return std::stoi(body.get().c_str());
		}
		
		return 0;
	});
}


pplx::task<void> Put(int id){
	return pplx::create_task([id]{
		json::value postData;
		postData["name"] = json::value::string("Joe Y Smith");
		postData["sport"] = json::value::string("Baseball in Cuba");

		auto reqBuilder = builder;
		uri uid(std::to_string(id));
		reqBuilder.append(uid);
		return myClient.request(methods::PUT, reqBuilder.to_string(), postData.serialize().c_str(), "application/json");
	}).then([](http_response response){
		if(response.status_code() == status_codes::OK){
			auto body = response.extract_string();
			std::cout << "Updated: " << body.get().c_str() << std::endl;
		}
	});
}

pplx::task<void> Delete(int id){
	return pplx::create_task([id]{
		auto reqBuilder = builder;
		uri uuid(std::to_string(id));
		return myClient.request(methods::DEL, reqBuilder.to_string());
	}).then([](http_response response){
		if(response.status_code() == status_codes::OK){
			auto body = response.extract_string();
			std::cout << "Deleted: " << body.get().c_str() << std::endl;
		}
	});
}