cmake_minimum_required(VERSION 3.10.2)

if (NOT DEFINED ENV{GITHUB_RUN_NUMBER})
    set(ENV{GITHUB_RUN_NUMBER} 1)
endif()
project(async VERSION 0.0.$ENV{GITHUB_RUN_NUMBER})
find_package(Threads REQUIRED)
set(CXX_STANDARD 14)
configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

add_library(async SHARED ${PROJECT_SOURCE_DIR}/src/CmdReader.cpp 
                         ${PROJECT_SOURCE_DIR}/src/Logger.cpp 
                         ${PROJECT_SOURCE_DIR}/src/Context.cpp 
                         ${PROJECT_SOURCE_DIR}/src/async.cpp)

add_executable(async_cli ${PROJECT_SOURCE_DIR}/main.cpp)
set_target_properties(async PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wall;-Wunused-parameter;-Wextra;-pedantic;-Werror"
)
set_target_properties(async_cli PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wall;-Wunused-parameter;-Wextra;-pedantic;-Werror"
)
target_link_libraries(async_cli -fsanitize=address
    ${CMAKE_THREAD_LIBS_INIT}
    async
)
install(TARGETS async_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
