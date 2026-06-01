#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <limits>
#include <sstream>
#include <memory>
#include <chrono>
#include <thread>

// --- Global Data Files ---
const std::string USERS_FILE = "users.txt";
const std::string QUIZZES_FILE = "quizzes.txt";

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

// --- Question Class ---
class Question {
private:
    std::string questionText;
    std::vector<std::string> options;
    char correctAnswer;
public:
    Question(std::string text = "", std::vector<std::string> opts = {}, char answer = 'A')
        : questionText(text), options(opts), correctAnswer(answer) {}

    // Getters
    std::string getQuestionText() const { return questionText; }
    const std::vector<std::string>& getOptions() const { return options; }
    char getCorrectAnswer() const { return correctAnswer; }

    // Display method
    void display() const {
        std::cout << "Q: " << questionText << std::endl;
        char optionChar = 'A';
        for (const auto& opt : options) {
            std::cout << "   " << optionChar << ". " << opt << std::endl;
            optionChar++;
        }
    }

    // Convert object to string for file storage
    std::string toString() const {
        std::ostringstream ss;
        ss << questionText << ";";
        for (const auto& opt : options) {
            ss << opt << ";";
        }
        ss << correctAnswer;
        return ss.str();
    }
    
    // Static method to create a Question object from a string
    static Question fromString(const std::string& str) {
        std::stringstream ss(str);
        std::string part;
        std::getline(ss, part, ';');
        std::string text = part;
        
        std::vector<std::string> opts;
        for (int i = 0; i < 4; ++i) { // Assuming 4 options
            std::getline(ss, part, ';');
            opts.push_back(part);
        }
        
        char answer;
        ss >> answer;
        
        return Question(text, opts, answer);
    }
};

// --- Quiz Class ---
class Quiz {
private:
    std::string quizID;
    std::string topic;
    int durationMinutes;
    std::vector<Question> questions;
public:
    Quiz(std::string id = "", std::string topic = "", int duration = 0, std::vector<Question> q = {})
        : quizID(id), topic(topic), durationMinutes(duration), questions(q) {}

    // Getters
    std::string getID() const { return quizID; }
    std::string getTopic() const { return topic; }
    int getDuration() const { return durationMinutes; }
    const std::vector<Question>& getQuestions() const { return questions; }

    // Add question
    void addQuestion(const Question& q) { questions.push_back(q); }

    // Display method
    void display() const {
        std::cout << "ID: " << quizID << std::endl;
        std::cout << "Topic: " << topic << std::endl;
        std::cout << "Duration: " << durationMinutes << " minutes" << std::endl;
        std::cout << "Questions: " << questions.size() << std::endl;
    }

    // Convert object to string for file storage
    std::string toString() const {
        std::ostringstream ss;
        ss << quizID << "," << topic << "," << durationMinutes << "," << questions.size() << ",";
        for (const auto& q : questions) {
            ss << q.toString() << "|";
        }
        return ss.str();
    }
};

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
    std::vector<std::pair<std::string, double>> quizResults; // pair<quizID, score>
public:
    User(std::string id = "", std::string name = "", std::string password = "")
        : Person(id, name, password) {}

    std::string getRole() const override { return "User"; }
    
    void addResult(const std::string& quizID, double score) {
        quizResults.push_back({quizID, score});
    }

    const std::vector<std::pair<std::string, double>>& getResults() const {
        return quizResults;
    }

    std::string toString() const override {
        std::ostringstream ss;
        ss << Person::toString() << ",User,";
        for (const auto& result : quizResults) {
            ss << result.first << ":" << result.second << ";";
        }
        return ss.str();
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
std::vector<std::unique_ptr<Person>> people;
std::vector<Quiz> quizzes;

// --- File Handling Functions ---
void loadData() {
    people.clear();
    quizzes.clear();

    // Load Users and Admins
    std::ifstream userFile(USERS_FILE);
    if (userFile.is_open()) {
        std::string line;
        while (std::getline(userFile, line)) {
            std::stringstream ss(line);
            std::string id, name, password, role, resultsStr;
            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, password, ',');
            std::getline(ss, role, ',');

            if (role == "Admin") {
                people.push_back(std::make_unique<Admin>(id, name, password));
            } else if (role == "User") {
                std::getline(ss, resultsStr);
                auto user = std::make_unique<User>(id, name, password);
                std::stringstream resultsStream(resultsStr);
                std::string result;
                while(std::getline(resultsStream, result, ';')) {
                    if (!result.empty()) {
                        std::string quizID;
                        std::string scoreStr;
                        std::stringstream resultSS(result);
                        std::getline(resultSS, quizID, ':');
                        std::getline(resultSS, scoreStr);
                        user->addResult(quizID, std::stod(scoreStr));
                    }
                }
                people.push_back(std::move(user));
            }
        }
        userFile.close();
    }

    // Load Quizzes
    std::ifstream quizFile(QUIZZES_FILE);
    if (quizFile.is_open()) {
        std::string line;
        while (std::getline(quizFile, line)) {
            std::stringstream ss(line);
            std::string id, topic, durationStr, numQuestionsStr, questionsStr;
            std::getline(ss, id, ',');
            std::getline(ss, topic, ',');
            std::getline(ss, durationStr, ',');
            std::getline(ss, numQuestionsStr, ',');
            std::getline(ss, questionsStr);

            int duration = std::stoi(durationStr);
            int numQuestions = std::stoi(numQuestionsStr);
            
            std::vector<Question> quizQuestions;
            std::stringstream questionsSS(questionsStr);
            std::string questionStr;
            while(std::getline(questionsSS, questionStr, '|')) {
                if (!questionStr.empty()) {
                    quizQuestions.push_back(Question::fromString(questionStr));
                }
            }
            quizzes.push_back(Quiz(id, topic, duration, quizQuestions));
        }
        quizFile.close();
    }
}

