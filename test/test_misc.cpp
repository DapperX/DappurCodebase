#include <type_traits>
#include "gtest/gtest.h"
#include "wrapper.hpp"
#include "misc.hpp"

template<class>
struct reverse_wrapper_any;

template<typename ...Ts>
struct reverse_wrapper_any<DPCB::wrapper_any<Ts...>>
{
	using type = typename DPCB::reverse_pack<DPCB::wrapper_any<>, Ts...>::type;
};

TEST(TestMisc, ReversePack)
{
	using T_normal = DPCB::wrapper_any<int, double, bool, float>;
	using T_reverse = typename reverse_wrapper_any<T_normal>::type;
	using T_expect = DPCB::wrapper_any<float, bool, double, int>;

	EXPECT_TRUE((std::is_same<T_reverse,T_expect>::value))
		<< "expect: " << typeid(T_expect).name() << '\n'
		<< "actual: " << typeid(T_reverse).name();
}