#include <type_traits>
#include <typeinfo>
#include "gtest/gtest.h"
#include "wrapper.hpp"
#include "function.hpp"

struct MyClass
{
	bool method(int, float){return true;}
	static char smethod(short){return '\000';}
};

unsigned normal(double){return 0;}

struct function_like
	: DPCB::function_trait_interface<DPCB::function_category, void, void>
{
};

TEST(TestFunction, FunctionTrait)
{	
	using trait_method = DPCB::function_trait<decltype(&MyClass::method)>;
	EXPECT_TRUE((std::is_same<trait_method::category,DPCB::function_category_method>::value))
		<< typeid(trait_method::category).name();
	EXPECT_TRUE((std::is_same<trait_method::type_class,MyClass>::value))
		<< typeid(trait_method::type_class).name();
	EXPECT_TRUE((std::is_same<trait_method::type_return,bool>::value))
		<< typeid(trait_method::type_return).name();
	EXPECT_TRUE((std::is_same<trait_method::type_argument,DPCB::wrapper_any<int,float>>::value))
		<< typeid(trait_method::type_argument).name();

	using trait_smethod = DPCB::function_trait<decltype(MyClass::smethod)>;
	EXPECT_TRUE((std::is_same<trait_smethod::category,DPCB::function_category_normal>::value))
		<< typeid(trait_smethod::category).name();
	EXPECT_TRUE((std::is_same<trait_smethod::type_class,void>::value))
		<< typeid(trait_smethod::type_class).name();
	EXPECT_TRUE((std::is_same<trait_smethod::type_return,char>::value))
		<< typeid(trait_smethod::type_return).name();
	EXPECT_TRUE((std::is_same<trait_smethod::type_argument,DPCB::wrapper_any<short>>::value))
		<< typeid(trait_smethod::type_argument).name();

	using trait_normal = DPCB::function_trait<decltype(normal)>;
	EXPECT_TRUE((std::is_same<trait_normal::category,DPCB::function_category_normal>::value))
		<< typeid(trait_normal::category).name();
	EXPECT_TRUE((std::is_same<trait_normal::type_class,void>::value))
		<< typeid(trait_normal::type_class).name();
	EXPECT_TRUE((std::is_same<trait_normal::type_return,unsigned>::value))
		<< typeid(trait_normal::type_return).name();
	EXPECT_TRUE((std::is_same<trait_normal::type_argument,DPCB::wrapper_any<double>>::value))
		<< typeid(trait_normal::type_argument).name();

	std::function func = normal;
	using trait_function = DPCB::function_trait<decltype(func)>;
	EXPECT_TRUE((std::is_same<trait_function::category,DPCB::function_category_obj>::value))
		<< typeid(trait_function::category).name();
	EXPECT_TRUE((std::is_same<trait_function::type_class,decltype(func)>::value))
		<< typeid(trait_function::type_class).name();
	EXPECT_TRUE((std::is_same<trait_function::type_return,unsigned>::value))
		<< typeid(trait_function::type_return).name();
	EXPECT_TRUE((std::is_same<trait_function::type_argument,DPCB::wrapper_any<double>>::value))
		<< typeid(trait_function::type_argument).name();

	unsigned short env = 1;
	auto lambda = [&](long x){return env*x;};
	using trait_lambda = DPCB::function_trait<decltype(lambda)>;
	EXPECT_TRUE((std::is_same<trait_lambda::category,DPCB::function_category_obj>::value))
		<< typeid(trait_lambda::category).name();
	EXPECT_TRUE((std::is_same<trait_lambda::type_class,decltype(lambda)>::value))
		<< typeid(trait_lambda::type_class).name();
	EXPECT_TRUE((std::is_same<trait_lambda::type_return,long>::value))
		<< typeid(trait_lambda::type_return).name();
	EXPECT_TRUE((std::is_same<trait_lambda::type_argument,DPCB::wrapper_any<long>>::value))
		<< typeid(trait_lambda::type_argument).name();

	using trait_derive = DPCB::function_trait<function_like>;
	EXPECT_TRUE((std::is_same<trait_derive::category,DPCB::function_category>::value))
		<< typeid(trait_derive::category).name();
	EXPECT_TRUE((std::is_same<trait_derive::type_class,void>::value))
		<< typeid(trait_derive::type_class).name();
	EXPECT_TRUE((std::is_same<trait_derive::type_return,void>::value))
		<< typeid(trait_derive::type_return).name();
	EXPECT_TRUE((std::is_same<trait_derive::type_argument,DPCB::wrapper_any<>>::value))
		<< typeid(trait_derive::type_argument).name();
}

class ClassToBind
{
public:
	bool f(){return true;}
};

bool f(){return true;}

TEST(TestWrapper, BindThis)
{
	ClassToBind mc;
	auto g = DPCB::bind_this<&ClassToBind::f>::bind(&mc);
	EXPECT_TRUE(g());
	using func_t = typename DPCB::bind_this<&ClassToBind::f>::type_function;
	std::function<func_t> h = f;
	EXPECT_TRUE(h());
}