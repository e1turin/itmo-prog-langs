# CMAKE generated file: DO NOT EDIT!
# Generated by CMake Version 3.22
cmake_policy(SET CMP0009 NEW)

# common_sources at CMakeLists.txt:1 (file)
file(GLOB_RECURSE NEW_GLOB FOLLOW_SYMLINKS LIST_DIRECTORIES false "/tmp/tester/src/*.c")
set(OLD_GLOB
  "/tmp/tester/src/test_utils.c"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "/tmp/tester/out/build/debug/CMakeFiles/cmake.verify_globs")
endif()

# common_sources at CMakeLists.txt:1 (file)
file(GLOB_RECURSE NEW_GLOB FOLLOW_SYMLINKS LIST_DIRECTORIES false "/tmp/tester/src/*.h")
set(OLD_GLOB
  "/tmp/tester/src/test.h"
  "/tmp/tester/src/test_utils.h"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "/tmp/tester/out/build/debug/CMakeFiles/cmake.verify_globs")
endif()

# test_sources at CMakeLists.txt:10 (file)
file(GLOB_RECURSE NEW_GLOB FOLLOW_SYMLINKS LIST_DIRECTORIES false "/tmp/tester/tests/*.c")
set(OLD_GLOB
  "/tmp/tester/tests/_free.c"
  "/tmp/tester/tests/alloc_region.c"
  "/tmp/tester/tests/custom_tests.c"
  "/tmp/tester/tests/find_good_or_last.c"
  "/tmp/tester/tests/grow_heap.c"
  "/tmp/tester/tests/heap_init.c"
  "/tmp/tester/tests/memalloc.c"
  "/tmp/tester/tests/split_if_too_big.c"
  "/tmp/tester/tests/try_memalloc_existing.c"
  "/tmp/tester/tests/try_merge_with_next.c"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "/tmp/tester/out/build/debug/CMakeFiles/cmake.verify_globs")
endif()
