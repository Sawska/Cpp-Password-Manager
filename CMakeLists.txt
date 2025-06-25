cmake_minimum_required(VERSION 3.10)
project(Password_server CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(password_server
  main.cpp
  PasswordDbClass.cpp
  Server.cpp
  UserDbClass.cpp
  PasswordFilter.cpp
  PasswordGenerator.cpp
  PassEncryptorDecryptor.cpp
  Hasher.cpp
  EncryptedLocalStorage.cpp
  AppManager.cpp
)


find_library(SQLITE3_LIB sqlite3)
find_path(SQLITE3_INCLUDE_DIR sqlite3.h HINTS /usr/include /usr/local/include)
if(NOT SQLITE3_LIB OR NOT SQLITE3_INCLUDE_DIR)
  message(FATAL_ERROR "Sqlite3 not found")
endif()


find_path(CROW_INCLUDE_DIR crow/app.h HINTS /usr/local/include /usr/include)
if(NOT CROW_INCLUDE_DIR)
  message(FATAL_ERROR "Crow include directory not found")
endif()


find_path(JWT_INCLUDE_DIR jwt-cpp/jwt.h
  HINTS ${CMAKE_SOURCE_DIR}/jwt-cpp/include /usr/local/include /usr/include)
if(NOT JWT_INCLUDE_DIR)
  message(FATAL_ERROR "JWT include directory not found")
endif()


find_path(ASIO_INCLUDE_DIR asio.hpp HINTS /usr/include /usr/local/include)
if(NOT ASIO_INCLUDE_DIR)
  message(FATAL_ERROR "Asio include directory not found")
endif()


find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
  message(FATAL_ERROR "OpenSSL not found")
endif()

find_library(ARGON2_LIB argon2)
find_path(ARGON2_INCLUDE_DIR argon2.h)
if(NOT ARGON2_LIB OR NOT ARGON2_INCLUDE_DIR)
  message(FATAL_ERROR "Argon2 not found")
endif()


message(STATUS "JWT include dir: ${JWT_INCLUDE_DIR}")
message(STATUS "Crow include dir: ${CROW_INCLUDE_DIR}")
message(STATUS "SQLite3 include dir: ${SQLITE3_INCLUDE_DIR}")
message(STATUS "Asio include dir: ${ASIO_INCLUDE_DIR}")
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
message(STATUS "Argon2 include dir: ${ARGON2_INCLUDE_DIR}")
message(STATUS "Argon2 library: ${ARGON2_LIB}")


target_include_directories(password_server PRIVATE
  ${CROW_INCLUDE_DIR}
  ${SQLITE3_INCLUDE_DIR}
  ${ASIO_INCLUDE_DIR}
  ${JWT_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${ARGON2_INCLUDE_DIR}
)


target_link_libraries(password_server PRIVATE
  ${SQLITE3_LIB}
  ${OPENSSL_CRYPTO_LIBRARY}
  $