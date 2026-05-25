#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <sstream>

// --- Global Data Files and IDs ---
const std::string USERS_FILE = "users.txt";
const std::string ROOMS_FILE = "rooms.txt";
const std::string RESERVATIONS_FILE = "reservations.txt";
const std::string BILLS_FILE = "bills.txt";

// Helper function to get the current year for ID generation
int getCurrentYear() {
    time_t now = time(0);
    struct tm* ltm = localtime(&now);
    return 1900 + ltm->tm_year;
}

// Helper function to clear the console screen for a cleaner UI
void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

// --- Base Class for Users ---
class Person {
protected:
    std::string id;
    std::string name;
    std::string password;
public:
    Person(std::string id = "", std::string name = "", std::string password = "")
        : id(id), name(name), password(password) {}

    std::string getID() const { return id; }
    std::string getName() const { return name; }
    std::string getPassword() const { return password; }
    void setPassword(const std::string& newPassword) { this->password = newPassword; }

    virtual std::string getRole() const = 0; // Pure virtual function
    
    // Display method for formatted output
    void display() const {
        std::cout << std::left << std::setw(15) << id << std::setw(25) << name << std::setw(15) << password;
    }

    // Convert object to a string for file storage
    std::string toString() const {
        return id + "," + name + "," + password;
    }
};

// --- User Class (for Guests) ---
class User : public Person {
public:
    User(std::string id = "", std::string name = "", std::string password = "")
        : Person(id, name, password) {}
    
    std::string getRole() const override { return "User"; }
};

// --- Admin Class ---
class Admin : public Person {
public:
    Admin(std::string id = "", std::string name = "", std::string password = "")
        : Person(id, name, password) {}

    std::string getRole() const override { return "Admin"; }
};

// --- Room Class ---
class Room {
private:
    std::string roomNumber;
    std::string type; // e.g., "Single", "Double", "Suite"
    double price;
    std::string status; // e.g., "Available", "Booked", "Occupied"
public:
    Room(std::string number = "", std::string type = "", double price = 0.0, std::string status = "Available")
        : roomNumber(number), type(type), price(price), status(status) {}

    std::string getRoomNumber() const { return roomNumber; }
    std::string getType() const { return type; }
    double getPrice() const { return price; }
    std::string getStatus() const { return status; }
    
    void setType(const std::string& newType) { this->type = newType; }
    void setPrice(double newPrice) { this->price = newPrice; }
    void setStatus(const std::string& newStatus) { this->status = newStatus; }

    void display() const {
        std::cout << std::left << std::setw(15) << roomNumber << std::setw(20) << type << std::fixed << std::setprecision(2) << std::setw(15) << price << std::setw(15) << status << std::endl;
    }

    std::string toString() const {
        std::ostringstream ss;
        ss << roomNumber << "," << type << "," << price << "," << status;
        return ss.str();
    }
};

// --- Reservation Class ---
class Reservation {
private:
    std::string reservationId;
    std::string userId;
    std::string roomNumber;
    std::string checkInDate;
    std::string checkOutDate;
    std::vector<std::pair<std::string, double>> services;
    double totalAmount;
public:
    Reservation(std::string id = "", std::string userId = "", std::string roomNumber = "", std::string checkIn = "", std::string checkOut = "")
        : reservationId(id), userId(userId), roomNumber(roomNumber), checkInDate(checkIn), checkOutDate(checkOut), totalAmount(0.0) {}

    std::string getReservationId() const { return reservationId; }
    std::string getUserId() const { return userId; }
    std::string getRoomNumber() const { return roomNumber; }
    std::string getCheckInDate() const { return checkInDate; }
    std::string getCheckOutDate() const { return checkOutDate; }
    const std::vector<std::pair<std::string, double>>& getServices() const { return services; }
    double getTotalAmount() const { return totalAmount; }

    void addService(const std::string& name, double cost) { services.push_back({name, cost}); }
    void setTotalAmount(double amount) { this->totalAmount = amount; }
    void removeService(const std::string& serviceName);

