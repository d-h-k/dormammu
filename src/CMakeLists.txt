cmake_minimum_required(VERSION 3.10)
project(dormamu)
set(SOURCE main.c)
MESSAGE ("PROJECT : " ${CMAKE_PROJECT_NAME} )
SET ( CMAKE_VERBOSE_MAKEFILE true )

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(main ${SOURCE})
add_executable(http_client http_client.c)