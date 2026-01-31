#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include <cstdlib>
#include <iomanip>
using namespace std;

// Color defines
#define RESET  "\033[0m"
#define GREEN  "\033[32m"
#define RED    "\033[31m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"

// Global variables for revenue
float totalRevenue;              // Total income from bookings + services
float totalServicesRevenue;      // Revenue from services added to rooms
float totalRoomRevenue;          // Revenue from room booking only

class Room {
private:
    int roomNumber;
    string type;
    float price;
    int capacity;
    bool isAvailable;
    bool isUnderMaintenance;

public:
    Room();
    Room(int number, string type, float price, int capacity);

    int getRoomNumber() const;
    string getType() const;
    float getPrice() const;
    int getCapacity() const;
    bool getAvailability() const;
    bool getMaintenanceStatus() const;

    void setAvailability(bool status);
    void setMaintenanceStatus(bool status);
    void displayRoomInfo() const;
};

class Customer {
private:
    string name;
    string phone;
    int roomNumber;
    string checkInDate;
    string checkOutDate;

public:
    Customer();
    Customer(string name, string phone, int roomNumber, string checkIn, string checkOut);

    string getName() const;
    string getCheckInDate() const;
    string getCheckOutDate() const;
    string getPhone() const;
    int getRoomNumber() const;
    int getStayDuration() const;
    void displayCustomer() const;
};

class Staff {
private:
    int id;
    string name;
    string role;
    float salary;

public:
    Staff() {
        id = 0;
        salary = 0;
    }

    Staff(int i, string n, string r, float s) {
        id = i;
        name = n;
        role = r;
        salary = s;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    string getRole() const { return role; }
    float getSalary() const { return salary; }

    void display() const {
        cout << id << "\t" << name << "\t" << role << "\t" << salary << endl;
    }
};

class Hotel {
private:
    Room rooms[100];
    Customer customers[100];
    int roomCount;
    int customerCount;

    void saveRoomsToFile() const;
    void autoCreateFiles();
    void loadRoomsFromFile();
    void saveCustomersToFile() const;
    void loadCustomersFromFile();

    string feedbackList[200];        // Store feedback
    int feedbackCount;               // Number of feedback entries

    string maintenanceLog[200];      // Maintenance logs
    int maintenanceCount;

    Staff staffList[50];
    int staffCount;

    int feedbackStars[200];     // Star rating (1–5)

public:
    Hotel();
    ~Hotel();
    void initialize();

    // Room functions
    void addRoom();
    void searchRoomByType() const;
    void filterRoomsByPrice() const;
    void displayAvailableRooms() const;

    // Room booking functions
    void bookRoom();
    void cancelBooking();
    void displayAllBookings() const;

    // Service functions
    void addServiceToRoom(int roomNumber, string serviceName, float cost);

    // Billing functions
    void generateInvoice();
    void submitFeedback();
    void viewAllFeedback() const;

    // Maintenance functions
    
    void markRoomUnderMaintenance(int roomNumber);
    void logMaintenance(int roomNumber, string issue);

    // Customer functions
    void displayAllCustomers() const;
    void searchCustomerByPhone() const;
    float calculateStayBill(string phone);

    // Admin functions
    void viewProfits() const;
    void showAdminWindow();
    void viewCharts();
    void viewMaintenanceLogs() const;
    bool verifyAdminPassword();
    void adminLogin();

