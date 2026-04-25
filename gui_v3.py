import customtkinter as ctk
import requests
import json
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from tkinter import ttk, messagebox
import sys

ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("blue")

API_URL = "http://127.0.0.1:8080"

class HotelApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Luxe Hotel Management 3.0")
        self.geometry("1200x850")
        self.token = None
        self.role = None
        
        self.show_login()

    def show_login(self):
        self.login_frame = ctk.CTkFrame(self)
        self.login_frame.place(relx=0.5, rely=0.5, anchor="center")
        
        ctk.CTkLabel(self.login_frame, text="🏨 LUXE HOTEL LOGIN", font=("Segoe UI", 26, "bold")).pack(pady=(20, 10), padx=40)
        ctk.CTkLabel(self.login_frame, text="Please enter your credentials", font=("Segoe UI", 12)).pack(pady=(0, 20))
        
        self.user_entry = ctk.CTkEntry(self.login_frame, placeholder_text="Username", width=280, height=35)
        self.user_entry.pack(pady=10)
        self.user_entry.insert(0, "admin") # Default for convenience
        
        self.pass_entry = ctk.CTkEntry(self.login_frame, placeholder_text="Password", show="*", width=280, height=35)
        self.pass_entry.pack(pady=10)
        
        self.login_btn = ctk.CTkButton(self.login_frame, text="Login to System", font=("Segoe UI", 14, "bold"), 
                                      height=40, width=280, command=self.login)
        self.login_btn.pack(pady=30)

    def login(self):
        user = self.user_entry.get()
        pw = self.pass_entry.get()
        if not user or not pw:
            messagebox.showwarning("Warning", "Please fill in all fields")
            return
            
        try:
            res = requests.post(f"{API_URL}/auth/login", json={"username": user, "password": pw}, timeout=5)
            if res.status_code == 200:
                data = res.json()
                self.token = data["token"]
                self.role = data["role"]
                self.login_frame.destroy()
                self.show_main()
            else:
                messagebox.showerror("Error", res.json().get("detail", "Invalid Credentials"))
        except requests.exceptions.ConnectionError:
            messagebox.showerror("Error", "Could not connect to the Backend Server.\nMake sure XAMPP and backend_v3.py are running.")
        except Exception as e:
            messagebox.showerror("Error", f"Unexpected error: {e}")

    def show_main(self):
        # Sidebar
        self.sidebar = ctk.CTkFrame(self, width=240, corner_radius=0)
        self.sidebar.pack(side="left", fill="y")
        
        ctk.CTkLabel(self.sidebar, text="🏨 LUXE ADMIN", font=("Segoe UI", 20, "bold")).pack(pady=30)
        
        self.nav_btns = {}
        tabs = [
            ("Dashboard", "📊"), 
            ("Rooms", "🛏️"), 
            ("Bookings", "📝"), 
            ("Customers", "👥"), 
            ("Staff", "👔"), 
            ("Finance", "💰")
        ]
        
        for name, icon in tabs:
            b = ctk.CTkButton(self.sidebar, text=f"{icon}  {name}", corner_radius=0, height=50, 
                              fg_color="transparent", font=("Segoe UI", 13),
                              anchor="w", command=lambda x=name: self.switch_tab(x))
            b.pack(fill="x", padx=10, pady=2)
            self.nav_btns[name] = b

        ctk.CTkButton(self.sidebar, text="🚪 Logout", fg_color="#c0392b", hover_color="#962d22", 
                      command=self.logout).pack(side="bottom", fill="x", padx=20, pady=20)

        # Main Content
        self.content_frame = ctk.CTkFrame(self, corner_radius=15, fg_color="#1e1e1e")
        self.content_frame.pack(side="right", expand=True, fill="both", padx=20, pady=20)
        
        self.switch_tab("Dashboard")

    def logout(self):
        if messagebox.askyesno("Logout", "Are you sure you want to logout?"):
            python = sys.executable
            os.execl(python, python, *sys.argv)

    def switch_tab(self, name):
        for btn in self.nav_btns.values(): btn.configure(fg_color="transparent")
        self.nav_btns[name].configure(fg_color="#2980b9")
        
        for widget in self.content_frame.winfo_children(): widget.destroy()
        
        header = ctk.CTkLabel(self.content_frame, text=name.upper(), font=("Segoe UI", 24, "bold"))
        header.pack(pady=(20, 10))
        
        if name == "Dashboard": self.draw_dashboard()
        elif name == "Rooms": self.draw_rooms()
        elif name == "Bookings": self.draw_bookings()
        elif name == "Staff": self.draw_staff()
        elif name == "Finance": self.draw_finance()

    def draw_dashboard(self):
        try:
            res = requests.get(f"{API_URL}/rooms", timeout=3)
            rooms = res.json()
            total = len(rooms)
            avail = len([r for r in rooms if r["availability"] == 1])
            maint = len([r for r in rooms if r["maintenance"] == 1])
            booked = total - avail - maint

            # Stats Cards
            stats_f = ctk.CTkFrame(self.content_frame, fg_color="transparent")
            stats_f.pack(fill="x", padx=40, pady=10)
            
            cards = [("Total", total, "#3498db"), ("Available", avail, "#2ecc71"), 
                     ("Occupied", booked, "#e67e22"), ("In Maint.", maint, "#e74c3c")]
            
            for i, (l, v, c) in enumerate(cards):
                card = ctk.CTkFrame(stats_f, width=150, height=100, corner_radius=10)
                card.grid(row=0, column=i, padx=10, sticky="nsew")
                ctk.CTkLabel(card, text=l, font=("Segoe UI", 12, "bold"), text_color="#bdc3c7").pack(pady=(15, 0))
                ctk.CTkLabel(card, text=str(v), font=("Segoe UI", 28, "bold"), text_color=c).pack(pady=(0, 15))
            
            stats_f.grid_columnconfigure((0,1,2,3), weight=1)

            # Pie Chart
            fig, ax = plt.subplots(figsize=(6, 4.5), facecolor='#1e1e1e')
            labels = ['Available', 'Booked', 'Maintenance']
            sizes = [avail, booked, maint]
            if sum(sizes) == 0: sizes = [1, 0, 0] # Avoid empty pie
            
            ax.pie(sizes, labels=labels, autopct='%1.1f%%', colors=['#2ecc71', '#e67e22', '#e74c3c'], 
                   textprops={'color':"w", 'weight': 'bold'}, startangle=90, explode=(0.05, 0.05, 0.05))
            ax.set_title("Room Occupancy Status", color='w', pad=20, fontdict={'fontsize': 14, 'weight': 'bold'})
            
            canvas = FigureCanvasTkAgg(fig, master=self.content_frame)
            canvas.draw()
            canvas.get_tk_widget().pack(pady=10)
        except Exception as e:
            ctk.CTkLabel(self.content_frame, text=f"⚠️ Connection Error: {e}", text_color="#e74c3c").pack(pady=50)

    def draw_rooms(self):
        f = ctk.CTkFrame(self.content_frame, fg_color="transparent")
        f.pack(expand=True, fill="both", padx=30, pady=10)
        
        # Table Styling
        style = ttk.Style()
        style.configure("Custom.Treeview", background="#2b2b2b", foreground="white", fieldbackground="#2b2b2b", rowheight=30)
        style.map("Custom.Treeview", background=[('selected', '#2980b9')])
        
        self.room_tree = ttk.Treeview(f, columns=("Num", "Type", "Price", "Cap", "Status"), show="headings", style="Custom.Treeview")
        for col, head in zip(self.room_tree["columns"], ("Room No", "Type", "Price", "Cap", "Status")):
            self.room_tree.heading(col, text=head)
            self.room_tree.column(col, width=100, anchor="center")
        self.room_tree.pack(expand=True, fill="both")
        
        self.refresh_rooms()
        
        btn_f = ctk.CTkFrame(self.content_frame, fg_color="transparent")
        btn_f.pack(fill="x", padx=30, pady=10)
        ctk.CTkButton(btn_f, text="➕ Add New Room", command=self.add_room_window).pack(side="right", padx=5)
        ctk.CTkButton(btn_f, text="🔄 Refresh List", command=self.refresh_rooms, fg_color="#7f8c8d").pack(side="right", padx=5)

    def refresh_rooms(self):
        try:
            res = requests.get(f"{API_URL}/rooms", timeout=3)
            rooms = res.json()
            for i in self.room_tree.get_children(): self.room_tree.delete(i)
            for r in rooms:
                status = "⚠️ Maint" if r["maintenance"] == 1 else ("✅ Avail" if r["availability"] == 1 else "🏨 Occupied")
                self.room_tree.insert("", "end", values=(r["number"], r["type"], f"${r['price']}", r["capacity"], status))
        except: pass

    def add_room_window(self):
        win = ctk.CTkToplevel(self)
        win.title("Add Room")
        win.geometry("400x450")
        win.attributes('-topmost', True)
        
        ctk.CTkLabel(win, text="Add New Room", font=("Segoe UI", 18, "bold")).pack(pady=20)
        
        fields = ["Room Number", "Type", "Price", "Capacity"]
        entries = {}
        for f in fields:
            ctk.CTkLabel(win, text=f).pack(pady=(5, 0))
            e = ctk.CTkEntry(win, width=250)
            e.pack(pady=5)
            entries[f] = e
            
        def save():
            try:
                data = {
                    "number": int(entries["Room Number"].get()),
                    "type": entries["Type"].get(),
                    "price": float(entries["Price"].get()),
                    "capacity": int(entries["Capacity"].get())
                }
                res = requests.post(f"{API_URL}/rooms", json=data)
                if res.status_code == 200:
                    messagebox.showinfo("Success", "Room added!")
                    win.destroy()
                    self.refresh_rooms()
                else:
                    messagebox.showerror("Error", res.json().get("detail", "Failed"))
            except ValueError:
                messagebox.showerror("Error", "Invalid input format")

        ctk.CTkButton(win, text="Save Room", command=save).pack(pady=30)

    def draw_bookings(self):
        form = ctk.CTkFrame(self.content_frame, corner_radius=15)
        form.pack(pady=20, padx=50, fill="x")
        
        labels = ["Guest Name", "Phone", "Room Number", "Check-in (Date)", "Check-out (Date)"]
        keys = ["name", "phone", "room_number", "check_in", "check_out"]
        self.book_entries = {}
        
        for lbl, key in zip(labels, keys):
            f = ctk.CTkFrame(form, fg_color="transparent")
            f.pack(fill="x", padx=20, pady=5)
            ctk.CTkLabel(f, text=lbl, width=120, anchor="w").pack(side="left")
            e = ctk.CTkEntry(f, width=300)
            e.pack(side="left", expand=True, fill="x", padx=10)
            self.book_entries[key] = e
            
        def submit():
            try:
                data = {k: (int(v.get()) if k=="room_number" else v.get()) for k, v in self.book_entries.items()}
                res = requests.post(f"{API_URL}/bookings", json=data)
                if res.status_code == 200:
                    messagebox.showinfo("Success", "Booking successful!")
                    for e in self.book_entries.values(): e.delete(0, 'end')
                else:
                    messagebox.showerror("Error", res.json().get("detail", "Failed to book"))
            except:
                messagebox.showerror("Error", "Check your inputs")
                
        ctk.CTkButton(self.content_frame, text="✅ Confirm & Process Booking", font=("Segoe UI", 14, "bold"), 
                      height=45, width=300, command=submit).pack(pady=20)

    def draw_staff(self):
        if self.role != "admin":
            ctk.CTkLabel(self.content_frame, text="🔒 Staff management is restricted to Admins.", 
                         font=("Segoe UI", 16), text_color="#bdc3c7").pack(pady=100)
            return
        
        f = ctk.CTkFrame(self.content_frame, fg_color="transparent")
        f.pack(expand=True, fill="both", padx=30, pady=10)
        
        try:
            staff = requests.get(f"{API_URL}/staff").json()
            for s in staff:
                card = ctk.CTkFrame(f, height=60, corner_radius=8)
                card.pack(fill="x", pady=5)
                ctk.CTkLabel(card, text=f"👤 {s['name']}", font=("Segoe UI", 13, "bold"), width=200, anchor="w").pack(side="left", padx=20)
                ctk.CTkLabel(card, text=f"Role: {s['role']}", width=150).pack(side="left")
                ctk.CTkLabel(card, text=f"Salary: ${s['salary']:.2f}", text_color="#2ecc71").pack(side="right", padx=20)
        except Exception as e:
            ctk.CTkLabel(f, text="Failed to load staff list").pack()

    def draw_finance(self):
        try:
            rev = requests.get(f"{API_URL}/revenue").json()
            
            main_f = ctk.CTkFrame(self.content_frame, corner_radius=15)
            main_f.pack(pady=20, padx=40, fill="both", expand=True)
            
            ctk.CTkLabel(main_f, text="Revenue Summary", font=("Segoe UI", 20, "bold")).pack(pady=20)
            
            stats = [("Room Revenue", rev.get('Room', 0), "#3498db"), 
                     ("Service Revenue", rev.get('Service', 0), "#9b59b6"), 
                     ("Total Profit", rev['total'], "#2ecc71")]
            
            for l, v, c in stats:
                row = ctk.CTkFrame(main_f, fg_color="transparent")
                row.pack(fill="x", padx=50, pady=10)
                ctk.CTkLabel(row, text=l, font=("Segoe UI", 14)).pack(side="left")
                ctk.CTkLabel(row, text=f"${v:,.2f}", font=("Segoe UI", 18, "bold"), text_color=c).pack(side="right")
                
            # Chart
            fig, ax = plt.subplots(figsize=(5, 3), facecolor='#2b2b2b')
            ax.set_facecolor('#2b2b2b')
            labels = ['Room', 'Service']
            values = [rev.get('Room', 0), rev.get('Service', 0)]
            
            ax.bar(labels, values, color=['#3498db', '#9b59b6'])
            ax.tick_params(colors='white')
            for spine in ax.spines.values(): spine.set_color('white')
            
            canvas = FigureCanvasTkAgg(fig, master=main_f)
            canvas.draw()
            canvas.get_tk_widget().pack(pady=20)
            
        except Exception as e:
            ctk.CTkLabel(self.content_frame, text="Error loading financial data").pack(pady=50)

if __name__ == "__main__":
    import os
    app = HotelApp()
    app.mainloop()
