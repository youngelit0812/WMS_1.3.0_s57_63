cmake_minimum_required(VERSION 3.10)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configuration types" FORCE)

if(WIN32)
  set(CMAKE_GENERATOR_PLATFORM "Win32" CACHE STRING "Platform" FORCE)
endif()

set(OpenGL_GL_PREFERENCE "LEGACY")
set(PACKAGE_NAME WMSServer)

add_definitions("-DRAPIDJSON_HAS_STDSTRING=1")

project(WMSServer)

set(OPENGLES_FOUND "NO")
set(OPENGL_FOUND "YES")

# add_definitions(-DPRINTLOG_DEBUG)
# Global properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

macro(BUNDLE_DOCS_OPT onoff)
  option(OCPN_BUNDLE_DOCS "Include documentation in package" ${onoff})
endmacro(BUNDLE_DOCS_OPT)

macro(BUNDLE_TCDATA_OPT onoff)
  option(OCPN_BUNDLE_TCDATA "Include tide/current harmonics data in package" ${onoff})
endmacro(BUNDLE_TCDATA_OPT)

macro(BUNDLE_GSHHS_OPT onoff)
  option(OCPN_BUNDLE_GSHHS "Bundle minimal GSHHS data files" ${onoff})
endmacro(BUNDLE_GSHHS_OPT)

bundle_gshhs_opt("ON")
bundle_tcdata_opt("ON")
bundle_docs_opt("OFF")

if(UNIX)
  message("set Linux flag!!!")
  set(LINUX "1")
endif()

set(CMAKE_BUILD_TYPE Release)

# CMake module path
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Compiler features
include(SetCompilerFeatures)
include(SetCompilerWarnings)
include(SetPlatformFeatures)
include(SystemInformation)

# External packages
if(MSVC)
  set(OPENSSL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/modules/OpenSSL/VS")
  set(OPENSSL_USE_STATIC_LIBS TRUE)
  set(OPENSSL_MSVC_STATIC_RT TRUE)
endif()

if(WIN32)
  message("set x86 library----")
  set(FIND_LIBRARY_USE_LIB32_PATHS TRUE)
endif()

find_package(OpenSSL REQUIRED)

if(WIN32)
  find_package(Crypt)
  find_package(WinSock)
endif()

# Modules
add_subdirectory("modules")

if(MSVC)
  target_compile_options(asio PUBLIC
    $<$<CONFIG:Debug>:/MDd>
    $<$<CONFIG:Release>:/MD>
    $<$<PLATFORM_ID:Win32>:/DWIN32_LEAN_AND_MEAN>
  )
endif()

# Link libraries
list(APPEND LINKLIBS ${OPENSSL_LIBRARIES})

if(WIN32)
  list(APPEND LINKLIBS ${CRYPT_LIBRARIES})
  list(APPEND LINKLIBS ${WINSOCK_LIBRARIES})
endif()

list(APPEND LINKLIBS cppcommon)

if(MSVC)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/gshhs)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/tcdata)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/uidata)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/s57data)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/SENC)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/ENCs)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/env)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/tools)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/ENC_IMGs)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release/s63_files)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/gshhs)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/tcdata)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/uidata)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/s57data)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/SENC)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/ENCs)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/env)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/tools)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/ENC_IMGs)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/s63_files)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_libs/Release
    ${CMAKE_BINARY_DIR}/Release)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/env
    ${CMAKE_BINARY_DIR}/Release/env)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/gshhs
    ${CMAKE_BINARY_DIR}/Release/gshhs)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/tcdata
    ${CMAKE_BINARY_DIR}/Release/tcdata)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/uidata
    ${CMAKE_BINARY_DIR}/Release/uidata)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/s57data
    ${CMAKE_BINARY_DIR}/Release/s57data)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/tools
    ${CMAKE_BINARY_DIR}/Release/tools)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/s63_files
    ${CMAKE_BINARY_DIR}/Release/s63_files)

  file(COPY ${CMAKE_SOURCE_DIR}/Append_data/navobj.xml DESTINATION ${CMAKE_BINARY_DIR}/Release)
  file(COPY ${CMAKE_SOURCE_DIR}/Append_data/wmsserver.conf DESTINATION ${CMAKE_BINARY_DIR}/Release)
  file(COPY ${CMAKE_SOURCE_DIR}/Append_data/MMSINAME.CSV DESTINATION ${CMAKE_BINARY_DIR}/Release)
  file(COPY ${CMAKE_SOURCE_DIR}/Append_data/crashrpt_lang.ini DESTINATION ${CMAKE_BINARY_DIR}/Release)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_libs/Debug
    ${CMAKE_BINARY_DIR}/Debug)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/env
    ${CMAKE_BINARY_DIR}/Debug/env)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/gshhs
    ${CMAKE_BINARY_DIR}/Debug/gshhs)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/tcdata
    ${CMAKE_BINARY_DIR}/Debug/tcdata)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/uidata
    ${CMAKE_BINARY_DIR}/Debug/uidata)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/s57data
    ${CMAKE_BINARY_DIR}/Debug/s57data)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/tools
    ${CMAKE_BINARY_DIR}/Debug/tools)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Append_data/s63_files
    ${CMAKE_BINARY_DIR}/Debug/s63_files)

  file(COPY ${CMAKE_SOURCE_DIR}/Append_data/navobj.xml DESTINATION ${CMAKE_BINARY_DIR}/Debug)
  file(COPY ${CMAKE_SOURCE_DIR}/Append_data/wmsserver.conf DESTINATION ${CMAKE_BINARY_DIR}/Debug)
  file(COPY ${CMAKE_SOURCE_DIR}/Append_data/MMSINAME.CSV DESTINATION ${CMAKE_BINARY_DIR}/Debug)
  file(COPY ${CMAKE_SOURCE_DIR}/Append_data/crashrpt_lang.ini DESTINATION ${CMAKE_BINARY_DIR}/Debug)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ENC_IMGs)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/env)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/env
  ${CMAKE_BINARY_DIR}/env)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/s57data)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/Append_data/s57data
  ${CMAKE_BINARY_DIR}/s57data)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gshhs)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/Append_data/gshhs
  ${CMAKE_BINARY_DIR}/gshhs)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tcdata)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/Append_data/tcdata
  ${CMAKE_BINARY_DIR}/tcdata)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/uidata)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/Append_data/uidata
  ${CMAKE_BINARY_DIR}/uidata)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tools)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/Append_data/tools
  ${CMAKE_BINARY_DIR}/tools)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/s63_files)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/Append_data/s63_files
  ${CMAKE_BINARY_DIR}/s63_files)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/SENC)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ENCs)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib/plugins)

file(COPY ${CMAKE_SOURCE_DIR}/Append_data/navobj.xml DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Append_data/wmsserver.conf DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Append_data/MMSINAME.CSV DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Append_data/crashrpt_lang.ini DESTINATION ${CMAKE_BINARY_DIR})

# OpenSSL libraries
message(STATUS "OpenSSL version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")

# System directories
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/modules")

