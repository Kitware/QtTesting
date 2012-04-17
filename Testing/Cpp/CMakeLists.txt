include(../CMake/qtTestingMacroGenerateMocs.cmake)

set(KIT ${PROJECT_NAME})

set(TEST_SOURCES
  pqEventPlayerTest.cpp
  pqEventTranslatorTest.cpp
  )

create_test_sourcelist(Tests ${KIT}CppTests.cxx
  ${TEST_SOURCES}
  )

set(TestsToRun ${Tests})
remove(TestsToRun ${KIT}CppTests.cxx)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

QT4_GENERATE_MOCS(${TEST_SOURCES})

add_executable(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${PROJECT_NAME} ${QT_QTTEST_LIBRARY})

macro(SIMPLE_TEST testname)
  add_test(NAME ${testname} COMMAND $<TARGET_FILE:${KIT}CppTests> ${Testname} ${ARGN})
endmacro()

#
# Add Tests
#
SIMPLE_TEST( pqEventPlayerTest )
SIMPLE_TEST( pqEventTranslatorTest )
