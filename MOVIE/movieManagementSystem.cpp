#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <limits> // Required for numeric_limits
#include <sstream>

// --- Global Data Files ---
const std::string USERS_FILE = "users.txt";
const std::string MOVIES_FILE = "movies.txt";

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

// --- Movie Class ---
class Movie {
private:
    std::string title;
    std::string genre;
    std::string director;
    int releaseYear;
    int availableCopies;
public:
    // Default constructor
    Movie() : title(""), genre(""), director(""), releaseYear(0), availableCopies(0) {}

    // Parameterized constructor
    Movie(std::string title, std::string genre, std::string director, int year, int copies)
        : title(title), genre(genre), director(director), releaseYear(year), availableCopies(copies) {}

    // Getters
    std::string getTitle() const { return title; }
    std::string getGenre() const { return genre; }
    std::string getDirector() const { return director; }
    int getReleaseYear() const { return releaseYear; }
    int getAvailableCopies() const { return availableCopies; }

    // Setters
    void setAvailableCopies(int copies) { this->availableCopies = copies; }

    // Display method
    void display() const {
        std::cout << std::left << std::setw(35) << title
                  << std::setw(20) << genre
                  << std::setw(25) << director
                  << std::setw(15) << releaseYear
                  << std::setw(15) << availableCopies << std::endl;
    }

    // Convert object to a string for file storage
    std::string toString() const {
        std::ostringstream ss;
        ss << title << "," << genre << "," << director << "," << releaseYear << "," << availableCopies;
        return ss.str();
    }
};

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
    virtual std::string toString() const {
        return id + "," + name + "," + password;
    }
};

// --- User Class ---
class User : public Person {
private:
    std::vector<std::string> borrowedMovies;
public:
    User(std::string id = "", std::string name = "", std::string password = "")
        : Person(id, name, password) {}

    std::string getRole() const override { return "User"; }
    const std::vector<std::string>& getBorrowedMovies() const { return borrowedMovies; }
    
    void addBorrowedMovie(const std::string& movieTitle) { borrowedMovies.push_back(movieTitle); }
    void removeBorrowedMovie(const std::string& movieTitle) {
        auto it = std::find(borrowedMovies.begin(), borrowedMovies.end(), movieTitle);
        if (it != borrowedMovies.end()) {
            borrowedMovies.erase(it);
        }
    }

