#ifndef _DPCB_WRAPPER_HPP_
#define _DPCB_WRAPPER_HPP_

#include <utility>
#include "misc.hpp"

namespace DPCB{

template<typename ...Ts>
class wrapper_any
{
};

template<std::size_t, typename T>
class wrapper_label : T
{
public:
	using type = T;
};

template<auto F>
class wrapper_function
{
	template<typename> class inner;

	template<class Cls, typename Ret, typename... Args>
	class inner<Ret(Cls::*)(Args...)>
	{
	public:
		using type_func = Ret(Args...);

		static auto bind(Cls *obj)
		{
			return [=](Args ...args){
				return (obj->*F)(std::forward<Args>(args)...);
			};
		}
	};

public:
	using type_func = typename inner<decltype(F)>::type_func;
	static const constexpr auto bind = inner<decltype(F)>::bind;
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