#ifndef _DPCB_WRAPPER_HPP_
#define _DPCB_WRAPPER_HPP_

#include <utility>
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

}

#endif //_DPCB_WRAPPER_HPP_