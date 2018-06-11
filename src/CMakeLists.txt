set(CMAKE_CXX_STANDARD 17)

add_executable(madv main.cpp Node.cc Node.h WSN.cpp WSN.h Point.cpp Point.h Protocol.cc Protocol.h PingPongProtocol.cc PingPongProtocol.h)
target_link_libraries(madv ns3)

target_compile_definitions(madv PRIVATE HAVE_PTHREAD_H)
