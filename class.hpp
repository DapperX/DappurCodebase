#ifndef _DPCB_COUNT_MEMBER_HPP_
#define _DPCB_COUNT_MEMBER_HPP_

#include <cstdint>
#include <utility>
#include <type_traits>
#include "misc.hpp"
#include "wrapper.hpp"

namespace DPCB{

/*
	Inspired by 'https://www.zhihu.com/question/359629067/answer/927022588'
	Calculate the number of members by determining the maximum sizeof...(Args) where T{Args()...} is valid.
	Involve binary search to reduce the recursion depth.
*/
template<class T, std::size_t LB, std::size_t Len>
class count_member_impl
{
	template<class U, std::size_t... I>
	static auto match(std::index_sequence<I...>)
		-> decltype(
			(void)(U{{(I,std::declval<any_convertor>())}...}),
			std::true_type()
		);

	template<class U>
	static auto match(...) -> std::false_type;
public:
	static constexpr const std::size_t value =
		std::conditional_t<
			decltype(match<T>(std::make_index_sequence<LB+Len/2>()))::value,
			count_member_impl<T, LB+Len/2, Len-Len/2>,
			count_member_impl<T, LB, Len/2>
		>::value;
};

template<class T, std::size_t LB>
class count_member_impl<T, LB, 1>
{
public:
	static constexpr const std::size_t value = LB;
};

template<class T>
class count_member : public count_member_impl<T, 0, sizeof(T)+1>
{
	static_assert(std::is_class<T>::value, "'count_member' requires a class type");
};


template<typename ...Ts>
class multibase
{
	template<typename> class inner;

	template<std::size_t ...I>
	class inner<std::index_sequence<I...>>
	{
	public:
		using type = wrapper_base<
			wrapper_label<std::integral_constant<std::size_t,I>, Ts>...
		>;
	};

public:
	using type = typename inner<std::index_sequence_for<Ts...>>::type;
};

}

#endif //_DPCB_COUNT_MEMBER_HPP_
