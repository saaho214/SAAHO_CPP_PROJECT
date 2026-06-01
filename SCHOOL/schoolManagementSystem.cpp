#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <limits> // Required for numeric_limits
#include <sstream>
#include <map>

// --- Global Data Files ---
const std::string USERS_FILE = "users.txt";
const std::string STUDENTS_FILE = "students.txt";
const std::string TEACHERS_FILE = "teachers.txt";

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

    virtual ~Person() {}

    std::string getID() const { return id; }
    std::string getName() const { return name; }
    std::string getPassword() const { return password; }
    void setPassword(const std::string& newPassword) { this->password = newPassword; }

    virtual std::string getRole() const = 0; // Pure virtual function
    
    // Display method for formatted output
    virtual void display() const {
        std::cout << std::left << std::setw(15) << id << std::setw(30) << name << std::setw(15) << getRole();
    }

    // Convert object to a string for file storage
    virtual std::string toString() const {
        return id + "," + name + "," + password;
    }
};

// --- Student Class ---
class Student : public Person {
private:
    std::string grade;
    double feeAmount;
    std::string feeStatus;
public:
    Student(std::string id = "", std::string name = "", std::string password = "", std::string grade = "", double feeAmount = 0.0, std::string feeStatus = "")
        : Person(id, name, password), grade(grade), feeAmount(feeAmount), feeStatus(feeStatus) {}

    std::string getRole() const override { return "Student"; }
    std::string getGrade() const { return grade; }
    double getFeeAmount() const { return feeAmount; }
    std::string getFeeStatus() const { return feeStatus; }

    void setFeeStatus(const std::string& status) { this->feeStatus = status; }
    void setFeeAmount(double amount) { this->feeAmount = amount; }
    
    void display() const override {
        Person::display();
        std::cout << std::setw(15) << grade << std::setw(15) << feeStatus << std::endl;
    }

    std::string toString() const override {
        std::ostringstream ss;
        ss << Person::toString() << "," << getRole() << "," << grade << "," << feeAmount << "," << feeStatus;
        return ss.str();
    }
};

// --- Teacher Class ---
class Teacher : public Person {
private:
    std::string subject;
public:
    Teacher(std::string id = "", std::string name = "", std::string password = "", std::string subject = "")
        : Person(id, name, password), subject(subject) {}

    std::string getRole() const override { return "Teacher"; }
    std::string getSubject() const { return subject; }

    void display() const override {
        Person::display();
        std::cout << std::setw(20) << subject << std::endl;
    }

    std::string toString() const override {
        return Person::toString() + "," + getRole() + "," + subject;
    }
};

// --- Admin Class ---
class Admin : public Person {
public:
    Admin(std::string id = "", std::string name = "", std::string password = "")
        : Person(id, name, password) {}

    std::string getRole() const override { return "Admin"; }
    
    std::string toString() const override {
        return Person::toString() + "," + getRole();
    }
};

// --- Global Data Structures ---
std::vector<Person*> users;
std::vector<Student> students;
std::vector<Teacher> teachers;

// --- File Handling Functions ---
void loadData() {
    for (Person* p : users) {
        delete p;
    }
    users.clear();
    students.clear();
    teachers.clear();

    // Load Admins
    std::ifstream adminFile(USERS_FILE);
    if (adminFile.is_open()) {
        std::string line;
        while (std::getline(adminFile, line)) {
            std::stringstream ss(line);
            std::string id, name, password, role;
            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, password, ',');
            std::getline(ss, role, ',');
            if (role == "Admin") {
                users.push_back(new Admin(id, name, password));
            }
        }
        adminFile.close();
    }

    // Load Students
    std::ifstream studentFile(STUDENTS_FILE);
    if (studentFile.is_open()) {
        std::string line;
        while (std::getline(studentFile, line)) {
            std::stringstream ss(line);
            std::string id, name, password, role, grade, feeAmountStr, feeStatus;
            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, password, ',');
            std::getline(ss, role, ',');
            std::getline(ss, grade, ',');
            std::getline(ss, feeAmountStr, ',');
            std::getline(ss, feeStatus);
            students.push_back(Student(id, name, password, grade, std::stod(feeAmountStr), feeStatus));
            users.push_back(new Student(id, name, password)); // Add to generic user list for login
        }
        studentFile.close();
    }

    // Load Teachers
    std::ifstream teacherFile(TEACHERS_FILE);
    if (teacherFile.is_open()) {
        std::string line;
        while (std::getline(teacherFile, line)) {
            std::stringstream ss(line);
            std::string id, name, password, role, subject;
            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, password, ',');
            std::getline(ss, role, ',');
            std::getline(ss, subject);
            teachers.push_back(Teacher(id, name, password, subject));
            users.push_back(new Teacher(id, name, password)); // Add to generic user list for login
        }
        teacherFile.close();
    }
}

