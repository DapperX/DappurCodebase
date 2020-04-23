#ifndef _DPCB_MISC_HPP_
#define _DPCB_MISC_HPP_

#include <cstdint>

namespace DPCB{

class any_convertor
{
public:
	template<typename T> constexpr operator T();
};


template<std::size_t I, class Head, class ...Rs>
class retrieve_type
{
public:
	using type = typename retrieve_type<I-1, Rs...>::type;
};

template<class Head, class ...Rs>
class retrieve_type<0, Head, Rs...>
{
public:
	using type = Head;
};

}

#endif //_DPCB_MISC_HPP_