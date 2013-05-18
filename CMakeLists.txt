cmake_minimum_required(VERSION 2.8)
PROJECT( DBN_tutorial )
ADD_EXECUTABLE( DBN_tutorial DBN_tutorial.cpp )
TARGET_LINK_LIBRARIES( DBN_tutorial ${CMAKE_SOURCE_DIR}/libsmile.a )
SET(CMAKE_CXX_FLAGS "-std=c++0x")

