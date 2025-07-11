# 🔐 Password Manager Server

This project is the **second entry in my freelance C++ developer portfolio**. It demonstrates the creation of a secure, modular, and high-performance **password manager backend**, implemented entirely in **modern C++20**. The system manages user authentication, secure credential storage, password generation, and encrypted import/export functionality.

---

## 🚀 Project Overview

The goal is to build a **lightweight, RESTful backend server** for managing user credentials securely. The backend is designed for easy integration with future desktop or web-based clients. Key principles include:

* 🔒 **Security-first design**
* ⚙️ **Modular architecture**
* ⚡ **High performance**

---

## 🛠️ Tech Stack

| Component        | Technology                              |
| ---------------- | --------------------------------------- |
| Language         | C++20                                   |
| Web Framework    | [Crow](https://github.com/CrowCpp/crow) |
| Database         | SQLite                                  |
| Build System     | CMake                                   |
| Containerization | Docker                                  |
| Testing          | CTest + Custom Unit Tests               |
| CI/CD            | GitHub Actions                          |
| Version Control  | Git + GitHub                            |

---

## ⚙️ Core Features

* ✅ **REST API**: Secure endpoints for users and password entries
* ✅ **User management**: Register, login, logout
* ✅ **Session handling**: Token-based or in-memory cache
* ✅ **Encrypted password storage**: AES-128 / AES-256
* ✅ **Password hashing**: PBKDF2, bcrypt, or Argon2
* ✅ **Advanced search & filtering**
* ✅ **Custom password generator**
* ✅ **Encrypted import/export of credentials**
* 🔄 **Desktop UI (WIP)**: wxWidgets-based GUI in development

---

## 🔐 Security Highlights

* 🔒 AES-based encryption for local credential storage
* 🔒 Secure password hashing with PBKDF2 / bcrypt / Argon2
* 🔒 Input sanitization and secure defaults
* 🔒 Minimal external dependencies to reduce attack surface

---

## 🧪 Getting Started

### 🔧 Requirements

* C++20-compatible compiler (e.g. `g++`, `clang++`)
* CMake ≥ 3.16
* SQLite3 development headers
* Crow (header-only framework)

---

## ⚙️ Building the Project

```bash
git clone https://github.com/yourusername/password-manager-server.git
cd password-manager-server
mkdir build && cd build
cmake ..
make
./password_manager_server
```

---

## 🐳 Running with Docker

A Dockerfile is provided for simplified builds and deployment:

```bash
# Build Docker image
docker build -t cpp-password-manager .

# Run the container
docker run -p 8080:8080 cpp-password-manager
```

This exposes the REST API at `http://localhost:8080`.

---

## 🧪 Running Tests

Unit tests are located in the `tests/` directory and integrated via CMake.

### 🛠️ To build and run all tests:

```bash
cd build
cmake -DBUILD_TESTING=ON ..
make
ctest --output-on-failure
```

Or run individual test binaries:

```bash
./tests/test_Hasher
./tests/test_EncryptedLocalStorage
# etc.
```

---

## ⚙️ CI/CD Pipeline

Automated builds and test runs are configured using **GitHub Actions**. On every push or pull request to `main`, the pipeline:

* 🔧 Builds the entire project
* ✅ Runs all unit tests
* 📦 Optionally builds the Docker image

You can find the workflow file under `.github/workflows/`.

---

## 🤝 Contribution

Contributions are welcome! You can help by:

* 🧪 Improving test coverage
* 🔐 Reviewing security best practices
* 🖥️ Enhancing the UI integration
* 🪲 Reporting and fixing issues

To contribute:

1. Fork the repo
2. Create a feature branch
3. Submit a pull request

---

## 📜 License

Licensed under the [MIT License](LICENSE).


