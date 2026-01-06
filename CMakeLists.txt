cmake_minimum_required(VERSION 3.16)
project(dwl VERSION 0.8)

# --- Dependencies ---
find_package(PkgConfig REQUIRED)

# Wayland, xkbcommon, libinput
pkg_check_modules(WAYLAND REQUIRED wayland-server)
pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
pkg_check_modules(LIBINPUT REQUIRED libinput)

# wlroots
pkg_check_modules(WLR REQUIRED wlroots-0.19)

# XWayland (optional)
option(ENABLE_XWAYLAND "Enable XWayland support" OFF)
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
set(CONFIG_HEADER_DEFAULT ${CMAKE_CURRENT_SOURCE_DIR}/src/config.def.h)
if(NOT EXISTS ${CONFIG_HEADER})
    message(STATUS "Copying ${CONFIG_HEADER_DEFAULT} to ${CONFIG_HEADER}")
    file(COPY ${CONFIG_HEADER_DEFAULT} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src)
endif()

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
add_executable(dwl src/dwl.c src/util.c ${GENERATED_HEADERS})

# --- Include Directories ---
target_include_directories(dwl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GENERATED_HEADERS_DIR}
    ${WAYLAND_INCLUDE_DIRS}
    ${XKBCOMMON_INCLUDE_DIRS}
    ${LIBINPUT_INCLUDE_DIRS}
    ${WLR_INCLUDE_DIRS}
)
if(ENABLE_XWAYLAND)
    target_include_directories(dwl PRIVATE
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

target_compile_definitions(dwl PRIVATE
    -D_POSIX_C_SOURCE=200809L
    -DWLR_USE_UNSTABLE
    -DVERSION="${GIT_VERSION}"
)
target_compile_options(dwl PRIVATE
    -Wpedantic
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wshadow
    -Wunused-macros
    -Werror=strict-prototypes
    -Werror=implicit
    -Werror=return-type
    -Werror=incompatible-pointer-types
    -Wfloat-conversion
)

# --- Link Libraries ---
target_link_libraries(dwl PRIVATE
    ${WAYLAND_LIBRARIES}
    ${XKBCOMMON_LIBRARIES}
    ${LIBINPUT_LIBRARIES}
    ${WLR_LIBRARIES}
    m
)
if(ENABLE_XWAYLAND)
    target_link_libraries(dwl PRIVATE
        ${XCB_LIBRARIES}
        ${XCB_ICCCM_LIBRARIES}
    )
endif()

# --- Installation ---
install(TARGETS dwl DESTINATION bin)
install(FILES src/dwl.1 DESTINATION share/man/man1)
install(FILES src/dwl.desktop DESTINATION share/wayland-sessions)

# --- .clang-format ---
# Copy .clang-format to build directory for editor integration
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.clang-format ${CMAKE_CURRENT_BINARY_DIR}/.clang-format COPYONLY)