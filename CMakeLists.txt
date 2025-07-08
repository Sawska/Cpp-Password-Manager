cmake_minimum_required(VERSION 3.10)
project(Password_server CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===============================
# Executable source files
# ===============================
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
  MainFrame.cpp
  LoginDialog.cpp
  GuiApp.cpp
)

# ===============================
# Find Dependencies
# ===============================

# SQLite3
find_library(SQLITE3_LIB sqlite3)
find_path(SQLITE3_INCLUDE_DIR sqlite3.h HINTS /usr/include /usr/local/include)
if(NOT SQLITE3_LIB OR NOT SQLITE3_INCLUDE_DIR)
  message(FATAL_ERROR "Sqlite3 not found")
endif()

# Crow
find_path(CROW_INCLUDE_DIR crow/app.h HINTS /usr/local/include /usr/include)
if(NOT CROW_INCLUDE_DIR)
  message(FATAL_ERROR "Crow include directory not found")
endif()

# JWT
find_path(JWT_INCLUDE_DIR jwt-cpp/jwt.h
  HINTS ${CMAKE_SOURCE_DIR}/jwt-cpp/include /usr/local/include /usr/include)
if(NOT JWT_INCLUDE_DIR)
  message(FATAL_ERROR "JWT include directory not found")
endif()

# ASIO
find_path(ASIO_INCLUDE_DIR asio.hpp HINTS /usr/include /usr/local/include)
if(NOT ASIO_INCLUDE_DIR)
  message(FATAL_ERROR "Asio include directory not found")
endif()

# OpenSSL
find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
  message(FATAL_ERROR "OpenSSL not found")
endif()

# Argon2
find_library(ARGON2_LIB argon2)
find_path(ARGON2_INCLUDE_DIR argon2.h)
if(NOT ARGON2_LIB OR NOT ARGON2_INCLUDE_DIR)
  message(FATAL_ERROR "Argon2 not found")
endif()

# wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)
if(NOT wxWidgets_FOUND)
  message(FATAL_ERROR "wxWidgets not found")
endif()
include(${wxWidgets_USE_FILE})

# httplib
find_path(HTTPLIB_INCLUDE_DIR httplib.h HINTS /usr/include /usr/local/include)
if(NOT HTTPLIB_INCLUDE_DIR)
  message(FATAL_ERROR "httplib.h not found. Make sure you installed cpp-httplib.")
endif()

# nlohmann_json
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp HINTS /usr/include /usr/local/include)
if(NOT NLOHMANN_JSON_INCLUDE_DIR)
  message(FATAL_ERROR "nlohmann/json.hpp not found. Please install nlohmann-json.")
endif()


# ===============================
# Messages for Debugging
# ===============================
message(STATUS "JWT include dir: ${JWT_INCLUDE_DIR}")
message(STATUS "Crow include dir: ${CROW_INCLUDE_DIR}")
message(STATUS "SQLite3 include dir: ${SQLITE3_INCLUDE_DIR}")
message(STATUS "Asio include dir: ${ASIO_INCLUDE_DIR}")
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
message(STATUS "Argon2 include dir: ${ARGON2_INCLUDE_DIR}")
message(STATUS "Argon2 library: ${ARGON2_LIB}")
message(STATUS "wxWidgets include dirs: ${wxWidgets_INCLUDE_DIRS}")
message(STATUS "wxWidgets libraries: ${wxWidgets_LIBRARIES}")
message(STATUS "httplib include dir: ${HTTPLIB_INCLUDE_DIR}")
message(STATUS "nlohmann_json include dir: ${NLOHMANN_JSON_INCLUDE_DIR}")

# ===============================
# Include Directories
# ===============================
target_include_directories(password_server PRIVATE
  ${CROW_INCLUDE_DIR}
  ${SQLITE3_INCLUDE_DIR}
  ${ASIO_INCLUDE_DIR}
  ${JWT_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${ARGON2_INCLUDE_DIR}
  ${wxWidgets_INCLUDE_DIRS}
  ${HTTPLIB_INCLUDE_DIR}
   ${NLOHMANN_JSON_INCLUDE_DIR} 
)

# ===============================
# Link Libraries
# ===============================
target_link_libraries(password_server PRIVATE
  ${SQLITE3_LIB}
  ${OPENSSL_CRYPTO_LIBRARY}
  ${ARGON2_LIB}
  ${wxWidgets_LIBRARIES}
)

# ===============================
# Post-build Step (copy full src)
# ===============================
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SRC_DST ${CMAKE_BINARY_DIR}/src)

add_custom_command(
    TARGET password_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SRC_DIR}" "${SRC_DST}"
    COMMENT "Copying entire src folder to build directory"
)
