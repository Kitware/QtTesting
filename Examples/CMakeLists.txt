

SET (UI_FILES
  TestingDemo.ui
)

SET (SOURCE_FILES
  TestingDemo.cxx
  TestingDemo.h
)

SET (MOC_FILES
  TestingDemo.h
)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

IF(QtTesting_QT_VERSION VERSION_GREATER "5")
  QT6_WRAP_UI (EXAMPLE_UI_BUILT_SOURCES ${UI_FILES})
  QT6_WRAP_CPP(EXAMPLE_MOC_BUILT_SOURCES ${MOC_FILES})
ELSE()
  QT5_WRAP_UI (EXAMPLE_UI_BUILT_SOURCES ${UI_FILES})
  QT5_WRAP_CPP(EXAMPLE_MOC_BUILT_SOURCES ${MOC_FILES})
ENDIF()

add_executable (TestingDemo
  ${SOURCE_FILES}
  ${EXAMPLE_UI_BUILT_SOURCES}
  ${EXAMPLE_MOC_BUILT_SOURCES}
  )

target_link_libraries(TestingDemo
  qttesting ${qt_imported_targets})
set_target_properties(TestingDemo PROPERTIES
  COMPILE_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
