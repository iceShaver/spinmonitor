cmake_minimum_required(VERSION 3.16)
project(SpinMonitor)

set(CMAKE_CXX_STANDARD 20)
set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
find_package(Boost REQUIRED COMPONENTS program_options)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(${CMAKE_PROJECT_NAME} main.cc app.cc daemon.cc disk.cc disk.hh sata_disk.cc sata_disk.hh scsi_disk.cc scsi_disk.hh client.cc client.hh config.hh tools.cc tools.hh)
target_link_libraries(${CMAKE_PROJECT_NAME} Boost::program_options Threads::Threads)


