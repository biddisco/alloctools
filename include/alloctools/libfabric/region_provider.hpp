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

#include <alloctools/traits/memory_region_traits.hpp>
//
#include <rdma/fabric.h>
#include <rdma/fi_cm.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_endpoint.h>
#include <rdma/fi_eq.h>
#include <rdma/fi_errno.h>
//
#include <memory>
#include <utility>
//
namespace alloctools { namespace rma { namespace libfabric
{
    struct region_provider
    {
        // The internal memory region handle
        typedef struct fid_mr     provider_region;
        typedef struct fid_domain provider_domain;

        // register region
        template <typename... Args>
        static int register_memory(Args &&... args) {
            return fi_mr_reg(std::forward<Args>(args)...);
        }

        // unregister region
        static int unregister_memory(provider_region *region) {
            return fi_close(&region->fid);
        }

        // Default registration flags for this provider
        static int flags() { return
            FI_READ | FI_WRITE | FI_RECV | FI_SEND | FI_REMOTE_READ | FI_REMOTE_WRITE;
        }

        // Get the local descriptor of the memory region.
        static void *get_local_key(provider_region *region) {
            return fi_mr_desc(region);
        }

        // Get the remote key of the memory region.
        static uint64_t get_remote_key(provider_region *region) {
            return fi_mr_key(region);
        }
    };

}}}
