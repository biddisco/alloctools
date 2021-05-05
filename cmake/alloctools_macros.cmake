# ---------------------------------------------------------------------
# Function to create a config define with a given 'namespace'
# This is useful for writing out a config file for a subdir
# of a project so that cmake option changes only trigger rebuilds
# for the subset of the project using that #include
# ---------------------------------------------------------------------
function(alloctools_add_config_cond_define definition)

  # if(ARGN) ignores an argument "0"
  set(Args ${ARGN})
  list(LENGTH Args ArgsLen)
  if(ArgsLen GREATER 0)
    set_property(GLOBAL APPEND PROPERTY alloctools_CONFIG_COND_DEFINITIONS "${definition} ${ARGN}")
  else()
    set_property(GLOBAL APPEND PROPERTY alloctools_CONFIG_COND_DEFINITIONS "${definition}")
  endif()

endfunction()

# ---------------------------------------------------------------------
# Function to add config defines to a list that depends on a namespace variable
# #defines that match the namespace can later be written out to a file
# ---------------------------------------------------------------------
function(alloctools_add_config_define_namespace)
  set(options)
  set(one_value_args DEFINE NAMESPACE)
  set(multi_value_args VALUE)
  cmake_parse_arguments(OPTION
    "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  set(DEF_VAR alloctools_CONFIG_DEFINITIONS_${OPTION_NAMESPACE})

  # to avoid extra trailing spaces (no value), use an if check
  if(OPTION_VALUE)
    set_property(GLOBAL APPEND PROPERTY ${DEF_VAR} "${OPTION_DEFINE} ${OPTION_VALUE}")
  else()
    set_property(GLOBAL APPEND PROPERTY ${DEF_VAR} "${OPTION_DEFINE}")
  endif()

endfunction()

# ---------------------------------------------------------------------
# Function to write out all the config defines for a given namespace
# into a config file
# ---------------------------------------------------------------------
function(write_config_defines_file)
  set(options)
  set(one_value_args TEMPLATE NAMESPACE FILENAME)
  set(multi_value_args)
  cmake_parse_arguments(OPTION
    "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  if (${OPTION_NAMESPACE} STREQUAL "default")
    get_property(DEFINITIONS_VAR GLOBAL PROPERTY alloctools_CONFIG_DEFINITIONS)
    get_property(COND_DEFINITIONS_VAR GLOBAL PROPERTY alloctools_CONFIG_COND_DEFINITIONS)
  else()
    get_property(DEFINITIONS_VAR GLOBAL PROPERTY
      alloctools_CONFIG_DEFINITIONS_${OPTION_NAMESPACE})
  endif()

  if(DEFINED DEFINITIONS_VAR)
    list(SORT DEFINITIONS_VAR)
    list(REMOVE_DUPLICATES DEFINITIONS_VAR)
  endif()

  set(hpx_config_defines "\n")
  foreach(def ${DEFINITIONS_VAR})
    # C++17 specific variable
    string(FIND ${def} "HAVE_CXX17" _pos)
    if(NOT ${_pos} EQUAL -1)
      set(hpx_config_defines
         "${hpx_config_defines}#if __cplusplus >= 201500\n#define ${def}\n#endif\n")
    else()
      # C++14 specific variable
      string(FIND ${def} "HAVE_CXX14" _pos)
      if(NOT ${_pos} EQUAL -1)
        set(hpx_config_defines
           "${hpx_config_defines}#if __cplusplus >= 201402\n#define ${def}\n#endif\n")
      else()
        set(hpx_config_defines "${hpx_config_defines}#define ${def}\n")
      endif()
    endif()
  endforeach()

  if(DEFINED COND_DEFINITIONS_VAR)
    list(SORT COND_DEFINITIONS_VAR)
    list(REMOVE_DUPLICATES COND_DEFINITIONS_VAR)
    set(hpx_config_defines "${hpx_config_defines}\n")
  endif()
  foreach(def ${COND_DEFINITIONS_VAR})
    string(FIND ${def} " " _pos)
    if(NOT ${_pos} EQUAL -1)
      string(SUBSTRING ${def} 0 ${_pos} defname)
    else()
      set(defname ${def})
      string(STRIP ${defname} defname)
    endif()
    string(FIND ${def} "HAVE_CXX17" _pos)
    if(NOT ${_pos} EQUAL -1)
      set(hpx_config_defines
         "${hpx_config_defines}#if __cplusplus >= 201500 && !defined(${defname})\n#define ${def}\n#endif\n")
    else()
      # C++14 specific variable
      string(FIND ${def} "HAVE_CXX14" _pos)
      if(NOT ${_pos} EQUAL -1)
        set(hpx_config_defines
           "${hpx_config_defines}#if __cplusplus >= 201402 && !defined(${defname})\n#define ${def}\n#endif\n")
      else()
        set(hpx_config_defines
          "${hpx_config_defines}#if !defined(${defname})\n#define ${def}\n#endif\n")
      endif()
    endif()
  endforeach()

  # if the user has not specified a template, generate a proper header file
  if (NOT OPTION_TEMPLATE)
    string(TOUPPER ${OPTION_NAMESPACE} NAMESPACE_UPPER)
    set(PREAMBLE
      "\n"
      "// Do not edit this file! It has been generated by the cmake configuration step.\n"
      "\n"
      "#ifndef alloctools_CONFIG_${NAMESPACE_UPPER}_HPP\n"
      "#define alloctools_CONFIG_${NAMESPACE_UPPER}_HPP\n"
    )
    set(TEMP_FILENAME "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${NAMESPACE_UPPER}")
    file(WRITE ${TEMP_FILENAME}
        ${PREAMBLE}
        ${hpx_config_defines}
        "\n#endif\n"
    )
    configure_file("${TEMP_FILENAME}" "${OPTION_FILENAME}" COPYONLY)
    file(REMOVE "${TEMP_FILENAME}")
  else()
    configure_file("${OPTION_TEMPLATE}"
                   "${OPTION_FILENAME}"
                   @ONLY)
  endif()
endfunction()

include(CMakeParseArguments)

# ---------------------------------------------------------------------
# Categories that options might be assigned to
# ---------------------------------------------------------------------
set(alloctools_OPTION_CATEGORIES
  "Generic"
  "Build Targets"
  "Transport"
  "Profiling"
  "Debugging"
)

# ---------------------------------------------------------------------
# Function to create an option with a category type
# ---------------------------------------------------------------------
function(alloctools_option option type description default)
  set(options ADVANCED)
  set(one_value_args CATEGORY)
  set(multi_value_args STRINGS)
  cmake_parse_arguments(alloctools_OPTION "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  if(NOT DEFINED ${option})
    set(${option} ${default} CACHE ${type} "${description}" FORCE)
    if(alloctools_OPTION_ADVANCED)
      mark_as_advanced(${option})
    endif()
  else()
    # make sure that dependent projects can overwrite any of the GHEX options
    unset(${option} PARENT_SCOPE)

    get_property(_option_is_cache_property CACHE "${option}" PROPERTY TYPE SET)
    if(NOT _option_is_cache_property)
      set(${option} ${default} CACHE ${type} "${description}" FORCE)
      if(alloctools_OPTION_ADVANCED)
        mark_as_advanced(${option})
      endif()
    else()
      set_property(CACHE "${option}" PROPERTY HELPSTRING "${description}")
      set_property(CACHE "${option}" PROPERTY TYPE "${type}")
    endif()
  endif()

  if(alloctools_OPTION_STRINGS)
    if("${type}" STREQUAL "STRING")
      set_property(CACHE "${option}" PROPERTY STRINGS "${alloctools_OPTION_STRINGS}")
    else()
      message(FATAL_ERROR "alloctools_option(): STRINGS can only be used if type is STRING !")
    endif()
  endif()

  set(_category "Generic")
  if(alloctools_OPTION_CATEGORY)
    set(_category "${alloctools_OPTION_CATEGORY}")
  endif()
  set(${option}Category ${_category} CACHE INTERNAL "")
endfunction()

