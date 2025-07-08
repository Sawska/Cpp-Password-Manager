cmake_minimum_required(VERSION 3.16)
project(PasswordManagerTests)

enable_testing()

# ===============================
# Test Executable
# ===============================
add_executable(UnitTests
  test_UserDb.cpp
  test_PasswordDb.cpp
  test_Hasher.cpp
  test_PasswordGenerator.cpp
  test_PasswordFilter.cpp
  test_EncryptedLocalStorage.cpp
  test_Server.cpp
  main.cpp
)

# ===============================
# Dependencies
# ===============================

# SQLite3
find_library(SQLITE3_LIB sqlite3)
find_path(SQLITE3_INCLUDE_DIR sqlite3.h)
if(NOT SQLITE3_LIB OR NOT SQLITE3_INCLUDE_DIR)
  message(FATAL_ERROR "SQLite3 not found")
endif()

# Crow
find_path(CROW_INCLUDE_DIR crow/app.h)
if(NOT CROW_INCLUDE_DIR)
  message(FATAL_ERROR "Crow include directory not found")
endif()

# JWT
find_path(JWT_INCLUDE_DIR jwt-cpp/jwt.h)
if(NOT JWT_INCLUDE_DIR)
  message(FATAL_ERROR "JWT include directory not found")
endif()

# Asio
find_path(ASIO_INCLUDE_DIR asio.hpp)
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
include(${wxWidgets_USE_FILE})

# httplib
find_path(HTTPLIB_INCLUDE_DIR httplib.h)
if(NOT HTTPLIB_INCLUDE_DIR)
  message(FATAL_ERROR "cpp-httplib not found")
endif()

# nlohmann_json
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp)
if(NOT NLOHMANN_JSON_INCLUDE_DIR)
  message(FATAL_ERROR "nlohmann/json.hpp not found")
endif()

# ===============================
# Include Directories
# ===============================
target_include_directories(UnitTests PRIVATE
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
target_link_libraries(UnitTests PRIVATE
  ${SQLITE3_LIB}
  ${OPENSSL_LIBRARIES}
  ${ARGON2_LIB}
  ${wxWidgets_LIBRARIES}
  pthread
)

# ===============================
# Register Tests
# ===============================
add_test(NAME UnitTests COMMAND UnitTests)
