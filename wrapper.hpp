#ifndef _DPCB_WRAPPER_HPP_
#define _DPCB_WRAPPER_HPP_

#include <utility>

namespace DPCB{

template<auto F>
class wrapper_function
{
    template<typename> class inner;

    template<class Cls, typename Ret, typename... Args>
    class inner<Ret(Cls::*)(Args...)>
    {
    public:
        using type_func = Ret(Args...);

        static auto bind(Cls *obj)
        {
            return [=](Args ...args){
                return (obj->*F)(std::forward<Args>(args)...);
            };
        }
    };

public:
    using type_func = typename inner<decltype(F)>::type_func;
    static const constexpr auto bind = inner<decltype(F)>::bind;
};

}

#endif //_DPCB_WRAPPER_HPP_