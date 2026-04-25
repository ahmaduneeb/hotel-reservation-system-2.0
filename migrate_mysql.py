import pymysql
import os
import hashlib

# MySQL Configuration (XAMPP default)
DB_CONFIG = {
    "host": "localhost",
    "user": "root",
    "password": "",
    "database": "hotel_db"
}

def hash_password(password):
    return hashlib.sha256(password.encode()).hexdigest()

def init_db():
    # Connect to MySQL (without DB first to create it)
    conn = pymysql.connect(host=DB_CONFIG["host"], user=DB_CONFIG["user"], password=DB_CONFIG["password"])
    cursor = conn.cursor()
    cursor.execute(f"CREATE DATABASE IF NOT EXISTS {DB_CONFIG['database']}")
    conn.select_db(DB_CONFIG["database"])

    # Create Tables
    cursor.execute('''CREATE TABLE IF NOT EXISTS users (
        id INT AUTO_INCREMENT PRIMARY KEY,
        username VARCHAR(50) UNIQUE,
        password_hash VARCHAR(64),
        role VARCHAR(20)
    )''')

    cursor.execute('''CREATE TABLE IF NOT EXISTS rooms (
        number INT PRIMARY KEY,
        type VARCHAR(50),
        price DECIMAL(10,2),
        capacity INT,
        availability TINYINT(1),
        maintenance TINYINT(1)
    )''')

    cursor.execute('''CREATE TABLE IF NOT EXISTS customers (
        id INT AUTO_INCREMENT PRIMARY KEY,
        name VARCHAR(100),
        phone VARCHAR(20),
        room_number INT,
        check_in VARCHAR(20),
        check_out VARCHAR(20)
    )''')

    cursor.execute('''CREATE TABLE IF NOT EXISTS staff (
        id INT PRIMARY KEY,
        name VARCHAR(100),
        role VARCHAR(50),
        salary DECIMAL(10,2)
    )''')

    cursor.execute('''CREATE TABLE IF NOT EXISTS maintenance_logs (
        id INT AUTO_INCREMENT PRIMARY KEY,
        room_number INT,
        issue TEXT,
        status VARCHAR(20) DEFAULT 'Pending',
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )''')

    cursor.execute('''CREATE TABLE IF NOT EXISTS feedback (
        id INT AUTO_INCREMENT PRIMARY KEY,
        stars INT,
        text TEXT,
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )''')

    cursor.execute('''CREATE TABLE IF NOT EXISTS revenue_log (
        id INT AUTO_INCREMENT PRIMARY KEY,
        amount DECIMAL(10,2),
        source VARCHAR(50),
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )''')

    # Initial Admin
    admin_pass = "admin123"
    if os.path.exists("admin_credentials.txt"):
        with open("admin_credentials.txt", "r") as f:
            admin_pass = f.read().strip()
    
    try:
        cursor.execute("INSERT INTO users (username, password_hash, role) VALUES (%s, %s, %s)",
                       ("admin", hash_password(admin_pass), "admin"))
    except pymysql.err.IntegrityError:
        pass

    conn.commit()
    return conn

def migrate_rooms(conn):
    if not os.path.exists("rooms.txt"): return
    cursor = conn.cursor()
    rooms_data = {}
    with open("rooms.txt", "r") as f:
        lines = f.readlines()
        if not lines: return
        for line in lines[1:]:
            parts = line.split()
            if len(parts) >= 6:
                try:
                    num = int(parts[0])
                    rtype = parts[1]
                    price = float(parts[2])
                    cap = int(parts[3])
                    avail = int(parts[4])
                    maint = int(parts[5])
                    if num < 1 or rtype in ["9", "2000"]: continue
                    if cap > 100: cap = 3
                    if num not in rooms_data:
                        rooms_data[num] = (num, rtype, price, cap, avail, maint)
                except ValueError: continue
    
    for r in rooms_data.values():
        cursor.execute("REPLACE INTO rooms VALUES (%s, %s, %s, %s, %s, %s)", r)
    conn.commit()
    print(f"Migrated {len(rooms_data)} unique rooms to MySQL.")

def migrate_staff(conn):
    if not os.path.exists("staff.txt"): return
    cursor = conn.cursor()
    with open("staff.txt", "r") as f:
        lines = f.read().splitlines()
        i, count = 0, 0
        while i < len(lines):
            try:
                line = lines[i].strip()
                if not line or '|' in line: i += 1; continue
                sid = int(line)
                name = lines[i+1].strip()
                role = lines[i+2].strip()
                salary = float(lines[i+3].strip())
                cursor.execute("REPLACE INTO staff VALUES (%s, %s, %s, %s)", (sid, name, role, salary))
                count += 1
                i += 4
            except: i += 1
    conn.commit()
    print(f"Migrated {count} staff members to MySQL.")

def migrate_customers(conn):
    if not os.path.exists("customers.txt"): return
    cursor = conn.cursor()
    with open("customers.txt", "r") as f:
        lines = f.readlines()
        if not lines: return
        for line in lines[1:]:
            parts = line.split()
            if len(parts) >= 5:
                try:
                    name, phone, room, ci, co = parts[:5]
                    cursor.execute("INSERT INTO customers (name, phone, room_number, check_in, check_out) VALUES (%s, %s, %s, %s, %s)",
                                   (name, phone, int(room), ci, co))
                except: continue
    conn.commit()

def migrate_feedback(conn):
    if not os.path.exists("feedback.txt"): return
    cursor = conn.cursor()
    with open("feedback.txt", "r") as f:
        for line in f:
            if " | " in line:
                try:
                    stars, text = line.strip().split(" | ", 1)
                    cursor.execute("INSERT INTO feedback (stars, text) VALUES (%s, %s)", (int(stars), text))
                except: continue
    conn.commit()

def migrate_revenue(conn):
    if not os.path.exists("revenue.txt"): return
    cursor = conn.cursor()
    with open("revenue.txt", "r") as f:
        line = f.readline()
        if line:
            try:
                room_rev, serv_rev, total = [float(x) for x in line.split()]
                if room_rev > 0: cursor.execute("INSERT INTO revenue_log (amount, source) VALUES (%s, %s)", (room_rev, "Room"))
                if serv_rev > 0: cursor.execute("INSERT INTO revenue_log (amount, source) VALUES (%s, %s)", (serv_rev, "Service"))
            except: pass
    conn.commit()

if __name__ == "__main__":
    print("Starting migration to MySQL (XAMPP)...")
    try:
        conn = init_db()
        migrate_rooms(conn)
        migrate_staff(conn)
        migrate_customers(conn)
        migrate_feedback(conn)
        migrate_revenue(conn)
        conn.close()
        print("Migration complete. Database: hotel_db")
    except Exception as e:
        print(f"Error: {e}")
        print("Make sure XAMPP MySQL is running!")
