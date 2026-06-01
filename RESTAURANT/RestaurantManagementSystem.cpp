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
const std::string MENU_FILE = "menu.txt";
const std::string TABLES_FILE = "tables.txt";
const std::string BOOKINGS_FILE = "bookings.txt";
const std::string BILLS_FILE = "bills.txt";

// Helper function to get the current year
int getCurrentYear()
{
    time_t now = time(0);
    struct tm *ltm = localtime(&now);
    return 1900 + ltm->tm_year;
}

// --- Base Class for Users ---
class Person
{
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
    void setPassword(const std::string &newPassword) { this->password = newPassword; }

    virtual std::string getRole() const = 0; // Pure virtual function

    void display() const
    {
        std::cout << std::left << std::setw(15) << id << std::setw(25) << name << std::setw(15) << password;
    }

    std::string toString() const
    {
        return id + "," + name + "," + password;
    }
};

// --- User Class (for Customers) ---
class User : public Person
{
public:
    User(std::string id = "", std::string name = "", std::string password = "")
        : Person(id, name, password) {}

    std::string getRole() const override { return "User"; }
};

// --- Admin Class ---
class Admin : public Person
{
public:
    Admin(std::string id = "", std::string name = "", std::string password = "")
        : Person(id, name, password) {}

    std::string getRole() const override { return "Admin"; }
};

// --- Menu Item Class ---
class MenuItem
{
private:
    std::string id;
    std::string name;
    double price;

public:
    MenuItem(std::string id = "", std::string name = "", double price = 0.0)
        : id(id), name(name), price(price) {}

    std::string getID() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }

    void setName(const std::string &newName) { this->name = newName; }
    void setPrice(double newPrice) { this->price = newPrice; }

    void display() const
    {
        std::cout << std::left << std::setw(15) << id << std::setw(30) << name << std::fixed << std::setprecision(2) << std::setw(10) << price << std::endl;
    }

    std::string toString() const
    {
        std::ostringstream ss;
        ss << id << "," << name << "," << price;
        return ss.str();
    }
};

// --- Table Class ---
class Table
{
private:
    std::string id;
    int capacity;
    std::string status; // "Available" or "Booked"
public:
    Table(std::string id = "", int capacity = 0, std::string status = "Available")
        : id(id), capacity(capacity), status(status) {}

    std::string getID() const { return id; }
    int getCapacity() const { return capacity; }
    std::string getStatus() const { return status; }

    void setCapacity(int newCapacity) { this->capacity = newCapacity; }
    void setStatus(const std::string &newStatus) { this->status = newStatus; }

    void display() const
    {
        std::cout << std::left << std::setw(15) << id << std::setw(15) << capacity << std::setw(15) << status << std::endl;
    }

    std::string toString() const
    {
        std::ostringstream ss;
        ss << id << "," << capacity << "," << status;
        return ss.str();
    }
};

// --- Booking Class ---
class Booking
{
private:
    std::string id;
    std::string userId;
    std::string tableId;
    std::string date;
    std::string time;
    std::vector<std::pair<std::string, int>> items; // <itemId, quantity>
    std::string extraService;
    double totalAmount;

public:
    Booking(std::string id = "", std::string userId = "", std::string tableId = "", std::string date = "", std::string time = "", std::string extraService = "", double totalAmount = 0.0)
        : id(id), userId(userId), tableId(tableId), date(date), time(time), extraService(extraService), totalAmount(totalAmount) {}

    std::string getID() const { return id; }
    std::string getUserId() const { return userId; }
    std::string getTableId() const { return tableId; }
    std::string getDate() const { return date; }
    std::string getTime() const { return time; }
    const std::vector<std::pair<std::string, int>> &getItems() const { return items; }
    std::string getExtraService() const { return extraService; }
    double getTotalAmount() const { return totalAmount; }

    void addItem(const std::string &itemId, int quantity) { items.push_back({itemId, quantity}); }
    void setExtraService(const std::string &service) { this->extraService = service; }
    void setTotalAmount(double amount) { this->totalAmount = amount; }
    void removeItem(const std::string &itemId);

