import customtkinter as ctk
import requests
import json
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from tkinter import ttk, messagebox

ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("blue")

API_URL = "http://localhost:8080"

class HotelApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Luxe Hotel Management 3.0")
        self.geometry("1200x800")
        self.token = None
        self.role = None
        
        self.show_login()

    def show_login(self):
        self.login_frame = ctk.CTkFrame(self)
        self.login_frame.place(relx=0.5, rely=0.5, anchor="center")
        
        ctk.CTkLabel(self.login_frame, text="Admin Login", font=("Segoe UI", 24, "bold")).pack(pady=20, padx=40)
        
        self.user_entry = ctk.CTkEntry(self.login_frame, placeholder_text="Username", width=250)
        self.user_entry.pack(pady=10)
        
        self.pass_entry = ctk.CTkEntry(self.login_frame, placeholder_text="Password", show="*", width=250)
        self.pass_entry.pack(pady=10)
        
        ctk.CTkButton(self.login_frame, text="Login", command=self.login).pack(pady=20)

    def login(self):
        user = self.user_entry.get()
        pw = self.pass_entry.get()
        try:
            res = requests.post(f"{API_URL}/auth/login", json={"username": user, "password": pw})
            if res.status_code == 200:
                data = res.json()
                self.token = data["token"]
                self.role = data["role"]
                self.login_frame.destroy()
                self.show_main()
            else:
                messagebox.showerror("Error", "Invalid Credentials")
        except Exception as e:
            messagebox.showerror("Error", f"Could not connect to server: {e}")

    def show_main(self):
        # Sidebar
        self.sidebar = ctk.CTkFrame(self, width=200, corner_radius=0)
        self.sidebar.pack(side="left", fill="y")
        
        ctk.CTkLabel(self.sidebar, text="LUXE HOTEL", font=("Segoe UI", 20, "bold")).pack(pady=20)
        
        buttons = ["Dashboard", "Rooms", "Bookings", "Customers", "Staff", "Finance"]
        self.nav_btns = {}
        for btn in buttons:
            b = ctk.CTkButton(self.sidebar, text=btn, corner_radius=0, height=40, fg_color="transparent", 
                              anchor="w", command=lambda x=btn: self.switch_tab(x))
            b.pack(fill="x")
            self.nav_btns[btn] = b

        # Main Content
        self.content_frame = ctk.CTkFrame(self, corner_radius=10, fg_color="#1a1a1a")
        self.content_frame.pack(side="right", expand=True, fill="both", padx=20, pady=20)
        
        self.switch_tab("Dashboard")

    def switch_tab(self, name):
        for btn in self.nav_btns.values(): btn.configure(fg_color="transparent")
        self.nav_btns[name].configure(fg_color="#3b3b3b")
        
        for widget in self.content_frame.winfo_children(): widget.destroy()
        
        if name == "Dashboard": self.draw_dashboard()
        elif name == "Rooms": self.draw_rooms()
        elif name == "Bookings": self.draw_bookings()
        elif name == "Staff": self.draw_staff()
        elif name == "Finance": self.draw_finance()

    def draw_dashboard(self):
        lbl = ctk.CTkLabel(self.content_frame, text="Overview", font=("Segoe UI", 24, "bold"))
        lbl.pack(pady=20)
        
        try:
            rooms = requests.get(f"{API_URL}/rooms").json()
            total = len(rooms)
            avail = len([r for r in rooms if r["availability"]])
            maint = len([r for r in rooms if r["maintenance"]])
            booked = total - avail - maint

            # Charts
            fig, ax = plt.subplots(figsize=(5, 4), facecolor='#1a1a1a')
            labels = ['Available', 'Booked', 'Maint']
            sizes = [avail, booked, maint]
            colors = ['#2ecc71', '#e67e22', '#e74c3c']
            
            ax.pie(sizes, labels=labels, autopct='%1.1f%%', colors=colors, textprops={'color':"w"})
            ax.set_title("Occupancy Distribution", color='w')
            
            canvas = FigureCanvasTkAgg(fig, master=self.content_frame)
            canvas.draw()
            canvas.get_tk_widget().pack(pady=20)
        except:
            ctk.CTkLabel(self.content_frame, text="Error loading data").pack()

    def draw_rooms(self):
        title = ctk.CTkLabel(self.content_frame, text="Room Management", font=("Segoe UI", 24, "bold"))
        title.pack(pady=10)
        
        # Table
        style = ttk.Style()
        style.theme_use("clam")
        style.configure("Treeview", background="#2b2b2b", foreground="white", fieldbackground="#2b2b2b", borderwidth=0)
        style.map("Treeview", background=[('selected', '#3b3b3b')])
        
        tree_frame = ctk.CTkFrame(self.content_frame)
        tree_frame.pack(expand=True, fill="both", padx=20, pady=10)
        
        self.room_tree = ttk.Treeview(tree_frame, columns=("Num", "Type", "Price", "Cap", "Status"), show="headings")
        for col, head in zip(self.room_tree["columns"], ("Room #", "Type", "Price", "Capacity", "Status")):
            self.room_tree.heading(col, text=head)
            self.room_tree.column(col, width=100, anchor="center")
        self.room_tree.pack(expand=True, fill="both")
        
        self.refresh_rooms()

    def refresh_rooms(self):
        try:
            rooms = requests.get(f"{API_URL}/rooms").json()
            for i in self.room_tree.get_children(): self.room_tree.delete(i)
            for r in rooms:
                status = "Maint" if r["maintenance"] else ("Avail" if r["availability"] else "Occupied")
                self.room_tree.insert("", "end", values=(r["number"], r["type"], f"${r['price']}", r["capacity"], status))
        except: pass

    def draw_bookings(self):
        ctk.CTkLabel(self.content_frame, text="New Booking", font=("Segoe UI", 24, "bold")).pack(pady=20)
        
        form = ctk.CTkFrame(self.content_frame)
        form.pack(pady=10)
        
        entries = {}
        for lbl in ["name", "phone", "room_number", "check_in", "check_out"]:
            ctk.CTkLabel(form, text=lbl.capitalize()).pack()
            e = ctk.CTkEntry(form, width=300)
            e.pack(pady=5)
            entries[lbl] = e
            
        def submit():
            data = {k: (int(v.get()) if k=="room_number" else v.get()) for k, v in entries.items()}
            res = requests.post(f"{API_URL}/bookings", json=data)
            if res.status_code == 200:
                messagebox.showinfo("Success", "Booking created")
                self.switch_tab("Bookings")
            else:
                messagebox.showerror("Error", "Booking failed")
                
        ctk.CTkButton(self.content_frame, text="Confirm Booking", command=submit).pack(pady=20)

    def draw_staff(self):
        if self.role != "admin":
            ctk.CTkLabel(self.content_frame, text="Access Denied").pack(pady=100)
            return
        
        ctk.CTkLabel(self.content_frame, text="Staff Directory", font=("Segoe UI", 24, "bold")).pack(pady=20)
        
        try:
            staff = requests.get(f"{API_URL}/staff").json()
            for s in staff:
                card = ctk.CTkFrame(self.content_frame, height=50)
                card.pack(fill="x", padx=40, pady=5)
                ctk.CTkLabel(card, text=f"ID: {s['id']} | {s['name']} - {s['role']} | Salary: ${s['salary']}").pack(side="left", padx=20)
        except: pass

    def draw_finance(self):
        ctk.CTkLabel(self.content_frame, text="Financial Reports", font=("Segoe UI", 24, "bold")).pack(pady=20)
        try:
            rev = requests.get(f"{API_URL}/revenue").json()
            f = ctk.CTkFrame(self.content_frame)
            f.pack(pady=20)
            ctk.CTkLabel(f, text=f"Total Revenue: ${rev['total']:.2f}", font=("Segoe UI", 18, "bold")).pack(pady=10)
            ctk.CTkLabel(f, text=f"Room Revenue: ${rev.get('Room', 0):.2f}").pack()
            ctk.CTkLabel(f, text=f"Service Revenue: ${rev.get('Service', 0):.2f}").pack()
        except: pass

if __name__ == "__main__":
    app = HotelApp()
    app.mainloop()
