#include <iostream>
#include <string>

#include <EntityTraits.hpp>
#include <MemberSet.hpp>
#include <Bind.hpp>
#include <Validate.hpp>

#include "Account.h"

#ifdef EMSCRIPTEN
#include <binders/emscripten/Read.hpp>
#include <binders/emscripten/Write.hpp>
#include <binders/emscripten/ValidationErrors.hpp>
#include <emscripten/val.h>
using Read = typename nbdl::binders::emscripten::Read;
using Write = nbdl::binders::emscripten::Write;
using ValidationErrors = nbdl::binders::emscripten::ValidationErrors;
void fromString(std::string &json, Account &account)
{
	//todo create js object from json string
	emscripten::val emVal = emscripten::val::global("JSON").call<emscripten::val>("parse", json);
	Read r(emVal);
	nbdl::bind(r, account);
}

std::string toString(Account &account)
{
	Write r;

	nbdl::bind(r, account);
	return r.emVal.template as<std::string>();
}

std::string validate(Account &account)
{
	ValidationErrors e;

	nbdl::validate(e, account);
	return e.emVal.template as<std::string>();
}
#else //EMSCRIPTEN
#include <binders/JsonRead.h>
#include <binders/JsonWrite.h>
#include <binders/JsonCppErrorBinder.hpp>
#include <jsoncpp/json/json.h>
void fromString(std::string &json, Account &account)
{
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json, root, false))
		throw std::runtime_error("JSON parse error");
	nbdl::JsonRead r(root);
	nbdl::bind(r, account);
}

std::string toString(Account &account)
{
	Json::StyledWriter writer;
	Json::Value root;
	nbdl::JsonWrite r(root);

	nbdl::bind(r, account);
	return writer.write(root);
}

std::string validate(Account &account)
{
	Json::StyledWriter writer;
	Json::Value root;
	nbdl::binders::JsonCppValidationErrors e(root);

	nbdl::validate(e, account);
	return writer.write(root);
}
#endif //EMSCRIPTEN

int main()
{
	auto account = Account();
	std::string inputJson, outputJson;
	for (std::string line; std::getline(std::cin, line);)
		inputJson += line;

	fromString(inputJson, account);

	std::cout << validate(account);
	std::cout << toString(account);
	std::cout << std::endl;
	std::cout << +nbdl::MemberId<NBDL_MEMBER(&Account::nameFirst)>::value;
	std::cout << std::endl;
	std::cout << +nbdl::MemberId<NBDL_MEMBER(&Account::nameLast)>::value;
	std::cout << std::endl;
	std::cout << +nbdl::MemberId<NBDL_MEMBER(&Account::phoneNumber)>::value;
	std::cout << std::endl;
}

/*
path("/client/1/property/5/account/27");

path(Client{}, 1, Property{}, 5, Account{}, 27);

key(NBDL_MEMBER(&Account::id), 27);

client.path(Account{}, 1)
	.get(
		nbdl::cb::Done([&](Account account) {  }),
		nbdl::cb::NotFound([&]() { }),
		nbdl::cb::Fail([&]() {  }) 
	);

client.path(Account{}, 1)
	.listen(
		nbdl::cb::Diff([&](nbdl::Diff<Account> accountDiff) {  }),
		//below is only for the initial handshake
		nbdl::cb::NotFound([&]() { }),
		nbdl::cb::Fail([&]() {  }) 
	);
*/
