from fastapi import FastAPI, HTTPException, Request
from pydantic import BaseModel
import pymysql
import hashlib
from typing import List, Optional
import uvicorn
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("hotel-backend")

app = FastAPI()

# MySQL Configuration (XAMPP default)
DB_CONFIG = {
    "host": "localhost",
    "user": "root",
    "password": "",
    "database": "hotel_db",
    "charset": "utf8mb4",
    "cursorclass": pymysql.cursors.DictCursor
}

def get_db_conn():
    try:
        return pymysql.connect(**DB_CONFIG)
    except Exception as e:
        logger.error(f"Database connection failed: {e}")
        return None

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
async def login(req: LoginRequest):
    conn = get_db_conn()
    if not conn:
        raise HTTPException(status_code=500, detail="Database connection error")
    
    try:
        with conn.cursor() as cursor:
            sql = "SELECT role FROM users WHERE username = %s AND password_hash = %s"
            cursor.execute(sql, (req.username, hash_password(req.password)))
            user = cursor.fetchone()
            if user:
                return {"status": "success", "role": user["role"], "token": "dummy-jwt-token"}
    except Exception as e:
        logger.error(f"Login error: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")
    finally:
        conn.close()
    
    raise HTTPException(status_code=401, detail="Invalid username or password")

# --- ROOMS ---
@app.get("/rooms")
async def get_rooms():
    conn = get_db_conn()
    if not conn:
        return []
    try:
        with conn.cursor() as cursor:
            cursor.execute("SELECT * FROM rooms")
            return cursor.fetchall()
    finally:
        conn.close()

@app.post("/rooms")
async def add_room(req: RoomRequest):
    conn = get_db_conn()
    if not conn:
        raise HTTPException(status_code=500, detail="Database connection error")
    try:
        with conn.cursor() as cursor:
            sql = "INSERT INTO rooms (number, type, price, capacity, availability, maintenance) VALUES (%s, %s, %s, %s, 1, 0)"
            cursor.execute(sql, (req.number, req.type, req.price, req.capacity))
        conn.commit()
        return {"status": "success"}
    except pymysql.err.IntegrityError:
        raise HTTPException(status_code=400, detail="Room number already exists")
    except Exception as e:
        logger.error(f"Add room error: {e}")
        raise HTTPException(status_code=500, detail=str(e))
    finally:
        conn.close()

# --- BOOKINGS ---
@app.post("/bookings")
async def book_room(req: BookingRequest):
    conn = get_db_conn()
    if not conn:
        raise HTTPException(status_code=500, detail="Database connection error")
    try:
        with conn.cursor() as cursor:
            # Check room availability
            cursor.execute("SELECT availability, maintenance FROM rooms WHERE number = %s", (req.room_number,))
            room = cursor.fetchone()
            if not room:
                raise HTTPException(status_code=404, detail="Room not found")
            if not room["availability"] or room["maintenance"]:
                raise HTTPException(status_code=400, detail="Room is not available for booking")
            
            # Record customer
            cursor.execute("INSERT INTO customers (name, phone, room_number, check_in, check_out) VALUES (%s, %s, %s, %s, %s)",
                           (req.name, req.phone, req.room_number, req.check_in, req.check_out))
            
            # Mark room as occupied
            cursor.execute("UPDATE rooms SET availability = 0 WHERE number = %s", (req.room_number,))
            
            # Log Revenue
            cursor.execute("INSERT INTO revenue_log (amount, source) SELECT price, 'Room' FROM rooms WHERE number = %s", (req.room_number,))
            
        conn.commit()
        return {"status": "success"}
    except Exception as e:
        conn.rollback()
        logger.error(f"Booking error: {e}")
        raise HTTPException(status_code=500, detail=str(e))
    finally:
        conn.close()

# --- STAFF ---
@app.get("/staff")
async def get_staff():
    conn = get_db_conn()
    if not conn:
        return []
    try:
        with conn.cursor() as cursor:
            cursor.execute("SELECT * FROM staff")
            return cursor.fetchall()
    finally:
        conn.close()

# --- REVENUE ---
@app.get("/revenue")
async def get_revenue():
    conn = get_db_conn()
    if not conn:
        return {"Room": 0, "Service": 0, "total": 0}
    try:
        with conn.cursor() as cursor:
            cursor.execute("SELECT source, SUM(amount) as total FROM revenue_log GROUP BY source")
            rows = cursor.fetchall()
            
            rev = {"Room": 0.0, "Service": 0.0}
            total = 0.0
            for row in rows:
                source = row["source"]
                amt = float(row["total"]) if row["total"] is not None else 0.0
                rev[source] = amt
                total += amt
            rev["total"] = total
            return rev
    finally:
        conn.close()

if __name__ == "__main__":
    logger.info("Starting Backend Server...")
    uvicorn.run(app, host="127.0.0.1", port=8080)
