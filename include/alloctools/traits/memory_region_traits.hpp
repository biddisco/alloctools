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

#include <memory>

namespace alloctools { namespace traits {

    template <typename RegionProvider>
    struct rma_memory_region_traits
    {
        typedef typename RegionProvider::provider_domain provider_domain;
        typedef typename RegionProvider::provider_region provider_region;
        //
        static int register_memory(provider_domain* pd, const void* buf,
            size_t len, uint64_t access, uint64_t offset,
            uint64_t requested_key, uint64_t flags, provider_region** mr,
            void* context)
        {
            return RegionProvider::register_memory(pd, buf, len, access, offset,
                requested_key, flags, mr, context);
        }
        //
        static int unregister_memory(provider_region* mr)
        {
            return RegionProvider::unregister_memory(mr);
        }
        //
        static int flags()
        {
            return RegionProvider::flags();
        }
        //
        static void *get_local_key(provider_region *mr) {
            return RegionProvider::get_local_key(mr);
        }
        //
        static uint64_t get_remote_key(provider_region *mr) {
            return RegionProvider::get_remote_key(mr);
        }
    };
}}    // namespace alloctools::traits
