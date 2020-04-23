#include <string>
#include "gtest/gtest.h"
#include "count_member.hpp"

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

TEST(TestCountMember, All)
{
	EXPECT_EQ(DPCB::count_member<struct_empty>::value, 0);
	EXPECT_EQ(DPCB::count_member<struct_single>::value, 1);
	EXPECT_EQ(DPCB::count_member<struct_normal>::value, 5);
	EXPECT_EQ(DPCB::count_member<struct_large>::value, 5);
}