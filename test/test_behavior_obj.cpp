#include "gtest/gtest.h"
#include "experimental/behavior_obj.hpp"

using DPCB::experimental::behavior_v;
using DPCB::experimental::assembly_v;
using DPCB::experimental::multibehavior_v;

struct behavior_VA
{
	// int something;
	virtual int f() const = 0;
	virtual int f() = 0;
};

struct behavior_VB
{
	int y;
	behavior_VB(int y_):y(y_){}
	virtual int g() const{
		return y*100;
	}
	virtual int g(){
		return y;
	}
};

struct behavior_VC
{
	int z;
	behavior_VC(int z_):z(z_){}
	virtual int h() const = 0;
	virtual int h() = 0;
};

template<class D>
struct behavior_IA : behavior_v<D>, behavior_VA
{
	using behavior_v<D>::that;

	int x;
	behavior_IA(int x_):x(x_){}
	int f() const override{
		return that()->z*100;
	}
	int f() override{
		return that()->z;
	}
};

template<class D>
struct behavior_IB : behavior_VB
{
	using behavior_VB::behavior_VB;
};

template<class D>
struct behavior_IC : behavior_v<D>, behavior_VC
{
	using behavior_v<D>::that;
	using behavior_VC::behavior_VC;

	// behavior_IC(int z_):behavior_VC(z_){}
	int h() const override{
		return that()->x*100;
	}
	int h() override{
		return that()->x;
	}
};

struct IX : assembly_v<behavior_IA, behavior_IB, behavior_IC>
{
	using assembly_v<behavior_IA, behavior_IB, behavior_IC>::assembly_v;
};

struct IY : assembly_v<behavior_IA, behavior_IC>
{
	using assembly_v<behavior_IA, behavior_IC>::assembly_v;
};

struct C{
	int c;
};

struct B{
	int b;
	virtual int getb(){return b;}
};

struct IA{
	IA(){}
	virtual int geta() = 0;
};

struct A : IA{
	using IA::IA;

	int a;
	A(int a_): a(a_){}
	virtual void seta(int value){a=value;}
	virtual int geta(){return a;}
};

int test_singleb_v(behavior_VA &a)
{
	return a.f();
}

#define VALUE_CONST 1000

int test_multib_v_ref(const multibehavior_v<behavior_VA, behavior_VC> &ac)
{
	return ac.f() + ac.h() + VALUE_CONST;
}

int test_multib_v_ref(multibehavior_v<behavior_VA, behavior_VC> &ac)
{
	return ac.f() + ac.h();
}

int test_multib_v_ptr(const multibehavior_v<behavior_VA, behavior_VC> *ac)
{
	return ac->f() * ac->h() + VALUE_CONST;
}

int test_multib_v_ptr(multibehavior_v<behavior_VA, behavior_VC> *ac)
{
	return ac->f() * ac->h();
}

TEST(TestBehaviorObj, MakeBehaviorObj)
{
	IX ix(
		std::forward_as_tuple(1),
		std::forward_as_tuple(2),
		std::forward_as_tuple(3)
	);

	IY iy(
		std::forward_as_tuple(10),
		std::forward_as_tuple(30)
	);

	EXPECT_EQ(test_singleb_v(ix), ix.f());
	EXPECT_EQ(test_singleb_v(iy), iy.f());

	using DPCB::experimental::make_behavior_obj;
	auto ix_bo = make_behavior_obj<behavior_VA,behavior_VC>(ix);
	auto iy_bo = make_behavior_obj<behavior_VA,behavior_VC>(iy);
	EXPECT_EQ(test_multib_v_ref(*ix_bo), ix.f()+ix.h());
	EXPECT_EQ(test_multib_v_ref(*iy_bo), iy.f()+iy.h());
	EXPECT_EQ(test_multib_v_ptr(ix_bo), ix.f()*ix.h());
	EXPECT_EQ(test_multib_v_ptr(iy_bo), iy.f()*iy.h());

	using DPCB::experimental::const_behavior_obj;
	const IX &cix = ix;
	const IY &ciy = iy;
	auto cix_bo = make_behavior_obj<behavior_VA,behavior_VC>(cix);
	const_behavior_obj<behavior_VA,behavior_VC> ciy_bo = make_behavior_obj<behavior_VA,behavior_VC>(iy);
	EXPECT_EQ(test_multib_v_ref(*cix_bo), cix.f()+cix.h()+VALUE_CONST);
	EXPECT_EQ(test_multib_v_ref(*ciy_bo), ciy.f()+ciy.h()+VALUE_CONST);
	EXPECT_EQ(test_multib_v_ptr(cix_bo), cix.f()*cix.h()+VALUE_CONST);
	EXPECT_EQ(test_multib_v_ptr(ciy_bo), ciy.f()*ciy.h()+VALUE_CONST);

	EXPECT_EQ(ix_bo->h(), ix.h());
	EXPECT_EQ(iy_bo->h(), iy.h());

	EXPECT_EQ(cix_bo->h(), cix.h());
	EXPECT_EQ(ciy_bo->h(), ciy.h());
}