//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/def/builder/access_point_meta.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

int main()
{
  using builder::access_point_meta;
  constexpr auto name           = hana::int_c<0>;
  constexpr auto actions        = hana::int_c<1>;
  constexpr auto store          = hana::int_c<2>;
  constexpr auto entities       = hana::int_c<4>;
  constexpr auto path           = hana::int_c<5>;
  constexpr auto listen_paths   = hana::int_c<6>;

  constexpr auto x = builder::make_access_point_meta(
    name
  , actions
  , store
  , entities
  , path
  , listen_paths
  );
  BOOST_HANA_CONSTANT_ASSERT(access_point_meta::name(x) == name);
  BOOST_HANA_CONSTANT_ASSERT(access_point_meta::actions(x) == actions);
  BOOST_HANA_CONSTANT_ASSERT(access_point_meta::store(x) == store);
  BOOST_HANA_CONSTANT_ASSERT(access_point_meta::entities(x) == entities);
  BOOST_HANA_CONSTANT_ASSERT(access_point_meta::path(x) == path);
  BOOST_HANA_CONSTANT_ASSERT(access_point_meta::listen_paths(x) == listen_paths);

  BOOST_HANA_CONSTANT_ASSERT(hana::equal(x,
    builder::make_access_point_meta_with_map(
      access_point_meta::name = name
    , access_point_meta::actions = actions
    , access_point_meta::store = store
    , access_point_meta::entities = entities
    , access_point_meta::path = path
    , access_point_meta::listen_paths = listen_paths
    )
  ));
  BOOST_HANA_CONSTANT_ASSERT(hana::equal(x,
    builder::make_access_point_meta_with_map(
      access_point_meta::actions = actions
    , access_point_meta::name = name
    , access_point_meta::store = store
    , access_point_meta::path = path
    , access_point_meta::entities = entities
    , access_point_meta::listen_paths = listen_paths
    )
  ));
}
