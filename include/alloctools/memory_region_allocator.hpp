/*
 * AllocTools
 *
 * Copyright (c) 2017-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#pragma once

#include <alloctools/detail/memory_region_impl.hpp>
#include <alloctools/memory_pool.hpp>
#include <alloctools/memory_region.hpp>
#include <alloctools/memory_region_pointer.hpp>
//
#include <memory>

namespace alloctools { namespace rma  {

    // ------------------------------------
    // see https://howardhinnant.github.io/allocator_boilerplate.html
    // ------------------------------------

// Random access iterator requirements (non-members)
#define DEFINE_OPERATOR(oper, op, expr)                                        \
    template <typename T>                                                      \
    bool oper(memory_region_pointer<T> const& lhs,                             \
        memory_region_pointer<T> const& rhs) noexcept                          \
    {                                                                          \
        return expr;                                                           \
    }                                                                          \
    template <typename T>                                                      \
    bool oper(memory_region_pointer<T> const& lhs,                             \
        typename std::common_type<memory_region_pointer<T>>::type const&       \
            rhs) noexcept                                                      \
    {                                                                          \
        return lhs op rhs;                                                     \
    }                                                                          \
    template <typename T>                                                      \
    bool oper(                                                                 \
        typename std::common_type<memory_region_pointer<T>>::type const& lhs,  \
        memory_region_pointer<T> const& rhs) noexcept                          \
    {                                                                          \
        return lhs op rhs;                                                     \
    }
    // clang-format off
    DEFINE_OPERATOR(operator==, ==, (lhs.pointer_ == rhs.pointer_))
    DEFINE_OPERATOR(operator!=, !=, (lhs.pointer_ != rhs.pointer_))
    DEFINE_OPERATOR(operator<, <, (lhs.pointer_ < rhs.pointer_))
    DEFINE_OPERATOR(operator<=, <=, (lhs.pointer_ <= rhs.pointer_))
    DEFINE_OPERATOR(operator>, >, (lhs.pointer_ > rhs.pointer_))
    DEFINE_OPERATOR(operator>=, >=, (lhs.pointer_ >= rhs.pointer_))
    // clang-format on
#undef DEFINE_OPERATOR

    template <class T>
    struct memory_region_allocator
    {
        using value_type = T;

        using pointer = memory_region_pointer<value_type>;

        using const_pointer = typename std::pointer_traits<
            pointer>::template rebind<value_type const>;
        using void_pointer =
            typename std::pointer_traits<pointer>::template rebind<void>;
        using const_void_pointer = typename std::pointer_traits<
            pointer>::template rebind<const void>;

        using difference_type =
            typename std::pointer_traits<pointer>::difference_type;
        using size_type = std::make_unsigned_t<difference_type>;

        template <class U>
        struct rebind
        {
            typedef memory_region_allocator<U> other;
        };
        // ------------------------------------

        // --------------------------------------------------
        // mempool and region types are specific to transport layer, but these
        // are abstract base classes that can be used by all code
        using mempool_type = memory_pool_base;
        using region_type = rma::memory_region;

        // all allocators share the same mempool
        static mempool_type* mempool_ptr;

        // --------------------------------------------------
        // default constructor
        memory_region_allocator() noexcept {}

        // --------------------------------------------------
        // default copy constructor
        template <typename U>
        memory_region_allocator(memory_region_allocator<U> const&) noexcept
        {
        }

        // --------------------------------------------------
        // default move constructor
        template <typename U>
        memory_region_allocator(memory_region_allocator<U>&&) noexcept
        {
        }

        void set_memory_pool(mempool_type* mempool) noexcept
        {
            mempool_ptr = mempool;
        }

        mempool_type* get_memory_pool() noexcept
        {
            return mempool_ptr;
        }

        [[nodiscard]] pointer allocate(std::size_t n)
        {
            region_type* region = mempool_ptr->get_region(n);
            pointer p = pointer{
                reinterpret_cast<T*>(region->get_address()), region};
            return p;
        }

        void deallocate(pointer p, std::size_t)
        {
            mempool_ptr->release_region(p.region_);
        }

        void deallocate(pointer p)
        {
            mempool_ptr->release_region(p.region_);
        }

        template <typename U>
        void construct(U* ptr) noexcept(
            std::is_nothrow_default_constructible<U>::value)
        {
            ::new (static_cast<void*>(ptr)) U;
        }

        template <typename U, typename... Args>
        void construct(U* ptr, Args&&... args)
        {
            ::new (static_cast<void*>(ptr)) U(std::forward<Args>(args)...);
        }
    };

    template <class T, class U>
    bool operator==(const memory_region_allocator<T>&,
        const memory_region_allocator<U>&)
    {
        return true;
    }
    template <class T, class U>
    bool operator!=(const memory_region_allocator<T>&,
        const memory_region_allocator<U>&)
    {
        return false;
    }

}}    // namespace alloctools::rma

namespace alloctools { namespace rma {
    template <typename T>
    typename memory_region_allocator<T>::mempool_type*
        memory_region_allocator<T>::mempool_ptr = nullptr;
}}    // namespace alloctools::rma
