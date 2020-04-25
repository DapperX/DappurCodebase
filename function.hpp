#ifndef _DPCB_FUNCTION_HPP_
#define _DPCB_FUNCTION_HPP_

#include <type_traits>
#include <utility>
#include <functional>
#include "wrapper.hpp"

namespace DPCB{

class function_category{};
class function_category_normal : function_category{};
class function_category_method : function_category{};
class function_category_obj : function_category_method{};

template<class Category, class Cls, typename Ret, typename ...Args>
class function_trait_interface
{
	static_assert(std::is_class<Cls>::value||std::is_same<Cls,void>::value);
	static_assert(std::is_base_of<function_category, Category>::value);
public:
	using category = Category;
	using type_class = Cls;
	using type_return = Ret;
	using type_argument = wrapper_any<Args...>;
};

template<typename T>
class function_trait_cast
{
	template<typename... Args>
	static auto match_trait(function_trait_interface<Args...>)
		-> function_trait_interface<Args...>;

	template<typename Ret, typename ...Args>
	static auto match_callable(std::function<Ret(Args...)>)
		-> function_trait_interface<function_category_obj, T, Ret, Args...>;

	template<class Callable>
	static auto match(Callable obj)
		-> decltype(match_callable(std::function(obj)));

	template<class Trait>
	static auto match(Trait obj)
		-> decltype(match_trait(obj));
public:
	using type = decltype(match(std::declval<T>()));
};

template<typename T>
class function_trait
	: public function_trait_cast<T>::type
{
};

template<class Cls, typename Ret, typename ...Args>
class function_trait<Ret(Cls::*)(Args...)>
	: public function_trait_interface<function_category_method, Cls, Ret, Args...>
{
};

template<typename Ret, typename ...Args>
class function_trait<Ret(Args...)>
	: public function_trait_interface<function_category_normal, void, Ret, Args...>
{
};


template<auto F>
class bind_this
{
	using trait = function_trait<decltype(F)>;
	static_assert(
		std::is_base_of<
			function_category_method,
			typename trait::category
		>::value,
		"Expect to bind a class method"
	);

	template<class = typename trait::type_argument> class impl;

	template<typename ...Args>
	class impl<wrapper_any<Args...>>
	{
	public:
		using type = typename trait::type_return(Args...);

		static auto bind(typename trait::type_class *obj)
		{
			return [=](Args ...args) -> typename trait::type_return{
				return (obj->*F)(std::forward<Args>(args)...);
			};
		}
	};

public:
	using type_function = typename impl<>::type;
	static const constexpr auto bind = impl<>::bind;
};

}

#endif //_DPCB_FUNCTION_HPP_