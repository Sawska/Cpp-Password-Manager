cmake_minimum_required(VERSION 3.16)
project(PasswordManagerTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

# Find dependencies
find_library(SQLITE3_LIB sqlite3 REQUIRED)
find_path(SQLITE3_INCLUDE_DIR sqlite3.h REQUIRED)

find_package(OpenSSL REQUIRED)

find_library(ARGON2_LIB argon2 REQUIRED)
find_path(ARGON2_INCLUDE_DIR argon2.h REQUIRED)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

find_package(GTest REQUIRED)

find_path(CROW_INCLUDE_DIR crow/app.h REQUIRED)
find_path(JWT_INCLUDE_DIR jwt-cpp/jwt.h REQUIRED)
find_path(ASIO_INCLUDE_DIR asio.hpp REQUIRED)
find_path(HTTPLIB_INCLUDE_DIR httplib.h REQUIRED)
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp REQUIRED)

# Define test executable
add_executable(UnitTests
  test_UserDb.cpp
  test_PasswordDb.cpp
  test_Hasher.cpp
  test_PasswordGenerator.cpp
  test_PasswordFilter.cpp
  test_EncryptedLocalStorage.cpp
  main.cpp
  ../UserDbClass.cpp
  ../PasswordDbClass.cpp
  ../Hasher.cpp
  ../PasswordGenerator.cpp
  ../PasswordFilter.cpp
  ../EncryptedLocalStorage.cpp
  ../AppManager.cpp
  ../PassEncryptorDecryptor.cpp
  ../Server.cpp
  ../GuiApp.cpp
  ../LoginDialog.cpp
  ../MainFrame.cpp
)

# Include directories
target_include_directories(UnitTests PRIVATE
  ${CROW_INCLUDE_DIR}
  ${SQLITE3_INCLUDE_DIR}
  ${ASIO_INCLUDE_DIR}
  ${JWT_INCLUDE_DIR}
  ${ARGON2_INCLUDE_DIR}
  ${wxWidgets_INCLUDE_DIRS}
  ${HTTPLIB_INCLUDE_DIR}
  ${NLOHMANN_JSON_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
)

# Add pthread compile option explicitly if needed
target_compile_options(UnitTests PRIVATE -pthread)

# Link libraries - use imported targets for GTest
target_link_libraries(UnitTests PRIVATE
  GTest::gtest_main
  ${SQLITE3_LIB}
  OpenSSL::SSL
  OpenSSL::Crypto
  ${ARGON2_LIB}
  ${wxWidgets_LIBRARIES}
  pthread
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(UnitTests)
