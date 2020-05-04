#include <tuple>
#include "gtest/gtest.h"
#include "crtp.hpp"

template<class D, typename T>
struct behavior_list
{
	behavior_list(int size){(void)size;}
	T* push_back()
	{
		static_assert(std::is_class<typename D::type_allocator>::value);
		// puts("push_back()");
		return static_cast<D*>(this)->allocate();
	}
};

template<class D, typename T>
struct behavior_allocator
{
protected:
	using type_allocator = behavior_allocator;
	T* allocate(){return nullptr;}
};

template<class D, typename T>
struct behavior_alloc
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

TEST(TestCRTP, Assembly)
{
	list<int> l(std::forward_as_tuple(0x0));
	list<int, behavior_alloc> la(std::forward_as_tuple(0xa), std::forward_as_tuple());
	EXPECT_EQ(f(l), nullptr);
	EXPECT_EQ(f(la), nullptr);
}