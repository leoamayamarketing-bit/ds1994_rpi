set(SOURCES
    ds1994.cpp
)

add_library(ds1994 STATIC ${SOURCES})

target_include_directories(ds1994 PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ds1994 PUBLIC pthread)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ds1994 PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(ds1994_app main.cpp)
target_link_libraries(ds1994_app PRIVATE ds1994)
target_include_directories(ds1994_app PRIVATE ${CMAKE_SOURCE_DIR}/include)

install(TARGETS ds1994_app DESTINATION bin)
install(TARGETS ds1994 DESTINATION lib)
