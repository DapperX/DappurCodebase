#ifndef _DPCB_WRAPPER_HPP_
#define _DPCB_WRAPPER_HPP_

#include <utility>
#include <tuple>
#include "misc.hpp"

namespace DPCB{

template<typename ...Ts>
class wrapper_any
{
public:
	template<std::size_t Index>
	using type = typename retrieve_type<Index, Ts...>::type;
};

template<>
class wrapper_any<>
{
};


template<std::size_t, typename T>
class wrapper_label : public T
{
public:
	using type = T;
};


template<typename ...Ts>
class wrapper_base : public Ts...
{
public:
	template<std::size_t Index>
	using get_base = typename retrieve_type<Index, Ts...>::type;
};

template<class T>
class wrapper_construct_from_tuple : public T
{
public:
	template<std::size_t ...I, class Tuple>
	wrapper_construct_from_tuple(std::index_sequence<I...>, Tuple &&tp)
		: T(std::get<I>(std::forward<Tuple>(tp))...)
	{
	}

	template<class Tuple>
	wrapper_construct_from_tuple(Tuple &&tp) :
		wrapper_construct_from_tuple(
			std::make_index_sequence<
				std::tuple_size_v<std::remove_reference_t<Tuple>>
			>(),
			std::forward<Tuple>(tp)
		)
	{
	}
};

}

#endif //_DPCB_WRAPPER_HPP_