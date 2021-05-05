# AllocTools
#
# Copyright (c) 2017-2021, ETH Zurich
# All rights reserved.
#
# Please, refer to the LICENSE file in the root directory.
# SPDX-License-Identifier: BSD-3-Clause

PROJECT(alloctools)

cmake_minimum_required(VERSION 3.11)
cmake_policy(SET CMP0028 NEW) # Double colon in target name means ALIAS or IMPORTED target.
cmake_policy(SET CMP0048 NEW) # The ``project()`` command manages VERSION variables.

#------------------------------------------------------------------------------
# Module path
#------------------------------------------------------------------------------
set(ALLOCTOOLS_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${ALLOCTOOLS_MODULE_PATH}")

#------------------------------------------------------------------------------
# Define some handy macros
#------------------------------------------------------------------------------
include(alloctools_macros)

#------------------------------------------------------------------------------
# Debug output enable/disable option
#------------------------------------------------------------------------------
alloctools_option(ALLOCTOOLS_WITH_DEBUG BOOL
  "Enable debug printing in the library (default: OFF)"
  OFF CATEGORY "alloctools" ADVANCED)

if (ALLOCTOOLS_WITH_DEBUG)
  alloctools_add_config_define_namespace(
      DEFINE    ALLOCTOOLS_HAVE_DEBUG
      NAMESPACE alloctools)
endif()

# ------------------------------------------------------------------------
# define main contents of our header only library
# ------------------------------------------------------------------------
set(alloctools_headers
    alloctools/alloctools_fwd.hpp
    alloctools/memory_region.hpp
    alloctools/memory_region_allocator.hpp
    alloctools/memory_pool.hpp
    alloctools/detail/memory_region_impl.hpp
    alloctools/detail/memory_pool_stack.hpp
)

# ------------------------------------------------------------------------
# Support libfabric memory pinning?
# This will setup any options for libfabric use
# ------------------------------------------------------------------------
include(alloctools_libfabric)
if (ALLOCTOOLS_WITH_LIBFABRIC)
    set(alloctools_headers ${alloctools_headers}
        alloctools/libfabric/region_provider.hpp
    )
endif()

# ------------------------------------------------------------------------
# Declare import library
# (to allow import from a subdirectory use IMPORTED GLOBAL)
# ------------------------------------------------------------------------
add_library(alloctools INTERFACE IMPORTED GLOBAL)
target_include_directories(alloctools INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

# alias the library to the ALLOCTOOLS:: namespace
add_library(alloctools ALIAS alloctools)

# ------------------------------------------------------------------------
# @TODO setup install rules etc
# ------------------------------------------------------------------------