    // Staff related
    void addStaff();
    void viewAllStaff() const;
    void removeStaff();
    void staffPanel();
    void saveStaffToFile() const;
    void loadStaffFromFile();
    void buildRoomLinkedList();
};
struct RoomNode {
    Room data;
    RoomNode* next;
};

struct BookingNode {
    Customer data;
    BookingNode* next;
};

struct ServiceNode {
    string serviceName;
    float cost;
    ServiceNode* next;
};

struct CustomerNode {
    Customer data;
    CustomerNode* next;
};

// Global variables for linked lists
RoomNode* roomHead = nullptr;
BookingNode* frontBooking = nullptr;
BookingNode* rearBooking = nullptr;
ServiceNode* serviceTop = nullptr;

// Global functions for linked lists
void insertRoomNode(Room r) {
    RoomNode* node = new RoomNode{ r, nullptr };

    if (!roomHead) {
        roomHead = node;
    } else {
        RoomNode* temp = roomHead;
        while (temp->next)
            temp = temp->next;
        temp->next = node;
    }
}

void enqueueBooking(Customer c) {
    BookingNode* node = new BookingNode{ c, nullptr };

    if (!rearBooking) {
        frontBooking = rearBooking = node;
    } else {
        rearBooking->next = node;
        rearBooking = node;
    }
}

void pushService(string name, float cost) {
    ServiceNode* node = new ServiceNode{ name, cost, serviceTop };
    serviceTop = node;
}

RoomNode* createRoomList(Room rooms[], int roomCount) {
    RoomNode* head = nullptr;
    RoomNode* tail = nullptr;

    for (int i = 0; i < roomCount; ++i) {
        RoomNode* newNode = new RoomNode{rooms[i], nullptr};
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    return head;
}

CustomerNode* createCustomerList(Customer customers[], int customerCount) {
    CustomerNode* head = nullptr;
    CustomerNode* tail = nullptr;

    for (int i = 0; i < customerCount; ++i) {
        CustomerNode* newNode = new CustomerNode{customers[i], nullptr};
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    return head;
}

// Class definitions


// Room class implementations
Room::Room() : roomNumber(0), type(""), price(0.0), capacity(0), isAvailable(true), isUnderMaintenance(false) {}

Room::Room(int number, string type, float price, int capacity)
    : roomNumber(number), type(type), price(price), capacity(capacity), isAvailable(true), isUnderMaintenance(false) {}

int Room::getRoomNumber() const { return roomNumber; }
string Room::getType() const { return type; }
float Room::getPrice() const { return price; }
int Room::getCapacity() const { return capacity; }
bool Room::getAvailability() const { return isAvailable; }
bool Room::getMaintenanceStatus() const { return isUnderMaintenance; }

void Room::setAvailability(bool status) { isAvailable = status; }
void Room::setMaintenanceStatus(bool status) { isUnderMaintenance = status; }

void Room::displayRoomInfo() const {
    cout << "Room Number: " << roomNumber << ", Type: " << type
         << ", Price: $" << price << ", Capacity: " << capacity << endl;
}

// Customer class implementations
Customer::Customer() : name(""), phone(""), roomNumber(0), checkInDate(""), checkOutDate("") {}
Customer::Customer(string n, string p, int rn, string ci, string co) : name(n), phone(p), roomNumber(rn), checkInDate(ci), checkOutDate(co) {}

string Customer::getName() const { return name; }
string Customer::getPhone() const { return phone; }
int Customer::getRoomNumber() const { return roomNumber; }
string Customer::getCheckInDate() const { return checkInDate; }
string Customer::getCheckOutDate() const { return checkOutDate; }

int Customer::getStayDuration() const {
    return 3; // placeholder
}

void Customer::displayCustomer() const {
    cout << "Name: " << name << ", Phone: " << phone << ", Room: " << roomNumber
         << ", Check-in: " << checkInDate << ", Check-out: " << checkOutDate << endl;
}

// Hotel class implementations (sorted alphabetically by function name)
Hotel::Hotel() {
    roomCount = 0;
    customerCount = 0;
    feedbackCount = 0;
    maintenanceCount = 0;
    staffCount = 0;

    totalRevenue = 0;
    totalRoomRevenue = 0;
    totalServicesRevenue = 0;

    // Create files if missing
    autoCreateFiles();

    // Load data
    loadRoomsFromFile();
    loadCustomersFromFile();
    loadStaffFromFile();
    buildRoomLinkedList();
}

Hotel::~Hotel() {
    saveRoomsToFile();
    saveCustomersToFile();
    autoCreateFiles();
    saveStaffToFile();
}

void Hotel::addRoom() {
    if (roomCount >= 100) {
        cout << "Room limit reached!\n";
        system("pause");
        system("cls");
        return;
    }

    int number;
    string type;
    float price;
    int capacity;

    cout << "Enter room number: ";
    while (!(cin >> number)) {
        cout << "Invalid input. Enter a valid room number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    for (int i = 0; i < roomCount; ++i) {
        if (rooms[i].getRoomNumber() == number) {
            cout << "Room number already exists.\n";
            system("pause");
            system("cls");
            return;
        }
    }

    cout << "Enter room type: ";
    cin >> type;

    cout << "Enter price: ";
    while (!(cin >> price)) {
        cout << "Invalid input. Enter a valid price: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Enter capacity: ";
    while (!(cin >> capacity)) {
        cout << "Invalid input. Enter a valid capacity: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    rooms[roomCount++] = Room(number, type, price, capacity);
    cout << "Room added successfully.\n";
    saveRoomsToFile(); // Save updated room data to file
    system("pause");
    system("cls");
}

void Hotel::addServiceToRoom(int roomNumber, string serviceName, float cost) {
    RoomNode* head = createRoomList(rooms, roomCount);
    RoomNode* temp = head;

    while (temp) {
        if (temp->data.getRoomNumber() == roomNumber) {
            totalServicesRevenue += cost;
            totalRevenue += cost;

            cout << "Service added successfully!\n";
            return;
        }
        temp = temp->next;
    }

    cout << "Room not found.\n";
    system("pause");
    system("cls");
}

void Hotel::addStaff() {
    int id;
    string name, role;
    float salary;

    cout << "Enter Staff ID: ";
    cin >> id;
    cin.ignore();

    cout << "Enter Name: ";
    getline(cin, name);

    cout << "Enter Role: ";
    getline(cin, role);

    cout << "Enter Salary: ";
    cin >> salary;

    staffList[staffCount++] = Staff(id, name, role, salary);
    saveStaffToFile();

    cout << "Staff added successfully.\n";
}

void Hotel::adminLogin() {
    if (verifyAdminPassword()) {
        showAdminWindow();
    } else {
        cout << "Incorrect password. Access denied.\n";
    }
}

void Hotel::autoCreateFiles() {
    // Create rooms.txt
    {
        ifstream fin("rooms.txt");
        if (!fin) {
            ofstream fout("rooms.txt");
            fout << "";   // empty initial file
            fout.close();
        }
    }

    // Create customers.txt
    {
        ifstream fin("customers.txt");
        if (!fin) {
            ofstream fout("customers.txt");
            fout << "";
            fout.close();
        }
    }

    // Create feedback.txt
    {
        ifstream fin("feedback.txt");
        if (!fin) {
            ofstream fout("feedback.txt");
            fout << "";
            fout.close();
        }
    }

    // Create maintenance.txt
    {
        ifstream fin("maintenance.txt");
        if (!fin) {
            ofstream fout("maintenance.txt");
            fout << "";
            fout.close();
        }
    }

    // Create revenue.txt with default values
    {
        ifstream fin("revenue.txt");
        if (!fin) {
            ofstream fout("revenue.txt");
            fout << "0 0 0";  // roomRev serviceRev totalRev
            fout.close();
        }
    }

    // Create admin_credentials.txt
    {
        ifstream fin("admin_credentials.txt");
        if (!fin) {
            ofstream fout("admin_credentials.txt");
            fout << "admin123";  // default admin password
            fout.close();
        }
    }

    // Create staff.txt
    {
        ifstream fin("staff.txt");
        if (!fin) {
            ofstream fout("staff.txt");
            fout.close();
        }
    }
}

void Hotel::bookRoom() {
    if (customerCount >= 100) {
        cout << "Booking limit reached!\n";
        return;
    }

    string name, phone, checkIn, checkOut;
    int roomNumber;

    cout << "Enter name phone checkIn checkOut: ";
    cin >> name >> phone >> checkIn >> checkOut;

    displayAvailableRooms();

    cout << "Enter room number: ";
    cin >> roomNumber;

    // Linked list search
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].getRoomNumber() == roomNumber &&
            rooms[i].getAvailability()) {

            rooms[i].setAvailability(false);

            Customer c(name, phone, roomNumber, checkIn, checkOut);

            customers[customerCount++] = c;
            enqueueBooking(c);   // QUEUE USED

            saveRoomsToFile();
            saveCustomersToFile();

            cout << "Room booked successfully!\n";
            return;
        }
    }

    cout << "Room not available.\n";
    system("pause");
    system("cls");
}

void Hotel::buildRoomLinkedList() {
    roomHead = nullptr;

    for (int i = 0; i < roomCount; i++) {
        insertRoomNode(rooms[i]);
    }
}

float Hotel::calculateStayBill(string phone) {
    // This function is declared as float but has no return in original; assuming it's to calculate and perhaps print or return
    // Original has: float calculateStayBill(string phone); but body is empty in provided code. I'll assume it's to return the bill.
    for (int i = 0; i < customerCount; ++i) {
        if (customers[i].getPhone() == phone) {
            for (int j = 0; j < roomCount; ++j) {
                if (rooms[j].getRoomNumber() == customers[i].getRoomNumber()) {
                    return customers[i].getStayDuration() * rooms[j].getPrice();
                }
            }
        }
    }
    return 0.0;
}

void Hotel::cancelBooking() {
    string name;
    cout << "Enter name: ";
    cin >> name;

    bool found = false;

    for (int i = 0; i < customerCount; ++i) {
        if (customers[i].getName() == name) {
            int roomNo = customers[i].getRoomNumber();

            // Free the room
            for (int j = 0; j < roomCount; ++j) {
                if (rooms[j].getRoomNumber() == roomNo) {
                    rooms[j].setAvailability(true);
                    break;
                }
            }

            // Remove customer from array
            for (int j = i; j < customerCount - 1; ++j) {
                customers[j] = customers[j + 1];
            }

            customerCount--;
            saveRoomsToFile();
            saveCustomersToFile();

            cout << "Booking cancelled!\n";
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Booking not found.\n";
    }

    system("pause");
    system("cls");
}

void Hotel::displayAllBookings() const {
    for (int i = 0; i < customerCount; ++i) {
        customers[i].displayCustomer();
    }
    system("pause");
    system("cls");
}

void Hotel::displayAllCustomers() const {
    if (customerCount == 0) {
        cout << "No customers found.\n";
        system("pause");
        system("cls");
        return;
    }
    for (int i = 0; i < customerCount; ++i) {
        customers[i].displayCustomer();
    }
    system("pause");
    system("cls");
}

#include <iomanip>  // Make sure to include this

void Hotel::displayAvailableRooms() const {
    RoomNode* temp = roomHead;
    int width = 15;  // Adjust column width as needed

    cout << "\n==== ALL ROOMS (Linked List) ====\n";
    cout << left 
         << setw(width) << "Room No." 
         << setw(width) << "Type" 
         << setw(width) << "Price" 
         << setw(width) << "Status" << endl;
    cout << string(width * 4, '=') << endl;

    while (temp) {
        cout << left 
             << setw(width) << temp->data.getRoomNumber()
             << setw(width) << temp->data.getType()
             << setw(width) << temp->data.getPrice();

        // Determine room status
        string status;
        if (temp->data.getAvailability() && !temp->data.getMaintenanceStatus()) {
            status = "Available";
        } else if (!temp->data.getAvailability() && !temp->data.getMaintenanceStatus()) {
            status = "Occupied";
        } else if (temp->data.getMaintenanceStatus()) {
            status = "Under Maintenance";
        }

        cout << setw(width) << status << endl;

        temp = temp->next;
    }

    system("pause");
    system("cls");
}



void Hotel::filterRoomsByPrice() const {
    float minPrice, maxPrice;
    cout << "Enter min and max price: ";
    cin >> minPrice >> maxPrice;

    cout << "\nRoom No.\tType\tPrice\tAvailability\n";
    cout << "-------------------------------------------\n";

    for (int i = 0; i < roomCount; ++i) {
        float price = rooms[i].getPrice();
        if (price >= minPrice && price <= maxPrice) {
            cout << rooms[i].getRoomNumber() << "\t"
                 << rooms[i].getType() << "\t"
                 << price << "\t"
                 << (rooms[i].getAvailability() ? "Available" : "Not Available")
                 << endl;
        }
    }

    system("pause");
    system("cls");
}

void Hotel::generateInvoice() {
    string phone;
    cout << "Enter phone number: ";
    cin >> phone;
    float stayBill = calculateStayBill(phone);
    cout << "Room Stay Bill: " << stayBill << endl;

    cout << "Enter extra service charges (if any): ";
    float serviceCharges;
    cin >> serviceCharges;

    float totalBill = stayBill + serviceCharges;
    cout << "Total Bill = " << totalBill << endl;

    totalRoomRevenue += stayBill;
    totalServicesRevenue += serviceCharges;
    totalRevenue = totalRoomRevenue + totalServicesRevenue;

    // Save revenue to file
    ofstream fout("revenue.txt");
    fout << totalRoomRevenue << " " << totalServicesRevenue << " " << totalRevenue;
    fout.close();

    system("pause");
    system("cls");
}

void Hotel::initialize() {
    loadRoomsFromFile();
    loadCustomersFromFile();
}

void Hotel::loadCustomersFromFile() {
    ifstream inFile("customers.txt");
    if (!inFile) {
        cout << "No existing customer data found.\n";
        return;
    }
    inFile >> customerCount;
    for (int i = 0; i < customerCount; ++i) {
        string name, phone, checkIn, checkOut;
        int roomNumber;
        inFile >> name >> phone >> roomNumber >> checkIn >> checkOut;
        customers[i] = Customer(name, phone, roomNumber, checkIn, checkOut);
    }
    inFile.close();
}

void Hotel::loadRoomsFromFile() {
    ifstream inFile("rooms.txt");
    if (!inFile) {
        cout << "No existing room data found.\n";
        return;
    }
    inFile >> roomCount;
    for (int i = 0; i < roomCount; ++i) {
        int number, capacity;
        string type;
        float price;
        bool availability, maintenanceStatus;
        inFile >> number >> type >> price >> capacity >> availability >> maintenanceStatus;
        rooms[i] = Room(number, type, price, capacity);
        rooms[i].setAvailability(availability);
        rooms[i].setMaintenanceStatus(maintenanceStatus);
    }
    inFile.close();
}

void Hotel::loadStaffFromFile() {
    ifstream fin("staff.txt");
    if (!fin) return;

    staffCount = 0;
    int id;
    string name, role;
    float salary;

    while (fin >> id) {
        fin.ignore();
        getline(fin, name);
        getline(fin, role);
        fin >> salary;
        fin.ignore();

        staffList[staffCount++] = Staff(id, name, role, salary);
    }
    fin.close();
}

void Hotel::logMaintenance(int roomNumber, string issue) {
    string entry = "Room " + to_string(roomNumber) + ": " + issue;
    maintenanceLog[maintenanceCount++] = entry;

    ofstream fout("maintenance.txt", ios::app);
    fout << entry << endl;
    fout.close();

    cout << "Maintenance logged.\n";
}

void Hotel::markRoomUnderMaintenance(int roomNumber) {
    for (int i = 0; i < roomCount; ++i) {
        if (rooms[i].getRoomNumber() == roomNumber) {
            rooms[i].setMaintenanceStatus(true);
            rooms[i].setAvailability(false);
            cout << "Room marked under maintenance.\n";
            system("pause");
            system("cls");
            return;
        }
    }
    cout << "Room not found.\n";
    system("pause");
    system("cls");
}

void Hotel::removeStaff() {
    int id;
    cout << "Enter Staff ID to remove: ";
    cin >> id;

    for (int i = 0; i < staffCount; i++) {
        if (staffList[i].getId() == id) {
            for (int j = i; j < staffCount - 1; j++) {
                staffList[j] = staffList[j + 1];
            }
            staffCount--;
            saveStaffToFile();
            cout << "Staff removed.\n";
            return;
        }
    }
    cout << "Staff not found.\n";
}

void Hotel::saveCustomersToFile() const {
    ofstream outFile("customers.txt");
    if (!outFile) {
        cout << "Error saving customers to file.\n";
        return;
    }
    outFile << customerCount << endl;
    for (int i = 0; i < customerCount; ++i) {
        outFile << customers[i].getName() << " "
                << customers[i].getPhone() << " "
                << customers[i].getRoomNumber() << " "
                << customers[i].getCheckInDate() << " "
                << customers[i].getCheckOutDate() << endl;
    }
    outFile.close();
}

void Hotel::saveRoomsToFile() const {
    ofstream outFile("rooms.txt");
    if (!outFile) {
        cout << "Error saving rooms to file.\n";
        return;
    }
    outFile << roomCount << endl;
    for (int i = 0; i < roomCount; ++i) {
        outFile << rooms[i].getRoomNumber() << " "
                << rooms[i].getType() << " "
                << rooms[i].getPrice() << " "
                << rooms[i].getCapacity() << " "
                << rooms[i].getAvailability() << " "
                << rooms[i].getMaintenanceStatus() << endl;
    }
    outFile.close();
}

void Hotel::saveStaffToFile() const {
    ofstream fout("staff.txt");
    for (int i = 0; i < staffCount; i++) {
        fout << staffList[i].getId() << endl
             << staffList[i].getName() << endl
             << staffList[i].getRole() << endl
             << staffList[i].getSalary() << endl;
    }
    fout.close();
}

void Hotel::searchCustomerByPhone() const {
    string phone;
    cout << "Enter phone number to search: ";
    cin >> phone;
    for (int i = 0; i < customerCount; ++i) {
        if (customers[i].getPhone() == phone) {
            customers[i].displayCustomer();
            system("pause");
            system("cls");
            return;
        }
    }
    cout << "Customer not found.\n";
    system("pause");
    system("cls");
}

void Hotel::searchRoomByType() const {
    string type;
    cout << "Enter room type to search: ";
    cin >> type;

    cout << "\nRoom No.\tType\tPrice\tAvailability\n";
    cout << "-------------------------------------------\n";

    bool found = false;  // To check if any room of this type exists

    for (int i = 0; i < roomCount; ++i) {
        if (rooms[i].getType() == type) {
            found = true;
            cout << rooms[i].getRoomNumber() << "\t"
                 << rooms[i].getType() << "\t"
                 << rooms[i].getPrice() << "\t"
                 << (rooms[i].getAvailability() ? "Available" : "Not Available")
                 << endl;
        }
    }

    if (!found) {
        cout << "No rooms found of type: " << type << endl;
    }

    system("pause");
    system("cls");
}
void menu() {
    vector<string> art1 = {
        "                         vv                            ",
        "                     vvv^^^^vvvvv                      ",
        "                 vvvvvvvvv^^vvvvvv^^vvvvv              ",
        "        vvvvvvvvvvv^^^^^^^^^^^^^vvvvv^^^vvvvv          ",
        "      ^^^vvvvvvv^^vvvvv^^^^^^^^vvvvvvvvv^^^^^^vvvvv    ",
        "    v^^vvv^^^vvvvvvvvvv^vvvvv^vvvvvv^^^vvvvvvv^^vvvvv  ",
        "   ^vvvvv^^vvvvvvv^^vvvvvvv^^vvvvv^v##vvv^vvvv^^vvvvv  ",
        "   vvvvv^^vvvvvvvv^vv^vvv^^^^^^_____##^^^vvvvvvvv^^^   ",
        "    ^^vvvvvvv^^vvvvvvvvvv^^^^/\\@@@@@@\\#vvvv^^^vvv    ",
        "         ^^vvvvvv^^^^^^vvvvv/__\\@@@@@@\\^vvvv^v       ",
        "             ;^^vvvvvvvvvvv/____\\@@@@@@\\vvvvvvv      ",
        "             ;      \\_  ^\\|[  -:] ||--| | _/^^       ",
        "             ;        \\   |[   :] ||_/| |/            ",
        "             ;         \\\\ ||___:]______/             ",
        "             ;          \\   ;=; /                     ",
        "             ;           |  ;=;|                       ",
        "             ;          ()  ;=;|                       "
    };

    vector<string> art2 = {
        "                                                                        ",
        "                                                                        ",
        "                                                                        ",
        "              )\\         O_._._._A_._._._O         /(                  ",
        "                \\`--.___,'=================`.___,--'/                  ",
        "                 \\`--._.__                 __._,--'/                   ",
        "                   \\  ,. l`~~~~~~~~~~~~~~~'l ,.  /                     ",
        "       __            \\||(_)!_!_!_.-._!_!_!(_)||/            __         ",
        "       \\\\`-.__        ||_|____!!_|;|_!!____|_||        __,-'//        ",
        "        \\\\    `==---='-----------'='-----------`=---=='    //         ",
        "        | `--.                                         ,--' |           ",
        "         \\  ,.`~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~',.  /           ",
        "           \\||  ____,-------._,-------._,-------.____  ||/             ",
        "            ||\\|___!`=======\"!`=======\"!`=======\"!___|/||           ",
        "            || |---||--------||-| | |-!!--------||---| ||               ",
        "  __O_____O_ll_lO_____O_____O|| |'|'| ||O_____O_____Ol_ll_O_____O__     ",
        "  o H o o H o o H o o H o o |-----------| o o H o o H o o H o o H o     ",
        " ___H_____H_____H_____H____O =========== O____H_____H_____H_____H__     ",
        "                          /|=============|\\                            ",
        "()______()______()______() '==== +-+ ====' ()______()______()______()   ",
        "||{_}{_}||{_}{_}||{_}{_}/| ===== |_| ===== |\\{_}{_}||{_}{_}||{_}{_}||  ",
        "||      ||      ||     / |==== s(   )s ====| \\     ||      ||      ||  ",
        "======================()  =================  ()======================   ",
        "----------------------/| ------------------- |\\----------------------  ",
        "                     / |---------------------| \\                       ",
        "-'--'--'           ()  '---------------------'  ()                      ",
        "                   /| ------------------------- |\\    --'--'--'        ",
        "       --'--'     / |---------------------------| \\    '--'            ",
        "                ()  |___________________________|  ()           '--'-   ",
        "  --'-          /| _______________________________  |\\                 ",
        " --'           / |__________________________________| \\;               ",
    };

    // Copy art1 again after art2
    vector<string> art3 = art1;
    vector<string> art4 = {
        "___  ,--.  __________________________/   ,   /_______",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _   ,--.   _ _ _ __",
        "             _____                  ~'O---O'         ",
        "_____< Bahria|_____        __________________________",
        "         ||       /   ,   /                          ",
    };

    // Repeat art4 three times
    vector<string> art5 = art4;
    vector<string> art6 = art4;
    vector<string> art7 = art4;
    vector<string> art8{
        "ooooo   ooooo   .oooooo.   ooooooooooooo oooooooooooo ooooo         ooooooooo.   oooooooooooo  .oooooo..o oooooooooooo ooooooooo.  oooooo     oooo    .o.    ooooooooooooo ooooo   .oooooo.    ooooo      ooo  ",
        "`888'   `888'  d8P'  `Y8b  8'   888   `8 `888'     `8 `888'         `888   `Y88. `888'     `8 d8P'    `Y8 `888'     `8 `888   `Y88. `888.     .8'    .888.   8'   888   `8 `888'  d8P'  `Y8b   `888b.     `8'",
        " 888     888  888      888      888       888          888           888   .d88'  888         Y88bo.       888          888   .d88'  `888.   .8'    .8\"888.       888       888  888      888   8 `88b.    8",
        " 888ooooo888  888      888      888       888oooo8     888           888ooo88P'   888oooo8     \"Y8888o.    888oooo8     888ooo88P'    `888. .8'    .8' `888.      888       888  888      888   8   `88b.  8",
        " 888     888  888      888      888       888    \"     888           888`88b.     888    \"         `\"Y88b  888    \"     888`88b.       `888.8'    .88ooo8888.     888       888  888      888   8     `88b.8  ",
        " 888     888  `88b    d88'      888       888       o  888       o   888  `88b.   888       o oo     .d8P  888       o  888  `88b.      `888'    .8'     `888.    888       888  `88b    d88'   8       `888  oo  ",
        "o888o   o888o  `Y8bood8P'      o888o     o888ooooood8 o888ooooood8  o888o  o888o o888ooooood8  8''88888P'  o888ooooood8 o888o  o888o     `8'    o88o     o8888o  o888o     o888o  `Y8bood8P'   o8o        `8    ",
        "",
        "",
        "                                                       .oooooo..o oooooo   oooo  .oooooo..o ooooooooooooo oooooooooooo ooo        ooooo",
        "                                                       d8P'    `Y8 `888.   .8'  d8P'    `Y8 8'   888   `8 `888'     `8 `88.       .888",
        "                                                       Y88bo.       `888. .8'   Y88bo.           888       888          888b     d'888",
        "                                                       `\"Y8888o.     `888.8'     `\"Y8888o.       888       888oooo8     8 Y88. .P  888",
        "                                                           `\"Y88b     `888'          `\"Y88b      888       888    \"     8  `888'   888",
        "                                                             .d8P      888      oo     .d8P      888       888       o  8    Y     888",
        "                                                        8\"\"88888P'      o888o     8\"\"88888P'      o888o     o888ooooood8 o8o        o888o",
    };

    // Print art4, art5, and art6 under the upper art
    // Print side by side
    // Side-by-side upper art (art1, art2, art3)
    size_t maxLines = max(art1.size(), max(art2.size(), art3.size()));
    for (size_t i = 0; i < maxLines; ++i) {
        string line1 = (i < art1.size()) ? art1[i] : string(art1[0].size(), ' ');
        string line2 = (i < art2.size()) ? art2[i] : string(art2[0].size(), ' ');
        string line3 = (i < art3.size()) ? art3[i] : string(art3[0].size(), ' ');
        cout << line1 << line2<< line3 << endl;
    }

    // Stitch art4, art5, art6 horizontally without spacing
    size_t maxBottomLines = max(art4.size(), max(art5.size(), art6.size()));
    for (size_t i = 0; i < maxBottomLines; ++i) {
        string line1 = (i < art4.size()) ? art4[i] : string(art4[0].size(), ' ');
        string line2 = (i < art5.size()) ? art5[i] : string(art5[0].size(), ' ');
        string line3 = (i < art6.size()) ? art6[i] : string(art6[0].size(), ' ');
        string line4 = (i < art7.size()) ? art7[i] : string(art7[0].size(), ' ');
        cout << line1 << line2 << line3 << line4 << endl;
    }

    // No extra line here; print art7 immediately after
    for (const string& line : art8) {
        cout << line << endl;
    }
}

void Hotel::showAdminWindow() {
    int choice;
    do {
        cout << RESET;
        cout << GREEN;
        menu();
        cout << RESET;
        system("cls");
        cout << CYAN;
        cout << "\n=============================\n";
        cout << "          ADMIN PANEL\n";
        cout << "=============================\n";
        cout << "1. View Total Profits\n";
        cout << "2. View Charts (Rooms, Revenue, Status)\n";
        cout << "3. View All Feedback\n";
        cout << "4. View Maintenance Logs\n";
        cout << "5. Mark Room Under Maintenance\n";
        cout << "6. Log Maintenance Issue\n";
        cout << "7. Add Staff\n";
        cout << "8. View All Staff\n";
        cout << "9. Remove Staff\n";
        cout << "10. Exit Admin Panel\n";
        cout << "Enter choice: ";
        cin >> choice;
        cout << RESET;
        while (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter a number: ";
            cin >> choice;
        }

        switch (choice) {
        case 1:
            system("cls");
            cout << GREEN;
            cout << "********** VIEW TOTAL PROFITS **********\n";
            viewProfits();
            cout << RESET;
            break;

        case 2:
            system("cls");
            cout << YELLOW;
            cout << "********** VIEW CHARTS **********\n";
            viewCharts();
            cout << RESET;
            break;

        case 3:
            system("cls");
            cout << CYAN;
            viewAllFeedback();
            cout << RESET;
            break;

        case 4:
            system("cls");
            cout << RED;
            viewMaintenanceLogs();
            cout << RESET;
            break;

        case 5: {
            cout << YELLOW;
            int roomNum;
            cout << "Enter room number to mark under maintenance: ";
            cin >> roomNum;
            markRoomUnderMaintenance(roomNum);
            cout << RESET;
            break;
        }

        case 6: {
            system("cls");
            cout << CYAN;
            int roomNum;
            string issue;
            cout << "Enter room number: ";
            cin >> roomNum;
            cin.ignore();
            cout << "Enter maintenance issue: ";
            getline(cin, issue);
            logMaintenance(roomNum, issue);
            cout << RESET;
            break;
        }
        case 7:
            system("cls");
            cout << GREEN;
            cout << "********** ADD STAFF **********\n";
            addStaff();
            cout << RESET;
            break;

        case 8:
            system("cls");
            cout << YELLOW;
            viewAllStaff();
            cout << RESET;
            break;
        case 9:
            system("cls");
            cout << RED;
            cout << "********** REMOVE STAFF **********\n";
            removeStaff();
            cout << RESET;
            break;

        case 10:
            
            cout << GREEN;
            cout << "Exiting Admin Panel.\n";
            cout << RESET;
            break;
        default:
            cout << "Invalid option.\n";
        }
        system("pause");

    } while (choice != 10);
}

void Hotel::staffPanel() {
    int choice;
    int width = 127;

    do {
        cout << RESET;
        system("cls");
        cout << YELLOW;
        menu();
        cout << RESET;
        cout << GREEN;
        cout << setw(width) << "========================================================\n";
        cout << setw(width) << "||                HOTEL SYSTEM MENU                   ||\n";
        cout << setw(width) << "========================================================\n";
        cout << setw(width) << "|| 1.  Add New Room                 10. Generate Bill ||\n";
        cout << setw(width) << "|| 2.  View Available Rooms        11. Submit Feedback||\n";
        cout << setw(width) << "|| 3.  Search Room by Type          12. View Feedback ||\n";
        cout << setw(width) << "|| 4.  Filter Rooms by Price        13. View Customers||\n";
        cout << setw(width) << "|| 5.  Mark Room Under Maintenanc  14. Search Customer||\n";
        cout << setw(width) << "|| 6.  Log Maintenance Issue        15. Calculate Bill||\n";
        cout << setw(width) << "|| 7.  Book a Room                   16. Admin Panel  ||\n";
        cout << setw(width) << "|| 8.  Cancel Booking                17. Exit         ||\n";
        cout << setw(width) << "|| 9.  View All Bookings                              ||\n";
        cout << setw(width) << "========================================================\n";
        cout << setw(width) << "Enter your choice ==> ";
        cout << RESET;

        while (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
        case 1:
            system("cls");
            cout << GREEN;
            cout << "********** ADD NEW ROOM **********\n";
            addRoom();
            cout << RESET;
            break;

        case 2:
            system("cls");
            cout << CYAN;
            cout << "********** VIEW AVAILABLE ROOMS **********\n";
            displayAvailableRooms();
            cout << RESET;
            break;

        case 3:
            system("cls");
            cout << YELLOW;
            searchRoomByType();
            cout << RESET;
            break;

        case 4:
        system("cls");
        cout << YELLOW;
            filterRoomsByPrice();
            cout << RESET;
            break;

        case 5: {
            system("cls");
            cout << RED;
            int roomNumber;
            cout << "Enter Room Number: ";
            cin >> roomNumber;
            markRoomUnderMaintenance(roomNumber);
            cout << RESET;
            break;
        }

        case 6: {
            system("cls");
            cout << CYAN;
            int roomNumber;
            string issue;
            cout << "Enter Room Number: ";
            cin >> roomNumber;
            cin.ignore();
            cout << "Enter issue: ";
            getline(cin, issue);
            logMaintenance(roomNumber, issue);
            cout << RESET;
            break;
        }

        case 7:
        cout << GREEN;
        system("cls");
            bookRoom();
            cout << RESET;
            break;

        case 8:
        system("cls");
        cout << RED;
            cancelBooking();
            cout << RESET;
            break;

        case 9:
        system("cls");
        cout << CYAN;
            displayAllBookings();
            cout << RESET;
            break;

        case 10:
        system("cls");
        cout << YELLOW;
            generateInvoice();
            cout << RESET;
            break;

        case 11:
        system("cls");
        cout << GREEN;
            submitFeedback();
            cout << RESET;
            break;

        case 12:
        system("cls");
        cout << CYAN;
            viewAllFeedback();
            cout << RESET;
            break;

        case 13:
        system("cls");
        cout << YELLOW;
            displayAllCustomers();
            cout << RESET;
            break;

        case 14:
        system("cls");
        cout << GREEN;
            searchCustomerByPhone();
            cout << RESET;
            break;

        case 15: {
            system("cls");
            cout << YELLOW;
            string ph;
            cout << "Enter phone number: ";
            cin >> ph;
            cout << "Total bill: $" << calculateStayBill(ph) << endl;
            cout << RESET;
            break;
        }

        case 16:
        system("cls");
        cout << RED;
            adminLogin();\
            cout << RESET;
            break;

        case 17:
        system("cls");
        cout << GREEN;
            cout << "\nExiting Staff Panel...\n";
            cout << RESET;
            break;

        default:
        cout << RED;
            cout << "Invalid option.\n";
            cout << RESET;
        }

        if (choice != 17) {
            cout << GREEN;
            cout << "\nPress Enter to return...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            cout << RESET;
        }

    } while (choice != 17);
}

void Hotel::submitFeedback() {
    system("cls");
    if (feedbackCount >= 200) {
        cout << "Feedback limit reached.\n";
        system("pause");
        return;
    }

    int stars;
    string fb;

    cout << "Rate our hotel (1 to 5 stars): ";
    cin >> stars;

    // ⭐ Condition for valid stars
    while (stars < 1 || stars > 5) {
        cout << "Invalid rating. Enter between 1 and 5: ";
        cin >> stars;
    }

    cin.ignore();
    cout << "Enter your feedback: ";
    getline(cin, fb);

    // Store in arrays
    feedbackStars[feedbackCount] = stars;
    feedbackList[feedbackCount] = fb;
    feedbackCount++;

    // Save to file
    ofstream fout("feedback.txt", ios::app);
    fout << stars << " | " << fb << endl;
    fout.close();

    cout << "\nThank you! Feedback submitted successfully.\n";
    system("pause");
    system("cls");
}

bool Hotel::verifyAdminPassword() {
    string storedPassword = "admin123";
    string inputPassword;
    ifstream file("admin_credentials.txt");
    if (!file) {
        ofstream create("admin_credentials.txt");
        create << storedPassword;
        create.close();
    } else {
        getline(file, storedPassword);
        file.close();
    }

    cout << "\nEnter Admin Password: ";
    cin >> inputPassword;
    system("cls");
    return inputPassword == storedPassword;
}

void Hotel::viewAllFeedback() const {
    system("cls");
    ifstream fin("feedback.txt");
    if (!fin) {
        cout << "No feedback available.\n";
        system("pause");
        return;
    }

    string line;
    int count = 0;

    cout << "\n======= CUSTOMER FEEDBACK =======\n";

    while (getline(fin, line)) {
        if (line.empty()) continue;

        // Split line at " | "
        size_t delimPos = line.find(" | ");
        if (delimPos == string::npos) continue; // Skip if format is wrong

        int stars = stoi(line.substr(0, delimPos));
        string feedbackText = line.substr(delimPos + 3); // +3 to skip " | "

        count++;
        cout << count << ". Rating: ";
        for (int s = stars; s < 4; s++)
            cout << "#  ";

        cout << "\n   " << feedbackText << "\n\n";
    }

    if (count == 0)
        cout << "No feedback available.\n";

    fin.close();
    system("pause");
    system("cls");
}


void Hotel::viewAllStaff() const {
    cout << "\nID\tName\tRole\tSalary\n";
    cout << "--------------------------------\n";
    for (int i = 0; i < staffCount; i++) {
        staffList[i].display();
    }
}

void Hotel::viewCharts() {
    cout << "================= HOTEL CHARTS =================\n";

    int available = 0, booked = 0, maintenance = 0;

    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].getMaintenanceStatus()) maintenance++;
        else if (rooms[i].getAvailability()) available++;
        else booked++;
    }

