# set all libfabric related options and values

#------------------------------------------------------------------------------
# Enable libfabric support
#------------------------------------------------------------------------------
alloctools_option(ALLOCTOOLS_WITH_LIBFABRIC BOOL
  "Enable libfabric memory registration/pinning module"
  OFF CATEGORY "alloctools" ADVANCED)

if (ALLOCTOOLS_WITH_LIBFABRIC)
  alloctools_add_config_define_namespace(
      DEFINE    ALLOCTOOLS_HAVE_LIBFABRIC
      NAMESPACE alloctools)
endif()

#------------------------------------------------------------------------------
# Hardware device selection
#------------------------------------------------------------------------------
alloctools_option(ALLOCTOOLS_LIBFABRIC_PROVIDER STRING
  "The provider (verbs/gni/psm2/tcp/sockets)"
  "verbs" CATEGORY "alloctools" ADVANCED)

alloctools_add_config_define_namespace(
    DEFINE ALLOCTOOLS_LIBFABRIC_PROVIDER
    VALUE  "\"${ALLOCTOOLS_LIBFABRIC_PROVIDER}\""
    NAMESPACE alloctools)

if(ALLOCTOOLS_LIBFABRIC_PROVIDER MATCHES "verbs")
    alloctools_add_config_define_namespace(
        DEFINE ALLOCTOOLS_LIBFABRIC_VERBS
        NAMESPACE alloctools)
elseif(ALLOCTOOLS_LIBFABRIC_PROVIDER MATCHES "gni")
    alloctools_add_config_define_namespace(
        DEFINE ALLOCTOOLS_LIBFABRIC_GNI
        NAMESPACE alloctools)
elseif(ALLOCTOOLS_LIBFABRIC_PROVIDER MATCHES "tcp")
    alloctools_add_config_define_namespace(
        DEFINE ALLOCTOOLS_LIBFABRIC_TCP
        NAMESPACE alloctools)
elseif(ALLOCTOOLS_LIBFABRIC_PROVIDER MATCHES "sockets")
    alloctools_add_config_define_namespace(
        DEFINE ALLOCTOOLS_LIBFABRIC_SOCKETS
        NAMESPACE alloctools)
elseif(ALLOCTOOLS_LIBFABRIC_PROVIDER MATCHES "psm2")
    alloctools_add_config_define_namespace(
        DEFINE ALLOCTOOLS_LIBFABRIC_PSM2
        NAMESPACE alloctools)
endif()

#------------------------------------------------------------------------------
# Memory chunk/reservation options
#------------------------------------------------------------------------------
alloctools_option(ALLOCTOOLS_TINY_MEMORY_CHUNK_SIZE STRING
  "Number of bytes a tiny chunk in the memory pool can hold (default: 1K)"
  "4096" CATEGORY "alloctools" ADVANCED)

alloctools_option(ALLOCTOOLS_64K_PAGES STRING
  "Number of 64K pages we reserve for default message buffers (default: 10)"
  "10" CATEGORY "alloctools" ADVANCED)

alloctools_add_config_define_namespace(
    DEFINE    ALLOCTOOLS_TINY_MEMORY_CHUNK_SIZE
    VALUE     ${ALLOCTOOLS_TINY_MEMORY_CHUNK_SIZE}
    NAMESPACE alloctools)

alloctools_add_config_define_namespace(
    DEFINE    ALLOCTOOLS_64K_PAGES
    VALUE     ${ALLOCTOOLS_64K_PAGES}
    NAMESPACE alloctools)

#------------------------------------------------------------------------------
# Write options to file in build dir
#------------------------------------------------------------------------------
write_config_defines_file(
    NAMESPACE alloctools
    FILENAME  "${PROJECT_BINARY_DIR}/alloctools/config_defines.hpp"
)
target_include_directories(libfabric::libfabric INTERFACE "${PROJECT_BINARY_DIR}")
