cmake_minimum_required(VERSION 3.10)
project(Password_server CXX)

add_executable(password_server main.cpp
PasswordDbClass.cpp  Server.cpp  UserDbClass.cpp
)
find_library(SQLITE3_LIB sqlite3)
if(NOT SQLITE3_LIB)
  message(FATAL_ERROR "Sqlite3 library not found")
endif()

find_path(CROW_INCLUDE_DIR crow/app.h HINTS /usr/local/include)
if(NOT CROW_INCLUDE_DIR)
  message(FATAL_ERROR "Crow include directory not found")
endif()


target_link_libraries(password_server PRIVATE ${SQLITE3_LIB})


target_include_directories(password_server PRIVATE ${CROW_INCLUDE_DIR})
 
