-- Hotel Reservation System 3.0 - Database Setup Script
-- Database: hotel_db

CREATE DATABASE IF NOT EXISTS hotel_db;
USE hotel_db;

-- 1. Users Table (Authentication & Roles)
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(64) NOT NULL,
    role ENUM('admin', 'staff') DEFAULT 'staff',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- 2. Rooms Table
CREATE TABLE IF NOT EXISTS rooms (
    number INT PRIMARY KEY,
    type VARCHAR(50) NOT NULL,
    price DECIMAL(10, 2) NOT NULL,
    capacity INT NOT NULL,
    availability TINYINT(1) DEFAULT 1,
    maintenance TINYINT(1) DEFAULT 0
) ENGINE=InnoDB;

-- 3. Customers Table (Current Bookings)
CREATE TABLE IF NOT EXISTS customers (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    phone VARCHAR(20) NOT NULL,
    room_number INT,
    check_in VARCHAR(20),
    check_out VARCHAR(20),
    FOREIGN KEY (room_number) REFERENCES rooms(number) ON DELETE SET NULL
) ENGINE=InnoDB;

-- 4. Staff Table
CREATE TABLE IF NOT EXISTS staff (
    id INT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    role VARCHAR(50) NOT NULL,
    salary DECIMAL(10, 2) NOT NULL
) ENGINE=InnoDB;

-- 5. Maintenance Logs Table
CREATE TABLE IF NOT EXISTS maintenance_logs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    room_number INT,
    issue TEXT NOT NULL,
    status ENUM('Pending', 'In Progress', 'Resolved') DEFAULT 'Pending',
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (room_number) REFERENCES rooms(number) ON DELETE CASCADE
) ENGINE=InnoDB;

-- 6. Feedback Table
CREATE TABLE IF NOT EXISTS feedback (
    id INT AUTO_INCREMENT PRIMARY KEY,
    stars INT CHECK (stars >= 1 AND stars <= 5),
    text TEXT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- 7. Revenue Log Table
CREATE TABLE IF NOT EXISTS revenue_log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    amount DECIMAL(10, 2) NOT NULL,
    source VARCHAR(50) NOT NULL, -- e.g., 'Room', 'Service'
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- Initial Setup Data
-- Default Admin Password: admin123 (SHA-256 Hashed)
INSERT IGNORE INTO users (username, password_hash, role) 
VALUES ('admin', '240be518fabd2724ddb6f04030113657300899abb1f028e9f603c581ccc39ba6', 'admin');

-- Sample Rooms
INSERT IGNORE INTO rooms (number, type, price, capacity, availability, maintenance) VALUES
(101, 'Basic', 3000.00, 2, 1, 0),
(102, 'Basic', 3000.00, 2, 1, 0),
(201, 'Suite', 6000.00, 4, 1, 0),
(301, 'Presidential', 12000.00, 6, 1, 0);