    void display() const
    {
        std::cout << std::left << std::setw(15) << id << std::setw(15) << userId << std::setw(15) << tableId << std::setw(15) << date << std::setw(15) << time << std::endl;
    }

    std::string toString() const
    {
        std::ostringstream ss;
        ss << id << "," << userId << "," << tableId << "," << date << "," << time;
        if (!items.empty())
        {
            ss << ",";
            for (size_t i = 0; i < items.size(); ++i)
            {
                ss << items[i].first << ":" << items[i].second;
                if (i < items.size() - 1)
                    ss << ";";
            }
        }
        ss << "," << extraService << "," << totalAmount;
        return ss.str();
    }
};

void Booking::removeItem(const std::string &itemId)
{
    auto it = std::remove_if(items.begin(), items.end(), [&](const auto &p)
                             { return p.first == itemId; });
    if (it != items.end())
    {
        items.erase(it, items.end());
        std::cout << "Item " << itemId << " removed from order." << std::endl;
    }
    else
    {
        std::cout << "Item " << itemId << " not found in order." << std::endl;
    }
}

// --- Bill Class ---
class Bill
{
private:
    std::string id;
    std::string bookingId;
    std::string userId;
    std::string date;
    double totalAmount;

public:
    Bill(std::string id = "", std::string bookingId = "", std::string userId = "", std::string date = "", double totalAmount = 0.0)
        : id(id), bookingId(bookingId), userId(userId), date(date), totalAmount(totalAmount) {}

    std::string getID() const { return id; }
    std::string getBookingId() const { return bookingId; }
    std::string getUserId() const { return userId; }
    std::string getDate() const { return date; }
    double getTotalAmount() const { return totalAmount; }

    void display() const
    {
        std::cout << std::left << std::setw(15) << id << std::setw(15) << bookingId << std::setw(15) << userId << std::setw(15) << date << std::fixed << std::setprecision(2) << std::setw(15) << totalAmount << std::endl;
    }

    std::string toString() const
    {
        std::ostringstream ss;
        ss << id << "," << bookingId << "," << userId << "," << date << "," << totalAmount;
        return ss.str();
    }
};

// --- Global Data Structures ---
std::vector<Person *> people; // Stores both Users and Admins
std::vector<MenuItem> menuItems;
std::vector<Table> tables;
std::vector<Booking> bookings;
std::vector<Bill> bills;

// --- Helper Functions ---
void clearScreen()
{
// For Windows
#ifdef _WIN32
    system("cls");
// For Unix/Linux/macOS
#else
    system("clear");
#endif
}

std::string generateID(const std::string &name, const std::string &role)
{
    std::string prefix = name.substr(0, std::min((size_t)4, name.length()));
    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
    return prefix + std::to_string(getCurrentYear());
}

