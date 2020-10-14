# quality_cover_lib static library compilation configuration script
# use cmake to generate a build generation script

# minimum cmake version
cmake_minimum_required(VERSION 3.10)

# the name of the project
project (Quality_Cover)

# compiler option
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

# add include directories
find_package(OpenMP)

# set sources and headers
set(SOURCES ${SOURCES} ThreadPool.hpp TransactionalDataParser.cpp Bitset_Concept.cpp Logger.cpp DataConverter.cpp QCRunner.cpp TransactionalDataParser.cpp main.cpp)
set(HEADERS ${HEADERS} TransactionalDataParser.hpp Bitset_Concept.hpp Logger.hpp DataConverter.hpp QCRunner.hpp TransactionalDataParser.hpp)

add_executable(QualityCover main.cpp ${SOURCES} ${HEADERS})