if(LINUX)
  if(DEFINED DEB_VERSION)
    add_definitions("-DDEB_VERSION=${DEB_VERSION}")
  endif(DEFINED DEB_VERSION)

  if(NOT DEFINED OCPN_VERBOSE OR OCPN_VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
  endif()

  set(
    CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/build
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/cmake
  )

  set(CMAKE_CXX_VISIBILITY_PRESET=hidden)

  include(FindPkgConfig)
  include(CMakeDependentOption)
  include(CheckSymbolExists)
endif()

option(OCPN_VERBOSE "Make verbose builds" ON)
option(OCPN_PEDANTIC "Enable more compiler warnings" OFF)

set(OCPN_PACKAGE_RELEASE "1" CACHE STRING "Package release number")

option(OCPN_USE_GL "Enable OpenGL support" ON)
option(OCPN_USE_GLES_SHIM "Enable GLESv1 Shim library" OFF)
option(OCPN_USE_UDEV_PORTS "Use udev to enumerate serial ports" ON)
option(OCPN_USE_SYSFS_PORTS "Use sysfs to enumerate serial ports" OFF)

set(OCPN_TARGET_TUPLE "" CACHE STRING
  "List of target;version;arch like flatpak;20.08;aarch64 [\"\"]"
)

if(MSVC OR MINGW)
  option(OCPN_USE_CRASHREPORT "Enable crash reporting" ON)
else()
  set(OCPN_USE_CRASHREPORT FALSE)
endif()

if(NOT WIN32 AND NOT APPLE AND NOT QT_ANDROID)
  find_library(GTK2U NAMES wx_gtk2u_core)

  if(GTK2U)
    option(OCPN_FORCE_GTK3 "Force the build to use GTK3 [OFF]" "OFF")
  else()
    option(OCPN_FORCE_GTK3 "Force the build to use GTK3 [ON]" "ON")
  endif()
endif()

if(WIN32 OR APPLE OR QT_ANDROID)
  set(PA_DEFAULT "OFF")
else()
  set(PA_DEFAULT "ON")
endif()

option(
  OCPN_ENABLE_PORTAUDIO "Use portaudio(3) to play sounds if available"
  ${PA_DEFAULT}
)

if(LINUX)
  option(
    OCPN_ENABLE_PORTAUDIO "Use portaudio(3) to play sounds if available"
    ${PA_DEFAULT}
  )

  cmake_dependent_option(
    OCPN_ENABLE_SNDFILE "Use libsndfile for portaudio if available."
    ON "OCPN_ENABLE_PORTAUDIO" ON
  )

  if(${CMAKE_VERSION} VERSION_LESS 3.14)
    set(OCPN_BUILD_TEST OFF)
  else()
    option(OCPN_BUILD_TEST "Enable test subproject build" ON)
  endif()

  set(
    OCPN_WXWIDGETS_FORCE_VERSION
    CACHE STRING "Force usage of a specific wxWidgets version."
  )

  set(
    OCPN_WXWIDGETS_OPTIONS
    CACHE STRING "Additional flags to wxWidgets_CONFIG_OPTIONS"
  )

  if(CMAKE_VERSION VERSION_GREATER 3.4)
    option(OCPN_ENABLE_CLANG_TIDY "Add clang-tidy automatically to builds" OFF)
    set(ENABLE_SANITIZER "none" CACHE STRING "Add clang sanitizer to the build")
  endif()

  if(CMAKE_VERSION VERSION_GREATER 3.9)
    option(OCPN_ENABLE_CPPCHECK "Add cppcheck automatically to builds" OFF)
  endif()

  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(
      CMAKE_BUILD_TYPE RelWithDebInfo
      CACHE
      STRING
      "Choose type of build: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE
    )

    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

  option(OCPN_USE_EXTERN_CURL "Use external libcurl" OFF)

  if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX ${TENTATIVE_PREFIX})
  endif(NOT CMAKE_INSTALL_PREFIX)

  set(BUNDLED_LIBS_DEFAULT "OFF")

  set(OCPN_USE_BUNDLED_LIBS ${BUNDLED_LIBS_DEFAULT} CACHE STRING "Use bundled libraries instead of system's [ON, OFF or comma-sep list]")
  set(OCPN_RELEASE "" CACHE STRING "Hardcoded value for build number when using OCPN_CI_BUILD")

  option(OCPN_USE_NEWSERIAL "Use new serial communication implementation" ON)
  option(OCPN_USE_CURL "Use Curl libraries" ON)
  option(OCPN_USE_SVG "Use SVG graphics via wxSVG or wxBitmapBundle" ON)
  option(OCPN_USE_WEBVIEW "Use wxWidget's webview addon if available" ON)
  option(OCPN_USE_LZMA "Use LZMA for chart compression" ON)
  option(OCPN_CI_BUILD "Use CI build versioning rules" OFF)
  option(OCPN_USE_SYSTEM_LIBARCHIVE "Use the libarchive version provided by the system on MacOS" ON)
endif(LINUX)

add_definitions(-D__OCPN_USE_GLEW__)

if(LINUX)
  set(cmd_snd_default ON)

  option(
    OCPN_ENABLE_SYSTEM_CMD_SOUND
    "Use aplay(1), afplay(1) etc. to play sounds if available" ${cmd_snd_default}
  )
  set(OCPN_SOUND_CMD "" CACHE STRING
    "Hardcoded value for CLI command used in SystemCmdSound backend"
  )

  macro(USE_BUNDLED_LIB _result library)
    if("${OCPN_USE_BUNDLED_LIBS}" STREQUAL "")
      set(OCPN_USE_BUNDLED_LIBS "ON")
    endif()

    if(${OCPN_USE_BUNDLED_LIBS} STREQUAL "OFF")
      set(${_result} "OFF")
    elseif(${OCPN_USE_BUNDLED_LIBS} STREQUAL "ON")
      set(${_result} "ON")
    elseif(",${OCPN_USE_BUNDLED_LIBS}," MATCHES ",${library},")
      set(${_result} "ON")
    else()
      set(${_result} "OFF")
    endif()
  endmacro(USE_BUNDLED_LIB)

  include(GetArch)
  getarch()
  message(STATUS "*** Host Build Architecture is ${ARCH}")

  message(STATUS "Setting C++11 standard via cmake standard mechanism")

  # ##############
  # set(OBJ_VISIBILITY "-fvisibility=hidden")
  # ##############
  if(CMAKE_VERSION VERSION_GREATER 3.4)
    if(OCPN_ENABLE_CLANG_TIDY)
      find_program(
        CLANG_TIDY_EXE
        NAMES "clang-tidy"
        PATHS /usr/local/opt/llvm/bin
      )

      if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")

        set(CLANG_TIDY_CHECKS "-*,performance-*")
        set(
          CMAKE_CXX_CLANG_TIDY
          "${CLANG_TIDY_EXE};-checks=${CLANG_TIDY_CHECKS};-header-filter='${CMAKE_SOURCE_DIR}/*'"
          CACHE STRING ""
          FORCE
        )
      else()
        message(AUTHOR_WARNING "clang-tidy not found!")
        set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE) # delete it
      endif()
    endif()
  endif()

  if(CMAKE_VERSION VERSION_GREATER 3.4)
    set_property(
      CACHE ENABLE_SANITIZER
      PROPERTY STRINGS none address memory thread undefined
    )

    if(NOT "${ENABLE_SANITIZER}" MATCHES "none")
      add_compile_options(-fsanitize=${ENABLE_SANITIZER})
    endif()
  endif()

  if(CMAKE_VERSION VERSION_GREATER 3.9)
    if(OCPN_ENABLE_CPPCHECK)
      find_program(CPPCHECK_EXECUTABLE NAMES "cppcheck")
      set(CMAKE_CXX_CPPCHECK ${CPPCHECK_EXECUTABLE})
    endif()
  endif()

  add_compile_options(
    "-Wall"
    "-Warray-bounds=0"
  )

  if(NOT OCPN_PEDANTIC)
    add_compile_options(
      "-Wno-unused"
      "-fexceptions"
      "-rdynamic"
      "-fno-strict-aliasing"
      "-Wno-deprecated-declarations"
    )
  endif()

  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options("-O0")
  endif()

  add_definitions(" -DPREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\"")

  set(
    LINUX_LIB_PATHS
    /usr/local/lib
    /usr/local/lib64
    /usr/lib/i386-linux-gnu
    /usr/lib/x86_64-linux-gnu
    /usr/lib
    /usr/lib64
  )

  if(EXISTS /etc/debian_version AND NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
  endif()

  message(STATUS "CMAKE_INSTALL_LIBDIR2 ${CMAKE_INSTALL_LIBDIR}")
  include(GNUInstallDirs)
  message(STATUS "CMAKE_INSTALL_LIBDIR3  ${CMAKE_INSTALL_LIBDIR}")

  set(PREFIX_BIN ${CMAKE_INSTALL_BINDIR})
  set(PREFIX_INCLUDE ${CMAKE_INSTALL_INCLUDEDIR})
  set(PREFIX_DATA ${CMAKE_INSTALL_DATADIR})
  set(PREFIX_PKGDATA ${CMAKE_INSTALL_DATADIR}/${PACKAGE_NAME})
  set(PREFIX_LIB "${CMAKE_INSTALL_FULL_LIBDIR}")

  include(${CMAKE_SOURCE_DIR}/VERSION.cmake)

  string(TIMESTAMP VERSION_DATE "%Y-%m-%d")
  string(TIMESTAMP UNIX_TIMESTAMP "%s")

  if(OCPN_CI_BUILD)
    include(Utils)

    commit_id(COMMIT)
    build_num(BUILD_NUMBER)

    if(NOT "${OCPN_RELEASE}" STREQUAL "")
      set(BUILD_NUMBER ${OCPN_RELEASE})
    endif()

    set(VERSION_TAIL "+${COMMIT}")

    if(NOT "${BUILD_NUMBER}" STREQUAL "")
      set(VERSION_TAIL "-${BUILD_NUMBER}${VERSION_TAIL}")
    endif()
  endif()

  set(
    PACKAGE_VERSION
    "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_TAIL}"
  )
  message(STATUS "Building version ${PACKAGE_VERSION}")

  include(OcpnFindGtk)

  if(ARCH MATCHES "arm*")
    add_definitions(-DocpnARM)

    if(ARCH STREQUAL "armhf")
      add_definitions(-DocpnARMHF)
    endif()
  endif()

  if(DEFINED _wx_selected_config)
    if(_wx_selected_config MATCHES "androideabi-qt")
      INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/glshim/include/GLES)
    endif(_wx_selected_config MATCHES "androideabi-qt")
  endif(DEFINED _wx_selected_config)

  if(OCPN_USE_WEBVIEW)
    set(wxWidgets_FIND_COMPONENTS net xml html adv aui core base webview)
  else()
    set(wxWidgets_FIND_COMPONENTS net xml html adv aui core base)
  endif()
endif()

if(OPENGLES_FOUND)
  set(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} gl)
endif()