// --- File Handling Functions ---
void loadData()
{
    // Load Users and Admins
    std::ifstream userFile(USERS_FILE);
    if (userFile.is_open())
    {
        std::string line;
        while (getline(userFile, line))
        {
            std::stringstream ss(line);
            std::string id, name, password, role;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, password, ',');
            getline(ss, role, ',');
            if (role == "Admin")
            {
                people.push_back(new Admin(id, name, password));
            }
            else
            {
                people.push_back(new User(id, name, password));
            }
        }
        userFile.close();
    }

    // Load Menu Items
    std::ifstream menuFile(MENU_FILE);
    if (menuFile.is_open())
    {
        std::string line;
        while (getline(menuFile, line))
        {
            std::stringstream ss(line);
            std::string id, name, priceStr;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, priceStr, ',');
            menuItems.push_back(MenuItem(id, name, stod(priceStr)));
        }
        menuFile.close();
    }

    // Load Tables
    std::ifstream tableFile(TABLES_FILE);
    if (tableFile.is_open())
    {
        std::string line;
        while (getline(tableFile, line))
        {
            std::stringstream ss(line);
            std::string id, capacityStr, status;
            getline(ss, id, ',');
            getline(ss, capacityStr, ',');
            getline(ss, status, ',');
            tables.push_back(Table(id, stoi(capacityStr), status));
        }
        tableFile.close();
    }

    // Load Bookings
    std::ifstream bookingFile(BOOKINGS_FILE);
    if (bookingFile.is_open())
    {
        std::string line;
        while (getline(bookingFile, line))
        {
            std::stringstream ss(line);
            std::string id, userId, tableId, date, time, itemsStr, extraService, totalAmountStr;
            getline(ss, id, ',');
            getline(ss, userId, ',');
            getline(ss, tableId, ',');
            getline(ss, date, ',');
            getline(ss, time, ',');
            getline(ss, itemsStr, ',');
            getline(ss, extraService, ',');
            getline(ss, totalAmountStr, ',');

            Booking b(id, userId, tableId, date, time, extraService, stod(totalAmountStr));
            std::stringstream itemStream(itemsStr);
            std::string itemPair;
            while (getline(itemStream, itemPair, ';'))
            {
                std::stringstream pairStream(itemPair);
                std::string itemId, quantityStr;
                getline(pairStream, itemId, ':');
                getline(pairStream, quantityStr);
                b.addItem(itemId, stoi(quantityStr));
            }
            bookings.push_back(b);
        }
        bookingFile.close();
    }

    // Load Bills
    std::ifstream billFile(BILLS_FILE);
    if (billFile.is_open())
    {
        std::string line;
        while (getline(billFile, line))
        {
            std::stringstream ss(line);
            std::string id, bookingId, userId, date, totalAmountStr;
            getline(ss, id, ',');
            getline(ss, bookingId, ',');
            getline(ss, userId, ',');
            getline(ss, date, ',');
            getline(ss, totalAmountStr, ',');
            bills.push_back(Bill(id, bookingId, userId, date, stod(totalAmountStr)));
        }
        billFile.close();
    }
}

void saveData()
{
    // Save Users
    std::ofstream userFile(USERS_FILE);
    for (const auto &p : people)
    {
        userFile << p->toString() << "," << p->getRole() << "\n";
    }
    userFile.close();

    // Save Menu Items
    std::ofstream menuFile(MENU_FILE);
    for (const auto &item : menuItems)
    {
        menuFile << item.toString() << "\n";
    }
    menuFile.close();

    // Save Tables
    std::ofstream tableFile(TABLES_FILE);
    for (const auto &table : tables)
    {
        tableFile << table.toString() << "\n";
    }
    tableFile.close();

    // Save Bookings
    std::ofstream bookingFile(BOOKINGS_FILE);
    for (const auto &booking : bookings)
    {
        bookingFile << booking.toString() << "\n";
    }
    bookingFile.close();

    // Save Bills
    std::ofstream billFile(BILLS_FILE);
    for (const auto &bill : bills)
    {
        billFile << bill.toString() << "\n";
    }
    billFile.close();
}

// --- Menu Functions ---
void adminMenu();
void userMenu(const std::string &userId);

