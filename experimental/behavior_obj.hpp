#ifndef _DPCB_EXP_BEHAVIOR_V_HPP_
#define _DPCB_EXP_BEHAVIOR_V_HPP_

#include <cstdint>
#include <utility>
#include "behavior.hpp"

namespace DPCB::experimental{

namespace detail{

template<class T>
class wrapper_nonempty : public T
{
	char _; // placeholder to make itself not empty
};

} // namespace detail

template<class ...IBs>
class multibehavior_v :
	virtual public detail::wrapper_nonempty<IBs>...
{
};

template<class ...IBs>
class behavior_obj
{
protected:
	intptr_t arena[1+sizeof...(IBs)+2];

	template<class D, std::size_t ...Is>
	behavior_obj(const D *ptr_src, std::index_sequence<Is...>)
	{
		// assign vtable pointer
		intptr_t** ptr_obj = (intptr_t**)&arena[0];
		intptr_t* vtable = &arena[1+sizeof...(IBs)+2];
		*ptr_obj = vtable;

		// lack of RTTI information
		// vtable[-1] = ?;
		// vtable[-2] = ?;

		// fill the correct offset to each behavior
		intptr_t* vbase = &vtable[-3];
		((void)(
			vbase[-Is] = (intptr_t)static_cast<const IBs*>(ptr_src) - (intptr_t)ptr_obj
		), ...);
	}

	template<class D>
	behavior_obj(const D *ptr_src) :
		behavior_obj(ptr_src, std::make_index_sequence<sizeof...(IBs)>())
	{
		static_assert(std::conjunction<std::is_base_of<IBs,D>...>::value,
			"Fail to match all behavior. Check whether the converted type owns them");
	}

	void* get_this()
	{
		return (void*)&arena;
	}
	const void* get_this() const
	{
		return (const void*)&arena;
	}

	template<class ...FIBs, class D>
	friend behavior_obj<FIBs...> make_behavior_obj(D*);

	template<class ...FIBs, class D>
	friend const behavior_obj<FIBs...> make_behavior_obj(const D*);

public:
	behavior_obj(const behavior_obj &other)
	{
		intptr_t** ptr_obj = (intptr_t**)&arena[0];
		intptr_t* vtable = &arena[1+sizeof...(IBs)+2];
		*ptr_obj = vtable;

		intptr_t offset = (intptr_t)other.get_this()-(intptr_t)ptr_obj;
		// skip the arena[0] as it represents the vtable pointer
		for(std::size_t i=1; i<=sizeof...(IBs); ++i)
			arena[i] = other.arena[i] + offset;
	}

	operator multibehavior_v<IBs...>*()
	{
		return reinterpret_cast<multibehavior_v<IBs...>*>(get_this());
	}

	operator const multibehavior_v<IBs...>*() const
	{
		return reinterpret_cast<const multibehavior_v<IBs...>*>(get_this());
	}

	multibehavior_v<IBs...>& operator*()
	{
		return *reinterpret_cast<multibehavior_v<IBs...>*>(get_this());
	}

	const multibehavior_v<IBs...>& operator*() const
	{
		return *reinterpret_cast<const multibehavior_v<IBs...>*>(get_this());
	}

	multibehavior_v<IBs...>* operator->()
	{
		return reinterpret_cast<multibehavior_v<IBs...>*>(get_this());
	}

	multibehavior_v<IBs...>* operator->() const
	{
		return reinterpret_cast<const multibehavior_v<IBs...>*>(get_this());
	}
};

template<class ...IBs, class D>
inline const behavior_obj<IBs...> make_behavior_obj(const D *ptr_src)
{
	return behavior_obj<IBs...>(ptr_src);
}

template<class ...IBs, class D>
inline behavior_obj<IBs...> make_behavior_obj(D *ptr_src)
{
	return behavior_obj<IBs...>(ptr_src);
}

}

#endif //_DPCB_EXP_BEHAVIOR_V_HPP_