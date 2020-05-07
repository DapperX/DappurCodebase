#include "gtest/gtest.h"
#include "wrapper.hpp"

TEST(TestWrapper, WrapperAny)
{
	using w = DPCB::wrapper_any<int, double, bool>;

	EXPECT_TRUE((std::is_same<w::type<0>,int>::value))
		<< "expect: " << typeid(int).name() << '\n'
		<< "actual: " << typeid(w::type<0>).name();

	EXPECT_TRUE((std::is_same<w::type<1>,double>::value))
		<< "expect: " << typeid(double).name() << '\n'
		<< "actual: " << typeid(w::type<1>).name();

	EXPECT_TRUE((std::is_same<w::type<2>,bool>::value))
		<< "expect: " << typeid(bool).name() << '\n'
		<< "actual: " << typeid(w::type<2>).name();

	EXPECT_TRUE((std::is_same<w::type<-3>,int>::value))
		<< "expect: " << typeid(int).name() << '\n'
		<< "actual: " << typeid(w::type<-3>).name();

	EXPECT_TRUE((std::is_same<w::type<-2>,double>::value))
		<< "expect: " << typeid(double).name() << '\n'
		<< "actual: " << typeid(w::type<-2>).name();

	EXPECT_TRUE((std::is_same<w::type<-1>,bool>::value))
		<< "expect: " << typeid(bool).name() << '\n'
		<< "actual: " << typeid(w::type<-1>).name();
}