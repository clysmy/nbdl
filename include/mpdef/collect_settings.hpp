//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_COLLECT_SETTINGS_HPP
#define NBDL_MPDEF_COLLECT_SETTINGS_HPP

#include <mpdef/map.hpp>
#include <mpdef/tree_node.hpp>

#include <boost/hana.hpp>

namespace mpdef {

namespace hana = boost::hana;

struct collect_settings_fn
{
  template<typename State>
  struct helper_helper
  {
    template<typename Key>
    static constexpr auto helper(Key key, hana::optional<>)
    {
      return State{}[key];
    }

    template<typename Key, typename T>
    static constexpr auto helper(Key, hana::optional<T>)
    {
      return hana::just(T{});
    }
  };
  
  template<typename State, typename X>
  struct helper_fn
  {
    template<typename... Key>
    constexpr auto operator()(Key... key) const
    {
      return mpdef::make_map(mpdef::make_tree_node(key,
        helper_helper<State>::helper(key, hana::find(X{}, key)))...);
    }
  };

  template<typename State, typename X>
  constexpr auto helper(State state, X, hana::true_) const
  {
    return hana::unpack(hana::keys(state), helper_fn<State, X>{});
  }

  template<typename State, typename X>
  constexpr auto helper(State state, X, hana::false_) const
  {
    return state;
  }

  template<typename State, typename X>
  constexpr auto operator()(State state, X x) const
  {
    auto children = hana::second(x);
    return helper(
      state,
      children,
      hana::is_a<mpdef::map_tag>(children)
    );
  }
};
constexpr collect_settings_fn collect_settings{};

template<typename... Tag>
constexpr auto with_settings(Tag&&... tag)
{
  return mpdef::make_map(mpdef::make_tree_node(std::forward<Tag>(tag), hana::nothing)...);
}

}//mpdef
#endif
