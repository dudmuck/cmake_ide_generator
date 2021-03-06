cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE Debug)

# set the project name
project(cmake_ide_generator VERSION 0.1)

find_package(LibXml2 REQUIRED)

add_compile_options(-Wall)

add_library(json_eclipse_cdt STATIC json_eclipse_cdt.c board-mcu-hack.c xml.c)
set_property(TARGET json_eclipse_cdt PROPERTY C_STANDARD 99)
target_link_libraries(json_eclipse_cdt json-c ${LIBXML2_LIBRARIES})
target_include_directories(json_eclipse_cdt PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})

# add the executable
add_executable(query-cmake query-cmake.c)
add_executable(sw4stm32-parse-reply sw4stm32-parse-reply.c)
add_executable(ccs-parse-reply ccs-parse-reply.c)
add_executable(stm32cubeide-parse-reply stm32cubeide-parse-reply.c)
add_executable(e2studio-parse-reply e2studio-parse-reply.c)
add_executable(nxp-parse-reply nxp-parse-reply.c)
add_executable(uvision-parse-reply uvision-parse-reply.c board-mcu-hack.c mdk-startup.c)
add_executable(cdt-xml-compare cdt_xml_compare.c xml.c)

target_include_directories(cdt-xml-compare PUBLIC ${LIBXML2_INCLUDE_DIR})
target_link_libraries(cdt-xml-compare ${LIBXML2_LIBRARIES})

set_property(TARGET sw4stm32-parse-reply PROPERTY C_STANDARD 99)
set_property(TARGET ccs-parse-reply PROPERTY C_STANDARD 99)
set_property(TARGET stm32cubeide-parse-reply PROPERTY C_STANDARD 99)
set_property(TARGET e2studio-parse-reply PROPERTY C_STANDARD 99)
set_property(TARGET nxp-parse-reply PROPERTY C_STANDARD 99)
set_property(TARGET uvision-parse-reply PROPERTY C_STANDARD 99)
set_property(TARGET cdt-xml-compare PROPERTY C_STANDARD 99)

target_link_libraries(sw4stm32-parse-reply json_eclipse_cdt)
target_link_libraries(ccs-parse-reply json_eclipse_cdt)
target_link_libraries(stm32cubeide-parse-reply json_eclipse_cdt)
target_link_libraries(e2studio-parse-reply json_eclipse_cdt)
target_link_libraries(nxp-parse-reply json_eclipse_cdt)
target_link_libraries(uvision-parse-reply json-c ${LIBXML2_LIBRARIES})

target_include_directories(query-cmake PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(sw4stm32-parse-reply PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})
target_include_directories(ccs-parse-reply PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})
target_include_directories(stm32cubeide-parse-reply PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})
target_include_directories(e2studio-parse-reply PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})
target_include_directories(nxp-parse-reply PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})
target_include_directories(uvision-parse-reply PUBLIC "${PROJECT_BINARY_DIR}" ${LIBXML2_INCLUDE_DIR})

install(TARGETS query-cmake
    stm32cubeide-parse-reply
    sw4stm32-parse-reply
    uvision-parse-reply
    ccs-parse-reply
    e2studio-parse-reply
    nxp-parse-reply
)

if (CMAKE_HOST_WIN32)
    install(CODE
        [[file(GET_RUNTIME_DEPENDENCIES EXECUTABLES sw4stm32-parse-reply.exe)]]
    )
    set(CPACK_GENERATOR NSIS)
    # this utility intended to be run from command line, put onto PATH
    set(CPACK_NSIS_MODIFY_PATH ON)
elseif(CMAKE_HOST_UNIX)
    set(CPACK_GENERATOR DEB)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "WR")

    # autogenerate dependency information
    set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif()

include(CPack)

