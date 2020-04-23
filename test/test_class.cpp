#include <string>
#include <type_traits>
#include <typeinfo>
#include "gtest/gtest.h"
#include "class.hpp"

struct struct_empty{};

struct struct_single{char x;};

struct struct_normal
{
	int a;
	char b;
	void *c;
	double d[10];
	std::string e;
};

struct struct_large
{
	int a;
	char b;
	void *c;
	double d[1000];
	std::string f[2000];
};

TEST(TestClass, CountMember)
{
	EXPECT_EQ(DPCB::count_member<struct_empty>::value, 0);
	EXPECT_EQ(DPCB::count_member<struct_single>::value, 1);
	EXPECT_EQ(DPCB::count_member<struct_normal>::value, 5);
	EXPECT_EQ(DPCB::count_member<struct_large>::value, 5);
}


struct B1{
	char data[3];
};

struct B2{
	char data[5];
};

struct UD : DPCB::multibase<B1, B2, B1>::type
{
};

TEST(TestClass, MultiBase)
{
	EXPECT_EQ(sizeof(UD), sizeof(B1)+sizeof(B2)+sizeof(B1));
	typedef UD::get_base<0>::type t0;
	EXPECT_TRUE((std::is_same<t0,B1>::value)) << typeid(t0).name();
	typedef UD::get_base<1>::type t1;
	EXPECT_TRUE((std::is_same<t1,B2>::value)) << typeid(t1).name();
	typedef UD::get_base<2>::type t2;
	EXPECT_TRUE((std::is_same<t2,B1>::value)) << typeid(t2).name();
}