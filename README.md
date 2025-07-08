# 🔐 Password Manager Server

This project is the **second entry in my freelance C++ developer portfolio**. It showcases the development of a secure, modular, and high-performance **password manager backend**, built entirely in modern C++. The system handles user authentication, secure credential storage, and advanced features like encrypted import/export and password generation.

---

## 🚀 Project Overview

The goal is to develop a **lightweight RESTful server** for managing passwords securely. This backend is suitable for future integration with web or desktop clients. Security, modular design, and performance are key priorities.

---

## 🛠️ Tech Stack

| Component       | Technology           |
| --------------- | -------------------- |
| Language        | C++20                |
| Web Framework   | Crow                 |
| Database        | SQLite               |
| Build System    | CMake                |
| Version Control | Git + GitHub         |

---

## ⚙️ Core Features

* ✅ **REST API** with secure routes for users & passwords
* ✅ **User management**: register, login, logout
* ✅ **Session handling** using tokens or in-memory cache
* ✅ **Encrypted password storage** (AES-128/AES-256)
* ✅ **Password hashing** using PBKDF2, bcrypt, or Argon2
* ✅ **Search & filtering** for fast password lookup
* ✅ **Password generator** with customizable criteria
* ✅ **Encrypted import/export** of credentials
* ✅ **User Interface** in development (wxWidgets-based)

---

## 🔐 Security Highlights

* 🔒 AES-based encryption for local storage
* 🔒 Hashed user passwords with modern key derivation algorithms
* 🔒 Minimal third-party dependencies to reduce attack surface
* 🔒 Input validation and secure defaults throughout

---

## 🧩 Development Status

### ✅ Completed

* Project structure & CMake configuration
* REST routes for all core operations
* Full DB schema (users, passwords)
* Encrypted local storage & AES logic
* Password hashing & secure login
* Session management
* Password generation & search

### ⏳ In Progress

* Full test coverage
* CI/CD pipeline (GitHub Actions)

---

## 🧪 Getting Started

### 🔧 Requirements

* C++20 compatible compiler (e.g. `g++`, `clang++`)
* `cmake` ≥ 3.16
* Crow  headers
* SQLite3 client libraries

### ⚙️ Build Instructions

```bash
git clone https://github.com/yourusername/password-manager-server.git
cd password-manager-server
mkdir build && cd build
cmake ..
make
./password_manager_server
```

---

## 🤝 Contribution

Contributions are welcome! Here are a few ways you can help:

* 🧪 Write tests
* 🐞 Report issues
* 💡 Suggest or build UI features
* 🔐 Review security implementation

---

## 📜 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.


