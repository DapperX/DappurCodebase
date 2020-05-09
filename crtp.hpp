#ifndef _DPCB_CRTP_HPP_
#define _DPCB_CRTP_HPP_

#include <cstdint>
#include <type_traits>
#include <typeinfo>
#include <tuple>
#include "wrapper.hpp"

namespace DPCB{

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

}

#endif //_DPCB_CRTP_HPP_