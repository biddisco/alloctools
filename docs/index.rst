..

  AllocTools

  Copyright (c) 2017-2021, ETH Zurich
  All rights reserved.

  Please, refer to the LICENSE file in the root directory.
  SPDX-License-Identifier: BSD-3-Clause

.. _alloctools:

======
AllocTools
======

The AllocTools library contains a number of utilities that enable memory registration
for DMA from a networking library, it has been taken from code that was used in the
HPX libfabric parcelport and the GHEX libfabric transport layer.

* :cpp:class:`alloctools::rma::memory_region`
This is a abstract base class that different memory providers can inherit from
in order to provide registered memory blocks.
A memory region is simply an address (+range) which has been pinned and contains
the library specific memory registration key that is needed by network drivers
when they perform an RMA operation.
Low level parcelport code handles memory regions when buffers are created for
sending/receiving.

* :cpp:class:`alloctools::rma::memory_pool_base`
A abstract base class used for memory pools. It supports only two functions:
get_region and release_region which create and destroy memory regions.

* :cpp:class:`alloctools::rma::memory_pool`
memory_pool is templated over the RegionProvider, which might be libfabric, mpi, ucx
where the RegionProvider has certain requirements that control how memory_regions
are allocated and used. The memory pool is the primary interface for access
to memory_regions. It caches memory_regions so that registration and de-registration
are not performed before/after every request.
The memory_pool contains 4 memory_pool_stack objects which act as the internal storage
for memory regions. The stacks are configure to hold small,medium,large,huge
memory_region types and requests are routed through to stacks depending on size.
Note that the memory_pool is thread safe.

* :cpp:class:`alloctools::rma::memory_pool_stack`
This is just a stack of memory regions. The memory pools uses differnt stacks
ffor regions of different sizes and pushes and pops them onto stacks.
The use of a (lockfree) stack makes the pool threadsafe and the cache reuse is
hopefully improved by using a stack.

* :cpp:class:`alloctools::rma::memory_block_allocator`
The memory block allocator is a basic allocator that the main memory pools use
to initially allocate memory prior to registration/pinning.

* :cpp:class:`alloctools::rma::memory_region_impl`
The memory region_impl is an implementation of the abstract memory region class
and it is responsible for registering and deregistering memory chunks.
The implementation uses memory_region_traits to call register/deregister functions
on memory blocks and these much be provided by a "provider".

* :cpp:class:`alloctools::rma::memory_region_traits`
The traits class must be specialized by supplying a provider template that
implements the register/deregister memory functions as well as the ability to
return native handles to RMA keys.

* :cpp:class:`alloctools::rma::libfabric::region_provider`
This is a concrete implementation of a provider that can be used with the
memory_region_traits to create a memory allocator/pool/etc.

* :cpp:class:`alloctools::rma::memory_region_pointer`
This is a fancy pointer that can be used like a normal pointer as it derefernces
to the address, but it also contains memory region ino such as RMA keys that are needed
when performing RMA operation between nodes.

* :cpp:class:`alloctools::rma::memory_region_allocator`
This is an STL like allocator that returns fancy pointers of memory_region_pointer
type and can be used as a basic means of accessing pinned memory.
Most code in HPX/GHEx uses memory pools directly rather than the allocator, but it
is useful when porting network send/receive code that uses existing memory allocation
routines.


See the :ref:`API reference <alloctools>` of the module for more details.
