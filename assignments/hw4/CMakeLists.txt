cmake_minimum_required(VERSION 3.14)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(YGM-Application
        VERSION 0.1
        DESCRIPTION "YGM Appliction"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)


#
#  Threads
#
find_package(Threads REQUIRED)


#
#  MPI
find_package(MPI)

#
#  YGM
if (MPI_CXX_FOUND)
  find_package(YGM QUIET)
  if (NOT YGM_FOUND)
      set(JUST_INSTALL_YGM TRUE)
      FetchContent_Declare(YGM
          GIT_REPOSITORY https://github.com/llnl/ygm.git
          GIT_TAG v0.7
              )
      FetchContent_MakeAvailable(YGM)
  endif ()
endif ()


#
# Generate compile_commands.json
#
if (NOT CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_EXPORT_COMPILE_COMMANDS On)
  message(STATUS "Setting CMAKE_EXPORT_COMPILE_COMMANDS to On")
endif ()

#
# Default to a Release build
#
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
  message(STATUS "CMAKE_BUILD_TYPE is set as Release")
endif ()

#
#  Common functions for all YGM Executables
#
function(add_common_compile_options name)
    # Common
    #target_compile_options(${name} PRIVATE -Wall -Wextra -pedantic)

    # Debug
    target_compile_options(${name} PRIVATE $<$<CONFIG:Debug>:-O0>)
    target_compile_options(${name} PRIVATE $<$<CONFIG:Debug>:-g3>)
#    if (Linux)
#        target_compile_options(${name} PRIVATE $<$<CONFIG:Debug>:-pg>)
#    endif ()

    # Release
    target_compile_options(${name} PRIVATE $<$<CONFIG:Release>:-Ofast>)
    target_compile_options(${name} PRIVATE $<$<CONFIG:Release>:-DNDEBUG>)

    # Release with debug info
    target_compile_options(${name} PRIVATE $<$<CONFIG:RelWithDebInfo>:-Ofast>)
    target_compile_options(${name} PRIVATE $<$<CONFIG:RelWithDebInfo>:-g3>)
#    if (Linux)
#        target_compile_options(${name} PRIVATE $<$<CONFIG:RelWithDebInfo>:-pg>)
#    endif ()
endfunction()

#
# Function for setting up a target that uses YGM
#
function(setup_ygm_target exe_name)
    if (NOT MPI_CXX_FOUND)
        message(WARNING "YGM requires MPI; skip building ${exe_name}.")
        return()
    endif ()

    target_link_libraries(${exe_name} PRIVATE MPI::MPI_CXX)
    target_link_libraries(${exe_name} PRIVATE cereal)
    target_link_libraries(${exe_name} PRIVATE Threads::Threads)
    if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(${exe_name} PRIVATE rt)
    endif ()
    target_link_libraries(${exe_name} PRIVATE ygm::ygm)
endfunction()

function(add_ygm_executable name)
    add_executable(${name} ${name}.cpp)
    add_common_compile_options(${name})
    target_include_directories(${name} PRIVATE ${PROJECT_SOURCE_DIR}/include ${BOOST_INCLUDE_DIRS})
    setup_ygm_target(${name})
endfunction()

add_ygm_executable(bfs_example)
add_ygm_executable(delta_step_sssp)