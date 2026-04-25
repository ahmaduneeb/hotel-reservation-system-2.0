from fastapi import FastAPI, HTTPException, Depends
from pydantic import BaseModel
import pymysql
import hashlib
from typing import List, Optional

app = FastAPI()

# MySQL Configuration
DB_CONFIG = {
    "host": "localhost",
    "user": "root",
    "password": "",
    "database": "hotel_db"
}

def get_db():
    conn = pymysql.connect(**DB_CONFIG, cursorclass=pymysql.cursors.DictCursor)
    try:
        yield conn
    finally:
        conn.close()

def hash_password(password):
    return hashlib.sha256(password.encode()).hexdigest()

# Models
class LoginRequest(BaseModel):
    username: str
    password: str

class BookingRequest(BaseModel):
    name: str
    phone: str
    room_number: int
    check_in: str
    check_out: str

class RoomRequest(BaseModel):
    number: int
    type: str
    price: float
    capacity: int

# --- AUTH ---
@app.post("/auth/login")
def login(req: LoginRequest):
    conn = pymysql.connect(**DB_CONFIG, cursorclass=pymysql.cursors.DictCursor)
    with conn.cursor() as cursor:
        cursor.execute("SELECT role FROM users WHERE username = %s AND password_hash = %s", 
                       (req.username, hash_password(req.password)))
        user = cursor.fetchone()
    conn.close()
    
    if user:
        return {"status": "success", "role": user["role"], "token": "dummy-jwt-token"}
    raise HTTPException(status_code=401, detail="Invalid credentials")

# --- ROOMS ---
@app.get("/rooms")
def get_rooms():
    conn = pymysql.connect(**DB_CONFIG, cursorclass=pymysql.cursors.DictCursor)
    with conn.cursor() as cursor:
        cursor.execute("SELECT * FROM rooms")
        rooms = cursor.fetchall()
    conn.close()
    return rooms

@app.post("/rooms")
def add_room(req: RoomRequest):
    conn = pymysql.connect(**DB_CONFIG)
    try:
        with conn.cursor() as cursor:
            cursor.execute("INSERT INTO rooms (number, type, price, capacity, availability, maintenance) VALUES (%s, %s, %s, %s, 1, 0)",
                           (req.number, req.type, req.price, req.capacity))
        conn.commit()
        return {"status": "success"}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))
    finally:
        conn.close()

# --- BOOKINGS ---
@app.post("/bookings")
def book_room(req: BookingRequest):
    conn = pymysql.connect(**DB_CONFIG)
    try:
        with conn.cursor() as cursor:
            # Check availability
            cursor.execute("SELECT availability FROM rooms WHERE number = %s", (req.room_number,))
            room = cursor.fetchone()
            if not room or not room[0]:
                raise HTTPException(status_code=400, detail="Room not available")
            
            # Start Transaction
            cursor.execute("INSERT INTO customers (name, phone, room_number, check_in, check_out) VALUES (%s, %s, %s, %s, %s)",
                           (req.name, req.phone, req.room_number, req.check_in, req.check_out))
            cursor.execute("UPDATE rooms SET availability = 0 WHERE number = %s", (req.room_number,))
            
            # Revenue
            cursor.execute("INSERT INTO revenue_log (amount, source) SELECT price, 'Room' FROM rooms WHERE number = %s", (req.room_number,))
            
        conn.commit()
        return {"status": "success"}
    except Exception as e:
        conn.rollback()
        raise HTTPException(status_code=400, detail=str(e))
    finally:
        conn.close()

# --- STAFF ---
@app.get("/staff")
def get_staff():
    conn = pymysql.connect(**DB_CONFIG, cursorclass=pymysql.cursors.DictCursor)
    with conn.cursor() as cursor:
        cursor.execute("SELECT * FROM staff")
        staff = cursor.fetchall()
    conn.close()
    return staff

# --- REVENUE ---
@app.get("/revenue")
def get_revenue():
    conn = pymysql.connect(**DB_CONFIG, cursorclass=pymysql.cursors.DictCursor)
    with conn.cursor() as cursor:
        cursor.execute("SELECT source, SUM(amount) as total FROM revenue_log GROUP BY source")
        rows = cursor.fetchall()
        
        rev = {row["source"]: float(row["total"]) for row in rows}
        rev["total"] = sum(rev.values())
    conn.close()
    return rev

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8080)