void saveData() {
    // Save Users
    std::ofstream userFile(USERS_FILE);
    for (const auto& p : people) {
        userFile << p->toString() << "\n";
    }
    userFile.close();

    // Save Quizzes
    std::ofstream quizFile(QUIZZES_FILE);
    for (const auto& quiz : quizzes) {
        quizFile << quiz.toString() << "\n";
    }
    quizFile.close();
}

// --- Menu Functions ---
void adminMenu();
void userMenu(User* currentUser);
void login();
void registerNewAccount();

// --- Admin Sub-menus ---
void createQuiz() {
    clearScreen();
    std::cout << "--- Admin: Create a New Quiz ---" << std::endl;
    std::string topic;
    int duration, numQuestions;

    std::cout << "Enter Quiz Topic: ";
    std::cin.ignore();
    std::getline(std::cin, topic);
    std::cout << "Enter Quiz Duration (in minutes): ";
    std::cin >> duration;
    std::cout << "Enter Number of Questions: ";
    std::cin >> numQuestions;
    std::cin.ignore();

    std::vector<Question> newQuestions;
    for (int i = 0; i < numQuestions; ++i) {
        clearScreen();
        std::cout << "--- Question " << i + 1 << " ---" << std::endl;
        std::string questionText;
        std::cout << "Enter Question Text: ";
        std::getline(std::cin, questionText);
        
        std::vector<std::string> options;
        for (char optChar = 'A'; optChar <= 'D'; ++optChar) {
            std::string optText;
            std::cout << "Enter Option " << optChar << ": ";
            std::getline(std::cin, optText);
            options.push_back(optText);
        }
        
        char correctAnswer;
        std::cout << "Enter Correct Answer (A/B/C/D): ";
        std::cin >> correctAnswer;
        std::cin.ignore();
        
        newQuestions.push_back(Question(questionText, options, toupper(correctAnswer)));
    }

    quizzes.push_back(Quiz(generateID("Q"), topic, duration, newQuestions));
    saveData();
    std::cout << "\nQuiz created successfully!" << std::endl;
    pause();
}

void manageQuizzes() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin: Manage Quizzes ---" << std::endl;
        std::cout << "1. Create New Quiz" << std::endl;
        std::cout << "2. View All Quizzes" << std::endl;
        std::cout << "3. View All Quiz Results" << std::endl;
        std::cout << "4. Back to Admin Menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: createQuiz(); break;
            case 2:
                clearScreen();
                std::cout << "--- All Quizzes ---" << std::endl;
                for (const auto& q : quizzes) {
                    std::cout << "----------------------" << std::endl;
                    q.display();
                }
                std::cout << "----------------------" << std::endl;
                pause();
                break;
            case 3:
                clearScreen();
                std::cout << "--- All Quiz Results ---" << std::endl;
                for (const auto& p : people) {
                    if (p->getRole() == "User") {
                        User* user = dynamic_cast<User*>(p.get());
                        std::cout << "Results for User: " << user->getName() << " (" << user->getID() << ")" << std::endl;
                        const auto& results = user->getResults();
                        if (results.empty()) {
                            std::cout << "  No quizzes completed." << std::endl;
                        } else {
                            for (const auto& res : results) {
                                std::cout << "  - Quiz ID: " << res.first << ", Score: " << std::fixed << std::setprecision(2) << res.second << "%" << std::endl;
                            }
                        }
                        std::cout << std::endl;
                    }
                }
                pause();
                break;
            case 4: return;
            default:
                std::cout << "Invalid choice." << std::endl;
                pause();
                break;
        }
    } while (choice != 4);
}

