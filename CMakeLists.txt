cmake_minimum_required(VERSION 3.23)
project(P2PApp C)

set(CMAKE_C_STANDARD 23)

add_executable(index_server PDU/PDU.h PDU/PDU.c index_server.c)
add_executable(peer index_server PDU/PDU.h PDU/PDU.c peer.c)