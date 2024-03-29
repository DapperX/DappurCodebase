#ifndef _DPCB_WRAPPER_HPP_
#define _DPCB_WRAPPER_HPP_

#include <utility>
#include <tuple>

namespace DPCB{

namespace detail{

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


template<typename ...Ts>
class wrapper_any
{
protected:
	template<int Index, bool IsNeg=(Index<0)>
	class get_type
	{
	public:
		using type = typename detail::retrieve_type<Index, Ts...>::type;
	};

	template<int Index>
	class get_type<Index, true>
	{
	public:
		using type = typename detail::retrieve_type<Index+sizeof...(Ts), Ts...>::type;
	};

public:
	template<int Index>
	using type = typename get_type<Index>::type;
};

template<>
class wrapper_any<>
{
};


template<typename T>
class wrapper_class
{
	static_assert(!std::is_class_v<T>, "This wrapper does not apply to a class type");
public:
	T data;

	template<typename ...Args>
	wrapper_class(Args &&...args) : data(std::forward<Args>(args)...)
	{
	}

	operator T() const
	{
		return data;
	}
};


namespace detail{

template<class, class T>
class wrapper_label_impl : public T
{
public:
	using type = T;
	using T::T;
};

}

template<class Label, typename T>
using wrapper_label = detail::wrapper_label_impl<
	Label,
	std::conditional_t<std::is_class_v<T>, T, wrapper_class<T>>
>;


template<typename ...Ts>
class wrapper_base : public Ts...
{
public:
	template<std::size_t Index>
	using get_base = typename detail::retrieve_type<Index, Ts...>::type;
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