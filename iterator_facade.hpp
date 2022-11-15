#pragma once
#include <type_traits>
#include <iterator>
#include "utils.hpp"

using std::iterator_traits;

namespace Ammit
{
    template <typename derived>
    class iter_facade
    {
    public:
        using self_type = derived;

        friend bool operator == (self_type& rhs, self_type& lhs)
        {
            return rhs.dereference() == lhs.dereference();
        }

        template <typename D>
            requires (Details::convertble_to_arg<D, self_type> && Details::impl_advance<self_type>)
        friend self_type& operator += (self_type& self, D offset)
        {
            self.advance(offset);
            return self;
        }

        template <typename D>
            requires (Details::convertble_to_arg<D, self_type> && Details::impl_advance<self_type>)
        friend self_type operator + (self_type rhs, D offset)
        {
            return rhs += offset;
        }

        template <typename D>
            requires (Details::convertble_to_arg<D, self_type> && Details::impl_advance<self_type>)
        friend self_type operator + (D offset, self_type lhs)
        {
            return lhs += offset;
        }

        template <typename D>
            requires (Details::convertble_to_arg<D, self_type> && Details::impl_advance<self_type>)
        friend self_type& operator -= (self_type& self, D offset)
        {
            return self += -offset;
        }


        template <typename D>
            requires (Details::convertble_to_arg<D, self_type> && Details::impl_advance<self_type>)
        friend self_type operator - (self_type rhs, D offset)
        {
            return rhs -= offset;
        }

        template <typename D>
            requires (Details::convertble_to_arg<D, self_type> && Details::impl_advance<self_type>)
        friend self_type operator - (D offset, self_type lhs)
        {
            return lhs -= offset;
        }

        template <typename D>
            requires (Details::convertble_to_arg<D, self_type> && Details::impl_advance<self_type>)
        decltype(auto) operator [] (D offset)
        {
            return *(_self() + offset);
        }

        friend self_type& operator - (const self_type& rhs, const self_type& lhs)
            requires Details::impl_distance_to<self_type>
        {
            return rhs.distance_to(lhs);
        }

    private:

        self_type& _self()
        {
            return static_cast<self_type&>(*this);
        }

        const self_type& _self() const
        {
            return static_cast<const self_type&>(*this);
        }

    public:

        constexpr decltype(auto) operator * () const
        {
            return _self().dereference();
        }

        auto operator -> () const
        {
            decltype(auto) ref = **this;

            if constexpr (std::is_reference_v<decltype(ref)>)
                return std::addressof(ref);
            else
                return Details::arrow_proxy{std::move(ref)};
        }

        self_type& operator ++ ()
        {
            _self().increment();
            return _self();
        }

        self_type operator ++ (int)
        {
            auto prev = this->_self();
            ++(*this);
            return prev;
        }

        template <typename T = derived,
                typename = std::enable_if_t<static_cast<bool>(&T::decrement)>>
        self_type& operator -- ()
        {
            _self().decrement();
            return _self();
        }

        template <typename T = derived,
                typename = std::enable_if_t<static_cast<bool>(&T::decrement)>>
        self_type operator -- (int)
        {
            auto prev = _self();
            --(*this);

            return prev;
        }
    };
}

template <typename Iter>
    requires std::is_base_of_v<Ammit::iter_facade<Iter>, Iter>
struct iterator_traits<Iter>
{
    static const Iter& _iter;

    using reference         = decltype(*_iter);
    using pointer           = decltype(_iter.operator->());
    using difference_type   = Ammit::Details::infer_difference_type_t<Iter>;
    using value_type        = Ammit::Details::infer_value_type_t<Iter>;

    using iterator_category = 
    std::conditional_t<
        Ammit::Details::meet_random_acess<Iter>, std::random_access_iterator_tag,
        std::conditional_t<
            Ammit::Details::meet_bidirecional<Iter>, std::bidirectional_iterator_tag,
            std::conditional_t<
                Ammit::Details::decls_single_pass<Iter>, std::input_iterator_tag,
                std::forward_iterator_tag>>>;
    
    using iterator_concept = iterator_category;
};