

# 🏨 Hotel Reservation System 2.0

**Hotel Reservation System 2.0** is a modular, object-oriented C++ application that simulates real-world hotel booking and management operations. It provides separate panels for **Admin**, **Employee**, and **Customer-related services**, making the system structured, secure, and easy to manage.

This project demonstrates strong use of **OOP concepts**, **file handling**, and **menu-driven design** in C++.

---

## ✨ Features

### 🛏️ Room Management

* Add new rooms
* Update room details (price, type, status)
* Delete rooms
* View all rooms
* Track:

  * Availability
  * Maintenance status
  * Room type & pricing

### 📅 Reservation System

* Book rooms for customers
* Check-in / Check-out system
* Automatic availability updates
* Prevents double booking

### 👤 Customer Records

* Store customer details
* Maintain booking history
* Link customers with their reservations

### 🛠️ Admin Panel

* Manage rooms and staff
* View booking records
* Access profit and performance statistics

### 👨‍💼 Employee Panel

* Assist customers with bookings
* Update room status
* Handle check-in and check-out operations

### 🔍 Search & Filter

* Search rooms by type
* Filter rooms by price range
* Display only available rooms

### 💾 File Handling

* Persistent storage using files
* Saves:

  * Room data
  * Customer data
  * Reservation records

---

## 🧠 OOP Concepts Used

This project strongly applies:

* ✅ Classes & Objects
* ✅ Encapsulation
* ✅ Inheritance
* ✅ Polymorphism
* ✅ Function Overloading / Overriding
* ✅ Modular Programming (.h and .cpp files)

---

## 🏗️ Project Structure (Example)

```
Hotel-Reservation-System-2.0/
│
├── main.cpp
├── Hotel.h
├── Hotel.cpp
├── Room.h
├── Room.cpp
├── Customer.h
├── Customer.cpp
├── Reservation.h
├── Reservation.cpp
├── Admin.h
├── Admin.cpp
├── Employee.h
├── Employee.cpp
│
├── rooms.txt
├── customers.txt
├── reservations.txt
└── README.md
```

---

## ▶️ How to Compile & Run

### 🔹 Using g++

```bash
g++ main.cpp Hotel.cpp Room.cpp Customer.cpp Reservation.cpp Admin.cpp Employee.cpp -o hotel
./hotel
```

### 🔹 Using an IDE

You can run this project in:

* CodeBlocks
* Dev C++
* Visual Studio
* VS Code (with C++ extensions)

Just add all `.cpp` files to the project and build.

---

## 🖥️ Interface

* ASCII-based menu system
* Separate menus for:

  * Admin
  * Employee
  * Reservation operations

---

## 🎯 Learning Objectives

This project helps in understanding:

* Real-world system design
* Object-Oriented Programming in C++
* File handling for persistent storage
* Modular code organization
* Role-based system functionality

---

## 🚀 Future Improvements

* GUI version using **Qt / SFML / ImGui**
* Database integration (**MySQL / SQLite**)
* Online reservation simulation
* Graphical reports and statistics
* User authentication system with passwords

---

## 📌 Requirements

* C++ compiler (GCC / MinGW / MSVC)
* Basic understanding of OOP in C++

---

## 📜 License

This project is for **educational purposes**. You are free to modify and improve it.


