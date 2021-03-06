cmake_minimum_required(VERSION 2.8)
project(CPF C CXX)

# Prefix
if (NOT PREFIX)
    set(PREFIX ${CMAKE_CURRENT_BINARY_DIR})
endif (NOT PREFIX)
set(CMAKE_INSTALL_PREFIX ${PREFIX} CACHE STRING "Installation prefix." FORCE)

# Release or Debug
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif ()


# Detect MPI
set(MPI_LIBS)
find_package(MPI REQUIRED)
if (MPI_C_FOUND)
    include_directories(${MPI_C_INCLUDE_PATH})
    add_definitions(-DOMPI_SKIP_MPICXX)
    set (MPI_LIBS ${MPI_LIBS} ${MPI_C_LIBRARIES})
else (MPI_C_FOUND)
    message(STATUS "MPI not found.")
endif (MPI_C_FOUND)

# Detect OMP
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS ${CMAKE_C_CFLAGS} ${OpenMP_C_FLAGS})
    set (CMAKE_CXX_FLAGS ${CMAKE_CXX_CFLAGS} ${OpenMP_CXX_FLAGS})
else (OPENMP_FOUND)
    message(STATUS "OpenMP not found.")
endif (OPENMP_FOUND)

# Detect PNL
find_package(Pnl)
set(LIBS ${LIBS} ${PNL_LIBRARIES})
include_directories(${PNL_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-DPNL_RANGE_CHECK_OFF)
endif()

# Add math lib
set(LIBS ${LIBS} m)

# To add an excutable
add_executable(deadlock deadlock.c)
target_link_libraries(deadlock ${LIBS} ${MPI_LIBS})

# Print compiler flags
get_directory_property(DirDefs COMPILE_DEFINITIONS)
message(STATUS "COMPILE_DEFINITIONS = ${DirDefs}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}" )
    message(STATUS "C Flags: ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}" )
else()
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}" )
    message(STATUS "C Flags: ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}" )
endif()