void saveData() {
    // Save Admins
    std::ofstream userFile(USERS_FILE);
    for (const auto& p : users) {
        if (p->getRole() == "Admin") {
            userFile << p->toString() << "\n";
        }
    }
    userFile.close();

    // Save Students
    std::ofstream studentFile(STUDENTS_FILE);
    for (const auto& s : students) {
        studentFile << s.toString() << "\n";
    }
    studentFile.close();

    // Save Teachers
    std::ofstream teacherFile(TEACHERS_FILE);
    for (const auto& t : teachers) {
        teacherFile << t.toString() << "\n";
    }
    teacherFile.close();
}

// --- Menu Functions ---
void adminMenu();
void studentMenu(const Student& currentStudent);
void teacherMenu(const Teacher& currentTeacher);
void login();
void registerNewAccount();

// --- Admin Sub-menus ---
void manageStudents() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin: Manage Students ---" << std::endl;
        std::cout << "1. View All Students" << std::endl;
        std::cout << "2. Search Student" << std::endl;
        std::cout << "3. Delete Student" << std::endl;
        std::cout << "4. Update Student Details" << std::endl;
        std::cout << "5. Back to Main Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "--- All Students ---" << std::endl;
                std::cout << std::left << std::setw(15) << "ID" << std::setw(30) << "Name" << std::setw(15) << "Role" << std::setw(15) << "Grade" << std::setw(15) << "Fee Status" << std::endl;
                std::cout << std::string(90, '-') << std::endl;
                for (const auto& student : students) {
                    student.display();
                }
                std::cout << std::string(90, '-') << std::endl;
                break;
            }
            case 2: {
                std::string id;
                std::cout << "Enter student ID to search: ";
                std::cin >> id;
                auto it = std::find_if(students.begin(), students.end(), [&](const Student& s){ return s.getID() == id; });
                if (it != students.end()) {
                    clearScreen();
                    std::cout << "--- Student Found ---" << std::endl;
                    std::cout << std::left << std::setw(15) << "ID" << std::setw(30) << "Name" << std::setw(15) << "Role" << std::setw(15) << "Grade" << std::setw(15) << "Fee Status" << std::endl;
                    std::cout << std::string(90, '-') << std::endl;
                    it->display();
                    std::cout << std::string(90, '-') << std::endl;
                } else {
                    std::cout << "Student not found." << std::endl;
                }
                break;
            }
            case 3: {
                std::string id;
                std::cout << "Enter student ID to delete: ";
                std::cin >> id;
                auto it = std::remove_if(students.begin(), students.end(), [&](const Student& s){ return s.getID() == id; });
                if (it != students.end()) {
                    students.erase(it, students.end());
                    saveData();
                    std::cout << "Student deleted successfully." << std::endl;
                } else {
                    std::cout << "Student not found." << std::endl;
                }
                break;
            }
            case 4: {
                std::string id, newPassword;
                std::cout << "Enter student ID to update password: ";
                std::cin >> id;
                auto it = std::find_if(students.begin(), students.end(), [&](const Student& s){ return s.getID() == id; });
                if (it != students.end()) {
                    std::cout << "Enter new password: ";
                    std::cin >> newPassword;
                    it->setPassword(newPassword);
                    saveData();
                    std::cout << "Student details updated successfully." << std::endl;
                } else {
                    std::cout << "Student not found." << std::endl;
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

void manageTeachers() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin: Manage Teachers ---" << std::endl;
        std::cout << "1. View All Teachers" << std::endl;
        std::cout << "2. Search Teacher" << std::endl;
        std::cout << "3. Delete Teacher" << std::endl;
        std::cout << "4. Back to Main Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "--- All Teachers ---" << std::endl;
                std::cout << std::left << std::setw(15) << "ID" << std::setw(30) << "Name" << std::setw(15) << "Role" << std::setw(20) << "Subject" << std::endl;
                std::cout << std::string(90, '-') << std::endl;
                for (const auto& teacher : teachers) {
                    teacher.display();
                }
                std::cout << std::string(90, '-') << std::endl;
                break;
            }
            case 2: {
                std::string id;
                std::cout << "Enter teacher ID to search: ";
                std::cin >> id;
                auto it = std::find_if(teachers.begin(), teachers.end(), [&](const Teacher& t){ return t.getID() == id; });
                if (it != teachers.end()) {
                    clearScreen();
                    std::cout << "--- Teacher Found ---" << std::endl;
                    std::cout << std::left << std::setw(15) << "ID" << std::setw(30) << "Name" << std::setw(15) << "Role" << std::setw(20) << "Subject" << std::endl;
                    std::cout << std::string(90, '-') << std::endl;
                    it->display();
                    std::cout << std::string(90, '-') << std::endl;
                } else {
                    std::cout << "Teacher not found." << std::endl;
                }
                break;
            }
            case 3: {
                std::string id;
                std::cout << "Enter teacher ID to delete: ";
                std::cin >> id;
                auto it = std::remove_if(teachers.begin(), teachers.end(), [&](const Teacher& t){ return t.getID() == id; });
                if (it != teachers.end()) {
                    teachers.erase(it, teachers.end());
                    saveData();
                    std::cout << "Teacher deleted successfully." << std::endl;
                } else {
                    std::cout << "Teacher not found." << std::endl;
                }
                break;
            }
            case 4:
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        pause();
    } while (choice != 4);
}

