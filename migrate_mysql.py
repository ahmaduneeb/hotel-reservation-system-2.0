import pymysql
import os
import hashlib
import sys

# MySQL Configuration (XAMPP default)
DB_CONFIG = {
    "host": "localhost",
    "user": "root",
    "password": "",
    "database": "hotel_db"
}

def hash_password(password):
    return hashlib.sha256(password.encode()).hexdigest()

def connect_mysql(use_db=True):
    try:
        if use_db:
            return pymysql.connect(**DB_CONFIG)
        else:
            return pymysql.connect(host=DB_CONFIG["host"], user=DB_CONFIG["user"], password=DB_CONFIG["password"])
    except pymysql.err.OperationalError as e:
        print(f"❌ Error: Could not connect to MySQL. Is XAMPP running?\nDetail: {e}")
        sys.exit(1)

def setup_database():
    print("--- Initializing Database ---")
    conn = connect_mysql(use_db=False)
    cursor = conn.cursor()
    cursor.execute(f"CREATE DATABASE IF NOT EXISTS {DB_CONFIG['database']}")
    conn.select_db(DB_CONFIG["database"])

    # Create Tables with optimized structure
    tables = {
        "users": '''CREATE TABLE IF NOT EXISTS users (
            id INT AUTO_INCREMENT PRIMARY KEY,
            username VARCHAR(50) UNIQUE,
            password_hash VARCHAR(64),
            role VARCHAR(20)
        )''',
        "rooms": '''CREATE TABLE IF NOT EXISTS rooms (
            number INT PRIMARY KEY,
            type VARCHAR(50),
            price DECIMAL(10,2),
            capacity INT,
            availability TINYINT(1) DEFAULT 1,
            maintenance TINYINT(1) DEFAULT 0
        )''',
        "customers": '''CREATE TABLE IF NOT EXISTS customers (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(100),
            phone VARCHAR(20),
            room_number INT,
            check_in VARCHAR(20),
            check_out VARCHAR(20)
        )''',
        "staff": '''CREATE TABLE IF NOT EXISTS staff (
            id INT PRIMARY KEY,
            name VARCHAR(100),
            role VARCHAR(50),
            salary DECIMAL(10,2)
        )''',
        "revenue_log": '''CREATE TABLE IF NOT EXISTS revenue_log (
            id INT AUTO_INCREMENT PRIMARY KEY,
            amount DECIMAL(10,2),
            source VARCHAR(50),
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )'''
    }

    for name, sql in tables.items():
        cursor.execute(sql)
        print(f"✅ Table '{name}' is ready.")

    # Default Admin
    admin_pass = "admin123"
    cursor.execute("INSERT IGNORE INTO users (username, password_hash, role) VALUES (%s, %s, %s)",
                   ("admin", hash_password(admin_pass), "admin"))
    
    conn.commit()
    cursor.close()
    conn.close()

def migrate_all():
    print("\n--- Migrating Data from Files ---")
    conn = connect_mysql()
    cursor = conn.cursor()

    # Rooms
    if os.path.exists("rooms.txt"):
        with open("rooms.txt", "r") as f:
            lines = f.readlines()
            for line in lines[1:]:
                p = line.split()
                if len(p) >= 6:
                    try:
                        cursor.execute("REPLACE INTO rooms VALUES (%s, %s, %s, %s, %s, %s)", 
                                       (int(p[0]), p[1], float(p[2]), int(p[3]), int(p[4]), int(p[5])))
                    except: continue
        print("✅ Rooms migrated.")

    # Staff
    if os.path.exists("staff.txt"):
        with open("staff.txt", "r") as f:
            lines = f.read().splitlines()
            i = 0
            while i < len(lines):
                try:
                    if not lines[i].strip() or '|' in lines[i]: i += 1; continue
                    cursor.execute("REPLACE INTO staff VALUES (%s, %s, %s, %s)", 
                                   (int(lines[i]), lines[i+1], lines[i+2], float(lines[i+3])))
                    i += 4
                except: i += 1
        print("✅ Staff migrated.")

    conn.commit()
    cursor.close()
    conn.close()

if __name__ == "__main__":
    setup_database()
    migrate_all()
    print("\n🚀 All done! You can now run the system using run.bat.")
