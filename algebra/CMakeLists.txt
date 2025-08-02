# Header-only library for algebra classes
add_library(algebra INTERFACE)

# Set include directories for the library
# This makes #include "algebra/TreeAlgebra.hh" work from anywhere
target_include_directories(algebra INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include>
)

# Specify the header files (for IDE support)
target_sources(algebra INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Algebra.hh>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/TreeAlgebra.hh>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DoubleAlgebra.hh>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/StringAlgebra.hh>
)