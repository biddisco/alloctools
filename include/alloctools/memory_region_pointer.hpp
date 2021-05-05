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
//
#include <memory>

namespace alloctools { namespace rma {

    // memory_region_pointer is a "fancy pointer" that can be dereferenced to
    // an address like any pointer, but it also stores memory region info
    // that contains pinned memory registration key for RMA operations
    template <typename T>
    struct memory_region_pointer
    {
        template <class U>
        struct rebind
        {
            using other = memory_region_pointer<U>;
        };

        using region_type = rma::memory_region;

        T* pointer_;
        region_type* region_;

        region_type* get_region() const
        {
            return region_;
        }

        // Constructors
        memory_region_pointer() noexcept
          : pointer_(nullptr)
          , region_(nullptr)
        {
        }

        memory_region_pointer(T* native, region_type* r) noexcept
          : pointer_(native)
          , region_(r)
        {
        }

        memory_region_pointer(memory_region_pointer const& rhs) noexcept
          : pointer_(rhs.pointer_)
          , region_(rhs.region_)
        {
        }

        memory_region_pointer(std::nullptr_t) noexcept
        {
            pointer_ = nullptr;
            region_ = nullptr;
        }

        template <typename U,
            typename = typename std::enable_if<!std::is_same<T, U>::value &&
                std::is_same<typename std::remove_cv<T>::type, U>::value>::type>
        memory_region_pointer(memory_region_pointer<U> const& rhs) noexcept
          : pointer_(rhs.pointer_)
          , region_(rhs.region_)
        {
        }

        template <typename U, typename Dummy = void,
            typename = typename std::enable_if<
                !std::is_same<typename std::remove_cv<T>::type,
                    typename std::remove_cv<U>::type>::value &&
                    !std::is_void<U>::value,
                decltype(static_cast<T*>(std::declval<U*>()))>::type>
        memory_region_pointer(memory_region_pointer<U> const& rhs) noexcept
          : pointer_(rhs.pointer_)
          , region_(rhs.region_)
        {
        }

        // NullablePointer requirements
        explicit operator bool() const noexcept
        {
            return pointer_ != nullptr;
        }

        memory_region_pointer& operator=(T* p) noexcept
        {
            pointer_ = p;
            region_ = nullptr;
            return *this;
        }

        memory_region_pointer& operator=(
            memory_region_pointer const& rhs) noexcept
        {
            if (this != &rhs)
            {
                pointer_ = rhs.pointer_;
                region_ = rhs.region_;
            }
            return *this;
        }

        memory_region_pointer& operator=(std::nullptr_t) noexcept
        {
            pointer_ = nullptr;
            region_ = nullptr;
            return *this;
        }

        // For pointer traits
        static memory_region_pointer pointer_to(T& x)
        {
            return memory_region_pointer(std::addressof(x));
        }

        // ---------------------------------------------
        // Random access iterator requirements (members)
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = T&;
        using pointer = memory_region_pointer<T>;

        memory_region_pointer operator+(std::ptrdiff_t n) const
        {
            return memory_region_pointer(pointer_ + n, region_);
        }

        memory_region_pointer& operator+=(std::ptrdiff_t n)
        {
            pointer_ += n;
            return *this;
        }

        memory_region_pointer operator-(std::ptrdiff_t n) const
        {
            return memory_region_pointer(pointer_ - n, region_);
        }

        memory_region_pointer& operator-=(std::ptrdiff_t n)
        {
            pointer_ -= n;
            return *this;
        }

        std::ptrdiff_t operator-(memory_region_pointer const& rhs) const
        {
            return std::distance(rhs.pointer_, pointer_);
        }

        memory_region_pointer& operator++()
        {
            pointer_ += 1;
            return *this;
        }

        memory_region_pointer& operator--()
        {
            pointer_ -= 1;
            return *this;
        }

        memory_region_pointer operator++(int)
        {
            memory_region_pointer tmp(*this);
            ++*this;
            return tmp;
        }

        memory_region_pointer operator--(int)
        {
            memory_region_pointer tmp(*this);
            --*this;
            return tmp;
        }

        T* operator->() const noexcept
        {
            return pointer_;
        }
        T& operator*() const noexcept
        {
            return *pointer_;
        }
        T& operator[](std::size_t i) const noexcept
        {
            return pointer_[i];
        }
    };

}}    // namespace alloctools::rma