void processAdmission() {
    clearScreen();
    std::string name, password, grade;
    double fee;
    std::cout << "--- Student Admission Process ---" << std::endl;
    std::cout << "Enter Student's Full Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    std::cout << "Enter a temporary password: ";
    std::getline(std::cin, password);
    std::cout << "Enter Grade: ";
    std::getline(std::cin, grade);
    std::cout << "Enter Fee Amount: ";
    std::cin >> fee;

    std::string newID = generateID("STD");
    students.push_back(Student(newID, name, password, grade, fee, "Unpaid"));
    saveData();
    std::cout << "\nAdmission successful! Student ID: " << newID << std::endl;
    pause();
}

void processPayment() {
    clearScreen();
    std::string id;
    std::cout << "--- Process Payment ---" << std::endl;
    std::cout << "Enter Student ID to update fee status: ";
    std::cin >> id;

    auto it = std::find_if(students.begin(), students.end(), [&](const Student& s){ return s.getID() == id; });
    if (it != students.end()) {
        std::cout << "\nStudent Name: " << it->getName() << std::endl;
        std::cout << "Current Fee Status: " << it->getFeeStatus() << std::endl;
        if (it->getFeeStatus() == "Paid") {
            std::cout << "Fee is already paid." << std::endl;
        } else {
            std::cout << "Updating fee status to 'Paid'..." << std::endl;
            it->setFeeStatus("Paid");
            saveData();
            std::cout << "Fee status updated successfully." << std::endl;
        }
    } else {
        std::cout << "Student not found." << std::endl;
    }
    pause();
}

void adminMenu() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin Menu ---" << std::endl;
        std::cout << "1. Process Student Admission" << std::endl;
        std::cout << "2. Process Payment" << std::endl;
        std::cout << "3. Manage Students" << std::endl;
        std::cout << "4. Manage Teachers" << std::endl;
        std::cout << "5. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: processAdmission(); break;
            case 2: processPayment(); break;
            case 3: manageStudents(); break;
            case 4: manageTeachers(); break;
            case 5:
                std::cout << "Logging out..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                pause();
                break;
        }
    } while (choice != 5);
}

// --- Student Menu ---
void studentMenu(const Student& currentStudent) {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Student Menu ---" << std::endl;
        std::cout << "Welcome, " << currentStudent.getName() << "!" << std::endl;
        std::cout << "1. View My Profile" << std::endl;
        std::cout << "2. View My Fee Status" << std::endl;
        std::cout << "3. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                clearScreen();
                std::cout << "--- My Profile ---" << std::endl;
                std::cout << "ID: " << currentStudent.getID() << std::endl;
                std::cout << "Name: " << currentStudent.getName() << std::endl;
                std::cout << "Grade: " << currentStudent.getGrade() << std::endl;
                std::cout << "Fee Amount: $" << std::fixed << std::setprecision(2) << currentStudent.getFeeAmount() << std::endl;
                break;
            case 2:
                clearScreen();
                std::cout << "--- My Fee Status ---" << std::endl;
                std::cout << "Fee Status: " << currentStudent.getFeeStatus() << std::endl;
                break;
            case 3:
                std::cout << "Logging out..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        pause();
    } while (choice != 3);
}

