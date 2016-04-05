//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/message_api.hpp>
#include <Uid.hpp> // FIXME

#include <boost/hana/experimental/types.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <catch.hpp>

namespace hana = boost::hana;
namespace hanax = boost::hana::experimental;
namespace message = nbdl::message;

using nbdl::message::channel::upstream;
using nbdl::message::channel::downstream;
using nbdl::message::action::create;
using nbdl::message::action::read;
using nbdl::message::action::update;
using nbdl::message::action::update_raw;
using nbdl::message::action::delete_;
using maybe_uid = hana::optional<nbdl::uid>;

template <typename T>
constexpr auto decay_c(T)
{
  return hana::type_c<std::decay_t<T>>;
}

template <int i> struct Path { int id; };
template <int i> struct Payload { int value; };
template <int i> using MaybePayload = hana::optional<Payload<i>>;

namespace boost { namespace hana {
  template <int i>
  struct equal_impl<Path<i>, Path<i>>
  {
    template <typename T1, typename T2>
    static constexpr auto apply(T1 const& t1, T2 const& t2)
    { return t1.id == t2.id; }
  };

  template <int i>
  struct equal_impl<Payload<i>, Payload<i>>
  {
    template <typename T1, typename T2>
    static constexpr auto apply(T1 const& t1, T2 const& t2)
    { return t1.value == t2.value; }
  };
}} // boost::hana

// TODO create a better way for making a message type for testing (ie named params)
template <int i>
using UpstreamCreate = hana::tuple<upstream, create, Path<i>, maybe_uid, MaybePayload<i>>;
template <int i>
using UpstreamRead = hana::tuple<upstream, read, Path<i>, maybe_uid>;
template <int i>
using UpstreamUpdate = hana::tuple<upstream, update, Path<i>, maybe_uid, MaybePayload<i>>;
template <int i>
using UpstreamUpdateRaw = hana::tuple<upstream, update_raw, Path<i>, maybe_uid, MaybePayload<i>>;
template <int i>
using UpstreamDelete = hana::tuple<upstream, delete_, Path<i>, maybe_uid>;
template <int i>
using DownstreamCreate = hana::tuple<downstream, create, Path<i>, maybe_uid,
  hana::optional<bool>, MaybePayload<i>>;
template <int i>
using DownstreamRead = hana::tuple<downstream, read, Path<i>, maybe_uid,
  hana::optional<bool>, MaybePayload<i>>;
template <int i>
using DownstreamUpdate = hana::tuple<downstream, update, Path<i>, maybe_uid,
  hana::optional<bool>, MaybePayload<i>>;
template <int i>
using DownstreamUpdateRaw = hana::tuple<downstream, update_raw, Path<i>, maybe_uid,
  hana::optional<bool>, MaybePayload<i>>;
template <int i>
using DownstreamDelete = hana::tuple<downstream, delete_, Path<i>, maybe_uid, hana::optional<bool>>;

using UpstreamMessages = hanax::types<
    UpstreamCreate<1>
  , UpstreamRead<1>
  , UpstreamCreate<2>
  , UpstreamUpdateRaw<2>
  , UpstreamCreate<3>
  , UpstreamRead<3>
  , UpstreamUpdate<3>
  , UpstreamDelete<3>
  >;
using DownstreamMessages = hanax::types<
    DownstreamCreate<1>
  , DownstreamRead<1>
  , DownstreamCreate<2>
  , DownstreamUpdateRaw<2>
  , DownstreamCreate<3>
  , DownstreamRead<3>
  , DownstreamUpdate<3>
  , DownstreamDelete<3>
  >;

constexpr nbdl::message_api<UpstreamMessages, DownstreamMessages> msgs{};

// Test the search for message types

BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream{}, create{}, Path<1>{1}) == hana::type_c<UpstreamCreate<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream{}, read{}, Path<1>{1}) == hana::type_c<UpstreamRead<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(downstream{}, create{}, Path<1>{1}) == hana::type_c<DownstreamCreate<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(downstream{}, read{}, Path<1>{1}) == hana::type_c<DownstreamRead<1>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream{}, update_raw{}, Path<2>{2}) == hana::type_c<UpstreamUpdateRaw<2>>
);
BOOST_HANA_CONSTANT_ASSERT(
  msgs.get_message_type(upstream{}, delete_{}, Path<3>{3}) == hana::type_c<UpstreamDelete<3>>
);

TEST_CASE("make_upstream_create_message", "[message_api]")
{
  auto m = msgs.make_upstream_create_message(Path<1>{1}, Payload<1>{11});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<create>);
  CHECK(hana::equal(message::get_path(m), Path<1>{1}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<1>{11}));
}

TEST_CASE("make_downstream_create_message", "[message_api]")
{
  auto m = msgs.make_downstream_create_message(Path<1>{1}, Payload<1>{11});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<create>);
  CHECK(hana::equal(message::get_path(m), Path<1>{1}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<1>{11}));
  CHECK(*message::get_maybe_is_from_root(m) == false);
}

TEST_CASE("make_upstream_read_message", "[message_api]")
{
  auto m = msgs.make_upstream_read_message(Path<1>{1});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<read>);
  CHECK(hana::equal(message::get_path(m), Path<1>{1}));
}

TEST_CASE("make_upstream_update_message", "[message_api]")
{
  auto m = msgs.make_upstream_update_message(Path<3>{3}, Payload<3>{33});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<update>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<3>{33}));
}

TEST_CASE("make_downstream_update_message", "[message_api]")
{
  auto m = msgs.make_downstream_update_message(Path<3>{3}, Payload<3>{33});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<update>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<3>{33}));
  CHECK(*message::get_maybe_is_from_root(m) == false);
}

TEST_CASE("make_upstream_update_raw_message", "[message_api]")
{
  auto m = msgs.make_upstream_update_raw_message(Path<2>{2}, Payload<2>{22});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<update_raw>);
  CHECK(hana::equal(message::get_path(m), Path<2>{2}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<2>{22}));
}

TEST_CASE("make_downstream_update_raw_message", "[message_api]")
{
  auto m = msgs.make_downstream_update_raw_message(Path<2>{2}, Payload<2>{22});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<update_raw>);
  CHECK(hana::equal(message::get_path(m), Path<2>{2}));
  CHECK(hana::equal(*message::get_maybe_payload(m), Payload<2>{22}));
  CHECK(*message::get_maybe_is_from_root(m) == false);
}

TEST_CASE("make_upstream_delete_message", "[message_api]")
{
  auto m = msgs.make_upstream_delete_message(Path<3>{3});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<upstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<delete_>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
}

TEST_CASE("make_downstream_delete_message", "[message_api]")
{
  auto m = msgs.make_downstream_delete_message(Path<3>{3});
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_channel(m)) == hana::type_c<downstream>);
  BOOST_HANA_CONSTANT_ASSERT(decay_c(message::get_action(m)) == hana::type_c<delete_>);
  CHECK(hana::equal(message::get_path(m), Path<3>{3}));
  CHECK(*message::get_maybe_is_from_root(m) == false);
}
