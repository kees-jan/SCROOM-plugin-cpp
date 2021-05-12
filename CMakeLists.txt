cmake_minimum_required(VERSION 3.15)
project(ScroomCPPPlugins LANGUAGES CXX)

# Collect all libraries and binaries in a single location
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_subdirectory(scroom)

if(ENABLE_BOOST_TEST OR ENABLE_GOOGLE_TEST)
    enable_testing()
endif()

add_subdirectory(plugins)

# Generate the ENVIRONMENT file
set(abs_top_builddir "${ScroomCPPPlugins_BINARY_DIR}")
configure_file(ENVIRONMENT.in ${ScroomCPPPlugins_BINARY_DIR}/ENVIRONMENT @ONLY)
