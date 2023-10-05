# Compiler features

# Choose C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

# Configure Visual Studio compiler options
if(MSVC)  
  # Increase the number of sections that an object file can contain
  add_compile_options(/bigobj)  

  # Visual Studio should correctly reports __cplusplus
  # https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus
  add_compile_options(/Zc:__cplusplus)

endif()

# Configure Unix compiler options
if(UNIX)
  # Configure build in pthread library
  set(CMAKE_THREAD_LIBS_INIT "-lpthread")
  set(CMAKE_HAVE_THREADS_LIBRARY 1)
  set(CMAKE_USE_WIN32_THREADS_INIT 0)
  set(CMAKE_USE_PTHREADS_INIT 1)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()

set(CMAKE_BUILD_TYPE Release)