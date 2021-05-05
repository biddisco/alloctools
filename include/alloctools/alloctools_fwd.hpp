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

namespace alloctools { namespace rma {

    // a simple memory region abstraction for pinned memory
    class memory_region;

    // fancy pointer that embeds a memory_region
    template <typename T>
    struct memory_region_pointer

    // allocator that returns fancy pointers
    template <typename T>
    struct memory_region_allocator;

    // a memory pool
    template <typename RegionProvider, typename T = unsigned char>
    struct memory_pool;

}}    // namespace alloctools::rma
