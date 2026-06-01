#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <limits> // Required for numeric_limits
#include <sstream>
#include <memory> // For unique_ptr

// --- Global Data Files ---
const std::string USERS_FILE = "users.txt";
const std::string TICKETS_FILE = "tickets.txt";

// --- Utility Functions ---
// Function to clear the console for a cleaner user interface
void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

// Function to pause and wait for user input
void pause() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// Function to generate a simple ID
std::string generateID(const std::string& prefix) {
    static int nextID = 1000;
    std::ifstream file(USERS_FILE);
    if (file.is_open()) {
        std::string line;
        while(std::getline(file, line)) {
            nextID++;
        }
        file.close();
    }
    std::string id = prefix + std::to_string(nextID);
    nextID++;
    return id;
}

// --- Base Class for All Users ---
class Person {
protected:
    std::string id;
    std::string name;
    std::string password;
public:
    Person(std::string id = "", std::string name = "", std::string password = "")
        : id(id), name(name), password(password) {}

    virtual ~Person() = default;

    std::string getID() const { return id; }
    std::string getName() const { return name; }
    std::string getPassword() const { return password; }
    void setPassword(const std::string& newPassword) { this->password = newPassword; }

    virtual std::string getRole() const = 0; // Pure virtual function
    
    virtual void display() const {
        std::cout << std::left << std::setw(15) << id << std::setw(30) << name << std::setw(15) << getRole();
    }

    virtual std::string toString() const {
        return id + "," + name + "," + password;
    }
};

// --- User Class ---
class User : public Person {
private:
    std::vector<std::string> bookedTickets;
public:
    User(std::string id = "", std::string name = "", std::string password = "")
        : Person(id, name, password) {}

    std::string getRole() const override { return "User"; }
    const std::vector<std::string>& getBookedTickets() const { return bookedTickets; }
    
    void addBookedTicket(const std::string& ticketID) { bookedTickets.push_back(ticketID); }
    void removeBookedTicket(const std::string& ticketID) {
        auto it = std::find(bookedTickets.begin(), bookedTickets.end(), ticketID);
        if (it != bookedTickets.end()) {
            bookedTickets.erase(it);
        }
    }

    std::string toString() const override {
        std::string base = Person::toString() + ",User,";
        for (const auto& ticketID : bookedTickets) {
            base += ticketID + ";";
        }
        return base;
    }
};

// --- Admin Class ---
class Admin : public Person {
public:
    Admin(std::string id = "", std::string name = "", std::string password = "")
        : Person(id, name, password) {}

    std::string getRole() const override { return "Admin"; }
    
    std::string toString() const override {
        return Person::toString() + ",Admin,";
    }
};

// --- Base Class for Tickets ---
class Ticket {
protected:
    std::string ticketID;
    std::string type;
    std::string origin;
    std::string destination;
    std::string date;
    double price;
    std::string status; // "Available", "Booked", "Paid"
    std::string bookedByUserID;
public:
    Ticket(std::string id = "", std::string type = "", std::string origin = "", std::string destination = "", std::string date = "", double price = 0.0, std::string status = "Available", std::string bookedByUserID = "")
        : ticketID(id), type(type), origin(origin), destination(destination), date(date), price(price), status(status), bookedByUserID(bookedByUserID) {}

    virtual ~Ticket() = default;

    // Getters
    std::string getID() const { return ticketID; }
    std::string getType() const { return type; }
    std::string getOrigin() const { return origin; }
    std::string getDestination() const { return destination; }
    std::string getDate() const { return date; }
    double getPrice() const { return price; }
    std::string getStatus() const { return status; }
    std::string getBookedByUserID() const { return bookedByUserID; }

    // Setters
    void setStatus(const std::string& newStatus) { this->status = newStatus; }
    void setBookedByUserID(const std::string& userID) { this->bookedByUserID = userID; }

    virtual void display() const {
        std::cout << std::left << std::setw(10) << ticketID
                  << std::setw(10) << type
                  << std::setw(10) << origin
                  << std::setw(10) << destination
                  << std::setw(10) << date
                  << std::setw(10) << std::fixed << std::setprecision(2) << price
                  << std::setw(10) << status;
    }

    virtual std::string toString() const {
        std::ostringstream ss;
        ss << ticketID << "," << type << "," << origin << "," << destination << "," << date << "," << price << "," << status << "," << bookedByUserID;
        return ss.str();
    }
};

// --- Derived Ticket Classes ---
class BusTicket : public Ticket {
private:
    std::string busNumber;
public:
    BusTicket(std::string id = "", std::string origin = "", std::string destination = "", std::string date = "", double price = 0.0, std::string status = "Available", std::string bookedByUserID = "", std::string busNumber = "")
        : Ticket(id, "Bus", origin, destination, date, price, status, bookedByUserID), busNumber(busNumber) {}