void adminMenu() {
    int choice;
    do {
        clearScreen();
        std::cout << "--- Admin Menu ---" << std::endl;
        std::cout << "1. Manage Quizzes" << std::endl;
        std::cout << "2. View All Users" << std::endl;
        std::cout << "3. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: manageQuizzes(); break;
            case 2:
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
void takeQuiz(User* currentUser) {
    clearScreen();
    std::cout << "--- Available Quizzes ---" << std::endl;
    if (quizzes.empty()) {
        std::cout << "No quizzes available yet. Please check back later." << std::endl;
        pause();
        return;
    }

    std::cout << "Available Quizzes:" << std::endl;
    for (const auto& q : quizzes) {
        std::cout << "  - ID: " << q.getID() << ", Topic: " << q.getTopic() << ", Duration: " << q.getDuration() << " min" << std::endl;
    }

    std::string quizID;
    std::cout << "\nEnter the ID of the quiz you want to take: ";
    std::cin >> quizID;
    std::cin.ignore();

    auto it = std::find_if(quizzes.begin(), quizzes.end(), [&](const Quiz& q){ return q.getID() == quizID; });
    if (it == quizzes.end()) {
        std::cout << "Quiz not found." << std::endl;
        pause();
        return;
    }

    // Payment Process
    std::cout << "\nThis quiz costs $10.00. Would you like to pay to unlock it? (yes/no): ";
    std::string confirmation;
    std::cin >> confirmation;
    if (confirmation != "yes") {
        std::cout << "Payment canceled. Quiz will not be taken." << std::endl;
        pause();
        return;
    }
    std::cout << "Payment successful! The quiz is now unlocked." << std::endl;
    pause();

    // Start Quiz
    clearScreen();
    std::cout << "--- Starting Quiz: " << it->getTopic() << " ---" << std::endl;
    std::cout << "Time Limit: " << it->getDuration() << " minutes" << std::endl;
    std::cout << "Press Enter to begin..." << std::endl;
    std::cin.get();

    int score = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& q : it->getQuestions()) {
        clearScreen();
        q.display();
        std::cout << "Your answer: ";
        char userAnswer;
        std::cin >> userAnswer;
        std::cin.ignore();
        
        if (toupper(userAnswer) == q.getCorrectAnswer()) {
            score++;
            std::cout << "\nCorrect!" << std::endl;
        } else {
            std::cout << "\nIncorrect. The correct answer was " << q.getCorrectAnswer() << "." << std::endl;
        }
        pause();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    double finalScore = (static_cast<double>(score) / it->getQuestions().size()) * 100.0;
    
    clearScreen();
    std::cout << "--- Quiz Finished! ---" << std::endl;
    std::cout << "You scored " << score << " out of " << it->getQuestions().size() << "." << std::endl;
    std::cout << "Your final percentage is: " << std::fixed << std::setprecision(2) << finalScore << "%" << std::endl;
    std::cout << "Time taken: " << elapsed.count() << " seconds." << std::endl;

    currentUser->addResult(quizID, finalScore);
    saveData();
    pause();
}

void viewMyResults(User* currentUser) {
    clearScreen();
    std::cout << "--- Your Results ---" << std::endl;
    const auto& results = currentUser->getResults();
    if (results.empty()) {
        std::cout << "You have not completed any quizzes yet." << std::endl;
    } else {
        std::cout << std::left << std::setw(15) << "Quiz ID" << std::setw(30) << "Quiz Topic" << std::setw(15) << "Score (%)" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
        for (const auto& res : results) {
            std::string quizTopic = "Unknown";
            auto it = std::find_if(quizzes.begin(), quizzes.end(), [&](const Quiz& q){ return q.getID() == res.first; });
            if (it != quizzes.end()) {
                quizTopic = it->getTopic();
            }
            std::cout << std::left << std::setw(15) << res.first << std::setw(30) << quizTopic << std::setw(15) << std::fixed << std::setprecision(2) << res.second << std::endl;
        }
    }
    pause();
}

void userMenu(User* currentUser) {
    int choice;
    do {
        clearScreen();
        std::cout << "--- User Menu ---" << std::endl;
        std::cout << "Welcome, " << currentUser->getName() << "!" << std::endl;
        std::cout << "1. Take a Quiz" << std::endl;
        std::cout << "2. View My Results" << std::endl;
        std::cout << "3. Logout" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: takeQuiz(currentUser); break;
            case 2: viewMyResults(currentUser); break;
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
    
    people.push_back(std::make_unique<User>(generateID("USR"), name, password));
    saveData();
    std::cout << "\nRegistration successful! Your new ID is: " << people.back()->getID() << std::endl;
    pause();
}

// --- Main Program ---
int main() {
    loadData();

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
        std::cout << "--- Quiz Management System ---" << std::endl;
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
