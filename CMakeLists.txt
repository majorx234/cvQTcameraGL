cmake_minimum_required(VERSION 3.8)
PROJECT(cvQTcameraGL)

find_package( OpenCV REQUIRED )
set(CMAKE_AUTOMOC ON)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)
find_package( GLUT REQUIRED)
find_package( GLEW   REQUIRED )
find_package( OpenGL REQUIRED)

include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)
include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

QT5_WRAP_CPP( WIDGET_MOC_SOURCES
              ${CMAKE_CURRENT_SOURCE_DIR}/src/widget.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/include/widget.h)


QT5_WRAP_CPP(MAIN_MOCsrcs
   ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )

set(MAIN_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set(TEXTFILE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/textfile.cpp

)

set(WIDGET_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/widget.h

)

set(SHADER_SOURCES
 ${CMAKE_CURRENT_SOURCE_DIR}/shader/toonf2.frag
 ${CMAKE_CURRENT_SOURCE_DIR}/shader/toonf2.vert
)

ADD_LIBRARY(textfile
  ${TEXTFILE_SOURCES}

)

ADD_LIBRARY(widget)
target_sources(widget
  PRIVATE
  ${WIDGET_SOURCES}
  ${WIDGET_MOC_SOURCES}
  ${SHADER_SOURCES}
)
TARGET_LINK_LIBRARIES(widget
  ${OpenCV_LIBS} 
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARY}
  GLEW::GLEW
  textfile
  Qt5::Core
  Qt5::Widgets
)

ADD_EXECUTABLE(cvQTcameraGL)
target_sources(cvQTcameraGL
  PRIVATE
  ${MAIN_MOC_SOURCES}
  ${MAIN_SOURCES}
)
TARGET_LINK_LIBRARIES(cvQTcameraGL
  widget
)
add_custom_command(TARGET widget POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shader
        $<TARGET_FILE_DIR:widget>/shader)