    std::string toString() const override {
        std::string base = Person::toString() + ",User,";
        for (const auto& movie : borrowedMovies) {
            base += movie + ";";
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

// --- Global Data Structures ---
std::vector<Person*> people; // Stores both Users and Admins
std::vector<Movie> movies;

// --- File Handling Functions ---
void loadData() {
    // Load Users and Admins
    for (Person* p : people) {
        delete p;
    }
    people.clear();
    std::ifstream userFile(USERS_FILE);
    if (userFile.is_open()) {
        std::string line;
        while (std::getline(userFile, line)) {
            std::stringstream ss(line);
            std::string id, name, password, role, borrowedStr;
            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, password, ',');
            std::getline(ss, role, ',');
            std::getline(ss, borrowedStr);

            if (role == "Admin") {
                people.push_back(new Admin(id, name, password));
            } else {
                User* user = new User(id, name, password);
                std::stringstream borrowedStream(borrowedStr);
                std::string movieTitle;
                while(std::getline(borrowedStream, movieTitle, ';')) {
                    if (!movieTitle.empty()) {
                        user->addBorrowedMovie(movieTitle);
                    }
                }
                people.push_back(user);
            }
        }
        userFile.close();
    }

    // Load Movies
    movies.clear();
    std::ifstream movieFile(MOVIES_FILE);
    if (movieFile.is_open()) {
        std::string line;
        while (std::getline(movieFile, line)) {
            std::stringstream ss(line);
            std::string title, genre, director, yearStr, copiesStr;
            std::getline(ss, title, ',');
            std::getline(ss, genre, ',');
            std::getline(ss, director, ',');
            std::getline(ss, yearStr, ',');
            std::getline(ss, copiesStr);
            movies.push_back(Movie(title, genre, director, std::stoi(yearStr), std::stoi(copiesStr)));
        }
        movieFile.close();
    }
}

void saveData() {
    // Save Users
    std::ofstream userFile(USERS_FILE);
    for (const auto& p : people) {
        userFile << p->toString() << "\n";
    }
    userFile.close();

    // Save Movies
    std::ofstream movieFile(MOVIES_FILE);
    for (const auto& movie : movies) {
        movieFile << movie.toString() << "\n";
    }
    movieFile.close();
}

// --- Menu Functions ---
void adminMenu();
void userMenu(User* currentUser);
void login();
void registerNewAccount();

// --- Admin Sub-menus ---
void manageMovies() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin Movie Management ---" << std::endl;
        std::cout << "1. Add New Movie" << std::endl;
        std::cout << "2. View All Movies" << std::endl;
        std::cout << "3. Update Movie Details" << std::endl;
        std::cout << "4. Delete Movie" << std::endl;
        std::cout << "5. Search Movie" << std::endl;
        std::cout << "6. Back to Main Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string title, genre, director;
                int year, copies;
                std::cout << "Enter Movie Title: ";
                std::cin.ignore();
                std::getline(std::cin, title);
                std::cout << "Enter Genre: ";
                std::getline(std::cin, genre);
                std::cout << "Enter Director: ";
                std::getline(std::cin, director);
                std::cout << "Enter Release Year: ";
                std::cin >> year;
                std::cout << "Enter Number of Copies: ";
                std::cin >> copies;
                movies.push_back(Movie(title, genre, director, year, copies));
                saveData();
                std::cout << "Movie added successfully." << std::endl;
                break;
            }
            case 2:
                clearScreen();
                std::cout << "--- All Movies ---" << std::endl;
                std::cout << std::left << std::setw(35) << "Title" << std::setw(20) << "Genre" << std::setw(25) << "Director" << std::setw(15) << "Release Year" << std::setw(15) << "Available Copies" << std::endl;
                std::cout << std::string(110, '-') << std::endl;
                for (const auto& movie : movies) {
                    movie.display();
                }
                std::cout << std::string(110, '-') << std::endl;
                break;
            case 3: {
                std::string title;
                std::cout << "Enter movie title to update: ";
                std::cin.ignore();
                std::getline(std::cin, title);
                auto it = std::find_if(movies.begin(), movies.end(), [&](const Movie& m){ return m.getTitle() == title; });
                if (it != movies.end()) {
                    int newCopies;
                    std::cout << "Enter new number of available copies: ";
                    std::cin >> newCopies;
                    it->setAvailableCopies(newCopies);
                    saveData();
                    std::cout << "Movie details updated successfully." << std::endl;
                } else {
                    std::cout << "Movie not found." << std::endl;
                }
                break;
            }
            case 4: {
                std::string title;
                std::cout << "Enter movie title to delete: ";
                std::cin.ignore();
                std::getline(std::cin, title);
                auto it = std::remove_if(movies.begin(), movies.end(), [&](const Movie& m){ return m.getTitle() == title; });
                if (it != movies.end()) {
                    movies.erase(it, movies.end());
                    saveData();
                    std::cout << "Movie deleted successfully." << std::endl;
                } else {
                    std::cout << "Movie not found." << std::endl;
                }
                break;
            }
            case 5: {
                std::string title;
                std::cout << "Enter movie title to search: ";
                std::cin.ignore();
                std::getline(std::cin, title);
                auto it = std::find_if(movies.begin(), movies.end(), [&](const Movie& m){ return m.getTitle() == title; });
                if (it != movies.end()) {
                    clearScreen();
                    std::cout << "--- Movie Found ---" << std::endl;
                    std::cout << std::left << std::setw(35) << "Title" << std::setw(20) << "Genre" << std::setw(25) << "Director" << std::setw(15) << "Release Year" << std::setw(15) << "Available Copies" << std::endl;
                    std::cout << std::string(110, '-') << std::endl;
                    it->display();
                    std::cout << std::string(110, '-') << std::endl;
                } else {
                    std::cout << "Movie not found." << std::endl;
                }
                break;
            }
            case 6:
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        pause();
    } while (choice != 6);
}

void adminMenu() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin Menu ---" << std::endl;
        std::cout << "1. Manage Movies" << std::endl;
        std::cout << "2. View All Users" << std::endl;
        std::cout << "3. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: manageMovies(); break;
            case 2:
                clearScreen();
                std::cout << "--- All System Users ---" << std::endl;
                std::cout << std::left << std::setw(15) << "ID" << std::setw(25) << "Name" << std::setw(15) << "Role" << std::endl;
                std::cout << std::string(55, '-') << std::endl;
                for (const auto& p : people) {
                    std::cout << std::left << std::setw(15) << p->getID() << std::setw(25) << p->getName() << std::setw(15) << p->getRole() << std::endl;
                }
                std::cout << std::string(55, '-') << std::endl;
                pause();
                break;
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

// --- User Sub-menus ---
void viewAllMovies() {
    clearScreen();
    std::cout << "--- All Movies ---" << std::endl;
    std::cout << std::left << std::setw(35) << "Title" << std::setw(20) << "Genre" << std::setw(25) << "Director" << std::setw(15) << "Release Year" << std::setw(15) << "Available Copies" << std::endl;
    std::cout << std::string(110, '-') << std::endl;
    for (const auto& movie : movies) {
        movie.display();
    }
    std::cout << std::string(110, '-') << std::endl;
}

