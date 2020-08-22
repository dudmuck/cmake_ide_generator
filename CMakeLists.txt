cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE Debug)

# set the project name
project(cmake_ide_generator VERSION 0.1)

find_package(LibXml2 REQUIRED)

add_library(json_eclipse_cdt STATIC json_eclipse_cdt.c board-mcu-hack.c)
set_property(TARGET json_eclipse_cdt PROPERTY C_STANDARD 99)
target_link_libraries(json_eclipse_cdt json-c ${LIBXML2_LIBRARIES})
target_include_directories(json_eclipse_cdt PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})

# add the executable
add_executable(query-cmake query-cmake.c)
add_executable(sw4stm32-parse-reply sw4stm32-parse-reply.c)
add_executable(stm32cubeide-parse-reply stm32cubeide-parse-reply.c)

set_property(TARGET sw4stm32-parse-reply PROPERTY C_STANDARD 99)
set_property(TARGET stm32cubeide-parse-reply PROPERTY C_STANDARD 99)

target_link_libraries(sw4stm32-parse-reply json_eclipse_cdt)
target_link_libraries(stm32cubeide-parse-reply json_eclipse_cdt)

target_include_directories(query-cmake PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(sw4stm32-parse-reply PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})
target_include_directories(stm32cubeide-parse-reply PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})

install(TARGETS query-cmake stm32cubeide-parse-reply sw4stm32-parse-reply)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "WR")

# autogenerate dependency information
set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
