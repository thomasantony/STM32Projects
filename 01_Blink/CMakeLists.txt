cmake_minimum_required(VERSION 3.2)
set(PROJECT blink)
include(${CMAKE_SOURCE_DIR}/../cmake/ParticlePhoton.cmake)
project(${PROJECT} VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)

SetupPhotonFirmware()
