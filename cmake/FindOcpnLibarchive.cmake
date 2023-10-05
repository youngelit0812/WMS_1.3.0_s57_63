#
# Opencpn wrapper for FindLibarchive.cmake
#
# Export ocpn::libarchive transitive link object
#

if (TARGET ocpn::libarchive)
  return ()
endif ()

add_library(_archive_if INTERFACE)
add_library(ocpn::libarchive ALIAS _archive_if)

# On Android, use precompiled stuff and be done with it.
if (QT_ANDROID)
  set(_libarchive_root  ${CMAKE_SOURCE_DIR}/buildandroid/libarchive)
  set(_libarchive_lib "${_libarchive_root}/${ARCH}/libarchive.a")
  target_include_directories(
    _archive_if INTERFACE ${_libarchive_root}/include
  )
  target_link_libraries( _archive_if INTERFACE ${_libarchive_lib})
  if (NOT EXISTS ${_libarchive_lib})
    message(FATAL_ERROR "Required library ${_libarchive_lib} is missing")
  endif ()
  return ()
endif ()

if (APPLE)
  if (OCPN_USE_SYSTEM_LIBARCHIVE)
    list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/libarchive")
  else ()
    #  TODO This is a hack, due to the way libarchive is built and
    #  installed in CI build environment

    #list(APPEND CMAKE_PREFIX_PATH "/opt/local")
    # this results in:
    #libarchive header directory: /usr/local/include
    #libarchive library directory: /Applications/Xcode-13.4.1.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk/usr/lib/libarchive.tbd

    #try this:
    #list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt")

    # or this, very explicit for Brew install of libarchive, same as chardldr_pi
    #set(LibArchive_INCLUDE_DIRS /usr/local/opt/libarchive/include )
    #set(LibArchive_LIBRARIES /usr/local/opt/libarchive/lib/libarchive.dylib )
    #set(LibArchive_FOUND 1)

    # or this, for local build of libarchive
    set(LibArchive_INCLUDE_DIRS /usr/local/include )
    set(LibArchive_LIBRARIES /usr/local/lib/libarchive.13.dylib )
    set(LibArchive_FOUND 1)

    target_include_directories(_archive_if INTERFACE ${LibArchive_INCLUDE_DIRS})
    target_link_libraries(_archive_if INTERFACE ${LibArchive_LIBRARIES})

    message(STATUS "libarchive header directory: ${LibArchive_INCLUDE_DIRS}")
    message(STATUS "libarchive library directory: ${LibArchive_LIBRARIES}")
    return ()
  endif ()
endif ()

find_package(LibArchive REQUIRED)
target_include_directories(_archive_if INTERFACE ${LibArchive_INCLUDE_DIRS})
target_link_libraries(_archive_if INTERFACE ${LibArchive_LIBRARIES})
