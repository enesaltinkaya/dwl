cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # for clangd
set(PROJECT_NAME dwl)


# -Wunused-const-variable

set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # for clangd
project(${PROJECT_NAME} C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color -Wall -Wextra -Wpedantic -pedantic-errors")

# --- Dependencies ---
find_package(PkgConfig REQUIRED)

# Wayland, xkbcommon, libinput
pkg_check_modules(WAYLAND REQUIRED wayland-server)
pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
pkg_check_modules(LIBINPUT REQUIRED libinput)

# wlroots
pkg_check_modules(WLR REQUIRED wlroots-0.19)

# XWayland (optional)
option(ENABLE_XWAYLAND "Enable XWayland support" ON)
if(ENABLE_XWAYLAND)
    pkg_check_modules(XCB REQUIRED xcb)
    pkg_check_modules(XCB_ICCCM REQUIRED xcb-icccm)
    add_definitions(-DXWAYLAND)
endif()

# Wayland protocols
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
find_program(WAYLAND_SCANNER NAMES wayland-scanner REQUIRED)

# --- Configuration Header ---
set(CONFIG_HEADER ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

# --- Protocol Header Generation ---
set(PROTOCOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols)
set(GENERATED_HEADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/src)

# Wayland Protocols
add_custom_command(
    OUTPUT ${GENERATED_HEADERS_DIR}/cursor-shape-v1-protocol.h
    COMMAND ${WAYLAND_SCANNER} enum-header ${WAYLAND_PROTOCOLS_PREFIX}/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml ${GENERATED_HEADERS_DIR}/cursor-shape-v1-protocol.h
    DEPENDS ${WAYLAND_PROTOCOLS_PREFIX}/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml
)
add_custom_command(
    OUTPUT ${GENERATED_HEADERS_DIR}/pointer-constraints-unstable-v1-protocol.h
    COMMAND ${WAYLAND_SCANNER} enum-header ${WAYLAND_PROTOCOLS_PREFIX}/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml ${GENERATED_HEADERS_DIR}/pointer-constraints-unstable-v1-protocol.h
    DEPENDS ${WAYLAND_PROTOCOLS_PREFIX}/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml
)
add_custom_command(
    OUTPUT ${GENERATED_HEADERS_DIR}/xdg-shell-protocol.h
    COMMAND ${WAYLAND_SCANNER} server-header ${WAYLAND_PROTOCOLS_PREFIX}/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml ${GENERATED_HEADERS_DIR}/xdg-shell-protocol.h
    DEPENDS ${WAYLAND_PROTOCOLS_PREFIX}/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
)

# dwl protocols
add_custom_command(
    OUTPUT ${GENERATED_HEADERS_DIR}/wlr-layer-shell-unstable-v1-protocol.h
    COMMAND ${WAYLAND_SCANNER} enum-header ${PROTOCOLS_DIR}/wlr-layer-shell-unstable-v1.xml ${GENERATED_HEADERS_DIR}/wlr-layer-shell-unstable-v1-protocol.h
    DEPENDS ${PROTOCOLS_DIR}/wlr-layer-shell-unstable-v1.xml
)
add_custom_command(
    OUTPUT ${GENERATED_HEADERS_DIR}/wlr-output-power-management-unstable-v1-protocol.h
    COMMAND ${WAYLAND_SCANNER} server-header ${PROTOCOLS_DIR}/wlr-output-power-management-unstable-v1.xml ${GENERATED_HEADERS_DIR}/wlr-output-power-management-unstable-v1-protocol.h
    DEPENDS ${PROTOCOLS_DIR}/wlr-output-power-management-unstable-v1.xml
)

set(GENERATED_HEADERS
    ${GENERATED_HEADERS_DIR}/cursor-shape-v1-protocol.h
    ${GENERATED_HEADERS_DIR}/pointer-constraints-unstable-v1-protocol.h
    ${GENERATED_HEADERS_DIR}/xdg-shell-protocol.h
    ${GENERATED_HEADERS_DIR}/wlr-layer-shell-unstable-v1-protocol.h
    ${GENERATED_HEADERS_DIR}/wlr-output-power-management-unstable-v1-protocol.h
)

# --- Executable ---
add_executable(${PROJECT_NAME} src/dwl.c src/util.c ${GENERATED_HEADERS})

# --- Include Directories ---
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GENERATED_HEADERS_DIR}
    ${WAYLAND_INCLUDE_DIRS}
    ${XKBCOMMON_INCLUDE_DIRS}
    ${LIBINPUT_INCLUDE_DIRS}
    ${WLR_INCLUDE_DIRS}
)
if(ENABLE_XWAYLAND)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${XCB_INCLUDE_DIRS}
        ${XCB_ICCCM_INCLUDE_DIRS}
    )
