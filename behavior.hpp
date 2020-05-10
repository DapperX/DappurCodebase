#ifndef _DPCB_BEHAVIOR_HPP_
#define _DPCB_BEHAVIOR_HPP_

#include <cstdint>
#include <type_traits>
#include <typeinfo>
#include <tuple>
#include "misc.hpp"
#include "wrapper.hpp"

namespace DPCB{

template<class D>
class behavior
{
protected:
	D* that(){ return static_cast<D*>(this); }
	const D* that() const{ return static_cast<const D*>(this); }
};


namespace detail{

template<template<class> class B, class D>
class match_behavior_impl
{
	template<typename BT>
	static B<BT> match(B<BT>&&);
	static void match(...);
public:
	using type = decltype(match(std::declval<D>()));
};

template<template<class> class B, class D,
	class Cond = typename match_behavior_impl<B,D>::type>
class try_match_behavior_impl :
	public std::true_type
{
};

template<template<class> class B, class D>
class try_match_behavior_impl<B, D, void> :
	public std::false_type
{
};

} // namespace detail

template<template<class> class B, class D>
using match_behavior = std::enable_if_t<
	!std::is_void<typename detail::match_behavior_impl<B, D>::type>::value,
	detail::match_behavior_impl<B, D>
>;

template<template<class> class B, class D>
using try_match_behavior = detail::try_match_behavior_impl<B, D>;


namespace detail{

template<class D, class IBsw, template<class> class ...Bs>
class multibehavior_impl;

template<class D, class IB, class ...IBs, template<class> class B, template<class> class ...Bs>
class multibehavior_impl<D, wrapper_any<IB,IBs...>, B, Bs...> :
	public std::conditional<
		std::is_same<IB, typename match_behavior<B,D>::type>::value,
		IB,
		detail::padding<sizeof(IB)>
	>::type,
	public std::conditional<
		std::is_same<IB, typename match_behavior<B,D>::type>::value,
		multibehavior_impl<D, wrapper_any<IBs...>, Bs...>,
		multibehavior_impl<D, wrapper_any<IBs...>, B, Bs...>
	>::type
{
	static_assert(try_match_behavior<B,D>::value,
		"Fail to match behavior. Check whether the converted type owns it");
};

template<class D, class IBsw>
class multibehavior_impl<D, IBsw>
{
};

} // namespace detail


template<class D, template<class> class ...Bs>
class multibehavior :
	public detail::multibehavior_impl<D, typename D::__behavior, Bs...>
{
};


namespace detail{

template<class Any, template<class> class ...Bs>
class assembly_impl;

template<class ...IBs>
class assembly_impl<wrapper_any<IBs...>>:
	public wrapper_construct_from_tuple<IBs>...
{
	template<int> class tag;
public:
	template<class ...Tuples, tag<0>(*)=std::enable_if_t<(sizeof...(IBs)==sizeof...(Tuples)), std::nullptr_t>()>
	assembly_impl(Tuples &&...tps) :
		wrapper_construct_from_tuple<IBs>(tps)...
	{
	}

	template<std::size_t ...I, class ...Tuples>
	assembly_impl(std::index_sequence<I...>, Tuples &&...tps) :
		assembly_impl(
			std::forward<Tuples>(tps)...,
			((void)I, std::tuple())...
		)
	{
	}

	template<class ...Tuples, tag<1>(*)=std::enable_if_t<(sizeof...(IBs)>sizeof...(Tuples)), std::nullptr_t>()>
	assembly_impl(Tuples &&...tps) :
		assembly_impl(
			std::make_index_sequence<sizeof...(IBs)-sizeof...(Tuples)>(),
			std::forward<Tuples>(tps)...
		)
	{
	}

	using __behavior = wrapper_any<IBs...>;

	template<class D, template<class> class ...Bs>
	operator multibehavior<D,Bs...>&()
	{
		static_assert(std::is_base_of<assembly_impl,D>::value,
			"Try to convert to a different owner of the behavior");
		static_assert(std::conjunction<try_match_behavior<Bs,D>...>::value,
			"Fail to match all behavior. Check whether the converted type owns them");
		return *reinterpret_cast<multibehavior<D,Bs...>*>(this);
	}

	template<class D, template<class> class ...Bs>
	operator const multibehavior<D,Bs...>&() const
	{
		static_assert(std::is_base_of<assembly_impl,D>::value,
			"Try to convert to a different owner of the behavior");
		static_assert(std::conjunction<try_match_behavior<Bs,D>...>::value,
			"Fail to match all behavior. Check whether the converted type owns them");
		return *reinterpret_cast<const multibehavior<D,Bs...>*>(this);
	}
};

template<class ...Frds, template<class> class B, template<class> class ...Bs>
class assembly_impl<wrapper_any<Frds...>, B, Bs...> :
	public assembly_impl<wrapper_any<Frds...,B<assembly_impl<wrapper_any<Frds...>,B,Bs...>>>, Bs...>
{
	friend B<assembly_impl>;
public:
	using assembly_impl<wrapper_any<Frds...,B<assembly_impl<wrapper_any<Frds...>,B,Bs...>>>, Bs...>::assembly_impl;
};

} // namespace detail

template<template<class> class ...Bs>
using assembly = detail::assembly_impl<wrapper_any<>, Bs...>;


namespace detail{

template<class ...IBs>
inline auto behavior_upcast(assembly_impl<wrapper_any<IBs...>> *pa)
{
	return pa;
}

template<class ...IBs>
inline auto behavior_upcast(const assembly_impl<wrapper_any<IBs...>> *pa)
{
	return pa;
}

} // namespace detail

template<template<class> class ...Bs, class D, class MB=multibehavior<D,Bs...>>
inline MB& behavior_cast(D &d)
{
	return static_cast<MB&>(d);
}

template<template<class> class ...Bs, class D, class MB=multibehavior<D,Bs...>>
inline const MB& behavior_cast(const D &d)
{
	return static_cast<const MB&>(d);
}

template<template<class> class ...Bs, class D, class MB=multibehavior<D,Bs...>>
inline MB* behavior_cast(D *pd)
{
	static_assert(std::conjunction<try_match_behavior<Bs,D>...>::value,
		"Fail to match all behavior. Check whether the converted type owns them");
	return reinterpret_cast<MB*>(detail::behavior_upcast(pd));
}

template<template<class> class ...Bs, class D, class MB=multibehavior<D,Bs...>>
inline const MB* behavior_cast(const D *pd)
{
	static_assert(std::conjunction<try_match_behavior<Bs,D>...>::value,
		"Fail to match all behavior. Check whether the converted type owns them");
	return reinterpret_cast<const MB*>(detail::behavior_upcast(pd));
}

}

#endif //_DPCB_BEHAVIOR_HPP_