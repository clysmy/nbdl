#ifndef BINDER_HPP
#define BINDER_HPP

#include "Entity.hpp"

namespace nbdl {

template<class Impl>
class Binder
{
	public:

	template<typename T>
	typename std::enable_if<std::is_integral<T>>::type
		bindMember(const std::string name, T &field)
	{
		static_cast<Impl*>(this)->bind(name, field);
	}

	template<typename T>
	typename std::enable_if<std::is_base_of<EntityBase, T>::value>::type
		bindMember(const std::string name, T &field)
	{
		static_cast<Impl*>(this)->bindEntity(name, field);
	}
};

}//nbdl

#endif
