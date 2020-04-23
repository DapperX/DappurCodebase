#include <iostream>
#include <functional>
#include "gtest/gtest.h"
#include "wrapper.hpp"

class MyClass
{
public:
	bool f(){return true;}
};

bool f(){return true;}

TEST(TestWrapper, WrapperFunction)
{
	MyClass mc;
	auto g = DPCB::wrapper_function<&MyClass::f>::bind(&mc);
	EXPECT_TRUE(g());
	using func_t = typename DPCB::wrapper_function<&MyClass::f>::type_func;
	std::function<func_t> h = f;
	EXPECT_TRUE(h());
}