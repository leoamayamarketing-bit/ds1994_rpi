cmake_minimum_required(VERSION 3.14)
project(ds1994_framework VERSION 2.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DS1994_BUILD_EXAMPLES "Build example programs" ON)
option(DS1994_ENABLE_WARNINGS "Enable compiler warnings" ON)

if(DS1994_ENABLE_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)
    endif()
endif()

add_subdirectory(src)

if(DS1994_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

install(TARGETS ds1994_app DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY scripts/ DESTINATION scripts)
install(FILES README.md CHANGELOG.md LICENSE DESTINATION share/doc/ds1994)
install(DIRECTORY docs/ DESTINATION share/doc/ds1994)