    void display() const {
        std::cout << std::left << std::setw(15) << reservationId << std::setw(15) << userId << std::setw(15) << roomNumber << std::setw(15) << checkInDate << std::setw(15) << checkOutDate << std::endl;
    }

    std::string toString() const {
        std::ostringstream ss;
        ss << reservationId << "," << userId << "," << roomNumber << "," << checkInDate << "," << checkOutDate;
        if (!services.empty()) {
            ss << ",";
            for (size_t i = 0; i < services.size(); ++i) {
                ss << services[i].first << ":" << services[i].second;
                if (i < services.size() - 1) ss << ";";
            }
        } else {
            ss << ","; // Empty string for services
        }
        ss << "," << totalAmount;
        return ss.str();
    }
};

void Reservation::removeService(const std::string& serviceName) {
    auto it = std::remove_if(services.begin(), services.end(), [&](const auto& p) {
        return p.first == serviceName;
    });
    if (it != services.end()) {
        services.erase(it, services.end());
        std::cout << "Service '" << serviceName << "' removed from reservation." << std::endl;
    } else {
        std::cout << "Service '" << serviceName << "' not found in reservation." << std::endl;
    }
}

// --- Bill Class ---
class Bill {
private:
    std::string billId;
    std::string reservationId;
    std::string userId;
    std::string dateGenerated;
    double totalAmount;
public:
    Bill(std::string id = "", std::string resId = "", std::string userId = "", std::string date = "", double totalAmount = 0.0)
        : billId(id), reservationId(resId), userId(userId), dateGenerated(date), totalAmount(totalAmount) {}

    void display() const {
        std::cout << std::left << std::setw(15) << billId << std::setw(15) << reservationId << std::setw(15) << userId << std::setw(15) << dateGenerated << std::fixed << std::setprecision(2) << std::setw(15) << totalAmount << std::endl;
    }

    std::string toString() const {
        std::ostringstream ss;
        ss << billId << "," << reservationId << "," << userId << "," << dateGenerated << "," << totalAmount;
        return ss.str();
    }
};

// --- Global Data Structures ---
std::vector<Person*> people; // Stores both Users and Admins
std::vector<Room> rooms;
std::vector<Reservation> reservations;
std::vector<Bill> bills;

// --- Helper Functions ---
// Function to generate a unique ID
std::string generateID(const std::string& name, const std::string& role) {
    std::string prefix = name.substr(0, std::min((size_t)4, name.length()));
    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
    return prefix + std::to_string(getCurrentYear());
}

// --- File Handling Functions ---
void loadData() {
    // Load Users and Admins
    std::ifstream userFile(USERS_FILE);
    if (userFile.is_open()) {
        std::string line;
        while (getline(userFile, line)) {
            std::stringstream ss(line);
            std::string id, name, password, role;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, password, ',');
            getline(ss, role, ',');
            if (role == "Admin") {
                people.push_back(new Admin(id, name, password));
            } else {
                people.push_back(new User(id, name, password));
            }
        }
        userFile.close();
    }

    // Load Rooms
    std::ifstream roomFile(ROOMS_FILE);
    if (roomFile.is_open()) {
        std::string line;
        while (getline(roomFile, line)) {
            std::stringstream ss(line);
            std::string number, type, priceStr, status;
            getline(ss, number, ',');
            getline(ss, type, ',');
            getline(ss, priceStr, ',');
            getline(ss, status, ',');
            rooms.push_back(Room(number, type, stod(priceStr), status));
        }
        roomFile.close();
    }

    // Load Reservations
    std::ifstream resFile(RESERVATIONS_FILE);
    if (resFile.is_open()) {
        std::string line;
        while (getline(resFile, line)) {
            std::stringstream ss(line);
            std::string resId, userId, roomNumber, checkIn, checkOut, servicesStr, totalAmountStr;
            getline(ss, resId, ',');
            getline(ss, userId, ',');
            getline(ss, roomNumber, ',');
            getline(ss, checkIn, ',');
            getline(ss, checkOut, ',');
            getline(ss, servicesStr, ',');
            getline(ss, totalAmountStr, ',');

            Reservation r(resId, userId, roomNumber, checkIn, checkOut);
            std::stringstream serviceStream(servicesStr);
            std::string servicePair;
            while(getline(serviceStream, servicePair, ';')) {
                std::stringstream pairStream(servicePair);
                std::string serviceName, costStr;
                getline(pairStream, serviceName, ':');
                getline(pairStream, costStr);
                r.addService(serviceName, stod(costStr));
            }
            r.setTotalAmount(stod(totalAmountStr));
            reservations.push_back(r);
        }
        resFile.close();
    }

    // Load Bills
    std::ifstream billFile(BILLS_FILE);
    if (billFile.is_open()) {
        std::string line;
        while (getline(billFile, line)) {
            std::stringstream ss(line);
            std::string id, resId, userId, date, totalAmountStr;
            getline(ss, id, ',');
            getline(ss, resId, ',');
            getline(ss, userId, ',');
            getline(ss, date, ',');
            getline(ss, totalAmountStr, ',');
            bills.push_back(Bill(id, resId, userId, date, stod(totalAmountStr)));
        }
        billFile.close();
    }
}