if(NOT OPENGLES_FOUND)
  if(OCPN_USE_GL)
    find_package(OpenGL)
  else(OCPN_USE_GL)
    message(STATUS "OpenGL disabled by option USE_GL...")
  endif(OCPN_USE_GL)

  if(OPENGL_FOUND)
    set(wxWidgets_FIND_COMPONENTS gl ${wxWidgets_FIND_COMPONENTS})
    include_directories(${OPENGL_INCLUDE_DIR})

    message(STATUS "Found OpenGL....")
    message(STATUS "    GL Lib: " ${OPENGL_LIBRARIES})
    message(STATUS "    GL Include: " ${OPENGL_INCLUDE_DIR})
    add_definitions(-DocpnUSE_GL)
    add_definitions(-DocpnUSE_GLSL)

    # We need to remove GLU from the OPENGL_LIBRARIES list
    foreach(_currentLibFile ${OPENGL_LIBRARIES})
      # MESSAGE (STATUS "    Lib File: " ${_currentLibFile})
      set(UCNAME ${_currentLibFile})
      string(TOUPPER ${UCNAME} UCNAME)

      if(NOT ${UCNAME} MATCHES "(.*)GLU(.*)")
        set(
          REVISED_OPENGL_LIBRARIES ${_currentLibFile}
          ${REVISED_OPENGL_LIBRARIES}
        )
      endif()
    endforeach(_currentLibFile)

    set(OPENGL_LIBRARIES ${REVISED_OPENGL_LIBRARIES})
    message(STATUS "    Revised GL Lib: " ${OPENGL_LIBRARIES})

  else(OPENGL_FOUND)
    message(STATUS "OpenGL not found...")
  endif(OPENGL_FOUND)
endif()

# configure GLEW
if(OPENGL_FOUND)
  if(CMAKE_HOST_WIN32)
    message("+++******************GLEW: include directories*********************+++")
    include_directories(
      "${CMAKE_CURRENT_SOURCE_DIR}/cache/buildwin/include/glew"
    )
    link_libraries(${CMAKE_SOURCE_DIR}/cache/buildwin/glew32.lib)
  else(CMAKE_HOST_WIN32)
    message("******************GLEW: include directories*********************")

    if("${DEB_DISTRIBUTION}" STREQUAL "bionic")
      message(STATUS "Using GLEW_STATIC")
      set(GLEW_USE_STATIC_LIBS ON)
    endif("${DEB_DISTRIBUTION}" STREQUAL "bionic")

    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
  endif(CMAKE_HOST_WIN32)
endif(OPENGL_FOUND)

if(LINUX)
  set(wxWidgets_USE_DEBUG OFF)
  set(wxWidgets_USE_UNICODE ON)
  set(wxWidgets_USE_UNIVERSAL OFF)
  set(wxWidgets_USE_STATIC OFF)

  if(OCPN_WXWIDGETS_FORCE_VERSION)
    set(wxWidgets_CONFIG_OPTIONS --version=${OCPN_WXWIDGETS_FORCE_VERSION})
  endif()

  if(GTK2_FOUND)
    message("-----------GTK 2 FOUND-----------")
    set(wxWidgets_CONFIG_OPTIONS ${wxWidgets_CONFIG_OPTIONS} --toolkit=gtk2)
  elseif(GTK3_FOUND)
    message("-----------GTK 3 FOUND-----------")
    set(wxWidgets_CONFIG_OPTIONS ${wxWidgets_CONFIG_OPTIONS} --toolkit=gtk3)
  endif()

  find_package(wxWidgets COMPONENTS ${wxWidgets_FIND_COMPONENTS})

  if(wxWidgets_FOUND AND OCPN_USE_WEBVIEW)
    message(STATUS "Found wxWidgets webview add-on")
    add_definitions(-DwxUSE_WEBVIEW=1 -DHAVE_WEBVIEW)
  else()
    add_definitions(-DwxUSE_WEBVIEW=0)
    list(REMOVE_ITEM wxWidgets_FIND_COMPONENTS webview)
    message(STATUS "Could not find wxWidgets webview add-on")
    find_package(wxWidgets COMPONENTS ${wxWidgets_FIND_COMPONENTS})
  endif()

  if(NOT wxWidgets_FOUND)
    message(FATAL_ERROR
      "Cannot find wxWidgets, giving up. On linux you might want to try"
      " using -DOCPN_FORCE_GTK3."
    )
  endif()

  include(${wxWidgets_USE_FILE})

  message(STATUS "Found wxWidgets...")
  message(STATUS " wxWidgets Include: ${wxWidgets_INCLUDE_DIRS}")
  message(STATUS " wxWidgets Libraries: ${wxWidgets_LIBRARIES}")

  foreach(_currentLibFile ${wxWidgets_LIBRARIES})
    set(UCNAME ${_currentLibFile})
    string(TOUPPER ${UCNAME} UCNAME)

    if(NOT ${UCNAME} MATCHES "(.*)GLU(.*)")
      set(
        REVISED_wxWidgets_LIBRARIES ${REVISED_wxWidgets_LIBRARIES}
        ${_currentLibFile}
      )
    endif()
  endforeach(_currentLibFile)

  set(wxWidgets_LIBRARIES ${REVISED_wxWidgets_LIBRARIES})

  message(STATUS " Revised wxWidgets Libraries: ${wxWidgets_LIBRARIES}")

  if(GTK3_FOUND)
    set(_VERSPROG [=[
			#include <stdio.h>
			#include <wx/version.h>
			int main(int argc, char**argv) {
			printf("%d\\n", wxMAJOR_VERSION * 10 + wxMINOR_VERSION);
			}
		]=])
    set(TRY_COMPILE
      "echo '${_VERSPROG}' > wx-32.c; cc $(${wxWidgets_CONFIG_EXECUTABLE} --cflags) wx-32.c; ./a.out")
    execute_process(
      COMMAND "/bin/sh" "-c" "${TRY_COMPILE}"
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      OUTPUT_VARIABLE _WX_VERSION
      ERROR_VARIABLE _WX_ERRORS
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT "${_WX_ERRORS}" STREQUAL "" OR "${_WX_VERSION}" STREQUAL "")
      message(FATAL_ERROR "Cannot run wxWidgets  version test (!)")
    elseif("${_WX_VERSION}" LESS 32)
      add_definitions("-DNDEBUG")
      message(STATUS "Forcing -NDEBUG on gtk3 build (#1500).")
    endif()
  endif()
endif()

message(STATUS "Forcing wxWidgets_VERSION_STRING: ${wxWidgets_VERSION_STRING}")
set(wxWidgets_VERSION_STRING "3.2.0")

if(LINUX)
  message(STATUS "")
  message(STATUS "*** Staging to build ${PACKAGE_NAME}  ***")
  message(STATUS "*** Build type: ${CMAKE_BUILD_TYPE}")
  message(STATUS "*** Will install to ${CMAKE_INSTALL_PREFIX}  ***")
endif()

# Library
file(GLOB_RECURSE LIB_HEADER_FILES "include/*.h" "source/*.h")
file(GLOB_RECURSE LIB_INLINE_FILES "include/*.inl" "source/*.inl")
file(GLOB_RECURSE LIB_SOURCE_FILES "include/*.cpp" "source/*.cpp")
add_library(cppserver ${LIB_HEADER_FILES} ${LIB_INLINE_FILES} ${LIB_SOURCE_FILES})
set_target_properties(cppserver PROPERTIES COMPILE_FLAGS "${PEDANTIC_COMPILE_FLAGS}")

if(MSVC)
  target_include_directories(cppserver
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/main"
    "${CMAKE_CURRENT_SOURCE_DIR}/cache/buildwxWidgets/lib/vc_dll/mswu"
    "${CMAKE_CURRENT_SOURCE_DIR}/cache/buildwxWidgets/include")
  target_link_libraries(cppserver
    PUBLIC
    ${LINKLIBS}
    asio
    debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_gl
    debug ../cache/buildwxWidgets/lib/vc_dll/wxbase32ud_net
    debug ../cache/buildwxWidgets/lib/vc_dll/wxbase32ud_xml
    debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_html
    debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_adv
    debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_aui
    debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_core
    debug ../cache/buildwxWidgets/lib/vc_dll/wxbase32ud
    debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_webview
    optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_gl
    optimized ../cache/buildwxWidgets/lib/vc_dll/wxbase32u_net
    optimized ../cache/buildwxWidgets/lib/vc_dll/wxbase32u_xml
    optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_html
    optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_adv
    optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_aui
    optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_core
    optimized ../cache/buildwxWidgets/lib/vc_dll/wxbase32u
    optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_webview
  )
  target_compile_definitions(cppserver PUBLIC WXUSINGDLL)
endif()

if(LINUX)
  target_include_directories(cppserver
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/main
  )

  target_link_libraries(cppserver
    PUBLIC
    ${LINKLIBS}
    asio
  )
endif()

list(APPEND INSTALL_TARGETS cppserver)
list(APPEND LINKLIBS cppserver)

