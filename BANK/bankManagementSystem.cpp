#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <limits> // Required for numeric_limits

// --- Global Data Files ---
const std::string ACCOUNTS_FILE = "accounts.txt";

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

// Function to generate a simple account ID
std::string generateAccountID() {
    static int nextID = 1000;
    std::ifstream file(ACCOUNTS_FILE);
    if (file.is_open()) {
        std::string line;
        while(std::getline(file, line)) {
            nextID++;
        }
        file.close();
    }
    std::string id = "ACC" + std::to_string(nextID);
    nextID++;
    return id;
}

// --- Account Class ---
class Account {
private:
    std::string accountNumber;
    std::string accountHolderName;
    std::string password;
    double balance;
    std::string accountType; // "Admin" or "User"

public:
    // Default constructor
    Account() : accountNumber(""), accountHolderName(""), password(""), balance(0.0), accountType("User") {}

    // Parameterized constructor
    Account(std::string accNum, std::string name, std::string pwd, double bal, std::string type)
        : accountNumber(accNum), accountHolderName(name), password(pwd), balance(bal), accountType(type) {}

    // Getters
    std::string getAccountNumber() const { return accountNumber; }
    std::string getAccountHolderName() const { return accountHolderName; }
    std::string getPassword() const { return password; }
    double getBalance() const { return balance; }
    std::string getAccountType() const { return accountType; }

    // Setters
    void setAccountHolderName(const std::string& name) { this->accountHolderName = name; }
    void setPassword(const std::string& pwd) { this->password = pwd; }
    void setBalance(double bal) { this->balance = bal; }

    // Core functionalities
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            std::cout << "Deposit successful! New balance: $" << std::fixed << std::setprecision(2) << balance << std::endl;
        } else {
            std::cout << "Invalid deposit amount." << std::endl;
        }
    }

    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            std::cout << "Withdrawal successful! New balance: $" << std::fixed << std::setprecision(2) << balance << std::endl;
        } else if (amount > balance) {
            std::cout << "Error: Insufficient funds." << std::endl;
        } else {
            std::cout << "Invalid withdrawal amount." << std::endl;
        }
    }

    // Display method
    void display() const {
        std::cout << std::left << std::setw(15) << accountNumber
                  << std::setw(30) << accountHolderName
                  << "$" << std::setw(15) << std::fixed << std::setprecision(2) << balance
                  << std::setw(10) << accountType << std::endl;
    }

    // Convert object to a string for file storage
    std::string toString() const {
        std::ostringstream ss;
        ss << accountNumber << "," << accountHolderName << "," << password << "," << balance << "," << accountType;
        return ss.str();
    }
};

// --- Global Data Vector ---
std::vector<Account> accounts;

// --- File Handling Functions ---
void loadAccounts() {
    accounts.clear();
    std::ifstream file(ACCOUNTS_FILE);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string accNum, name, pwd, balStr, type;
            std::getline(ss, accNum, ',');
            std::getline(ss, name, ',');
            std::getline(ss, pwd, ',');
            std::getline(ss, balStr, ',');
            std::getline(ss, type);
            accounts.push_back(Account(accNum, name, pwd, std::stod(balStr), type));
        }
        file.close();
    }
}

void saveAccounts() {
    std::ofstream file(ACCOUNTS_FILE);
    if (file.is_open()) {
        for (const auto& acc : accounts) {
            file << acc.toString() << "\n";
        }
        file.close();
    }
}

// --- Menu Functions ---
void adminMenu();
void userMenu(Account& currentUser);

// --- Login & Registration ---
void login() {
    clearScreen();
    std::string accNum, pwd;
    std::cout << "--- Login ---" << std::endl;
    std::cout << "Enter Account Number: ";
    std::cin >> accNum;
    std::cout << "Enter Password: ";
    std::cin >> pwd;

    for (Account& acc : accounts) {
        if (acc.getAccountNumber() == accNum && acc.getPassword() == pwd) {
            std::cout << "\nLogin successful. Welcome, " << acc.getAccountHolderName() << "!" << std::endl;
            if (acc.getAccountType() == "Admin") {
                adminMenu();
            } else {
                userMenu(acc);
            }
            return;
        }
    }
    std::cout << "\nInvalid account number or password." << std::endl;
    pause();
}

void registerAccount() {
    clearScreen();
    std::string name, pwd, type;
    std::cout << "--- New Account Registration ---" << std::endl;
    std::cout << "Enter Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    std::cout << "Enter Password: ";
    std::cin >> pwd;
    std::cout << "Register as (User/Admin)? ";
    std::cin >> type;

    // Capitalize the first letter and convert rest to lowercase for consistency
    if (!type.empty()) {
        type[0] = toupper(type[0]);
        for (size_t i = 1; i < type.length(); ++i) {
            type[i] = tolower(type[i]);
        }
    }
    
    // Validate account type
    if (type != "User" && type != "Admin") {
        std::cout << "Invalid account type. Defaulting to 'User'." << std::endl;
        type = "User";
    }

    std::string newAccNum = generateAccountID();
    accounts.push_back(Account(newAccNum, name, pwd, 0.0, type));
    saveAccounts();
    std::cout << "\nRegistration successful! Your new account number is: " << newAccNum << std::endl;
    pause();
}

