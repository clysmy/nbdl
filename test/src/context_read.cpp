//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<functional>
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
namespace nbdl {
	NBDL_ENTITY(
		Client,
			id);
	NBDL_ENTITY(
		MyEntity,
			id,
			client_id );
}//nbdl

using OnlySupportedPath = typename nbdl::CreatePath<Client, MyEntity>::Type;
static_assert(std::is_same<typename OnlySupportedPath::Entity, MyEntity>::value, "Only supported type entity is not MyEntity.");

struct TestClient
{
	template<typename Fn>
	void read(OnlySupportedPath path, Fn fn)
	{
		if (path.getKey<Client>() == 1 && path.getKey<MyEntity>() == 5)
		{
			MyEntity my_entity = { 5, 1 };
			fn(my_entity);
		}
		else
		{
			fn(nbdl::NotFound{});
		}
	}
};

using MyContext = nbdl::Context<
	TestClient,
	nbdl::ListenerHandlerDummy<std::weak_ptr<int>>,
	nbdl::ApiDefinition<
		nbdl::AccessPoint<
			OnlySupportedPath,
			nbdl::Actions<nbdl::actions::Read>
		>
	>>;

TEST_CASE("Read an object from a context.", "[context]") 
{
	bool result;
	MyContext ctx(TestClient{});

	result = ctx.read(OnlySupportedPath(1, 5),
		[](nbdl::Unresolved) {
			return false;
		},
		[](nbdl::NotFound) {
			return false;
		},
		[](MyEntity m) {
			return (m.id == 5 && m.client_id == 1);
		});

	CHECK(result);
}

TEST_CASE("Context should propagate NotFound from server callback.", "[context]") 
{
	bool result = false;
	MyContext ctx(TestClient{});

	result = ctx.read(OnlySupportedPath(1, 6),
		[](nbdl::Unresolved) {
			return false;
		},
		[](nbdl::NotFound) {
			return true;
		},
		[](MyEntity) {
			return false;
		});

	CHECK(result);
}

class TestClientAsync
{
	static std::function<void()> m_fn;

	public:

	static void flush__()
	{
		m_fn();
	}

	template<typename Fn>
	void read(OnlySupportedPath path, Fn fn)
	{
		m_fn = [&]() {
			if (path.getKey<Client>() == 1 && path.getKey<MyEntity>() == 5)
			{
				MyEntity my_entity = { 5, 1 };
				fn(my_entity);
			}
			else
			{
				fn(nbdl::NotFound{});
			}
		};
	}
};
std::function<void()> TestClientAsync::m_fn = std::function<void()>();

using MyContextAsync = nbdl::Context<
	TestClientAsync,
	nbdl::ListenerHandlerDummy<std::weak_ptr<int>>,
	nbdl::ApiDefinition<
		nbdl::AccessPoint<
			OnlySupportedPath,
			nbdl::Actions<nbdl::actions::Read>
		>
	>>;

TEST_CASE("Context should emit change to listener", "[context]")
{
	int result = 0;
	auto shared = std::make_shared<int>(5);
	auto ctx = std::make_shared<MyContextAsync>(TestClientAsync{});
	OnlySupportedPath path(1, 5);

	auto listener = ctx->makeListener(path, shared, [&]() {
		result = ctx->read(path,
			[](nbdl::Unresolved) {
				return 1;
			},
			[](MyEntity) {
				return 2;
			},
			[](nbdl::NotFound) {
				return 3;
			});
	});

	CHECK_FALSE(result);
	TestClientAsync::flush__();
	CHECK(result == 2);
}
