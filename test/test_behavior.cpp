#include <cstdio>
#include <cstddef>
#include <iostream>
#include <tuple>
#include <type_traits>
#include "gtest/gtest.h"
#include "behavior.hpp"
using namespace DPCB;

template<class D, typename T>
struct behavior_list : DPCB::behavior<D>
{
	behavior_list(int size){(void)size;}
	T* push_back()
	{
		static_assert(std::is_class<typename D::type_allocator>::value);
		// puts("push_back()");
		return behavior<D>::that()->allocate();
	}
};

template<class D, typename T>
struct behavior_allocator : DPCB::behavior<D>
{
protected:
	using type_allocator = behavior_allocator;
	T* allocate(){return nullptr;}
};

template<class D, typename T>
struct behavior_alloc : DPCB::behavior<D>
{
protected:
	using type_allocator = behavior_alloc;
	T* allocate(){return nullptr;}
};

template<typename T, template<class, typename> class Alloc>
class list_impl{
public:
	template<class D> using b_l_t = behavior_list<D, T>;
	template<class D> using b_a_t = Alloc<D, T>;
	struct inner : DPCB::assembly<b_l_t, b_a_t>{
		// export behaviors
		template<class D> using behavior_list = b_l_t<D>;
		// export constructors
		using DPCB::assembly<b_l_t, b_a_t>::assembly;
	};
public:
	using type = inner;
};

template<typename T, template<class, typename> class Alloc=behavior_allocator>
using list = typename list_impl<T, Alloc>::type;

template<class D>
int* f(list<int>::behavior_list<D> &l)
{
	return l.push_back();
}

template<class D>
struct behavior_A : DPCB::behavior<D>
{
	using behavior<D>::that;
	int x;
	behavior_A(int x_):x(x_){}
	int f()const{return that()->z;}
};

template<class D>
struct behavior_B : DPCB::behavior<D>
{
	using behavior<D>::that;
	int y;
	behavior_B(int y_):y(y_){}
	int g(){return that()->y;}
};

template<class D>
struct behavior_C : DPCB::behavior<D>
{
	using behavior<D>::that;
	int z;
	behavior_C(int z_):z(z_){}
	int h(){return that()->x;}
};

struct X : DPCB::assembly<behavior_A, behavior_B, behavior_C>
{
	using DPCB::assembly<behavior_A, behavior_B, behavior_C>::assembly;
};

struct Y : DPCB::assembly<behavior_A, behavior_C>
{
	using DPCB::assembly<behavior_A, behavior_C>::assembly;
};

template<class D>
int test_singleb(behavior_B<D> &b)
{
	return b.g();
}

#define VALUE_CONST 1000

template<class D>
int test_multib_ref(multibehavior<D, behavior_A, behavior_C> &ac)
{
	return ac.f() + ac.h();
}

template<class D>
int test_multib_ref(const multibehavior<D, behavior_A, behavior_C> &ac)
{
	return ac.f() + VALUE_CONST;
}

template<class D>
int test_multib_ptr(multibehavior<D, behavior_A, behavior_C> *ac)
{
	return ac->f() * ac->h();
}

template<class D>
int test_multib_ptr(const multibehavior<D, behavior_A, behavior_C> *ac)
{
	return ac->f() * VALUE_CONST;
}

template<class B, class D>
std::ptrdiff_t get_offset()
{
	static_assert(std::is_base_of<B,D>::value);
	auto addr_derive = reinterpret_cast<D*>(alignof(D));
	auto addr_base = static_cast<B*>(addr_derive);
	return (char*)addr_base - (char*)addr_derive;
};

TEST(TestBehavior, Assembly)
{
	list<int> l(std::forward_as_tuple(0x0));
	list<int, behavior_alloc> la(std::forward_as_tuple(0xa), std::forward_as_tuple());
	EXPECT_EQ(f(l), nullptr);
	EXPECT_EQ(f(la), nullptr);

	X x(
		std::forward_as_tuple(1),
		std::forward_as_tuple(2),
		std::forward_as_tuple(3)
	);

	EXPECT_EQ(x.f(), 3);
	EXPECT_EQ(x.g(), 2);
	EXPECT_EQ(x.h(), 1);

	using BAC = multibehavior<X,behavior_A,behavior_C>;
	using BA = typename match_behavior<behavior_A,X>::type;
	using BC = typename match_behavior<behavior_C,X>::type;

	EXPECT_TRUE((std::is_base_of<BA, BAC>::value));
	EXPECT_TRUE((std::is_base_of<BC, BAC>::value));

	EXPECT_EQ((get_offset<BA, BAC>()), 0);
	EXPECT_EQ((get_offset<BC, BAC>()), 2*sizeof(int));
	EXPECT_EQ((get_offset<BA, X>()), 0);
	EXPECT_EQ((get_offset<BC, X>()), 2*sizeof(int));

	EXPECT_EQ(sizeof(X), 3*sizeof(int));
	EXPECT_EQ(sizeof(multibehavior<X,behavior_A,behavior_B,behavior_C>), 3*sizeof(int));
	EXPECT_EQ(sizeof(multibehavior<X,behavior_A,behavior_C>), 3*sizeof(int));
	EXPECT_EQ(sizeof(multibehavior<Y,behavior_A,behavior_C>), 2*sizeof(int));
}

TEST(TestBehavior, BehaviorCast)
{
	X x(
		std::forward_as_tuple(1),
		std::forward_as_tuple(2),
		std::forward_as_tuple(3)
	);

	Y y(
		std::forward_as_tuple(10),
		std::forward_as_tuple(30)
	);

	EXPECT_EQ(test_singleb(x), 2);
	EXPECT_EQ(test_singleb(DPCB::behavior_cast<behavior_B>(x)), 2);

	EXPECT_EQ(test_multib_ref<X>(x), x.f()+x.h());
	EXPECT_EQ(test_multib_ref<Y>(y), y.f()+y.h());

	EXPECT_EQ(
		test_multib_ptr(DPCB::behavior_cast<behavior_A,behavior_C>(&x)),
		x.f()*x.h()
	);
	EXPECT_EQ(
		test_multib_ref(DPCB::behavior_cast<behavior_A,behavior_C>(y)),
		y.f()+y.h()
	);

	const auto &cx = x;
	const auto &cy = y;
	EXPECT_EQ(test_multib_ref<X>(cx), x.f()+VALUE_CONST);
	EXPECT_EQ(test_multib_ref<Y>(cy), y.f()+VALUE_CONST);

	EXPECT_EQ(
		test_multib_ref(DPCB::behavior_cast<behavior_A,behavior_C>(cx)),
		cx.f()+VALUE_CONST
	);
	EXPECT_EQ(
		test_multib_ptr(DPCB::behavior_cast<behavior_A,behavior_C>(&cy)),
		cy.f()*VALUE_CONST
	);
}