    cout << "\nROOM STATUS BAR CHART:\n";
    cout << "Available  : " << string(available, '#') << " (" << available << ")\n";
    cout << "Booked     : " << string(booked, '#') << " (" << booked << ")\n";
    cout << "Maintenance: " << string(maintenance, '#') << " (" << maintenance << ")\n";

    // Revenue Chart
    ifstream fin("revenue.txt");
    float roomRev = 0, serviceRev = 0, total = 0;
    if (fin) fin >> roomRev >> serviceRev >> total;

    cout << "\nREVENUE CHART:\n";
    cout << "Room Revenue     : " << string(roomRev / 1000, '#') << " (" << roomRev << ")\n";
    cout << "Service Revenue  : " << string(serviceRev / 1000, '#') << " (" << serviceRev << ")\n";
    cout << "Total Revenue    : " << string(total / 1000, '#') << " (" << total << ")\n";
}

void Hotel::viewMaintenanceLogs() const {
    ifstream fin("maintenance.txt");
    if (!fin) {
        cout << "No maintenance logs found.\n";
        return;
    }

    cout << "\n=========== MAINTENANCE LOGS ===========\n";
    string line;
    while (getline(fin, line)) {
        cout << "- " << line << endl;
    }
    fin.close();
}

void Hotel::viewProfits() const {
    ifstream fin("revenue.txt");
    float roomRev = 0, serviceRev = 0, total = 0;

    if (fin) {
        fin >> roomRev >> serviceRev >> total;
        fin.close();
    }

    cout << "\n======== TOTAL PROFITS ========\n";
    cout << "Room Revenue: " << roomRev << endl;
    cout << "Service Revenue: " << serviceRev << endl;
    cout << "--------------------------------\n";
    cout << "Total Revenue: " << total << endl;
}

// Menu function


// System entry function
void systemEntry(Hotel &hotelSystem) {
    int choice;
    int width = 127;
    do {
        system("cls");
        cout << YELLOW;
        menu();
        cout << RESET;
        cout << GREEN;
        cout << setw(width) << "============================================\n";
        cout << setw(width) << "||        HOTEL MANAGEMENT SYSTEM         ||\n";
        cout << setw(width) << "============================================\n";
        cout << setw(width) << "|| 1. Admin Login                         ||\n";
        cout << setw(width) << "|| 2. Staff Panel                         ||\n";
        cout << setw(width) << "|| 3. Exit                                ||\n";
        cout << setw(width) << "============================================\n";
        cout << setw(width) << "Enter choice ==> ";

        cin >> choice;

        switch (choice) {
        case 1:
            hotelSystem.adminLogin();
            break;
        case 2:
            hotelSystem.staffPanel();
            break;
        case 3:
            cout << "Goodbye!\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }

        if (choice != 3) {
            cout << "\nPress Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }

    } while (choice != 3);
}

// Main function
int main() {
    cout << GREEN;
    menu();
    cout << RESET;
    Hotel hotelSystem;
    systemEntry(hotelSystem);
    return 0;
}