void saveData() {
    // Save Users
    std::ofstream userFile(USERS_FILE);
    for (const auto& p : people) {
        userFile << p->toString() << "," << p->getRole() << "\n";
    }
    userFile.close();

    // Save Rooms
    std::ofstream roomFile(ROOMS_FILE);
    for (const auto& room : rooms) {
        roomFile << room.toString() << "\n";
    }
    roomFile.close();

    // Save Reservations
    std::ofstream resFile(RESERVATIONS_FILE);
    for (const auto& res : reservations) {
        resFile << res.toString() << "\n";
    }
    resFile.close();

    // Save Bills
    std::ofstream billFile(BILLS_FILE);
    for (const auto& bill : bills) {
        billFile << bill.toString() << "\n";
    }
    billFile.close();
}

// --- Menu Functions ---
void adminMenu();
void userMenu(const std::string& userId);

void login() {
    clearScreen();
    std::string id, password;
    std::cout << "--- Login ---" << std::endl;
    std::cout << "Enter ID: ";
    std::cin >> id;
    std::cout << "Enter Password: ";
    std::cin >> password;

    for (const auto& p : people) {
        if (p->getID() == id && p->getPassword() == password) {
            std::cout << "Login successful. Welcome, " << p->getName() << "!" << std::endl;
            if (p->getRole() == "Admin") {
                adminMenu();
            } else {
                userMenu(p->getID());
            }
            return;
        }
    }
    std::cout << "Invalid ID or Password." << std::endl;
    std::cout << "Press Enter to try again...";
    std::cin.ignore();
    std::cin.get();
    login();
}

