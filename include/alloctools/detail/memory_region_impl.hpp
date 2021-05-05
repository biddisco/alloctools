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

#include <alloctools/debugging/print.hpp>
#include <alloctools/memory_region.hpp>
#include <alloctools/traits/memory_region_traits.hpp>
//
#include <memory>

namespace alloctools {
    // cppcheck-suppress ConfigurationNotChecked
    static alloctools::debug::enable_print<false> memr_deb("MEM_REG1");
}    // namespace alloctools

namespace alloctools { namespace rma { namespace detail {
    // --------------------------------------------------------------------
    // a memory region is a pinned block of memory that has been specialized
    // by a particular region provider. Each provider (infiniband, libfabric,
    // other) has a different definition for the region object and the protection
    // domain used to limit access.
    // Code that does not 'know' which parcelport is being used, must use
    // the memory_region class to manage regions, but if specialized
    // may use memory_region_impl for the transport layer in question.
    // --------------------------------------------------------------------
    template <typename RegionProvider>
    class memory_region_impl : public memory_region
    {
    public:
        typedef typename RegionProvider::provider_domain provider_domain;
        typedef typename RegionProvider::provider_region provider_region;

        // --------------------------------------------------------------------
        memory_region_impl()
          : memory_region()
          , region_(nullptr)
        {
        }

        // --------------------------------------------------------------------
        memory_region_impl(provider_region* region, char* address,
            char* base_address, uint64_t size, uint32_t flags)
          : memory_region(address, base_address, size, flags)
          , region_(region)
        {
        }

        // --------------------------------------------------------------------
        // construct a memory region object by registering an existing address buffer
        memory_region_impl(
            provider_domain* pd, const void* buffer, const uint64_t length)
        {
            address_ = static_cast<char*>(const_cast<void*>(buffer));
            base_addr_ = address_;
            size_ = length;
            used_space_ = length;
            flags_ = BLOCK_USER;

            int ret = traits::rma_memory_region_traits<
                RegionProvider>::register_memory(pd, const_cast<void*>(buffer),
                length,
                traits::rma_memory_region_traits<RegionProvider>::flags(), 0,
                (uint64_t) address_, 0, &(region_), nullptr);

            if (ret)
            {
                memr_deb.debug("error registering region ",
                    alloctools::debug::ptr(buffer), alloctools::debug::hex<6>(length));
            }
            else
            {
                memr_deb.trace("OK registering region ",
                    alloctools::debug::ptr(buffer), alloctools::debug::ptr(address_), "desc ",
                    alloctools::debug::ptr(fi_mr_desc(region_)), "rkey ",
                    alloctools::debug::ptr(fi_mr_key(region_)), "length ",
                    alloctools::debug::hex<6>(size_));
            }
        }

        // --------------------------------------------------------------------
        // allocate a block of size length and register it
        int allocate(provider_domain* pd, uint64_t length)
        {
            // Allocate storage for the memory region.
            void* buffer = new char[length];
            if (buffer != nullptr)
            {
                memr_deb.trace(
                    "allocated storage for memory region with malloc OK ",
                    alloctools::debug::hex<4>(length));
            }
            address_ = static_cast<char*>(buffer);
            base_addr_ = static_cast<char*>(buffer);
            size_ = length;
            used_space_ = 0;

            int ret = traits::rma_memory_region_traits<
                RegionProvider>::register_memory(pd, const_cast<void*>(buffer),
                length,
                traits::rma_memory_region_traits<RegionProvider>::flags(), 0,
                (uint64_t) address_, 0, &(region_), nullptr);

            if (ret)
            {
                memr_deb.debug("error registering region ",
                    alloctools::debug::ptr(buffer), alloctools::debug::hex<6>(length));
            }
            else
            {
                memr_deb.trace("OK registering region ",
                    alloctools::debug::ptr(buffer), alloctools::debug::ptr(address_), "desc ",
                    alloctools::debug::ptr(fi_mr_desc(region_)), "rkey ",
                    alloctools::debug::ptr(fi_mr_key(region_)), "length ",
                    alloctools::debug::hex<6>(size_));
            }

            memr_deb.trace("allocated/registered memory region ",
                alloctools::debug::ptr(this), "with local key ",
                alloctools::debug::ptr(get_local_key()), "at address ",
                alloctools::debug::ptr(get_address()), "with length ",
                alloctools::debug::hex<6>(get_size()));
            return 0;
        }

        // --------------------------------------------------------------------
        // destroy the region and memory according to flag settings
        ~memory_region_impl()
        {
            if (get_partial_region())
                return;
            release();
        }

        // --------------------------------------------------------------------
        // Deregister and free the memory region.
        // returns 0 when successful, -1 otherwise
        int release(void)
        {
            if (region_ != nullptr)
            {
                memr_deb.trace("About to release memory region with local key ",
                    alloctools::debug::ptr(get_local_key()));
                // get these before deleting/unregistering (for logging)
                const void* buffer = get_base_address();
                auto length = memr_deb.declare_variable<uint64_t>(get_size());
                (void) length;
                //
                if (traits::rma_memory_region_traits<
                        RegionProvider>::unregister_memory(region_))
                {
                    memr_deb.debug("Error, fi_close mr failed\n");
                    return -1;
                }
                else
                {
                    memr_deb.trace("deregistered memory region with local key ",
                        alloctools::debug::ptr(get_local_key()), "at address ",
                        alloctools::debug::ptr(buffer), "with length ",
                        alloctools::debug::hex<6>(length));
                }
                if (!get_user_region())
                {
                    delete[](static_cast<const char*>(buffer));
                }
                region_ = nullptr;
            }
            return 0;
        }

        // --------------------------------------------------------------------
        // Get the local descriptor of the memory region.
        virtual void* get_local_key(void) const
        {
            return traits::rma_memory_region_traits<
                RegionProvider>::get_local_key(region_);
        }

        // --------------------------------------------------------------------
        // Get the remote key of the memory region.
        virtual uint64_t get_remote_key(void) const
        {
            return traits::rma_memory_region_traits<
                RegionProvider>::get_remote_key(region_);
        }

        // --------------------------------------------------------------------
        // return the underlying infiniband region handle
        inline provider_region* get_region() const
        {
            return region_;
        }

    private:
        // The internal network type dependent memory region handle
        provider_region* region_;
    };

}}}    // namespace alloctools::rma::detail