endif()



# --- Compiler Flags ---
# Get version from git
execute_process(
    COMMAND git describe --tags --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT GIT_VERSION)
    set(GIT_VERSION ${project_VERSION})
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -D_POSIX_C_SOURCE=200809L
    -DWLR_USE_UNSTABLE
    -DVERSION="${GIT_VERSION}"
)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wpedantic
    -Wall
    -Wextra
    -Wno-unused-parameter
    -W-no-unused-const-variable
    -Wshadow
    -Wunused-macros
    -Werror=strict-prototypes
    -Werror=implicit
    -Werror=return-type
    -Werror=incompatible-pointer-types
    -Wfloat-conversion
)

if (WIN32)
    set(dir "build-win")
elseif (LINUX)
    set(dir "build-linux")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(tag "release")
else ()
    set(tag "debug")
endif ()


set(thirdparty "/home/enes/project/c/cpp-thirdparty")
target_include_directories(${PROJECT_NAME} PUBLIC ${thirdparty}/c-utils/c-utils/src)
target_precompile_headers(${PROJECT_NAME} PUBLIC "${thirdparty}/c-utils/c-utils/src/Defines.h")
target_precompile_headers(${PROJECT_NAME} PUBLIC "${thirdparty}/c-utils/c-utils/src/logger/Logger.h")


# --- Link Libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${WAYLAND_LIBRARIES}
    ${XKBCOMMON_LIBRARIES}
    ${LIBINPUT_LIBRARIES}
    ${WLR_LIBRARIES}
    m
    ${thirdparty}/c-utils/c-utils/${dir}/libc-utils-${tag}.a

    #zip
    ${thirdparty}/libzip/git/${dir}/lib/libzip.a
    ${thirdparty}/bzip2/git/${dir}/libbz2_static.a
    ${thirdparty}/zlib/git/${dir}/libzs.a
    ${thirdparty}/zstd/git/${dir}/lib/libzstd.a


    #tools
    ${thirdparty}/stb/git/${dir}/libstb.a
    ${thirdparty}/lua/git/${dir}/liblua.a
    ${thirdparty}/jansson/git/${dir}/lib/libjansson.a
    ${thirdparty}/thpool/${dir}/libthpool.a
    ${thirdparty}/sqlite/git/${dir}/libsqlite3.a
    ${thirdparty}/buddy_alloc/git/${dir}/libbuddy_alloc.a
    ${thirdparty}/freetype/git/${dir}/libfreetype.a
    ${thirdparty}/harfbuzz/git/${dir}/libharfbuzz.a
    ${thirdparty}/png/git/${dir}/libpng.a
    ${thirdparty}/brotli/git/${dir}/libbrotlidec.a
    ${thirdparty}/brotli/git/${dir}/libbrotlicommon.a

)
if(ENABLE_XWAYLAND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${XCB_LIBRARIES}
        ${XCB_ICCCM_LIBRARIES}
    )
endif()

# --- Installation ---
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES src/dwl.1 DESTINATION share/man/man1)
install(FILES src/dwl.desktop DESTINATION share/wayland-sessions)

# --- .clang-format ---
# Copy .clang-format to build directory for editor integration
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.clang-format ${CMAKE_CURRENT_BINARY_DIR}/.clang-format COPYONLY)