void registerNewUserOrAdmin() {
    clearScreen();
    std::string name, password, role;
    std::cout << "--- New Registration ---" << std::endl;
    std::cout << "Enter Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    std::cout << "Enter Password: ";
    std::cin >> password;
    std::cout << "Register as (User/Admin)? ";
    std::cin >> role;
    std::transform(role.begin(), role.end(), role.begin(), ::tolower);

    std::string newId = generateID(name, role);
    std::cout << "Your new ID is: " << newId << std::endl;

    if (role == "admin") {
        people.push_back(new Admin(newId, name, password));
    } else {
        people.push_back(new User(newId, name, password));
    }
    saveData();
    std::cout << "Registration successful!" << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

// --- Admin Sub-menus ---
void manageRooms() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin Room Management ---" << std::endl;
        std::cout << "1. Add New Room" << std::endl;
        std::cout << "2. View All Rooms" << std::endl;
        std::cout << "3. Update Room Details" << std::endl;
        std::cout << "4. Delete Room" << std::endl;
        std::cout << "5. Search Room" << std::endl;
        std::cout << "6. Back to Main Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string number, type;
                double price;
                std::cout << "Enter Room Number: ";
                std::cin >> number;
                std::cout << "Enter Room Type (e.g., Single, Double, Suite): ";
                std::cin.ignore();
                std::getline(std::cin, type);
                std::cout << "Enter Price per Night: ";
                std::cin >> price;
                rooms.push_back(Room(number, type, price));
                saveData();
                std::cout << "Room added successfully." << std::endl;
                break;
            }
            case 2:
                clearScreen();
                std::cout << "--- All Rooms ---" << std::endl;
                std::cout << std::left << std::setw(15) << "Room Number" << std::setw(20) << "Type" << std::setw(15) << "Price/Night" << std::setw(15) << "Status" << std::endl;
                std::cout << std::string(65, '-') << std::endl;
                for (const auto& room : rooms) {
                    room.display();
                }
                std::cout << std::string(65, '-') << std::endl;
                break;
            case 3: {
                std::string number;
                std::cout << "Enter Room Number to update: ";
                std::cin >> number;
                auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& room){ return room.getRoomNumber() == number; });
                if (it != rooms.end()) {
                    std::string newType;
                    double newPrice;
                    std::cout << "Enter new type: ";
                    std::cin.ignore();
                    std::getline(std::cin, newType);
                    std::cout << "Enter new price: ";
                    std::cin >> newPrice;
                    it->setType(newType);
                    it->setPrice(newPrice);
                    saveData();
                    std::cout << "Room details updated successfully." << std::endl;
                } else {
                    std::cout << "Room not found." << std::endl;
                }
                break;
            }
            case 4: {
                std::string number;
                std::cout << "Enter Room Number to delete: ";
                std::cin >> number;
                auto it = std::remove_if(rooms.begin(), rooms.end(), [&](const Room& room){ return room.getRoomNumber() == number; });
                if (it != rooms.end()) {
                    rooms.erase(it, rooms.end());
                    saveData();
                    std::cout << "Room deleted successfully." << std::endl;
                } else {
                    std::cout << "Room not found." << std::endl;
                }
                break;
            }
            case 5: {
                std::string number;
                std::cout << "Enter Room Number to search: ";
                std::cin >> number;
                auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& room){ return room.getRoomNumber() == number; });
                if (it != rooms.end()) {
                    clearScreen();
                    std::cout << "--- Room Found ---" << std::endl;
                    std::cout << std::left << std::setw(15) << "Room Number" << std::setw(20) << "Type" << std::setw(15) << "Price/Night" << std::setw(15) << "Status" << std::endl;
                    std::cout << std::string(65, '-') << std::endl;
                    it->display();
                    std::cout << std::string(65, '-') << std::endl;
                } else {
                    std::cout << "Room not found." << std::endl;
                }
                break;
            }
            case 6:
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    } while (choice != 6);
}

void manageUsers() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin User Management ---" << std::endl;
        std::cout << "1. View All Users & Admins" << std::endl;
        std::cout << "2. Register New User/Admin" << std::endl;
        std::cout << "3. Update User Info" << std::endl;
        std::cout << "4. Delete User" << std::endl;
        std::cout << "5. Back to Main Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                clearScreen();
                std::cout << "--- All System Users ---" << std::endl;
                std::cout << std::left << std::setw(15) << "ID" << std::setw(25) << "Name" << std::setw(15) << "Password" << std::setw(10) << "Role" << std::endl;
                std::cout << std::string(65, '-') << std::endl;
                for (const auto& p : people) {
                    p->display();
                    std::cout << std::setw(10) << p->getRole() << std::endl;
                }
                std::cout << std::string(65, '-') << std::endl;
                break;
            case 2:
                registerNewUserOrAdmin();
                break;
            case 3: {
                std::string id;
                std::cout << "Enter User ID to update: ";
                std::cin >> id;
                auto it = std::find_if(people.begin(), people.end(), [&](const Person* p){ return p->getID() == id; });
                if (it != people.end()) {
                    std::string newPassword;
                    std::cout << "Enter new password: ";
                    std::cin >> newPassword;
                    (*it)->setPassword(newPassword);
                    saveData();
                    std::cout << "User information updated." << std::endl;
                } else {
                    std::cout << "User not found." << std::endl;
                }
                break;
            }
            case 4: {
                std::string id;
                std::cout << "Enter User ID to delete: ";
                std::cin >> id;
                auto it = std::remove_if(people.begin(), people.end(), [&](const Person* p){ return p->getID() == id; });
                if (it != people.end()) {
                    people.erase(it, people.end());
                    saveData();
                    std::cout << "User deleted successfully." << std::endl;
                } else {
                    std::cout << "User not found." << std::endl;
                }
                break;
            }
            case 5:
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    } while (choice != 5);
}