# Additional module components: benchmarks, examples, plugins, tests, tools and install
if(NOT CPPSERVER_MODULE)
  file(GLOB EXAMPLE_HEADER_FILES "main/*.h")
  file(GLOB EXAMPLE_INLINE_FILES "main/*.inl")

  if(MSVC)
    add_executable(WMSServer ${EXAMPLE_HEADER_FILES} ${EXAMPLE_INLINE_FILES} "main/WebServer.cpp")
    target_include_directories(WMSServer
      PUBLIC
      prebuilt/include
      include
      buildwin/crashrpt
      cache/buildwin/include
      libs/observable/include
      libs/ssl_sha1/include
      libs/picosha2
      libs/mongoose/include
      libs/mongoose/../../cache/buildwin/include
      prebuilt/_deps/rapidjson_src-src/include
      libs/mDNS/include
      libs/sqlite/include
      libs/SQLiteCpp/include
      libs/wxcurl/include
      libs/serial/include
      libs/libtess2/Include
      libs/sound/include
      libs/wxJSON/include
      libs/tinyxml/include
      libs/s52plib/src
      libs/pugixml
      libs/N2KParser/include
      libs/nmea0183/src
      libs/iso8211/include
      libs/gdal/include
      libs/texcmp/squish
      libs/mipmap/include
      libs/lz4/src
      libs/easywsclient
      libs/glu/include
      source
      cache/buildwxWidgets/lib/vc_dll/mswu
      cache/buildwxWidgets/include
    )

    target_sources(WMSServer
      PUBLIC
      ${PROJECT_SOURCE_DIR}/src/about.cpp
      ${PROJECT_SOURCE_DIR}/src/AboutFrame.cpp
      ${PROJECT_SOURCE_DIR}/src/AboutFrameImpl.cpp
      ${PROJECT_SOURCE_DIR}/src/base_platform.cpp
      ${PROJECT_SOURCE_DIR}/src/bbox.cpp
      ${PROJECT_SOURCE_DIR}/src/CanvasConfig.cpp
      ${PROJECT_SOURCE_DIR}/src/canvasMenu.cpp
      ${PROJECT_SOURCE_DIR}/src/CanvasOptions.cpp
      ${PROJECT_SOURCE_DIR}/src/cat_settings.cpp
      ${PROJECT_SOURCE_DIR}/src/catalog_handler.cpp
      ${PROJECT_SOURCE_DIR}/src/catalog_mgr.cpp
      ${PROJECT_SOURCE_DIR}/src/catalog_parser.cpp
      ${PROJECT_SOURCE_DIR}/src/certificates.cpp
      ${PROJECT_SOURCE_DIR}/src/chartdata_input_stream.cpp
      ${PROJECT_SOURCE_DIR}/src/chartdb.cpp
      ${PROJECT_SOURCE_DIR}/src/chartdbs.cpp
      ${PROJECT_SOURCE_DIR}/src/chartimg.cpp
      ${PROJECT_SOURCE_DIR}/src/chcanv.cpp
      ${PROJECT_SOURCE_DIR}/src/ChInfoWin.cpp
      ${PROJECT_SOURCE_DIR}/src/cm93.cpp
      ${PROJECT_SOURCE_DIR}/src/color_handler.cpp
      ${PROJECT_SOURCE_DIR}/src/comm_vars.cpp
      ${PROJECT_SOURCE_DIR}/src/compass.cpp
      ${PROJECT_SOURCE_DIR}/src/concanv.cpp
      ${PROJECT_SOURCE_DIR}/src/config_vars.cpp
      ${PROJECT_SOURCE_DIR}/src/ConfigMgr.cpp
      ${PROJECT_SOURCE_DIR}/src/cutil.cpp
      ${PROJECT_SOURCE_DIR}/src/DetailSlider.cpp
      ${PROJECT_SOURCE_DIR}/src/download_mgr.cpp
      ${PROJECT_SOURCE_DIR}/src/downloader.cpp
      ${PROJECT_SOURCE_DIR}/src/FlexHash.cpp
      ${PROJECT_SOURCE_DIR}/src/FontDesc.cpp
      ${PROJECT_SOURCE_DIR}/src/FontMgr.cpp
      ${PROJECT_SOURCE_DIR}/src/geodesic.cpp
      ${PROJECT_SOURCE_DIR}/src/georef.cpp
      ${PROJECT_SOURCE_DIR}/src/gshhs.cpp
      ${PROJECT_SOURCE_DIR}/src/gui_lib.cpp
      ${PROJECT_SOURCE_DIR}/src/hyperlink.cpp
      ${PROJECT_SOURCE_DIR}/src/IDX_entry.cpp
      ${PROJECT_SOURCE_DIR}/src/kml.cpp
      ${PROJECT_SOURCE_DIR}/src/Layer.cpp
      ${PROJECT_SOURCE_DIR}/src/line_clip.cpp
      ${PROJECT_SOURCE_DIR}/src/LLRegion.cpp
      ${PROJECT_SOURCE_DIR}/src/LOD_reduce.cpp
      ${PROJECT_SOURCE_DIR}/src/logger.cpp
      ${PROJECT_SOURCE_DIR}/main/MainApp.cpp
      ${PROJECT_SOURCE_DIR}/src/navutil.cpp
      ${PROJECT_SOURCE_DIR}/src/navutil_base.cpp
      ${PROJECT_SOURCE_DIR}/src/NMEALogWindow.cpp
      ${PROJECT_SOURCE_DIR}/src/OCPN_AUIManager.cpp
      ${PROJECT_SOURCE_DIR}/src/ocpn_frame.cpp
      ${PROJECT_SOURCE_DIR}/src/ocpn_pixel.cpp
      ${PROJECT_SOURCE_DIR}/src/ocpn_plugin.cpp
      ${PROJECT_SOURCE_DIR}/src/ocpn_print.cpp
      ${PROJECT_SOURCE_DIR}/src/ocpn_utils.cpp
      ${PROJECT_SOURCE_DIR}/src/ocpndc.cpp
      ${PROJECT_SOURCE_DIR}/src/OCPNPlatform.cpp
      ${PROJECT_SOURCE_DIR}/src/OCPNRegion.cpp
      ${PROJECT_SOURCE_DIR}/src/ogrs57datasource.cpp
      ${PROJECT_SOURCE_DIR}/src/ogrs57layer.cpp
      ${PROJECT_SOURCE_DIR}/src/options.cpp
      ${PROJECT_SOURCE_DIR}/src/Osenc.cpp
      ${PROJECT_SOURCE_DIR}/src/own_ship.cpp
      ${PROJECT_SOURCE_DIR}/src/piano.cpp
      ${PROJECT_SOURCE_DIR}/src/pluginmanager.cpp
      ${PROJECT_SOURCE_DIR}/src/plugin_blacklist.cpp
      ${PROJECT_SOURCE_DIR}/src/plugin_cache.cpp
      ${PROJECT_SOURCE_DIR}/src/plugin_handler.cpp
      ${PROJECT_SOURCE_DIR}/src/plugin_loader.cpp
      ${PROJECT_SOURCE_DIR}/src/plugin_paths.cpp
      ${PROJECT_SOURCE_DIR}/src/poly_math.cpp
      ${PROJECT_SOURCE_DIR}/src/Quilt.cpp
      ${PROJECT_SOURCE_DIR}/src/RolloverWin.cpp
      ${PROJECT_SOURCE_DIR}/src/s57chart.cpp
      ${PROJECT_SOURCE_DIR}/src/s57classregistrar.cpp
      ${PROJECT_SOURCE_DIR}/src/s57featuredefns.cpp
      ${PROJECT_SOURCE_DIR}/src/s57obj.cpp
      ${PROJECT_SOURCE_DIR}/src/s57reader.cpp
      ${PROJECT_SOURCE_DIR}/src/s57RegistrarMgr.cpp
      ${PROJECT_SOURCE_DIR}/src/safe_mode.cpp
      ${PROJECT_SOURCE_DIR}/src/semantic_vers.cpp
      ${PROJECT_SOURCE_DIR}/src/SencManager.cpp
      ${PROJECT_SOURCE_DIR}/src/shaders.cpp
      ${PROJECT_SOURCE_DIR}/src/Station_Data.cpp
      ${PROJECT_SOURCE_DIR}/src/styles.cpp
      ${PROJECT_SOURCE_DIR}/src/svg_utils.cpp
      ${PROJECT_SOURCE_DIR}/src/TCDataFactory.cpp
      ${PROJECT_SOURCE_DIR}/src/TCDataSource.cpp
      ${PROJECT_SOURCE_DIR}/src/TCDS_Ascii_Harmonic.cpp
      ${PROJECT_SOURCE_DIR}/src/TCDS_Binary_Harmonic.cpp
      ${PROJECT_SOURCE_DIR}/src/tcmgr.cpp
      ${PROJECT_SOURCE_DIR}/src/TCWin.cpp
      ${PROJECT_SOURCE_DIR}/src/thumbwin.cpp
      ${PROJECT_SOURCE_DIR}/src/TTYScroll.cpp
      ${PROJECT_SOURCE_DIR}/src/TTYWindow.cpp
      ${PROJECT_SOURCE_DIR}/src/viewport.cpp
      ${PROJECT_SOURCE_DIR}/src/update_mgr.cpp
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/dict.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libutil/error.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/geom.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libutil/glue.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/memalloc.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/mesh.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/normal.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/priorityq.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libutil/project.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libutil/registry.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/render.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/sweep.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/tess.c
      ${PROJECT_SOURCE_DIR}/libs/glu/libtess/tessmono.c
      ${PROJECT_SOURCE_DIR}/include/base_platform.h
      ${PROJECT_SOURCE_DIR}/include/bbox.h
      ${PROJECT_SOURCE_DIR}/include/CanvasConfig.h
      ${PROJECT_SOURCE_DIR}/include/certificates.h
      ${PROJECT_SOURCE_DIR}/include/chart_ctx_factory.h
      ${PROJECT_SOURCE_DIR}/include/chartbase.h
      ${PROJECT_SOURCE_DIR}/include/chartdata_input_stream.h
      ${PROJECT_SOURCE_DIR}/include/chartdb.h
      ${PROJECT_SOURCE_DIR}/include/chartdbs.h
      ${PROJECT_SOURCE_DIR}/include/chartimg.h
      ${PROJECT_SOURCE_DIR}/include/chcanv.h
      ${PROJECT_SOURCE_DIR}/include/ChInfoWin.h
      ${PROJECT_SOURCE_DIR}/include/cm93.h
      ${PROJECT_SOURCE_DIR}/include/color_handler.h
      ${PROJECT_SOURCE_DIR}/include/color_types.h
      ${PROJECT_SOURCE_DIR}/include/comm_vars.h
      ${PROJECT_SOURCE_DIR}/include/compass.h
      ${PROJECT_SOURCE_DIR}/include/config_vars.h
      ${PROJECT_SOURCE_DIR}/include/cutil.h
      ${PROJECT_SOURCE_DIR}/include/dychart.h
      ${PROJECT_SOURCE_DIR}/include/FlexHash.h
      ${PROJECT_SOURCE_DIR}/include/FontDesc.h
      ${PROJECT_SOURCE_DIR}/include/FontMgr.h
      ${PROJECT_SOURCE_DIR}/include/geodesic.h
      ${PROJECT_SOURCE_DIR}/include/georef.h
      ${PROJECT_SOURCE_DIR}/include/gui_lib.h
      ${PROJECT_SOURCE_DIR}/include/gshhs.h
      ${PROJECT_SOURCE_DIR}/include/IDX_entry.h
      ${PROJECT_SOURCE_DIR}/include/kml.h
      ${PROJECT_SOURCE_DIR}/include/Layer.h
      ${PROJECT_SOURCE_DIR}/include/line_clip.h
      ${PROJECT_SOURCE_DIR}/include/linmath.h
      ${PROJECT_SOURCE_DIR}/include/LLRegion.h
      ${PROJECT_SOURCE_DIR}/include/LOD_reduce.h
      ${PROJECT_SOURCE_DIR}/include/logger.h
      ${PROJECT_SOURCE_DIR}/include/navutil.h
      ${PROJECT_SOURCE_DIR}/include/navutil_base.h
      ${PROJECT_SOURCE_DIR}/include/ocpn_frame.h
      ${PROJECT_SOURCE_DIR}/include/ocpn_pixel.h
      ${PROJECT_SOURCE_DIR}/include/ocpn_plugin.h
      ${PROJECT_SOURCE_DIR}/include/ocpn_print.h
      ${PROJECT_SOURCE_DIR}/include/ocpn_types.h
      ${PROJECT_SOURCE_DIR}/include/ocpn_utils.h
      ${PROJECT_SOURCE_DIR}/include/ocpndc.h
      ${PROJECT_SOURCE_DIR}/include/OCPNPlatform.h
      ${PROJECT_SOURCE_DIR}/include/OCPNRegion.h
      ${PROJECT_SOURCE_DIR}/include/ogr_s57.h
      ${PROJECT_SOURCE_DIR}/include/options.h
      ${PROJECT_SOURCE_DIR}/include/Osenc.h
      ${PROJECT_SOURCE_DIR}/include/own_ship.h
      ${PROJECT_SOURCE_DIR}/include/piano.h
      ${PROJECT_SOURCE_DIR}/include/pluginmanager.h
      ${PROJECT_SOURCE_DIR}/include/plugin_blacklist.h
      ${PROJECT_SOURCE_DIR}/include/plugin_cache.h
      ${PROJECT_SOURCE_DIR}/include/plugin_handler.h
      ${PROJECT_SOURCE_DIR}/include/plugin_loader.h
      ${PROJECT_SOURCE_DIR}/include/plugin_paths.h
      ${PROJECT_SOURCE_DIR}/include/poly_math.h
      ${PROJECT_SOURCE_DIR}/include/Quilt.h
      ${PROJECT_SOURCE_DIR}/include/RolloverWin.h
      ${PROJECT_SOURCE_DIR}/include/s57.h
      ${PROJECT_SOURCE_DIR}/include/s57chart.h
      ${PROJECT_SOURCE_DIR}/include/S57ClassRegistrar.h
      ${PROJECT_SOURCE_DIR}/include/S57Light.h
      ${PROJECT_SOURCE_DIR}/include/S57ObjectDesc.h
      ${PROJECT_SOURCE_DIR}/include/s57RegistrarMgr.h
      ${PROJECT_SOURCE_DIR}/include/S57Sector.h
      ${PROJECT_SOURCE_DIR}/include/safe_mode.h
      ${PROJECT_SOURCE_DIR}/include/semantic_vers.h
      ${PROJECT_SOURCE_DIR}/include/SencManager.h
      ${PROJECT_SOURCE_DIR}/include/shaders.h
      ${PROJECT_SOURCE_DIR}/include/Station_Data.h
      ${PROJECT_SOURCE_DIR}/include/styles.h
      ${PROJECT_SOURCE_DIR}/include/svg_utils.h
      ${PROJECT_SOURCE_DIR}/include/TCDataFactory.h
      ${PROJECT_SOURCE_DIR}/include/TCDataSource.h
      ${PROJECT_SOURCE_DIR}/include/TCDS_Ascii_Harmonic.h
      ${PROJECT_SOURCE_DIR}/include/TCDS_Binary_Harmonic.h
      ${PROJECT_SOURCE_DIR}/include/tcmgr.h
      ${PROJECT_SOURCE_DIR}/include/TCWin.h
      ${PROJECT_SOURCE_DIR}/include/thumbwin.h
      ${PROJECT_SOURCE_DIR}/include/TTYScroll.h
      ${PROJECT_SOURCE_DIR}/include/TTYWindow.h
      ${PROJECT_SOURCE_DIR}/include/vector2D.h
      ${PROJECT_SOURCE_DIR}/include/viewport.h
      ${PROJECT_SOURCE_DIR}/include/update_mgr.h
    )

    target_link_libraries(WMSServer
      PUBLIC
      debug ../prebuilt/libs/observable/Debug/_OBSERVABLE
      debug ../prebuilt/libs/ssl_sha1/Debug/SHA1
      debug ../prebuilt/libs/mongoose/Debug/MONGOOSE
      debug ../prebuilt/libs/mDNS/Debug/MDNS
      debug ../buildwin/crashrpt/CrashRpt1403
      debug ../prebuilt/libs/SQLiteCpp/Debug/SQLITE_CPP
      debug ../prebuilt/libs/wxcurl/Debug/WXCURL
      debug ../prebuilt/libs/serial/Debug/SERIAL
      debug ../prebuilt/libs/libtess2/Debug/TESS2
      debug ../prebuilt/libs/sound/Debug/SOUND
      debug ../prebuilt/libs/wxJSON/Debug/WXJSON
      debug ../prebuilt/libs/tinyxml/Debug/TINYXML
      debug ../prebuilt/libs/s52plib/Debug/S52PLIB
      debug ../prebuilt/libs/pugixml/Debug/OCPN_PUGIXML
      debug ../prebuilt/libs/N2KParser/Debug/N2KPARSER
      debug ../prebuilt/libs/texcmp/Debug/TEXCMP
      debug ../prebuilt/libs/mipmap/Debug/MIPMAP
      debug ../prebuilt/libs/lz4/Debug/LZ4
      debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_gl
      debug ../cache/buildwxWidgets/lib/vc_dll/wxbase32ud_net
      debug ../cache/buildwxWidgets/lib/vc_dll/wxbase32ud_xml
      debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_html
      debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_adv
      debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_aui
      debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_core
      debug ../cache/buildwxWidgets/lib/vc_dll/wxbase32ud
      debug ../cache/buildwxWidgets/lib/vc_dll/wxmsw32ud_webview
      debug Userenv
      debug opengl32
      debug winmm
      debug comctl32
      debug uuid
      debug oleacc
      debug uxtheme
      debug rpcrt4
      debug shlwapi
      debug version
      debug wsock32
      debug setupapi.lib
      debug ../prebuilt/lib/Debug/GLU
      debug psapi.lib
      debug iphlpapi.lib
      debug gdiplus
      debug ../prebuilt/libs/nmea0183/Debug/NMEA0183
      debug ../prebuilt/libs/easywsclient/Debug/EASYWSCLIENT
      debug ../cache/buildwin/lzma
      debug ../cache/buildwin/libssl
      debug ../cache/buildwin/libcrypto
      debug ../prebuilt/libs/sqlite/Debug/SQLITE3
      debug ../cache/buildwin/archive
      debug ../cache/buildwin/libcurl
      debug ../cache/buildwin/zlib1
      debug glu32.lib
      debug ../prebuilt/libs/iso8211/Debug/ISO8211
      debug ../prebuilt/libs/gdal/Debug/GDAL
      debug ws2_32
      optimized ../prebuilt/libs/observable/Release/_OBSERVABLE
      optimized ../prebuilt/libs/ssl_sha1/Release/SHA1
      optimized ../prebuilt/libs/mongoose/Release/MONGOOSE
      optimized ../prebuilt/libs/mDNS/Release/MDNS
      optimized ../buildwin/crashrpt/CrashRpt1403
      optimized ../prebuilt/libs/SQLiteCpp/Release/SQLITE_CPP
      optimized ../prebuilt/libs/wxcurl/Release/WXCURL
      optimized ../prebuilt/libs/serial/Release/SERIAL
      optimized ../prebuilt/libs/libtess2/Release/TESS2
      optimized ../prebuilt/libs/sound/Release/SOUND
      optimized ../prebuilt/libs/wxJSON/Release/WXJSON
      optimized ../prebuilt/libs/tinyxml/Release/TINYXML
      optimized ../prebuilt/libs/s52plib/Release/S52PLIB
      optimized ../prebuilt/libs/pugixml/Release/OCPN_PUGIXML
      optimized ../prebuilt/libs/N2KParser/Release/N2KPARSER
      optimized ../prebuilt/libs/texcmp/Release/TEXCMP
      optimized ../prebuilt/libs/mipmap/Release/MIPMAP
      optimized ../prebuilt/libs/lz4/Release/LZ4
      optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_gl
      optimized ../cache/buildwxWidgets/lib/vc_dll/wxbase32u_net
      optimized ../cache/buildwxWidgets/lib/vc_dll/wxbase32u_xml
      optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_html
      optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_adv
      optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_aui
      optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_core
      optimized ../cache/buildwxWidgets/lib/vc_dll/wxbase32u
      optimized ../cache/buildwxWidgets/lib/vc_dll/wxmsw32u_webview
      optimized Userenv
      optimized opengl32
      optimized winmm
      optimized comctl32
      optimized uuid
      optimized oleacc
      optimized uxtheme
      optimized rpcrt4
      optimized shlwapi
      optimized version
      optimized wsock32
      optimized setupapi.lib
      optimized ../prebuilt/lib/Release/GLU
      optimized psapi.lib
      optimized iphlpapi.lib
      optimized gdiplus
      optimized ../prebuilt/libs/nmea0183/Release/NMEA0183
      optimized ../prebuilt/libs/easywsclient/Release/EASYWSCLIENT
      optimized ../cache/buildwin/lzma
      optimized ../cache/buildwin/libssl
      optimized ../cache/buildwin/libcrypto
      optimized ../prebuilt/libs/sqlite/Release/SQLITE3
      optimized ../cache/buildwin/archive
      optimized ../cache/buildwin/libcurl
      optimized ../cache/buildwin/zlib1
      optimized glu32.lib
      optimized ../prebuilt/libs/iso8211/Release/ISO8211
      optimized ../prebuilt/libs/gdal/Release/GDAL
      optimized ws2_32
    )

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT WMSServer)
    target_compile_definitions(WMSServer PUBLIC PSAPI_VERSION=1)
    target_compile_definitions(WMSServer PUBLIC TIXML_USE_STL)
    target_compile_definitions(WMSServer PUBLIC __MSVC__)
    target_compile_definitions(WMSServer PUBLIC WXUSINGDLL)
  else()
    set(
      HDRS
      include/base_platform.h
      include/bbox.h
      include/CanvasConfig.h
      include/certificates.h
      include/chart_ctx_factory.h
      include/chartbase.h
      include/chartdata_input_stream.h
      include/chartdb.h
      include/chartdbs.h
      include/chartimg.h
      include/chcanv.h
      include/ChInfoWin.h
      include/color_handler.h
      include/color_types.h
      include/comm_vars.h
      include/compass.h
      include/config_vars.h
      include/cutil.h
      include/dychart.h
      include/FlexHash.h
      include/FontDesc.h
      include/FontMgr.h
      include/geodesic.h
      include/georef.h
      include/gshhs.h
      include/gui_lib.h
      include/IDX_entry.h
      include/kml.h
      include/Layer.h
      include/line_clip.h
      include/linmath.h
      include/LLRegion.h
      include/LOD_reduce.h
      include/logger.h
      include/navutil.h
      include/navutil_base.h
      include/ocpn_frame.h
      include/ocpn_pixel.h
      include/ocpn_plugin.h
      include/ocpn_print.h
      include/ocpn_types.h
      include/ocpn_utils.h
      include/ocpndc.h
      include/OCPNPlatform.h
      include/OCPNRegion.h
      include/ogr_s57.h
      include/options.h
      include/Osenc.h
      include/own_ship.h
      include/piano.h
      include/pluginmanager.h
      include/plugin_blacklist.h
      include/plugin_cache.h
      include/plugin_handler.h
      include/plugin_loader.h
      include/plugin_paths.h
      include/poly_math.h
      include/Quilt.h
      include/RolloverWin.h
      include/s57.h
      include/s57chart.h
      include/S57ClassRegistrar.h
      include/S57Light.h
      include/S57ObjectDesc.h
      include/s57RegistrarMgr.h
      include/S57Sector.h
      include/safe_mode.h
      include/semantic_vers.h
      include/SencManager.h
      include/shaders.h
      include/Station_Data.h
      include/styles.h
      include/svg_utils.h
      include/TCDataFactory.h
      include/TCDataSource.h
      include/TCDS_Ascii_Harmonic.h
      include/TCDS_Binary_Harmonic.h
      include/tcmgr.h
      include/TCWin.h
      include/thumbwin.h
      include/TTYScroll.h
      include/TTYWindow.h
      include/vector2D.h
      include/viewport.h
      include/update_mgr.h
      ${EXAMPLE_HEADER_FILES}
    )

    set(MODEL_SRC
      ${CMAKE_SOURCE_DIR}/src/base_platform.cpp
      ${CMAKE_SOURCE_DIR}/src/chartdata_input_stream.cpp
      ${CMAKE_SOURCE_DIR}/src/config_vars.cpp
      ${CMAKE_SOURCE_DIR}/src/cutil.cpp
      ${CMAKE_SOURCE_DIR}/src/georef.cpp
      ${CMAKE_SOURCE_DIR}/src/logger.cpp
      ${CMAKE_SOURCE_DIR}/src/navutil_base.cpp
      ${CMAKE_SOURCE_DIR}/src/ocpn_plugin.cpp
      ${CMAKE_SOURCE_DIR}/src/ocpn_utils.cpp
      ${CMAKE_SOURCE_DIR}/src/own_ship.cpp
    )

    set(
      SRCS
      src/about.cpp
      src/AboutFrame.cpp
      src/AboutFrameImpl.cpp
      src/bbox.cpp
      src/base_platform.cpp
      src/CanvasConfig.cpp
      src/canvasMenu.cpp
      src/CanvasOptions.cpp
      src/cat_settings.cpp
      src/catalog_handler.cpp
      src/catalog_mgr.cpp
      src/catalog_parser.cpp
      src/certificates.cpp
      src/chartdata_input_stream.cpp
      src/chartdb.cpp
      src/chartdbs.cpp
      src/chartimg.cpp
      src/chcanv.cpp
      src/ChInfoWin.cpp
      src/cm93.cpp
      src/color_handler.cpp
      src/comm_vars.cpp
      src/compass.cpp
      src/concanv.cpp
      src/config_vars.cpp
      src/ConfigMgr.cpp
      src/cutil.cpp
      src/DetailSlider.cpp
      src/download_mgr.cpp
      src/downloader.cpp
      src/FlexHash.cpp
      src/FontDesc.cpp
      src/FontMgr.cpp
      src/geodesic.cpp
      src/georef.cpp
      src/gshhs.cpp
      src/gui_lib.cpp
      src/hyperlink.cpp
      src/IDX_entry.cpp
      src/kml.cpp
      src/Layer.cpp
      src/line_clip.cpp
      src/LLRegion.cpp
      src/LOD_reduce.cpp
      src/logger.cpp
      src/mbtiles.cpp
      src/navutil.cpp
      src/navutil_base.cpp
      src/NMEALogWindow.cpp
      src/OCPN_AUIManager.cpp
      src/ocpn_frame.cpp
      src/ocpn_pixel.cpp
      src/ocpn_plugin.cpp
      src/ocpn_print.cpp
      src/ocpn_utils.cpp
      src/ocpndc.cpp
      src/OCPNPlatform.cpp
      src/OCPNRegion.cpp
      src/ogrs57datasource.cpp
      src/ogrs57layer.cpp
      src/options.cpp
      src/Osenc.cpp
      src/own_ship.cpp
      src/piano.cpp
      src/pluginmanager.cpp
      src/plugin_blacklist.cpp
      src/plugin_cache.cpp
      src/plugin_handler.cpp
      src/plugin_loader.cpp
      src/plugin_paths.cpp
      src/poly_math.cpp
      src/Quilt.cpp
      src/RolloverWin.cpp
      src/s57chart.cpp
      src/s57classregistrar.cpp
      src/s57featuredefns.cpp
      src/s57obj.cpp
      src/s57reader.cpp
      src/s57RegistrarMgr.cpp
      src/safe_mode.cpp
      src/semantic_vers.cpp
      src/SencManager.cpp
      src/shaders.cpp
      src/Station_Data.cpp
      src/styles.cpp
      src/svg_utils.cpp
      src/TCDataFactory.cpp
      src/TCDataSource.cpp
      src/TCDS_Ascii_Harmonic.cpp
      src/TCDS_Binary_Harmonic.cpp
      src/tcmgr.cpp
      src/TCWin.cpp
      src/thumbwin.cpp
      src/TTYScroll.cpp
      src/TTYWindow.cpp
      src/viewport.cpp
      src/update_mgr.cpp
      main/MainApp.cpp
      main/WebServer.cpp
    )

    if(OCPN_USE_CRASHREPORT)
      list(APPEND HDRS include/crashprint.h)
      list(APPEND SRCS src/crashprint.cpp)
    endif()

    set(SRCS ${SRCS} ${MODEL_SRC})

    add_executable(WMSServer ${HDRS} ${EXAMPLE_INLINE_FILES} ${SRCS})

    set_target_properties(
      WMSServer
      PROPERTIES
      ENABLE_EXPORTS 1
      OUTPUT_NAME WMSServer
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_include_directories(
      WMSServer
      PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    find_package(Gettext REQUIRED)

    add_subdirectory(libs/observable)
    target_link_libraries(WMSServer PRIVATE observable::observable)

    add_subdirectory(libs/ssl_sha1)
    target_link_libraries(WMSServer PRIVATE ssl::sha1)

    add_subdirectory(libs/picosha2)
    target_link_libraries(WMSServer PRIVATE pico_sha2)

    add_subdirectory(libs/mongoose)
    target_link_libraries(WMSServer PUBLIC ocpn::mongoose)

    add_subdirectory(libs/mDNS)
    target_link_libraries(WMSServer PUBLIC ocpn::mDNS)

    find_package(LIBUDEV)

    if(LIBUDEV_FOUND)
      target_link_libraries(WMSServer PUBLIC ocpn::libudev)
      set(HAVE_LIBUDEV 1)
    endif()

    if(wxWidgets_VERSION_STRING VERSION_LESS "3.1.6")
      find_package(PANGO)

      if(PANGO_FOUND)
        target_link_libraries(WMSServer PRIVATE ocpn::pango)
        set(HAVE_PANGO 1)
      endif()
    endif()

    find_path(HAVE_UNISTD_H NAMES unistd.h)
    find_path(HAVE_LIBGEN_H NAMES libgen.h)
    find_path(HAVE_DIRENT_H NAMES dirent.h)
    find_path(HAVE_LINUX_SERIAL_H PATH_SUFFIXES linux NAMES serial.h)
    find_path(HAVE_SYS_STAT_H PATH_SUFFIXES sys NAMES stat.h)
    find_path(HAVE_SYS_IOCTL_H PATH_SUFFIXES sys NAMES ioctl.h)
    find_path(HAVE_FCNTL_H NAMES fcntl.h)

    if(NOT HAVE_FCNTL_H)
      find_path(HAVE_SYS_FCNTL_H PATH_SUFFIXES sys NAMES fcntl.h)
    endif()

    find_path(HAVE_SYS_TYPES_H PATH_SUFFIXES sys NAMES types.h)

    check_symbol_exists(readlink unistd.h HAVE_READLINK)

    include(OcpnFindGtk)
    target_link_libraries(WMSServer PUBLIC ocpn::gtk)

    if(DEFINED _wx_selected_config)
      message(STATUS "selected config ${_wx_selected_config}")

      if(_wx_selected_config MATCHES "androideabi-qt")
      endif(_wx_selected_config MATCHES "androideabi-qt")
    endif(DEFINED _wx_selected_config)

    if((_wx_selected_config MATCHES "qt-armv7"))
      set(wxWidgets_FIND_COMPONENTS base core xml html adv aui)
    else()
      set(wxWidgets_FIND_COMPONENTS net xml html adv aui core base webview)
    endif()
  endif()

  if(ARCH MATCHES "arm*" AND OCPN_USE_GLES_SHIM)
    find_path(OPENGLESv1_INCLUDE_DIR GLES/gl.h)

    if(OPENGLESv1_INCLUDE_DIR)
      message(STATUS "Found OpenGLESv1")
      add_definitions(-DocpnUSE_GL) # ocpnUSE_GLES is in config.h

      set(OPENGL_LIBRARIES "GL_static" "EGL" "X11" "drm")
    endif()
  endif()

  if(OPENGLES_FOUND)
    list(APPEND wxwidgets_FIND_COMPONENTS gl)
  endif()

  if(NOT OPENGLES_FOUND)
    if(OCPN_USE_GL)
      find_package(OpenGL)
    else(OCPN_USE_GL)
      message(STATUS "OpenGL disabled by option USE_GL...")
    endif(OCPN_USE_GL)

    if(OPENGL_FOUND)
      list(APPEND wxwidgets_FIND_COMPONENTS gl)
      target_include_directories(WMSServer PRIVATE ${OPENGL_INCLUDE_DIR})

      message(STATUS "Found OpenGL....")

      if(OCPN_VERBOSE)
        message(STATUS "    GL Lib: " ${OPENGL_gl_LIBRARIES})
        message(STATUS "    GL Include: " ${OPENGL_INCLUDE_DIR})
      endif()

      set(wxWidgets_USE_LIBS gl ${wxWidgets_USE_LIBS})
      add_definitions(-DocpnUSE_GL)
      target_link_libraries(WMSServer PRIVATE ${OPENGL_gl_LIBRARIES})
      target_include_directories(WMSServer PRIVATE ${OPENGL_INCLUDE_DIR})
    else(OPENGL_FOUND)
      message(STATUS "OpenGL not found...")
    endif(OPENGL_FOUND)
  endif()

  if(LINUX)
    if(OCPN_USE_CRASHREPORT)
      message(STATUS "Crash reporting enabled")

      if(CMAKE_BUILD_TYPE MATCHES "Rel*")
        message(STATUS "Using Linux crash reporting")
      endif()
    endif(OCPN_USE_CRASHREPORT)

    add_subdirectory("libs/sqlite")
    target_link_libraries(WMSServer PUBLIC ocpn::sqlite)

    add_subdirectory("libs/SQLiteCpp")
    target_link_libraries(WMSServer PUBLIC ocpn::sqlite_cpp)

    find_package(BZip2 REQUIRED)
    target_include_directories(WMSServer PUBLIC ${BZIP2_INCLUDE_DIR})
    target_link_libraries(WMSServer PUBLIC ${BZIP2_LIBRARIES})
    find_package(ZLIB REQUIRED)
    target_include_directories(WMSServer PUBLIC ${ZLIB_INCLUDE_DIR})
    target_link_libraries(WMSServer PUBLIC ${ZLIB_LIBRARY})
    find_package(X11)

    if(X11_FOUND)
      target_link_libraries(WMSServer PUBLIC ${X11_LIBRARIES})
      set(OCPN_HAVE_X11 ON)
    endif()

    find_package(TinyXML)

    if(OCPN_USE_LZMA)
      find_package(LibLZMA)

      if(LIBLZMA_FOUND)
        message(STATUS "Building with lzma support: ${LIBLZMA_LIBRARIES}")
        target_include_directories(WMSServer PUBLIC ${LIBLZMA_INCLUDE_DIRS})
      else(LIBLZMA_FOUND)
        message(STATUS "lzma library not found")
      endif(LIBLZMA_FOUND)
    endif(OCPN_USE_LZMA)

    if(OCPN_USE_CURL)
      set(OCPN_USE_EXTERN_CURL ON)

      if(OCPN_USE_EXTERN_CURL)
        find_package(CURL REQUIRED)
        message(STATUS "ocpn: using external curl libraries...")
        message(STATUS "  curl includes: " ${CURL_INCLUDE_DIRS})
        message(STATUS "  curl libraries: " ${CURL_LIBRARIES})

        target_include_directories(WMSServer PUBLIC ${CURL_INCLUDE_DIRS})
      else(OCPN_USE_EXTERN_CURL)
        include("Curl")
        message(STATUS "Using bundled curl library...")
      endif(OCPN_USE_EXTERN_CURL)

      pkg_search_module(SYS_WXCURL libwxcurl wxcurl)
      use_bundled_lib(USE_BUNDLED_WXCURL wxcurl)

      if(SYS_WXCURL_FOUND AND NOT USE_BUNDLED_WXCURL)
        message(STATUS "Building with system wxcurl")
        target_include_directories(
          WMSServer
          PRIVATE ${CURL_INCLUDE_DIRS} ${SYS_WXCURL_INCLUDE_DIR}
        )
        target_link_libraries(WMSServer PRIVATE ${SYS_WXCURL_LIBRARIES})
      else()
        message(STATUS "Building with bundled wxcurl")
        include("Curl")
        add_subdirectory("libs/wxcurl")
        target_link_libraries(WMSServer PRIVATE ocpn::wxcurl)
      endif()

      target_link_libraries(WMSServer PUBLIC ${CURL_LIBRARIES})
    endif(OCPN_USE_CURL)

    if(OCPN_USE_NEWSERIAL)
      add_subdirectory("libs/serial")
      target_link_libraries(WMSServer PRIVATE ocpn::serial)
      message(STATUS "Using new serial library...")
    endif(OCPN_USE_NEWSERIAL)
  endif()

  if(OPENGL_FOUND)
    target_sources(
      ${PACKAGE_NAME}
      PRIVATE
      include/glChartCanvas.h
      include/glTextureDescriptor.h
      include/glTexCache.h
      include/glTextureManager.h
      src/glTextureDescriptor.cpp
      src/glTexCache.cpp
      src/glChartCanvas.cpp
      src/glTextureManager.cpp
    )
  endif()

  if(LINUX)
    add_subdirectory(libs/glu)
    set(OPENGL_LIBRARIES "GLU_static" ${OPENGL_LIBRARIES})

    if(OCPN_VERBOSE)
      message(STATUS "    Revised GL Lib (with local): " ${OPENGL_LIBRARIES})
    endif()

    find_package(OcpnLibarchive)

    if(NOT TARGET libarchive)
      message(FATAL:_ERROR "No libarchive!!!")
    endif()

    target_link_libraries(WMSServer PUBLIC ocpn::libarchive)

    add_subdirectory(libs/libtess2)
    target_link_libraries(WMSServer PRIVATE ocpn::tess2)

    include("SoundConfig")
    add_subdirectory(libs/sound)
    target_link_libraries(WMSServer PRIVATE ocpn::sound)

    add_subdirectory(libs/wxJSON)
    target_link_libraries(WMSServer PRIVATE ocpn::wxjson)

    add_subdirectory(libs/tinyxml)
    target_link_libraries(WMSServer PRIVATE ocpn::tinyxml)

    add_subdirectory(libs/s52plib)
    target_link_libraries(WMSServer PRIVATE ocpn::s52plib)

    add_subdirectory(libs/pugixml)
    target_link_libraries(WMSServer PRIVATE ocpn::pugixml)

    message(STATUS "S57 ENC support: enabled")
    set(
      SRC_S57ENC
      include/bbox.h
      include/cm93.h
      include/line_clip.h
      include/linmath.h
      include/LLRegion.h
      include/LOD_reduce.h
      include/ogr_s57.h
      include/Osenc.h
      include/poly_math.h
      include/s57chart.h
      include/s57RegistrarMgr.h
      include/SencManager.h
      include/vector2D.h
      src/bbox.cpp
      src/cm93.cpp
      src/line_clip.cpp
      src/LLRegion.cpp
      src/LOD_reduce.cpp
      src/ogrs57datasource.cpp
      src/ogrs57layer.cpp
      src/Osenc.cpp
      src/poly_math.cpp
      src/s57chart.cpp
      src/s57classregistrar.cpp
      src/s57featuredefns.cpp
      src/s57obj.cpp
      src/s57reader.cpp
      src/s57RegistrarMgr.cpp
      src/SencManager.cpp
    )
    add_library(S57ENC STATIC ${SRC_S57ENC})
    target_link_libraries(
      S57ENC
      PUBLIC
      ocpn::nmea0183
      ocpn::tess2
      ocpn::sound
      ocpn::wxjson
      ssl::sha1
      ocpn::tinyxml
      observable::observable
    )

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang|GNU") # Apple is AppleClang
      target_compile_options(S57ENC PUBLIC -fvisibility=default)
    endif()

    if(OCPN_USE_CURL)
      target_link_libraries(S57ENC PUBLIC ocpn::wxcurl)
    endif()

    add_subdirectory(libs/iso8211)
    target_link_libraries(S57ENC PUBLIC ocpn::iso8211)

    add_subdirectory(libs/gdal)
    target_link_libraries(S57ENC PUBLIC ocpn::gdal)

    add_subdirectory(libs/N2KParser)
    target_link_libraries(WMSServer PUBLIC ocpn::N2KParser)

    if(OCPN_USE_LZMA AND LIBLZMA_FOUND)
      target_include_directories(S57ENC PUBLIC ${LIBLZMA_INCLUDE_DIR})
      target_link_libraries(S57ENC PUBLIC ${LIBLZMA_LIBRARY})
    endif()

    target_link_libraries(S57ENC PUBLIC ocpn::pugixml)
    target_link_libraries(S57ENC PUBLIC ocpn::s52plib)

    target_include_directories(S57ENC PUBLIC ${CMAKE_SOURCE_DIR}/include)

    # #############
    # set_property(TARGET S57ENC PROPERTY COMPILE_FLAGS "${OBJ_VISIBILITY}")
    # ############
    target_link_libraries(WMSServer PUBLIC S57ENC)

    if(OPENGL_FOUND)
      add_subdirectory("libs/texcmp")
      target_link_libraries(WMSServer PUBLIC ocpn::texcmp)

      add_subdirectory("libs/mipmap")
      target_link_libraries(WMSServer PUBLIC ocpn::mipmap)
    endif(OPENGL_FOUND)

    pkg_search_module(LZ4 liblz4 lz4)
    USE_BUNDLED_LIB(USE_BUNDLED_LZ4 lz4)

    if(LZ4_FOUND AND NOT USE_BUNDLED_LZ4)
      message(STATUS "Building with system lz4")
      include_directories(${LZ4_INCLUDE_DIR})

      if(COMMAND target_link_directories)
        target_link_directories(TEXCMP PUBLIC ${LZ4_LIBRARY_DIRS})
      else()
        link_directories(${LZ4_LIBRARY_DIRS})
      endif()

      target_link_libraries(WMSServer PUBLIC ${LZ4_LIBRARIES})
    else()
      message(STATUS "Building with bundled lz4")
      add_subdirectory("libs/lz4")
      target_link_libraries(WMSServer PUBLIC LZ4)
    endif()

    find_path(LIBELF_INCLUDE_DIR NAMES libelf.h gelf.h PATH_SUFFIXES libelf)
    find_library(LIBELF_LIBRARY NAMES elf)

    if(LIBELF_INCLUDE_DIR AND LIBELF_LIBRARY)
      message(STATUS "Found LibELF...")
      message(STATUS "    ELF Lib: ${LIBELF_INCLUDE_DIR}")
      message(STATUS "    ELF Include: ${LIBELF_LIBRARY}")
      target_include_directories(WMSServer PUBLIC "${LIBELF_INCLUDE_DIR}")
      target_link_libraries(WMSServer PUBLIC "${LIBELF_LIBRARY}")

      set(USE_LIBELF ON) # => config.h
    else()
      message(
        WARNING
        "Did not found LibELF, plugin compatibility check will be simplified."
      )
    endif()

    include(TargetSetup)

    if("${wxWidgets_LIBRARIES}" MATCHES "gtk3u")
      if(${PKG_TARGET_VERSION} VERSION_LESS 22.04)
        if(NOT ${PKG_TARGET} MATCHES "-gtk3")
          string(REPLACE ubuntu ubuntu-gtk3 PKG_TARGET ${PKG_TARGET})
        endif()
      endif()

      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__WXGTK3__")
    endif()

    configure_file(${CMAKE_SOURCE_DIR}/cmake/in-files/config.h.in ${CMAKE_BINARY_DIR}/include/config.h)
    include_directories(BEFORE "${CMAKE_BINARY_DIR}/include")

    target_link_libraries(WMSServer PRIVATE ${wxWidgets_LIBRARIES})

    target_link_libraries(
      WMSServer
      PRIVATE ${OPENGL_LIBRARIES} ${GETTEXT_LIBRARY} SQLITE_CPP
    )

    message(STATUS "Did not find x11 libraries")
    message(
      STATUS "support for transparent toolbar in opengl mode not detected"
    )

    add_subdirectory("libs/nmea0183")
    target_link_libraries(WMSServer PUBLIC ocpn::nmea0183)

    add_subdirectory("libs/easywsclient")
    target_link_libraries(WMSServer PUBLIC ocpn::easywsclient)

    if(CMAKE_VERSION VERSION_GREATER 3.4)
      if(NOT "${ENABLE_SANITIZER}" MATCHES "none")
        target_link_libraries(${PACKAGE_NAME}
          PRIVATE -fsanitize=${ENABLE_SANITIZER}
        )
      endif()
    endif()

    if(LIBLZMA_FOUND)
      target_link_libraries(WMSServer PUBLIC ${LIBLZMA_LIBRARY})
    endif(LIBLZMA_FOUND)

    target_link_libraries(WMSServer PRIVATE dl)
    set_target_properties(WMSServer PROPERTIES LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,$ORIGIN/Core")
  endif()

  set_target_properties(WMSServer PROPERTIES COMPILE_FLAGS "${PEDANTIC_COMPILE_FLAGS}")
  target_link_libraries(WMSServer PUBLIC ${LINKLIBS})

  add_subdirectory(plugins)

  # Install *.pdb files
  if(MSVC)
    foreach(INSTALL_TARGET_PDB ${INSTALL_TARGETS_PDB})
      install(FILES $<TARGET_PDB_FILE:${INSTALL_TARGET_PDB}> DESTINATION "${PROJECT_SOURCE_DIR}/bin")
    endforeach()
  endif()
endif()
