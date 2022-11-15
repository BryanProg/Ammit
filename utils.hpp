#pragma once
#include <type_traits>
#include <iterator>

namespace Ammit::Details
{
    template <typename Reference>
    class arrow_proxy
    {
        Reference ref;

    public:
        Reference* operator -> () {return &ref;}
    };

    template <typename T, 
            typename = std::enable_if_t<static_cast<bool>(&T::distance_to)>>
    struct infer_difference_type
    {
        static const T& i;
        using type = decltype(i.distance_to());
    };

    template <>
    struct infer_difference_type <std::ptrdiff_t, void>
    {
        using type = std::ptrdiff_t;
    };

    template <typename T>
    using infer_difference_type_t = infer_difference_type<T>::type;

    template <typename T>
        requires requires { typename T::value_type; }
    struct infer_value_type
    {
        using type = T::value_type;
    };

    template <typename T>
    concept impl_distance_to = requires(T t) {t.distance(t);};

    template <typename T>
    concept impl_decrement = requires(T t) {t.decrement();};

    template <typename T>
    concept impl_advance = requires(T it, const infer_difference_type_t<T> offset) 
    { it.advance(offset);};

    template <typename T>
    concept impl_equal_to = requires(T t)
    {
        t.equal_to() -> boolean;
    };

    template <typename T>
    concept meet_random_acess = impl_advance<T> && impl_distance_to<T>;

    template <typename T>
    concept meet_bidirecional = meet_random_acess<T> || impl_decrement<T>;

    template <typename T>
    concept decls_single_pass = bool(T::single_pass_iterator);

    template <typename arg, typename Iter>
    concept convertble_to_arg = std::is_convertible_v<arg, infer_difference_type_t<Iter>>;

    template <typename T>
    using infer_value_type_t = infer_value_type<T>::type;

}