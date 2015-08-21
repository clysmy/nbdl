#include<nbdl>
#include "catch.hpp"

struct Client
{
	int id;
};
struct MyEntity
{
	int id;
	int client_id;
};
using ClientPath = nbdl::Path<Client, int>;
using MyEntityPath = nbdl::Path<MyEntity, int, ClientPath>;
namespace nbdl {
	NBDL_ENTITY(
		Client,
			id);
	NBDL_ENTITY(
		MyEntity,
			id,
			client_id);
}//nbdl

TEST_CASE("Access an uninitialized value from a store.", "[store]") 
{
	nbdl::Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(5, ClientPath(1));
	bool did_make_request = false;

	bool result = store.get([&](MyEntityPath ) { did_make_request = true; }, path,
		[](nbdl::Unresolved) {
			return true;
		},
		[]() {
			return false;
		});

	CHECK(result == true);
	CHECK(did_make_request == true);
}
TEST_CASE("Force assign and access a value from a store.", "[store]") 
{
	nbdl::Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(5, ClientPath(1));
	MyEntity my_entity = { 5, 1 };
	bool did_make_request = false;
	store.forceAssign(path, my_entity);

	bool result = store.get(
		[&](MyEntityPath ) { 
			did_make_request = true; 
		}, 
		path,
		[](MyEntity entity) {
			CHECK(entity.id == 5);
			return true;	
		},
		[]() {
			return false;
		});

	CHECK(result == true);
	CHECK(did_make_request == false);
}

TEST_CASE("Suggest a value to a store.", "[store]") 
{
	nbdl::Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(5, ClientPath(1));
	MyEntity my_entity = { 5, 1 };
	bool did_make_request = false;
	store.suggestAssign(path, my_entity);

	bool result = store.get(
		[&](MyEntityPath ) { 
			did_make_request = true; 
		}, 
		path,
		[](MyEntity entity) {
			CHECK(entity.id == 5);
			return true;	
		},
		[]() {
			return false;
		});

	CHECK(result);
	CHECK_FALSE(did_make_request);
	
}

TEST_CASE("Suggest a value to a store where the value already exists.", "[store]") 
{
	nbdl::Store<MyEntityPath> store;
	MyEntityPath path = MyEntityPath(5, ClientPath(1));
	MyEntity my_entity_original = { 6, 1 };
	MyEntity my_entity = { 5, 1 };
	bool did_make_request = false;

	store.forceAssign(path, my_entity_original);

	store.suggestAssign(path, my_entity);

	bool result = store.get(
		[&](MyEntityPath) { 
			did_make_request = true; 
		}, 
		path,
		[](MyEntity entity) {
			CHECK(entity.id == 6);
			return true;
		},
		[]() {
			return false;
		});

	CHECK(result);
	CHECK_FALSE(did_make_request);
}