// --- Admin Functions ---
void manageAccounts() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin Account Management ---" << std::endl;
        std::cout << "1. View All Accounts" << std::endl;
        std::cout << "2. Search for an Account" << std::endl;
        std::cout << "3. Delete an Account" << std::endl;
        std::cout << "4. Update Account Password" << std::endl;
        std::cout << "5. Back to Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "--- All Bank Accounts ---" << std::endl;
                std::cout << std::left << std::setw(15) << "Account No." << std::setw(30) << "Account Holder Name" << std::setw(16) << "Balance" << std::setw(10) << "Type" << std::endl;
                std::cout << std::string(81, '-') << std::endl;
                for (const auto& acc : accounts) {
                    acc.display();
                }
                std::cout << std::string(81, '-') << std::endl;
                break;
            }
            case 2: {
                std::string accNum;
                std::cout << "Enter account number to search: ";
                std::cin >> accNum;
                auto it = std::find_if(accounts.begin(), accounts.end(), [&](const Account& acc){ return acc.getAccountNumber() == accNum; });
                if (it != accounts.end()) {
                    clearScreen();
                    std::cout << "--- Account Found ---" << std::endl;
                    std::cout << std::left << std::setw(15) << "Account No." << std::setw(30) << "Account Holder Name" << std::setw(16) << "Balance" << std::setw(10) << "Type" << std::endl;
                    std::cout << std::string(81, '-') << std::endl;
                    it->display();
                    std::cout << std::string(81, '-') << std::endl;
                } else {
                    std::cout << "Account not found." << std::endl;
                }
                break;
            }
            case 3: {
                std::string accNum;
                std::cout << "Enter account number to delete: ";
                std::cin >> accNum;
                auto it = std::remove_if(accounts.begin(), accounts.end(), [&](const Account& acc){ return acc.getAccountNumber() == accNum; });
                if (it != accounts.end()) {
                    accounts.erase(it, accounts.end());
                    saveAccounts();
                    std::cout << "Account deleted successfully." << std::endl;
                } else {
                    std::cout << "Account not found." << std::endl;
                }
                break;
            }
            case 4: {
                std::string accNum, newPwd;
                std::cout << "Enter account number to update password: ";
                std::cin >> accNum;
                auto it = std::find_if(accounts.begin(), accounts.end(), [&](const Account& acc){ return acc.getAccountNumber() == accNum; });
                if (it != accounts.end()) {
                    std::cout << "Enter new password: ";
                    std::cin >> newPwd;
                    it->setPassword(newPwd);
                    saveAccounts();
                    std::cout << "Password updated successfully." << std::endl;
                } else {
                    std::cout << "Account not found." << std::endl;
                }
                break;
            }
            case 5:
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        pause();
    } while (choice != 5);
}

void adminMenu() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin Menu ---" << std::endl;
        std::cout << "1. Manage Accounts" << std::endl;
        std::cout << "2. Register New Account" << std::endl;
        std::cout << "3. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: manageAccounts(); break;
            case 2: registerAccount(); break;
            case 3:
                std::cout << "Logging out..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                pause();
                break;
        }
    } while (choice != 3);
}

// --- User Functions ---
void userMenu(Account& currentUser) {
    int choice;
    do {
        clearScreen();
        std::cout << "--- User Menu ---" << std::endl;
        std::cout << "Welcome, " << currentUser.getAccountHolderName() << "!" << std::endl;
        std::cout << "1. View Balance" << std::endl;
        std::cout << "2. Deposit" << std::endl;
        std::cout << "3. Withdraw" << std::endl;
        std::cout << "4. Change Password" << std::endl;
        std::cout << "5. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::cout << "\nYour current balance is: $" << std::fixed << std::setprecision(2) << currentUser.getBalance() << std::endl;
                break;
            }
            case 2: {
                double amount;
                std::cout << "Enter amount to deposit: $";
                std::cin >> amount;
                currentUser.deposit(amount);
                saveAccounts();
                break;
            }
            case 3: {
                double amount;
                std::cout << "Enter amount to withdraw: $";
                std::cin >> amount;
                currentUser.withdraw(amount);
                saveAccounts();
                break;
            }
            case 4: {
                std::string newPwd;
                std::cout << "Enter new password: ";
                std::cin >> newPwd;
                currentUser.setPassword(newPwd);
                saveAccounts();
                std::cout << "Password updated successfully." << std::endl;
                break;
            }
            case 5:
                std::cout << "Logging out..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        pause();
    } while (choice != 5);
}

// --- Main Program ---
int main() {
    loadAccounts();

    // Create a default admin account if none exists
    bool adminExists = false;
    for(const auto& acc : accounts) {
        if (acc.getAccountType() == "Admin") {
            adminExists = true;
            break;
        }
    }
    if (!adminExists) {
        std::cout << "No Admin account found. Creating a default Admin." << std::endl;
        accounts.push_back(Account("ADMIN001", "Admin", "admin123", 1000000.0, "Admin"));
        saveAccounts();
    }

    int choice;
    do {
        clearScreen();
        std::cout << "--- Bank Management System ---" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Register New Account" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: login(); break;
            case 2: registerAccount(); break;
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