// --- Teacher Menu ---
void teacherMenu(const Teacher& currentTeacher) {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Teacher Menu ---" << std::endl;
        std::cout << "Welcome, " << currentTeacher.getName() << "!" << std::endl;
        std::cout << "1. View My Profile" << std::endl;
        std::cout << "2. View Students by Grade" << std::endl;
        std::cout << "3. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                clearScreen();
                std::cout << "--- My Profile ---" << std::endl;
                std::cout << "ID: " << currentTeacher.getID() << std::endl;
                std::cout << "Name: " << currentTeacher.getName() << std::endl;
                std::cout << "Subject: " << currentTeacher.getSubject() << std::endl;
                break;
            case 2: {
                clearScreen();
                std::string grade;
                std::cout << "Enter grade to view students: ";
                std::cin >> grade;
                std::cout << "\n--- Students in Grade " << grade << " ---" << std::endl;
                std::cout << std::left << std::setw(15) << "ID" << std::setw(30) << "Name" << std::setw(15) << "Fee Status" << std::endl;
                std::cout << std::string(60, '-') << std::endl;
                bool found = false;
                for (const auto& student : students) {
                    if (student.getGrade() == grade) {
                        std::cout << std::left << std::setw(15) << student.getID() << std::setw(30) << student.getName() << std::setw(15) << student.getFeeStatus() << std::endl;
                        found = true;
                    }
                }
                if (!found) {
                    std::cout << "No students found in this grade." << std::endl;
                }
                std::cout << std::string(60, '-') << std::endl;
                break;
            }
            case 3:
                std::cout << "Logging out..." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        pause();
    } while (choice != 3);
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

    for (const auto& student : students) {
        if (student.getID() == id && student.getPassword() == password) {
            std::cout << "\nLogin successful. Welcome, " << student.getName() << "!" << std::endl;
            studentMenu(student);
            return;
        }
    }

    for (const auto& teacher : teachers) {
        if (teacher.getID() == id && teacher.getPassword() == password) {
            std::cout << "\nLogin successful. Welcome, " << teacher.getName() << "!" << std::endl;
            teacherMenu(teacher);
            return;
        }
    }

    for (const auto& user : users) {
        if (user->getID() == id && user->getPassword() == password) {
            std::cout << "\nLogin successful. Welcome, " << user->getName() << "!" << std::endl;
            adminMenu();
            return;
        }
    }

    std::cout << "\nInvalid ID or Password." << std::endl;
    pause();
}

void registerNewAccount() {
    clearScreen();
    std::string name, password, role, subject;
    std::cout << "--- New Account Registration ---" << std::endl;
    std::cout << "Enter Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    std::cout << "Enter Password: ";
    std::cin >> password;
    std::cout << "Register as (Admin/Teacher)? ";
    std::cin >> role;

    if (!role.empty()) {
        role[0] = toupper(role[0]);
        for (size_t i = 1; i < role.length(); ++i) {
            role[i] = tolower(role[i]);
        }
    }
    
    if (role == "Admin") {
        users.push_back(new Admin(generateID("ADM"), name, password));
        saveData();
        std::cout << "\nRegistration successful! Your new ID is: " << users.back()->getID() << std::endl;
    } else if (role == "Teacher") {
        std::cout << "Enter Subject: ";
        std::cin.ignore();
        std::getline(std::cin, subject);
        std::string newID = generateID("TCH");
        teachers.push_back(Teacher(newID, name, password, subject));
        saveData();
        std::cout << "\nRegistration successful! Your new ID is: " << newID << std::endl;
    } else {
        std::cout << "Invalid role. Only 'Admin' or 'Teacher' can register this way." << std::endl;
    }
    pause();
}

// --- Main Program ---
int main() {
    loadData();

    // Create a default admin if no users exist
    bool adminExists = false;
    for(const auto& p : users) {
        if (p->getRole() == "Admin") {
            adminExists = true;
            break;
        }
    }
    if (!adminExists) {
        std::cout << "No Admin account found. Creating a default Admin." << std::endl;
        users.push_back(new Admin("ADMIN123", "Default Admin", "admin123"));
        saveData();
    }

    int choice;
    do {
        clearScreen();
        std::cout << "--- School Management System ---" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Register New Staff/Admin" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: login(); break;
            case 2: registerNewAccount(); break;
            case 3:
                std::cout << "Exiting the program. Thank you!" << std::endl;
                for(Person* p : users) {
                    delete p;
                }
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                pause();
                break;
        }
    } while (choice != 3);

    return 0;
}