void viewReservations() {
    clearScreen();
    std::cout << "--- All Reservations ---" << std::endl;
    std::cout << std::left << std::setw(15) << "Res ID" << std::setw(15) << "User ID" << std::setw(15) << "Room No" << std::setw(15) << "Check-in" << std::setw(15) << "Check-out" << std::endl;
    std::cout << std::string(75, '-') << std::endl;
    for (const auto& res : reservations) {
        res.display();
    }
    std::cout << std::string(75, '-') << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void adminMenu() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin Menu ---" << std::endl;
        std::cout << "1. Manage Rooms" << std::endl;
        std::cout << "2. Manage Users" << std::endl;
        std::cout << "3. View All Reservations" << std::endl;
        std::cout << "4. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: manageRooms(); break;
            case 2: manageUsers(); break;
            case 3: viewReservations(); break;
            case 4:
                std::cout << "Logging out..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                std::cout << "Press Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                break;
        }
    } while (choice != 4);
}

// --- User Sub-menus ---
void viewAvailableRooms() {
    clearScreen();
    std::cout << "--- Available Rooms ---" << std::endl;
    std::cout << std::left << std::setw(15) << "Room Number" << std::setw(20) << "Type" << std::setw(15) << "Price/Night" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    for (const auto& room : rooms) {
        if (room.getStatus() == "Available") {
            std::cout << std::left << std::setw(15) << room.getRoomNumber() << std::setw(20) << room.getType() << std::fixed << std::setprecision(2) << std::setw(15) << room.getPrice() << std::endl;
        }
    }
    std::cout << std::string(50, '-') << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void makeReservation(const std::string& userId) {
    clearScreen();
    std::cout << "--- Make a Reservation ---" << std::endl;
    
    // Check for a currently active reservation
    auto activeReservation = std::find_if(reservations.begin(), reservations.end(), [&](const Reservation& r) {
        return r.getUserId() == userId && r.getTotalAmount() == 0.0;
    });
    
    if (activeReservation != reservations.end()) {
        std::cout << "You have an existing active reservation (" << activeReservation->getReservationId() << "). Please check out first to make a new one." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    viewAvailableRooms();
    std::string roomNumber, checkIn, checkOut;
    std::cout << "Enter Room Number to reserve: ";
    std::cin >> roomNumber;
    std::cout << "Enter Check-in Date (DD-MM-YYYY): ";
    std::cin >> checkIn;
    std::cout << "Enter Check-out Date (DD-MM-YYYY): ";
    std::cin >> checkOut;

    auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& room){ return room.getRoomNumber() == roomNumber && room.getStatus() == "Available"; });
    if (it != rooms.end()) {
        it->setStatus("Booked");
        std::string resId = "RES" + std::to_string(reservations.size() + 1);
        reservations.push_back(Reservation(resId, userId, roomNumber, checkIn, checkOut));
        saveData();
        std::cout << "Room " << roomNumber << " reserved successfully!" << std::endl;
    } else {
        std::cout << "Invalid room number or room is not available." << std::endl;
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void manageServices(const std::string& userId) {
    clearScreen();
    std::cout << "--- Add Services to Your Reservation ---" << std::endl;

    auto currentRes = std::find_if(reservations.rbegin(), reservations.rend(), [&](const Reservation& r){
        return r.getUserId() == userId && r.getTotalAmount() == 0.0;
    });

    if (currentRes == reservations.rend()) {
        std::cout << "You must have an active reservation to add services." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    std::cout << "Available Services:" << std::endl;
    std::cout << "1. Laundry ($25.00)" << std::endl;
    std::cout << "2. Room Service ($15.00)" << std::endl;
    std::cout << "3. Mini Bar ($40.00)" << std::endl;
    std::cout << "4. Custom Service" << std::endl;
    std::cout << "5. Remove a service" << std::endl;
    std::cout << "6. Back" << std::endl;
    std::cout << "Enter your choice: ";
    int serviceChoice;
    std::cin >> serviceChoice;

    switch (serviceChoice) {
        case 1: currentRes->addService("Laundry", 25.00); break;
        case 2: currentRes->addService("Room Service", 15.00); break;
        case 3: currentRes->addService("Mini Bar", 40.00); break;
        case 4: {
            std::string customName;
            double customPrice;
            std::cout << "Enter custom service name: ";
            std::cin.ignore();
            std::getline(std::cin, customName);
            std::cout << "Enter service cost: ";
            std::cin >> customPrice;
            currentRes->addService(customName, customPrice);
            break;
        }
        case 5: {
            std::string serviceName;
            std::cout << "Enter service name to remove: ";
            std::cin.ignore();
            std::getline(std::cin, serviceName);
            currentRes->removeService(serviceName);
            break;
        }
        case 6: return;
        default: std::cout << "Invalid choice." << std::endl;
    }
    saveData();
    std::cout << "Services updated successfully." << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void viewYourBookings(const std::string& userId) {
    clearScreen();
    std::cout << "--- Your Reservations ---" << std::endl;
    std::cout << std::left << std::setw(15) << "Res ID" << std::setw(15) << "UserId" << std::setw(15) << "Room No" << std::setw(15) << "Check-in" << std::setw(15) << "Check-out" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::vector<Reservation> userReservations;
    for (const auto& res : reservations) {
        if (res.getUserId() == userId) {
            res.display();
            userReservations.push_back(res);
        }
    }
    std::cout << std::string(60, '-') << std::endl;

    if (userReservations.empty()) {
        std::cout << "You have no active reservations." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    std::cout << "1. Delete a reservation" << std::endl;
    std::cout << "2. Back" << std::endl;
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        std::string resId;
        std::cout << "Enter Reservation ID to delete: ";
        std::cin >> resId;
        auto it = std::remove_if(reservations.begin(), reservations.end(), [&](const Reservation& r){ return r.getReservationId() == resId && r.getUserId() == userId; });
        if (it != reservations.end()) {
            // Find the room and update its status
            auto roomIt = std::find_if(rooms.begin(), rooms.end(), [&](const Room& r){ return r.getRoomNumber() == it->getRoomNumber(); });
            if (roomIt != rooms.end()) {
                roomIt->setStatus("Available");
            }
            reservations.erase(it, reservations.end());
            saveData();
            std::cout << "Reservation deleted successfully." << std::endl;
        } else {
            std::cout << "Invalid reservation ID or you do not own this reservation." << std::endl;
        }
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void generateBill(const std::string& userId) {
    clearScreen();
    std::cout << "--- Generate Bill ---" << std::endl;
    auto it = std::find_if(reservations.rbegin(), reservations.rend(), [&](const Reservation& r){ return r.getUserId() == userId && r.getTotalAmount() == 0.0; });

    if (it == reservations.rend()) {
        std::cout << "You have no active reservations to generate a bill for." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    Reservation currentRes = *it;
    double roomPrice = 0.0;
    auto roomIt = std::find_if(rooms.begin(), rooms.end(), [&](const Room& r){ return r.getRoomNumber() == currentRes.getRoomNumber(); });
    if (roomIt != rooms.end()) {
        roomPrice = roomIt->getPrice();
    }
    
    double servicesTotal = 0.0;
    for (const auto& service : currentRes.getServices()) {
        servicesTotal += service.second;
    }
    
    double tax = (roomPrice + servicesTotal) * 0.1; // 10% tax
    double totalAmount = roomPrice + servicesTotal + tax;
    
    // Update the reservation with the final total amount
    it->setTotalAmount(totalAmount);
    saveData();

    clearScreen();
    std::cout << "--- Receipt ---" << std::endl;
    std::cout << "Reservation ID: " << currentRes.getReservationId() << std::endl;
    std::cout << "Customer ID: " << currentRes.getUserId() << std::endl;
    std::cout << "Room Number: " << currentRes.getRoomNumber() << std::endl;
    std::cout << "Check-in Date: " << currentRes.getCheckInDate() << std::endl;
    std::cout << "Check-out Date: " << currentRes.getCheckOutDate() << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    std::cout << std::left << std::setw(30) << "Room Charges" << std::fixed << std::setprecision(2) << roomPrice << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    
    std::cout << "Services:" << std::endl;
    if (currentRes.getServices().empty()) {
        std::cout << "  No additional services." << std::endl;
    } else {
        for (const auto& service : currentRes.getServices()) {
            std::cout << std::left << "  " << std::setw(28) << service.first << std::fixed << std::setprecision(2) << service.second << std::endl;
        }
    }
    
    std::cout << std::string(40, '-') << std::endl;
    std::cout << std::right << std::setw(30) << "Subtotal: " << std::fixed << std::setprecision(2) << roomPrice + servicesTotal << std::endl;
    std::cout << std::right << std::setw(30) << "Tax (10%): " << std::fixed << std::setprecision(2) << tax << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    std::cout << std::right << std::setw(30) << "TOTAL: " << std::fixed << std::setprecision(2) << totalAmount << std::endl;

    // Save to bills file
    std::string billId = "BILL" + std::to_string(bills.size() + 1);
    std::string date = "00-00-0000"; // Placeholder for current date logic
    bills.push_back(Bill(billId, currentRes.getReservationId(), userId, date, totalAmount));
    saveData();
    std::cout << "Bill generated and saved successfully!" << std::endl;
    
    // Set the room status back to available
    if (roomIt != rooms.end()) {
        roomIt->setStatus("Available");
    }
    saveData();
    
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void userMenu(const std::string& userId) {
    int choice;
    do {
        clearScreen();
        std::cout << "--- User Menu ---" << std::endl;
        std::cout << "1. View Available Rooms" << std::endl;
        std::cout << "2. Make a Reservation" << std::endl;
        std::cout << "3. Add/Manage Services" << std::endl;
        std::cout << "4. View/Cancel Your Bookings" << std::endl;
        std::cout << "5. Generate Bill & Check Out" << std::endl;
        std::cout << "6. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: viewAvailableRooms(); break;
            case 2: makeReservation(userId); break;
            case 3: manageServices(userId); break;
            case 4: viewYourBookings(userId); break;
            case 5: generateBill(userId); break;
            case 6:
                std::cout << "Logging out..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    } while (choice != 6);
}

int main() {
    loadData();

    // Create a default admin if no users exist
    if (people.empty()) {
        std::cout << "No users found. Creating a default Admin account." << std::endl;
        people.push_back(new Admin(generateID("Admin", "Admin"), "Admin", "admin123"));
        saveData();
    }

    int choice;
    do {
        clearScreen();
        std::cout << "--- Hotel Management System ---" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Register New Account" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: login(); break;
            case 2: registerNewUserOrAdmin(); break;
            case 3:
                std::cout << "Exiting the program. Thank you!" << std::endl;
                // Clean up dynamic memory
                for(Person* p : people) {
                    delete p;
                }
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                std::cout << "Press Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                break;
        }
    } while (choice != 3);

    return 0;
}
