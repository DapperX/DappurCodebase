#ifndef _DPCB_MISC_HPP_
#define _DPCB_MISC_HPP_

#include <cstdint>
#include "wrapper.hpp"

namespace DPCB{

using detail::retrieve_type;

class any_convertor
{
public:
	template<typename T> constexpr operator T();
};


template<class Result, typename ...Ts>
class reverse_pack_impl;

template<class ...Rs, typename T, typename ...Ts>
class reverse_pack_impl<wrapper_any<Rs...>, T, Ts...>
{
public:
	using type = typename reverse_pack_impl<wrapper_any<T, Rs...>, Ts...>::type;
};

template<class ...Rs, typename T>
class reverse_pack_impl<wrapper_any<Rs...>, T>
{
public:
	using type = wrapper_any<T, Rs...>;
};

template<typename ...Ts>
using reverse_pack = reverse_pack_impl<wrapper_any<>, Ts...>;

namespace detail{

template<std::size_t N>
class padding{
	std::uint8_t __dummy_data[N];
};

template<>
class padding<0>{
};

}

}

#endif //_DPCB_MISC_HPP_