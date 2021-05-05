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
//
#include <boost/lockfree/stack.hpp>
//
#include <array>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <string>

#define GHEX_DP_ONLY(printer, Expr)                                            \
    if (printer.is_enabled())                                                  \
    {                                                                          \
        printer.Expr;                                                          \
    };

namespace alloctools { namespace rma { namespace detail {
    static alloctools::debug::enable_print<false> mbs_deb("MBALLOC");

    // --------------------------------------------------------------------
    // This is a simple class that implements only malloc and free but is
    // templated over the memory region provider which is transport layer
    // dependent. The blocks returned are registered using the API
    // of the region provider and returned to the pool that is using
    // this allocator where they are sub-divided into smalller blocks
    // and used by user facing code. Users should not directly call this
    // allocator, but instead use the memory pool.
    // Blocks are returned from this allocator as shared pointers to
    // memory regions.
    template <typename RegionProvider>
    struct memory_block_allocator
    {
        typedef typename RegionProvider::provider_domain domain_type;
        typedef memory_region_impl<RegionProvider> region_type;
        typedef std::shared_ptr<region_type> region_ptr;

        // default empty constructor
        memory_block_allocator() {}

        // allocate a registered memory region
        static region_ptr malloc(domain_type* pd, const std::size_t bytes)
        {
            region_ptr region = std::make_shared<region_type>();
            region->allocate(pd, bytes);
            GHEX_DP_ONLY(mbs_deb,
                trace(alloctools::debug::str<>("Allocating"),
                    alloctools::debug::hex<4>(bytes), "chunk mallocator", *region));
            return region;
        }

        // release a registered memory region
        static void free(region_ptr region)
        {
            GHEX_DP_ONLY(mbs_deb,
                trace(
                    alloctools::debug::str<>("Freeing"), "chunk mallocator", *region));
            region.reset();
        }
    };

}}}    // namespace alloctools::rma::detail
