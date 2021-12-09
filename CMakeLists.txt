cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_COVERAGE "Build code coverage" OFF)

set(
  HUNTER_CACHE_SERVERS
  "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
  CACHE STRING "Default cache server"
)

include("cmake/HunterGate.cmake")

HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.312.tar.gz"
  SHA1 "42311324f6b859983d833cffc2151d0b86b8bea4"
  LOCAL
)

project(ejudje)

#string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
#string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
#string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

add_library(${PROJECT_NAME}-lib STATIC
  # enum your files and delete this comment
  module_2/B/m2_taskB.hpp
  module_3/A/m3_taskA.cpp
  )

#add_executable(demo
#  ${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp
#  )

add_executable(${PROJECT_NAME} module_3/A/m3_taskA.cpp #[[module_3/exmpl/003640.cpp]] )


target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>"
  )

#target_link_libraries(demo ${PROJECT_NAME})


if(BUILD_TESTS)
  add_executable(tests
    module_2/B/tests/tests.cpp
    )
  target_link_libraries(tests ${PROJECT_NAME}-lib GTest::gtest_main)
  enable_testing()
  add_test(NAME unit_tests COMMAND tests)
  # Add symlink to /misc dir
  set(source "${CMAKE_CURRENT_SOURCE_DIR}/module_2")
  set(destination "${CMAKE_CURRENT_BINARY_DIR}/module_2")
  add_custom_command(
    TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
    DEPENDS ${destination}
    COMMENT "symbolic link resources folder from ${source} => ${destination}"
  )
endif()

if(BUILD_COVERAGE)
  set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/coverage/cmake")
  find_package(codecov)
  add_coverage(${PROJECT_NAME})
  add_coverage(tests)
  coverage_evaluate()
endif()

if(BUILD_DOCS)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/_docs)
  set(DOXYGEN_EXAMPLE_PATH examples)
  set(DOXYGEN_GENERATE_HTML NO)
  set(DOXYGEN_GENERATE_MAN YES)
  find_package(Doxygen)
  doxygen_add_docs(docs include)
  install(DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}/man DESTINATION share)
  # add_dependencies(install docs) need to call target docs before install or package
endif()

set_target_properties(${PROJECT_NAME}-lib PROPERTIES LINKER_LANGUAGE CXX)

install(TARGETS ${PROJECT_NAME}
  EXPORT "${PROJECT_NAME}-targets"
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  INCLUDES DESTINATION include
  )

#configure_file(<input> <output> COPYONLY)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/misc/ DESTINATION misc)

install(EXPORT "${PROJECT_NAME}-targets"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "lib/cmake"
  )