    void display() const override {
        Ticket::display();
        std::cout << std::setw(15) << "Bus No: " << busNumber << std::endl;
    }

    std::string toString() const override {
        return Ticket::toString() + "," + busNumber;
    }
};

class PlaneTicket : public Ticket {
private:
    std::string flightNumber;
public:
    PlaneTicket(std::string id = "", std::string origin = "", std::string destination = "", std::string date = "", double price = 0.0, std::string status = "Available", std::string bookedByUserID = "", std::string flightNumber = "")
        : Ticket(id, "Plane", origin, destination, date, price, status, bookedByUserID), flightNumber(flightNumber) {}

    void display() const override {
        Ticket::display();
        std::cout << std::setw(15) << "Flight No: " << flightNumber << std::endl;
    }

    std::string toString() const override {
        return Ticket::toString() + "," + flightNumber;
    }
};

class TrainTicket : public Ticket {
private:
    std::string trainNumber;
public:
    TrainTicket(std::string id = "", std::string origin = "", std::string destination = "", std::string date = "", double price = 0.0, std::string status = "Available", std::string bookedByUserID = "", std::string trainNumber = "")
        : Ticket(id, "Train", origin, destination, date, price, status, bookedByUserID), trainNumber(trainNumber) {}

    void display() const override {
        Ticket::display();
        std::cout << std::setw(15) << "Train No: " << trainNumber << std::endl;
    }

    std::string toString() const override {
        return Ticket::toString() + "," + trainNumber;
    }
};

class MetroTicket : public Ticket {
private:
    std::string lineNumber;
public:
    MetroTicket(std::string id = "", std::string origin = "", std::string destination = "", std::string date = "", double price = 0.0, std::string status = "Available", std::string bookedByUserID = "", std::string lineNumber = "")
        : Ticket(id, "Metro", origin, destination, date, price, status, bookedByUserID), lineNumber(lineNumber) {}

    void display() const override {
        Ticket::display();
        std::cout << std::setw(15) << "Line: " << lineNumber << std::endl;
    }

    std::string toString() const override {
        return Ticket::toString() + "," + lineNumber;
    }
};

// --- Global Data Structures ---
std::vector<std::unique_ptr<Person>> people;
std::vector<std::unique_ptr<Ticket>> tickets;

// --- File Handling Functions ---
void loadData() {
    people.clear();
    tickets.clear();

    // Load Users and Admins
    std::ifstream userFile(USERS_FILE);
    if (userFile.is_open()) {
        std::string line;
        while (std::getline(userFile, line)) {
            std::stringstream ss(line);
            std::string id, name, password, role, bookedStr;
            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, password, ',');
            std::getline(ss, role, ',');

            if (role == "Admin") {
                people.push_back(std::make_unique<Admin>(id, name, password));
            } else if (role == "User") {
                std::getline(ss, bookedStr);
                auto user = std::make_unique<User>(id, name, password);
                std::stringstream bookedStream(bookedStr);
                std::string ticketID;
                while(std::getline(bookedStream, ticketID, ';')) {
                    if (!ticketID.empty()) {
                        user->addBookedTicket(ticketID);
                    }
                }
                people.push_back(std::move(user));
            }
        }
        userFile.close();
    }

    // Load Tickets
    std::ifstream ticketFile(TICKETS_FILE);
    if (ticketFile.is_open()) {
        std::string line;
        while (std::getline(ticketFile, line)) {
            std::stringstream ss(line);
            std::string id, type, origin, destination, date, priceStr, status, bookedBy, specialData;
            std::getline(ss, id, ',');
            std::getline(ss, type, ',');
            std::getline(ss, origin, ',');
            std::getline(ss, destination, ',');
            std::getline(ss, date, ',');
            std::getline(ss, priceStr, ',');
            std::getline(ss, status, ',');
            std::getline(ss, bookedBy, ',');
            std::getline(ss, specialData);
            
            double price = std::stod(priceStr);

            if (type == "Bus") {
                tickets.push_back(std::make_unique<BusTicket>(id, origin, destination, date, price, status, bookedBy, specialData));
            } else if (type == "Plane") {
                tickets.push_back(std::make_unique<PlaneTicket>(id, origin, destination, date, price, status, bookedBy, specialData));
            } else if (type == "Train") {
                tickets.push_back(std::make_unique<TrainTicket>(id, origin, destination, date, price, status, bookedBy, specialData));
            } else if (type == "Metro") {
                tickets.push_back(std::make_unique<MetroTicket>(id, origin, destination, date, price, status, bookedBy, specialData));
            }
        }
        ticketFile.close();
    }
}

