

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

QT4_WRAP_UI (EXAMPLE_UI_BUILT_SOURCES ${UI_FILES})
QT4_WRAP_CPP(EXAMPLE_MOC_BUILT_SOURCES ${MOC_FILES})

add_executable (TestingDemo
  ${SOURCE_FILES}
  ${EXAMPLE_UI_BUILT_SOURCES}
  ${EXAMPLE_MOC_BUILT_SOURCES}
  )

target_link_libraries(TestingDemo
  QtTesting)