void borrowMovie(User* currentUser) {
    clearScreen();
    std::cout << "--- Borrow a Movie ---" << std::endl;
    viewAllMovies();
    std::string title;
    std::cout << "Enter the title of the movie you wish to borrow: ";
    std::cin.ignore();
    std::getline(std::cin, title);

    auto it = std::find_if(movies.begin(), movies.end(), [&](const Movie& m){ return m.getTitle() == title; });
    if (it != movies.end() && it->getAvailableCopies() > 0) {
        it->setAvailableCopies(it->getAvailableCopies() - 1);
        currentUser->addBorrowedMovie(title);
        saveData();
        std::cout << "You have successfully borrowed '" << title << "'." << std::endl;
    } else if (it != movies.end() && it->getAvailableCopies() == 0) {
        std::cout << "Sorry, all copies of this movie are currently borrowed." << std::endl;
    } else {
        std::cout << "Movie not found." << std::endl;
    }
    pause();
}

void returnMovie(User* currentUser) {
    clearScreen();
    std::cout << "--- Return a Movie ---" << std::endl;
    const auto& borrowed = currentUser->getBorrowedMovies();
    if (borrowed.empty()) {
        std::cout << "You have no movies to return." << std::endl;
        pause();
        return;
    }

    std::cout << "Your currently borrowed movies:" << std::endl;
    for (const auto& title : borrowed) {
        std::cout << "- " << title << std::endl;
    }
    std::cout << "\nEnter the title of the movie you wish to return: ";
    std::string title;
    std::cin.ignore();
    std::getline(std::cin, title);

    if (std::find(borrowed.begin(), borrowed.end(), title) != borrowed.end()) {
        auto movieIt = std::find_if(movies.begin(), movies.end(), [&](const Movie& m){ return m.getTitle() == title; });
        if (movieIt != movies.end()) {
            movieIt->setAvailableCopies(movieIt->getAvailableCopies() + 1);
            currentUser->removeBorrowedMovie(title);
            saveData();
            std::cout << "You have successfully returned '" << title << "'." << std::endl;
        } else {
            std::cout << "Error: Movie not found in database. Contact an administrator." << std::endl;
        }
    } else {
        std::cout << "You did not borrow this movie." << std::endl;
    }
    pause();
}

void userMenu(User* currentUser) {
    int choice;
    do {
        clearScreen();
        std::cout << "--- User Menu ---" << std::endl;
        std::cout << "Welcome, " << currentUser->getName() << "!" << std::endl;
        std::cout << "1. View All Movies" << std::endl;
        std::cout << "2. Borrow a Movie" << std::endl;
        std::cout << "3. Return a Movie" << std::endl;
        std::cout << "4. View My Borrowed Movies" << std::endl;
        std::cout << "5. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: viewAllMovies(); pause(); break;
            case 2: borrowMovie(currentUser); break;
            case 3: returnMovie(currentUser); break;
            case 4:
                clearScreen();
                std::cout << "--- Your Borrowed Movies ---" << std::endl;
                if (currentUser->getBorrowedMovies().empty()) {
                    std::cout << "You have no movies currently borrowed." << std::endl;
                } else {
                    for (const auto& title : currentUser->getBorrowedMovies()) {
                        std::cout << "- " << title << std::endl;
                    }
                }
                pause();
                break;
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

// --- Login & Registration ---
void login() {
    clearScreen();
    std::string id, password;
    std::cout << "--- Login ---" << std::endl;
    std::cout << "Enter ID: ";
    std::cin >> id;
    std::cout << "Enter Password: ";
    std::cin >> password;

    for (Person* p : people) {
        if (p->getID() == id && p->getPassword() == password) {
            std::cout << "\nLogin successful. Welcome, " << p->getName() << "!" << std::endl;
            if (dynamic_cast<Admin*>(p)) {
                adminMenu();
            } else {
                userMenu(dynamic_cast<User*>(p));
            }
            return;
        }
    }
    std::cout << "\nInvalid ID or Password." << std::endl;
    pause();
}

void registerNewAccount() {
    clearScreen();
    std::string name, password, role;
    std::cout << "--- New Account Registration ---" << std::endl;
    std::cout << "Enter Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    std::cout << "Enter Password: ";
    std::cin >> password;
    std::cout << "Register as (User/Admin)? ";
    std::cin >> role;

    if (!role.empty()) {
        role[0] = toupper(role[0]);
        for (size_t i = 1; i < role.length(); ++i) {
            role[i] = tolower(role[i]);
        }
    }
    
    if (role == "Admin") {
        people.push_back(new Admin(generateID("ADM"), name, password));
    } else {
        people.push_back(new User(generateID("USR"), name, password));
    }
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
        people.push_back(new Admin("ADMIN123", "Default Admin", "admin123"));
        saveData();
    }

    int choice;
    do {
        clearScreen();
        std::cout << "--- Movie Management System ---" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Register New Account" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: login(); break;
            case 2: registerNewAccount(); break;
            case 3:
                std::cout << "Exiting the program. Thank you!" << std::endl;
                for(Person* p : people) {
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
