//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_ENDPOINT_INFO_HPP
#define NBDL_CONCEPT_ENDPOINT_INFO_HPP

#include <nbdl/fwd/concept/EndpointInfo.hpp>
#include <nbdl/fwd/endpoint_open.hpp>

#include <boost/hana/core/default.hpp>
#include <boost/hana/core/tag_of.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct EndpointInfo
  {
    using Tag = typename hana::tag_of<T>::type;
    static constexpr bool value = !hana::is_default<nbdl::endpoint_open_impl<Tag>>::value;
  };
}

#endif
