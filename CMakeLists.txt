cmake_minimum_required(VERSION 3.8)
PROJECT(cvQTcameraGL)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(GNUInstallDirs)

find_package( OpenCV REQUIRED )

set(OpenGL_GL_PREFERENCE GLVND)
find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)
find_package( GLUT REQUIRED)
find_package( GLEW   REQUIRED )
find_package( OpenGL REQUIRED)

set(WIDGET_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/widget.h
)

set(MAIN_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set(TEXTFILE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/textfile.cpp
)

QT5_WRAP_CPP( WIDGET_MOC_SOURCES
  ${WIDGET_SOURCES}
)
	
QT5_WRAP_CPP(MAIN_MOCsrcs
  ${MAIN_SOURCES}
)

set(SHADER_SRC
 ${CMAKE_CURRENT_SOURCE_DIR}/shader/toonf2.frag
 ${CMAKE_CURRENT_SOURCE_DIR}/shader/toonf2.vert
)

ADD_LIBRARY(textfile)
target_sources(textfile
  PRIVATE
  ${TEXTFILE_SOURCES}
)
target_include_directories(textfile
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
  include	
	)

ADD_LIBRARY(widget)
target_sources(widget
  PRIVATE
  ${WIDGET_SOURCES}
  ${WIDGET_MOC_SOURCES}
  ${SHADER_SOURCES}
)
target_include_directories(widget
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
  include	
	)
target_include_directories(widget SYSTEM
  PUBLIC
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
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
target_include_directories(cvQTcameraGL
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
  include	
	)
TARGET_LINK_LIBRARIES(cvQTcameraGL
  widget
)
add_custom_command(TARGET widget POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shader
        $<TARGET_FILE_DIR:widget>/shader)