void saveData() {
    // Save Users
    std::ofstream userFile(USERS_FILE);
    for (const auto& p : people) {
        userFile << p->toString() << "\n";
    }
    userFile.close();

    // Save Tickets
    std::ofstream ticketFile(TICKETS_FILE);
    for (const auto& ticket : tickets) {
        ticketFile << ticket->toString() << "\n";
    }
    ticketFile.close();
}

// --- Menu Functions ---
void adminMenu();
void userMenu(User* currentUser);
void login();
void registerNewAccount();

// --- Admin Sub-menus ---
void addTicket() {
    clearScreen();
    std::cout << "--- Admin: Add New Ticket ---" << std::endl;
    std::string type, origin, destination, date, specialData;
    double price;

    std::cout << "Enter Ticket Type (Bus/Plane/Train/Metro): ";
    std::cin >> type;
    std::cin.ignore();
    std::cout << "Enter Origin: ";
    std::getline(std::cin, origin);
    std::cout << "Enter Destination: ";
    std::getline(std::cin, destination);
    std::cout << "Enter Date (YYYY-MM-DD): ";
    std::getline(std::cin, date);
    std::cout << "Enter Price: ";
    std::cin >> price;
    std::cin.ignore();

    if (type == "Bus") {
        std::cout << "Enter Bus Number: ";
        std::getline(std::cin, specialData);
        tickets.push_back(std::make_unique<BusTicket>(generateID("T"), origin, destination, date, price, "Available", "", specialData));
    } else if (type == "Plane") {
        std::cout << "Enter Flight Number: ";
        std::getline(std::cin, specialData);
        tickets.push_back(std::make_unique<PlaneTicket>(generateID("T"), origin, destination, date, price, "Available", "", specialData));
    } else if (type == "Train") {
        std::cout << "Enter Train Number: ";
        std::getline(std::cin, specialData);
        tickets.push_back(std::make_unique<TrainTicket>(generateID("T"), origin, destination, date, price, "Available", "", specialData));
    } else if (type == "Metro") {
        std::cout << "Enter Line Number: ";
        std::getline(std::cin, specialData);
        tickets.push_back(std::make_unique<MetroTicket>(generateID("T"), origin, destination, date, price, "Available", "", specialData));
    } else {
        std::cout << "Invalid ticket type." << std::endl;
        pause();
        return;
    }
    
    saveData();
    std::cout << "Ticket added successfully." << std::endl;
    pause();
}

void viewAllTickets() {
    clearScreen();
    std::cout << "--- All Tickets ---" << std::endl;
    std::cout << std::left << std::setw(10) << "ID"
              << std::setw(10) << "Type"
              << std::setw(10) << "Origin"
              << std::setw(10) << "Destination"
              << std::setw(10) << "Date"
              << std::setw(10) << "Price"
              << std::setw(10) << "Status"
              << std::setw(10) << "Details" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    for (const auto& ticket : tickets) {
        ticket->display();
    }
    std::cout << std::string(100, '-') << std::endl;
}

void adminMenu() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin Menu ---" << std::endl;
        std::cout << "1. Add a New Ticket" << std::endl;
        std::cout << "2. View All Tickets" << std::endl;
        std::cout << "3. View All Users" << std::endl;
        std::cout << "4. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: addTicket(); break;
            case 2: viewAllTickets(); pause(); break;
            case 3:
                clearScreen();
                std::cout << "--- All System Users ---" << std::endl;
                std::cout << std::left << std::setw(15) << "ID" << std::setw(30) << "Name" << std::setw(15) << "Role" << std::endl;
                std::cout << std::string(60, '-') << std::endl;
                for (const auto& p : people) {
                    p->display();
                    std::cout << std::endl;
                }
                std::cout << std::string(60, '-') << std::endl;
                pause();
                break;
            case 4:
                std::cout << "Logging out..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                pause();
                break;
        }
    } while (choice != 4);
}

// --- User Sub-menus ---
void viewAvailableTickets() {
    clearScreen();
    std::cout << "--- Available Tickets ---" << std::endl;
    std::cout << std::left << std::setw(15) << "ID"
              << std::setw(10) << "Type"
              << std::setw(20) << "Origin"
              << std::setw(20) << "Destination"
              << std::setw(15) << "Date"
              << std::setw(10) << "Price"
              << std::setw(20) << "Details" << std::endl;
    std::cout << std::string(120, '-') << std::endl;
    for (const auto& ticket : tickets) {
        if (ticket->getStatus() == "Available") {
            ticket->display();
        }
    }
    std::cout << std::string(120, '-') << std::endl;
}

