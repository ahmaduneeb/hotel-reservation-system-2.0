import pymysql
import hashlib

# MySQL Configuration
DB_CONFIG = {
    "host": "localhost",
    "user": "root",
    "password": "",
    "database": "hotel_db"
}

def hash_password(password):
    return hashlib.sha256(password.encode()).hexdigest()

def reset_admin():
    try:
        conn = pymysql.connect(**DB_CONFIG)
        cursor = conn.cursor()
        
        username = "admin"
        password = "admin123"
        hashed = hash_password(password)
        
        # Delete existing admin to be sure
        cursor.execute("DELETE FROM users WHERE username = %s", (username,))
        
        # Insert fresh admin
        cursor.execute("INSERT INTO users (username, password_hash, role) VALUES (%s, %s, %s)",
                       (username, hashed, "admin"))
        
        conn.commit()
        print(f"✅ Admin credentials have been force-reset!")
        print(f"Username: {username}")
        print(f"Password: {password}")
        
        cursor.close()
        conn.close()
    except Exception as e:
        print(f"❌ Error: {e}")

if __name__ == "__main__":
    reset_admin()
