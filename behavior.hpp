#ifndef _DPCB_CRTP_HPP_
#define _DPCB_CRTP_HPP_

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

template<template<class> class B, class D>
class match_behavior
{
	template<typename BT>
	static B<BT> match(B<BT>&&);
public:
	using type = decltype(match(std::declval<D>()));
};

template<template<class> class B, class D, class Cond=std::void_t<>>
class try_match_behavior_impl :
	public std::false_type
{
};

template<template<class> class B, class D>
class try_match_behavior_impl<B, D, std::void_t<typename match_behavior<B,D>::type>> :
	public std::true_type
{
};

template<template<class> class B, class D>
using try_match_behavior = try_match_behavior_impl<B, D>;


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
	static_assert(try_match_behavior<B,D>::value);
};

template<class D, class IBsw>
class multibehavior_impl<D, IBsw>
{
};


template<class D, template<class> class ...Bs>
class multibehavior :
	public multibehavior_impl<D, typename D::__behavior, Bs...>
{
};


template<class Any, template<class> class ...Bs>
class assembly_impl;

template<class ...IBs>
class assembly_impl<wrapper_any<IBs...>>:
	public wrapper_construct_from_tuple<IBs>...
{
	template<int> class tag{};
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
		static_assert(std::is_base_of<assembly_impl,D>::value);
		// static_assert(std::conjunction<try_match_behavior<Bs,D>...>::value);
		return *reinterpret_cast<multibehavior<D,Bs...>*>(this);
	}

	template<class D, template<class> class ...Bs>
	operator const multibehavior<D,Bs...>&() const
	{
		static_assert(std::is_base_of<assembly_impl,D>::value);
		// static_assert(std::conjunction<try_match_behavior<Bs,D>...>::value);
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

template<template<class> class ...Bs>
using assembly = assembly_impl<wrapper_any<>, Bs...>;


namespace detail{

template<class ...IBs>
inline auto behavior_downcast(assembly_impl<wrapper_any<IBs...>> *pa)
{
	return pa;
}

template<class ...IBs>
inline auto behavior_downcast(const assembly_impl<wrapper_any<IBs...>> *pa)
{
	return pa;
}

}

template<template<class> class ...Bs, class D>
inline multibehavior<D,Bs...>& behavior_cast(D &d)
{
	return static_cast<multibehavior<D,Bs...>&>(d);
}

template<template<class> class ...Bs, class D>
inline const multibehavior<D,Bs...>& behavior_cast(const D &d)
{
	return static_cast<const multibehavior<D,Bs...>&>(d);
}

template<template<class> class ...Bs, class D>
inline multibehavior<D,Bs...>* behavior_cast(D *pd)
{
	static_assert(std::conjunction<try_match_behavior<Bs,D>...>::value);
	return reinterpret_cast<multibehavior<D,Bs...>*>(detail::behavior_downcast(pd));
}

template<template<class> class ...Bs, class D>
inline const multibehavior<D,Bs...>* behavior_cast(const D *pd)
{
	static_assert(std::conjunction<try_match_behavior<Bs,D>...>::value);
	return reinterpret_cast<const multibehavior<D,Bs...>*>(detail::behavior_downcast(pd));
}

}

#endif //_DPCB_CRTP_HPP_