void bookTicket(User* currentUser) {
    clearScreen();
    std::cout << "--- Book a Ticket ---" << std::endl;
    viewAvailableTickets();
    std::string ticketID;
    std::cout << "\nEnter the ID of the ticket you wish to book: ";
    std::cin >> ticketID;

    auto it = std::find_if(tickets.begin(), tickets.end(), [&](const std::unique_ptr<Ticket>& t){ return t->getID() == ticketID; });
    if (it != tickets.end() && (*it)->getStatus() == "Available") {
        std::cout << "\nTicket Price: $" << std::fixed << std::setprecision(2) << (*it)->getPrice() << std::endl;
        std::cout << "Would you like to proceed with payment? (yes/no): ";
        std::string confirmation;
        std::cin >> confirmation;

        if (confirmation == "yes") {
            (*it)->setStatus("Paid");
            (*it)->setBookedByUserID(currentUser->getID());
            currentUser->addBookedTicket(ticketID);
            saveData();
            std::cout << "\nPayment successful! Ticket booked." << std::endl;
        } else {
            std::cout << "Payment canceled. Ticket not booked." << std::endl;
        }
    } else {
        std::cout << "Ticket not found or is not available." << std::endl;
    }
    pause();
}

void viewMyTickets(User* currentUser) {
    clearScreen();
    std::cout << "--- Your Booked Tickets ---" << std::endl;
    if (currentUser->getBookedTickets().empty()) {
        std::cout << "You have no tickets currently booked." << std::endl;
    } else {
        std::cout << std::left << std::setw(15) << "ID"
                  << std::setw(10) << "Type"
                  << std::setw(20) << "Origin"
                  << std::setw(20) << "Destination"
                  << std::setw(15) << "Date"
                  << std::setw(10) << "Price"
                  << std::setw(15) << "Status"
                  << std::setw(20) << "Details" << std::endl;
        std::cout << std::string(135, '-') << std::endl;
        for (const auto& ticketID : currentUser->getBookedTickets()) {
            auto it = std::find_if(tickets.begin(), tickets.end(), [&](const std::unique_ptr<Ticket>& t){ return t->getID() == ticketID; });
            if (it != tickets.end()) {
                (*it)->display();
            }
        }
        std::cout << std::string(135, '-') << std::endl;
    }
    pause();
}

void userMenu(User* currentUser) {
    int choice;
    do {
        clearScreen();
        std::cout << "--- User Menu ---" << std::endl;
        std::cout << "Welcome, " << currentUser->getName() << "!" << std::endl;
        std::cout << "1. View Available Tickets" << std::endl;
        std::cout << "2. Book a Ticket (Payment Process)" << std::endl;
        std::cout << "3. View My Booked Tickets" << std::endl;
        std::cout << "4. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: viewAvailableTickets(); pause(); break;
            case 2: bookTicket(currentUser); break;
            case 3: viewMyTickets(currentUser); break;
            case 4:
                std::cout << "Logging out..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                pause();
                break;
        }
    } while (choice != 4);
}

// --- Login & Registration ---
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
            std::cout << "\nLogin successful. Welcome, " << p->getName() << "!" << std::endl;
            if (p->getRole() == "Admin") {
                adminMenu();
            } else {
                userMenu(dynamic_cast<User*>(p.get()));
            }
            return;
        }
    }
    std::cout << "\nInvalid ID or Password." << std::endl;
    pause();
}

void registerNewAccount() {
    clearScreen();
    std::string name, password;
    std::cout << "--- New Account Registration ---" << std::endl;
    std::cout << "Enter Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    std::cout << "Enter Password: ";
    std::cin >> password;
    
    // Default to User account for simplicity
    people.push_back(std::make_unique<User>(generateID("USR"), name, password));
    saveData();
    std::cout << "\nRegistration successful! Your new ID is: " << people.back()->getID() << std::endl;
    pause();
}

// --- Main Program ---
int main() {
    loadData();

    // Create a default admin if no users exist
    bool adminExists = false;
    for(const auto& p : people) {
        if (p->getRole() == "Admin") {
            adminExists = true;
            break;
        }
    }
    if (!adminExists) {
        std::cout << "No Admin account found. Creating a default Admin." << std::endl;
        people.push_back(std::make_unique<Admin>("ADMIN123", "Default Admin", "admin123"));
        saveData();
    }

    int choice;
    do {
        clearScreen();
        std::cout << "--- Ticket Management System ---" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Register New User Account" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: login(); break;
            case 2: registerNewAccount(); break;
            case 3:
                std::cout << "Exiting the program. Thank you!" << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                pause();
                break;
        }
    } while (choice != 3);

    return 0;
}