void login()
{
    clearScreen();
    std::string id, password;
    std::cout << "--- Login ---" << std::endl;
    std::cout << "Enter ID: ";
    std::cin >> id;
    std::cout << "Enter Password: ";
    std::cin >> password;

    for (const auto &p : people)
    {
        if (p->getID() == id && p->getPassword() == password)
        {
            std::cout << "Login successful. Welcome, " << p->getName() << "!" << std::endl;
            if (p->getRole() == "Admin")
            {
                adminMenu();
            }
            else
            {
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

void registerNewUserOrAdmin()
{
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

    std::string newId = generateID(name, role);
    std::cout << "Your new ID is: " << newId << std::endl;

    if (role == "Admin")
    {
        people.push_back(new Admin(newId, name, password));
    }
    else
    {
        people.push_back(new User(newId, name, password));
    }
    saveData();
    std::cout << "Registration successful!" << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

// --- Admin Sub-menus ---
void manageMenu()
{
    int choice;
    do
    {
        clearScreen();
        std::cout << "--- Admin Menu Management ---" << std::endl;
        std::cout << "1. Add Menu Item" << std::endl;
        std::cout << "2. View Menu" << std::endl;
        std::cout << "3. Update Menu Item" << std::endl;
        std::cout << "4. Delete Menu Item" << std::endl;
        std::cout << "5. Back to Main Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
            std::string id, name;
            double price;
            std::cout << "Enter Item ID: ";
            std::cin >> id;
            std::cout << "Enter Item Name: ";
            std::cin.ignore();
            std::getline(std::cin, name);
            std::cout << "Enter Price: ";
            std::cin >> price;
            menuItems.push_back(MenuItem(id, name, price));
            saveData();
            std::cout << "Item added successfully." << std::endl;
            break;
        }
        case 2:
            clearScreen();
            std::cout << "--- Full Menu ---" << std::endl;
            std::cout << std::left << std::setw(15) << "Item ID" << std::setw(30) << "Name" << std::setw(10) << "Price" << std::endl;
            std::cout << std::string(55, '-') << std::endl;
            for (const auto &item : menuItems)
            {
                item.display();
            }
            std::cout << std::string(55, '-') << std::endl;
            break;
        case 3:
        {
            std::string id;
            std::cout << "Enter Item ID to update: ";
            std::cin >> id;
            auto it = std::find_if(menuItems.begin(), menuItems.end(), [&](const MenuItem &item)
                                   { return item.getID() == id; });
            if (it != menuItems.end())
            {
                std::string newName;
                double newPrice;
                std::cout << "Enter new name: ";
                std::cin.ignore();
                std::getline(std::cin, newName);
                std::cout << "Enter new price: ";
                std::cin >> newPrice;
                it->setName(newName);
                it->setPrice(newPrice);
                saveData();
                std::cout << "Item updated successfully." << std::endl;
            }
            else
            {
                std::cout << "Item not found." << std::endl;
            }
            break;
        }
        case 4:
        {
            std::string id;
            std::cout << "Enter Item ID to delete: ";
            std::cin >> id;
            auto it = std::remove_if(menuItems.begin(), menuItems.end(), [&](const MenuItem &item)
                                     { return item.getID() == id; });
            if (it != menuItems.end())
            {
                menuItems.erase(it, menuItems.end());
                saveData();
                std::cout << "Item deleted successfully." << std::endl;
            }
            else
            {
                std::cout << "Item not found." << std::endl;
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

void manageUsers()
{
    int choice;
    do
    {
        clearScreen();
        std::cout << "--- Admin User Management ---" << std::endl;
        std::cout << "1. View All Users & Admins" << std::endl;
        std::cout << "2. Register New User/Admin" << std::endl;
        std::cout << "3. Update User Info" << std::endl;
        std::cout << "4. Delete User" << std::endl;
        std::cout << "5. Back to Main Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            clearScreen();
            std::cout << "--- All System Users ---" << std::endl;
            std::cout << std::left << std::setw(15) << "ID" << std::setw(25) << "Name" << std::setw(15) << "Password" << std::setw(10) << "Role" << std::endl;
            std::cout << std::string(65, '-') << std::endl;
            for (const auto &p : people)
            {
                p->display();
                std::cout << std::setw(10) << p->getRole() << std::endl;
            }
            std::cout << std::string(65, '-') << std::endl;
            break;
        case 2:
            registerNewUserOrAdmin();
            break;
        case 3:
        {
            std::string id;
            std::cout << "Enter User ID to update: ";
            std::cin >> id;
            auto it = std::find_if(people.begin(), people.end(), [&](const Person *p)
                                   { return p->getID() == id; });
            if (it != people.end())
            {
                std::string newPassword;
                std::cout << "Enter new password: ";
                std::cin >> newPassword;
                (*it)->setPassword(newPassword);
                saveData();
                std::cout << "User information updated." << std::endl;
            }
            else
            {
                std::cout << "User not found." << std::endl;
            }
            break;
        }
        case 4:
        {
            std::string id;
            std::cout << "Enter User ID to delete: ";
            std::cin >> id;
            auto it = std::remove_if(people.begin(), people.end(), [&](const Person *p)
                                     { return p->getID() == id; });
            if (it != people.end())
            {
                people.erase(it, people.end());
                saveData();
                std::cout << "User deleted successfully." << std::endl;
            }
            else
            {
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

void manageTables()
{
    int choice;
    do
    {
        clearScreen();
        std::cout << "--- Admin Table Management ---" << std::endl;
        std::cout << "1. Add New Table" << std::endl;
        std::cout << "2. View All Tables" << std::endl;
        std::cout << "3. Update Table Info" << std::endl;
        std::cout << "4. Delete Table" << std::endl;
        std::cout << "5. Search Table by ID" << std::endl;
        std::cout << "6. Sort Tables by Capacity" << std::endl;
        std::cout << "7. Back to Main Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
            std::string id;
            int capacity;
            std::cout << "Enter Table ID: ";
            std::cin >> id;
            std::cout << "Enter Capacity: ";
            std::cin >> capacity;
            tables.push_back(Table(id, capacity));
            saveData();
            std::cout << "Table added successfully." << std::endl;
            break;
        }
        case 2:
            clearScreen();
            std::cout << "--- All Tables ---" << std::endl;
            std::cout << std::left << std::setw(15) << "Table ID" << std::setw(15) << "Capacity" << std::setw(15) << "Status" << std::endl;
            std::cout << std::string(45, '-') << std::endl;
            for (const auto &table : tables)
            {
                table.display();
            }
            std::cout << std::string(45, '-') << std::endl;
            break;
        case 3:
        {
            std::string id;
            std::cout << "Enter Table ID to update: ";
            std::cin >> id;
            auto it = std::find_if(tables.begin(), tables.end(), [&](const Table &t)
                                   { return t.getID() == id; });
            if (it != tables.end())
            {
                int newCapacity;
                std::cout << "Enter new capacity: ";
                std::cin >> newCapacity;
                it->setCapacity(newCapacity);
                saveData();
                std::cout << "Table updated successfully." << std::endl;
            }
            else
            {
                std::cout << "Table not found." << std::endl;
            }
            break;
        }
        case 4:
        {
            std::string id;
            std::cout << "Enter Table ID to delete: ";
            std::cin >> id;
            auto it = std::remove_if(tables.begin(), tables.end(), [&](const Table &t)
                                     { return t.getID() == id; });
            if (it != tables.end())
            {
                tables.erase(it, tables.end());
                saveData();
                std::cout << "Table deleted successfully." << std::endl;
            }
            else
            {
                std::cout << "Table not found." << std::endl;
            }
            break;
        }
        case 5:
        {
            std::string id;
            std::cout << "Enter Table ID to search: ";
            std::cin >> id;
            auto it = std::find_if(tables.begin(), tables.end(), [&](const Table &t)
                                   { return t.getID() == id; });
            if (it != tables.end())
            {
                clearScreen();
                std::cout << "--- Table Found ---" << std::endl;
                std::cout << std::left << std::setw(15) << "Table ID" << std::setw(15) << "Capacity" << std::setw(15) << "Status" << std::endl;
                std::cout << std::string(45, '-') << std::endl;
                it->display();
                std::cout << std::string(45, '-') << std::endl;
            }
            else
            {
                std::cout << "Table not found." << std::endl;
            }
            break;
        }
        case 6:
        {
            std::sort(tables.begin(), tables.end(), [](const Table &a, const Table &b)
                      { return a.getCapacity() < b.getCapacity(); });
            std::cout << "Tables sorted by capacity." << std::endl;
            break;
        }
        case 7:
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            break;
        }
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    } while (choice != 7);
}

void viewHistory()
{
    int choice;
    do
    {
        clearScreen();
        std::cout << "--- Admin History View ---" << std::endl;
        std::cout << "1. View All Bookings" << std::endl;
        std::cout << "2. View All Bills" << std::endl;
        std::cout << "3. Delete User Bookings" << std::endl;
        std::cout << "4. Back to Main Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            clearScreen();
            std::cout << "--- All Bookings ---" << std::endl;
            std::cout << std::left << std::setw(15) << "Booking ID" << std::setw(15) << "User ID" << std::setw(15) << "Table ID" << std::setw(15) << "Date" << std::setw(15) << "Time" << std::endl;
            std::cout << std::string(75, '-') << std::endl;
            for (const auto &booking : bookings)
            {
                booking.display();
            }
            std::cout << std::string(75, '-') << std::endl;
            break;
        case 2:
            clearScreen();
            std::cout << "--- All Bills ---" << std::endl;
            std::cout << std::left << std::setw(15) << "Bill ID" << std::setw(15) << "Booking ID" << std::setw(15) << "User ID" << std::setw(15) << "Date" << std::setw(15) << "Total Amount" << std::endl;
            std::cout << std::string(75, '-') << std::endl;
            for (const auto &bill : bills)
            {
                bill.display();
            }
            std::cout << std::string(75, '-') << std::endl;
            break;
        case 3:
        {
            std::string userId;
            std::cout << "Enter User ID whose bookings to delete: ";
            std::cin >> userId;
            auto it = std::remove_if(bookings.begin(), bookings.end(), [&](const Booking &b)
                                     { return b.getUserId() == userId; });
            if (it != bookings.end())
            {
                bookings.erase(it, bookings.end());
                saveData();
                std::cout << "User's bookings deleted successfully." << std::endl;
            }
            else
            {
                std::cout << "No bookings found for this user." << std::endl;
            }
            break;
        }
        case 4:
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            break;
        }
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    } while (choice != 4);
}

void adminMenu()
{
    int choice;
    do
    {
        clearScreen();
        std::cout << "--- Admin Menu ---" << std::endl;
        std::cout << "1. Manage Menu Items" << std::endl;
        std::cout << "2. Manage Users" << std::endl;
        std::cout << "3. Manage Tables" << std::endl;
        std::cout << "4. View History & Manage Bookings" << std::endl;
        std::cout << "5. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            manageMenu();
            break;
        case 2:
            manageUsers();
            break;
        case 3:
            manageTables();
            break;
        case 4:
            viewHistory();
            break;
        case 5:
            std::cout << "Logging out..." << std::endl;
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
            std::cin.get();
            break;
        }
    } while (choice != 5);
}

// --- User Sub-menus ---
void bookTable(const std::string &userId)
{
    clearScreen();
    std::cout << "--- Book a Table ---" << std::endl;
    std::cout << "Available tables:" << std::endl;
    std::cout << std::left << std::setw(15) << "Table ID" << std::setw(15) << "Capacity" << std::setw(15) << "Status" << std::endl;
    std::cout << std::string(45, '-') << std::endl;
    std::vector<Table *> availableTables;
    for (auto &table : tables)
    {
        if (table.getStatus() == "Available")
        {
            table.display();
            availableTables.push_back(&table);
        }
    }
    std::cout << std::string(45, '-') << std::endl;

    if (availableTables.empty())
    {
        std::cout << "Sorry, no tables are available at the moment." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    std::string tableId, date, time;
    std::cout << "Enter Table ID to book: ";
    std::cin >> tableId;
    std::cout << "Enter Date (DD-MM-YYYY): ";
    std::cin >> date;
    std::cout << "Enter Time (HH:MM): ";
    std::cin >> time;

    auto it = std::find_if(tables.begin(), tables.end(), [&](const Table &t)
                           { return t.getID() == tableId && t.getStatus() == "Available"; });
    if (it != tables.end())
    {
        it->setStatus("Booked");
        std::string bookingId = "BOOK" + std::to_string(bookings.size() + 1);
        bookings.push_back(Booking(bookingId, userId, tableId, date, time));
        saveData();
        std::cout << "Table " << tableId << " booked successfully!" << std::endl;
    }
    else
    {
        std::cout << "Invalid table ID or table is not available." << std::endl;
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void orderFood(const std::string &userId)
{
    clearScreen();
    std::cout << "--- Order Food and Services ---" << std::endl;
    std::cout << "1. Book Food Items" << std::endl;
    std::cout << "2. Add Extra Service" << std::endl;
    std::cout << "3. Delete an Item from Order" << std::endl;
    std::cout << "4. Back to User Menu" << std::endl;
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;

    switch (choice)
    {
    case 1:
    {
        clearScreen();
        std::cout << "--- Menu Items ---" << std::endl;
        std::cout << std::left << std::setw(15) << "Item ID" << std::setw(30) << "Name" << std::setw(10) << "Price" << std::endl;
        std::cout << std::string(55, '-') << std::endl;
        for (const auto &item : menuItems)
        {
            item.display();
        }
        std::cout << std::string(55, '-') << std::endl;

        // Find current user's most recent booking to add items to it
        auto currentBooking = std::find_if(bookings.rbegin(), bookings.rend(), [&](const Booking &b)
                                           { return b.getUserId() == userId; });
        if (currentBooking == bookings.rend())
        {
            std::cout << "You must book a table first before ordering food." << std::endl;
            break;
        }

        std::string itemId;
        int quantity;
        char continueOrder = 'y';
        while (continueOrder == 'y' || continueOrder == 'Y')
        {
            std::cout << "Enter Item ID to order: ";
            std::cin >> itemId;
            std::cout << "Enter quantity: ";
            std::cin >> quantity;

            auto it = std::find_if(menuItems.begin(), menuItems.end(), [&](const MenuItem &item)
                                   { return item.getID() == itemId; });
            if (it != menuItems.end())
            {
                currentBooking->addItem(itemId, quantity);
                std::cout << quantity << " " << it->getName() << "(s) added to your order." << std::endl;
            }
            else
            {
                std::cout << "Invalid Item ID." << std::endl;
            }
            std::cout << "Add another item? (y/n): ";
            std::cin >> continueOrder;
        }
        saveData();
        std::cout << "Order updated successfully." << std::endl;
        break;
    }
    case 2:
    {
        std::string service;
        std::cout << "Enter extra service details (e.g., 'birthday cake', 'special decor'): ";
        std::cin.ignore();
        std::getline(std::cin, service);

        auto currentBooking = std::find_if(bookings.rbegin(), bookings.rend(), [&](const Booking &b)
                                           { return b.getUserId() == userId; });
        if (currentBooking != bookings.rend())
        {
            currentBooking->setExtraService(service);
            saveData();
            std::cout << "Extra service added successfully." << std::endl;
        }
        else
        {
            std::cout << "You must book a table first before adding services." << std::endl;
        }
        break;
    }
    case 3:
    {
        auto currentBooking = std::find_if(bookings.rbegin(), bookings.rend(), [&](const Booking &b)
                                           { return b.getUserId() == userId; });
        if (currentBooking != bookings.rend())
        {
            std::string itemId;
            std::cout << "Enter Item ID to delete from your order: ";
            std::cin >> itemId;
            currentBooking->removeItem(itemId);
            saveData();
        }
        else
        {
            std::cout << "You have no active order to modify." << std::endl;
        }
        break;
    }
    case 4:
        return;
    default:
        std::cout << "Invalid choice." << std::endl;
        break;
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void viewBookings(const std::string &userId)
{
    clearScreen();
    std::cout << "--- Your Bookings ---" << std::endl;
    std::cout << std::left << std::setw(15) << "Booking ID" << std::setw(15) << "Table ID" << std::setw(15) << "Date" << std::setw(15) << "Time" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::vector<Booking> userBookings;
    for (const auto &booking : bookings)
    {
        if (booking.getUserId() == userId)
        {
            booking.display();
            userBookings.push_back(booking);
        }
    }
    std::cout << std::string(60, '-') << std::endl;

    if (userBookings.empty())
    {
        std::cout << "You have no bookings." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    std::cout << "1. Delete a booking" << std::endl;
    std::cout << "2. Back" << std::endl;
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;

    if (choice == 1)
    {
        std::string bookingId;
        std::cout << "Enter Booking ID to delete: ";
        std::cin >> bookingId;
        auto it = std::remove_if(bookings.begin(), bookings.end(), [&](const Booking &b)
                                 { return b.getID() == bookingId && b.getUserId() == userId; });
        if (it != bookings.end())
        {
            bookings.erase(it, bookings.end());
            saveData();
            std::cout << "Booking deleted successfully." << std::endl;
        }
        else
        {
            std::cout << "Invalid booking ID or you do not own this booking." << std::endl;
        }
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void generateBill(const std::string &userId)
{
    clearScreen();
    std::cout << "--- Generate Bill ---" << std::endl;
    auto it = std::find_if(bookings.rbegin(), bookings.rend(), [&](const Booking &b)
                           { return b.getUserId() == userId; });

    if (it == bookings.rend())
    {
        std::cout << "You have no active bookings to generate a bill for." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    Booking currentBooking = *it;
    double subtotal = 0.0;

    clearScreen();
    std::cout << "--- Receipt ---" << std::endl;
    std::cout << "Booking ID: " << currentBooking.getID() << std::endl;
    std::cout << "Customer ID: " << currentBooking.getUserId() << std::endl;
    std::cout << "Table ID: " << currentBooking.getTableId() << std::endl;
    std::cout << "Date & Time: " << currentBooking.getDate() << " " << currentBooking.getTime() << std::endl;
    std::cout << std::string(30, '-') << std::endl;
    std::cout << std::left << std::setw(20) << "Item" << std::setw(10) << "Price" << std::setw(10) << "Qty" << std::setw(10) << "Total" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (const auto &itemPair : currentBooking.getItems())
    {
        std::string itemId = itemPair.first;
        int quantity = itemPair.second;
        auto menuIt = std::find_if(menuItems.begin(), menuItems.end(), [&](const MenuItem &item)
                                   { return item.getID() == itemId; });
        if (menuIt != menuItems.end())
        {
            double itemPrice = menuIt->getPrice();
            double itemTotal = itemPrice * quantity;
            subtotal += itemTotal;
            std::cout << std::left << std::setw(20) << menuIt->getName()
                      << std::fixed << std::setprecision(2) << std::setw(10) << itemPrice
                      << std::setw(10) << quantity
                      << std::setw(10) << itemTotal << std::endl;
        }
    }

    double tax = subtotal * 0.1;
    double totalAmount = subtotal + tax;
    currentBooking.setTotalAmount(totalAmount);

    std::cout << std::string(50, '-') << std::endl;
    std::cout << std::right << std::setw(40) << "Subtotal: " << std::fixed << std::setprecision(2) << subtotal << std::endl;
    std::cout << std::right << std::setw(40) << "Tax (10%): " << std::fixed << std::setprecision(2) << tax << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::cout << std::right << std::setw(40) << "TOTAL: " << std::fixed << std::setprecision(2) << totalAmount << std::endl;

    // Save to bills file
    std::string billId = "BILL" + std::to_string(bills.size() + 1);
    bills.push_back(Bill(billId, currentBooking.getID(), userId, currentBooking.getDate(), totalAmount));
    saveData();
    std::cout << "Bill generated successfully!" << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void userMenu(const std::string &userId)
{
    int choice;
    do
    {
        clearScreen();
        std::cout << "--- User Menu ---" << std::endl;
        std::cout << "1. View Menu" << std::endl;
        std::cout << "2. Book a Table" << std::endl;
        std::cout << "3. Order Food & Services" << std::endl;
        std::cout << "4. View/Delete Your Bookings" << std::endl;
        std::cout << "5. Generate Bill & Print Receipt" << std::endl;
        std::cout << "6. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            clearScreen();
            std::cout << "--- Full Menu ---" << std::endl;
            std::cout << std::left << std::setw(15) << "Item ID" << std::setw(30) << "Name" << std::setw(10) << "Price" << std::endl;
            std::cout << std::string(55, '-') << std::endl;
            for (const auto &item : menuItems)
            {
                item.display();
            }
            std::cout << std::string(55, '-') << std::endl;
            break;
        case 2:
            bookTable(userId);
            break;
        case 3:
            orderFood(userId);
            break;
        case 4:
            viewBookings(userId);
            break;
        case 5:
            generateBill(userId);
            break;
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

int main()
{
    loadData();

    // Create a default admin if no users exist
    if (people.empty())
    {
        std::cout << "No users found. Creating a default Admin account." << std::endl;
        people.push_back(new Admin(generateID("Admin", "Admin"), "Admin", "admin123"));
        saveData();
    }

    int choice;
    do
    {
        clearScreen();
        std::cout << "--- Restaurant Management System ---" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Register New Account" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            login();
            break;
        case 2:
            registerNewUserOrAdmin();
            break;
        case 3:
            std::cout << "Exiting the program. Thank you!" << std::endl;
            // Clean up dynamic memory
            for (Person *p : people)
            {
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
