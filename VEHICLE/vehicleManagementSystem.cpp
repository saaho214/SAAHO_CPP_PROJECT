#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib> 

using namespace std;

// ==========================================
// VEHICLE CLASS
// ==========================================
class Vehicle {
public:
    int id;
    string brand;
    string model;
    int year;
    double pricePerDay;
    bool isAvailable;
    bool needsMaintenance;
    string category; // Sedan, SUV, Bike
    vector<string> reviews;

    Vehicle(int i, string b, string m, int y, double p, string cat) {
        id = i;
        brand = b;
        model = m;
        year = y;
        pricePerDay = p;
        isAvailable = true;
        needsMaintenance = false;
        category = cat;
    }
};

// Global data stores
vector<Vehicle> fleet;
double totalSystemRevenue = 0.0;

// ==========================================
// ADMIN SIDE (12 Features)
// ==========================================
class Admin {
public:
    // 1. Add Vehicle
    void addVehicle() {
        int id, year;
        string brand, model, cat;
        double price;
        cout << "\n--- Add New Vehicle ---\n";
        cout << "Enter ID: "; cin >> id;
        cin.ignore();
        cout << "Enter Brand: "; getline(cin, brand);
        cout << "Enter Model: "; getline(cin, model);
        cout << "Enter Category (Sedan/SUV/Bike): "; getline(cin, cat);
        cout << "Enter Year: "; cin >> year;
        cout << "Enter Rental Price Per Day: INR "; cin >> price;
        
        fleet.push_back(Vehicle(id, brand, model, year, price, cat));
        cout << "Vehicle added successfully!\n";
    }

    // 2. Remove Vehicle
    void removeVehicle() {
        int id;
        cout << "\nEnter Vehicle ID to remove: "; cin >> id;
        for (auto it = fleet.begin(); it != fleet.end(); ++it) {
            if (it->id == id) {
                fleet.erase(it);
                cout << "Vehicle removed successfully.\n";
                return;
            }
        }
        cout << "Vehicle ID not found.\n";
    }

    // 3. Update Vehicle Price
    void updatePrice() {
        int id;
        double newPrice;
        cout << "\nEnter Vehicle ID to update price: "; cin >> id;
        for (auto &v : fleet) {
            if (v.id == id) {
                cout << "Current Price: INR " << v.pricePerDay << "\nEnter New Price: INR ";
                cin >> newPrice;
                v.pricePerDay = newPrice;
                cout << "Price updated successfully.\n";
                return;
            }
        }
        cout << "Vehicle not found.\n";
    }

    // 4. View Entire Fleet Status
    void viewAllVehicles() {
        cout << "\n---------------------------------------------------------------------------------\n";
        cout << "ID   | Brand      | Model      | Category | Year | Price/Day | Status    | Repair\n";
        cout << "---------------------------------------------------------------------------------\n";
        for (const auto &v : fleet) {
            cout << setw(4) << v.id << " | "
                 << setw(10) << v.brand << " | "
                 << setw(10) << v.model << " | "
                 << setw(8) << v.category << " | "
                 << setw(4) << v.year << " | INR "
                 << setw(8) << v.pricePerDay << " | "
                 << (v.isAvailable ? "Available" : "Rented   ") << " | "
                 << (v.needsMaintenance ? "YES" : "NO") << "\n";
        }
        cout << "---------------------------------------------------------------------------------\n";
    }

    // 5. Send Vehicle to Maintenance
    void flagMaintenance() {
        int id;
        cout << "\nEnter Vehicle ID for Maintenance: "; cin >> id;
        for (auto &v : fleet) {
            if (v.id == id) {
                v.needsMaintenance = true;
                v.isAvailable = false;
                cout << "Vehicle " << id << " is now marked for maintenance.\n";
                return;
            }
        }
        cout << "Vehicle not found.\n";
    }

    // 6. Release Vehicle from Maintenance
    void releaseMaintenance() {
        int id;
        cout << "\nEnter Vehicle ID to Release from Maintenance: "; cin >> id;
        for (auto &v : fleet) {
            if (v.id == id) {
                v.needsMaintenance = false;
                v.isAvailable = true;
                cout << "Vehicle " << id << " is back online and available.\n";
                return;
            }
        }
        cout << "Vehicle not found.\n";
    }

    // 7. View Earnings Dashboard
    void viewRevenue() {
        cout << "\n========================================\n";
        cout << " TOTAL SYSTEM REVENUE GENERATED: INR " << totalSystemRevenue << "\n";
        cout << "========================================\n";
    }

    // 8. Search Vehicle Globally by ID
    void searchVehicleAdmin() {
        int id;
        cout << "\nEnter Vehicle ID to search: "; cin >> id;
        for (const auto &v : fleet) {
            if (v.id == id) {
                cout << "\n[Match Found] " << v.brand << " " << v.model << " (" << v.year << ") - Category: " << v.category << "\n";
                return;
            }
        }
        cout << "No matching records.\n";
    }

    // 9. View Vehicle Reviews/Feedback
    void viewVehicleFeedback() {
        int id;
        cout << "\nEnter Vehicle ID to see user reviews: "; cin >> id;
        for (const auto &v : fleet) {
            if (v.id == id) {
                cout << "\nReviews for " << v.brand << " " << v.model << ":\n";
                if (v.reviews.empty()) cout << "No reviews yet.\n";
                for (const auto &r : v.reviews) cout << "- " << r << "\n";
                return;
            }
        }
        cout << "Vehicle not found.\n";
    }

    // 10. Generate Fleet Capacity Summary Report
    void generateReport() {
        int total = fleet.size();
        int available = 0, maintenance = 0;
        for (const auto &v : fleet) {
            if (v.isAvailable) available++;
            if (v.needsMaintenance) maintenance++;
        }
        cout << "\n=== SYSTEM SUMMARY REPORT ===\n";
        cout << "Total Registered Vehicles : " << total << "\n";
        cout << "Available for Rent        : " << available << "\n";
        cout << "Under Maintenance         : " << maintenance << "\n";
        cout << "Rented Out / Occupied     : " << (total - available - maintenance) << "\n";
    }

    // 11. [NEW ADMIN FEATURE] Bulk Update Category Price (e.g., Inflation Surge)
    void updateCategoryRate() {
        string targetCat;
        double rateChange;
        cout << "\nEnter target category to adjust (Sedan/SUV/Bike): ";
        cin.ignore();
        getline(cin, targetCat);
        cout << "Enter flat dollar amount adjustment (e.g., 5.50 or -5.50): INR ";
        cin >> rateChange;

        for (auto &v : fleet) {
            if (v.category == targetCat) {
                v.pricePerDay += rateChange;
                if (v.pricePerDay < 0) v.pricePerDay = 0; // Prevent negative metrics
            }
        }
        cout << "All fleet units under " << targetCat << " updated successfully.\n";
    }

    // 12. [NEW ADMIN FEATURE] Filter & Search Fleet by Production Year
    void searchByYear() {
        int targetYear;
        cout << "\nEnter Production Year to inventory audit: "; cin >> targetYear;
        cout << "\n--- Vehicles Manufactured in " << targetYear << " ---\n";
        bool found = false;
        for (const auto &v : fleet) {
            if (v.year == targetYear) {
                cout << "ID: " << v.id << " | " << v.brand << " " << v.model << " | Rate: INR " << v.pricePerDay << "/day\n";
                found = true;
            }
        }
        if (!found) cout << "No vehicles found matching production year " << targetYear << ".\n";
    }
};

// ==========================================
// USER SIDE (12 Features)
// ==========================================
class User {
private:
    // Helper function to print dynamic receipt invoice
    void generateReceipt(const Vehicle &v, int days, double basePrice, double finalBill, double discount) {
        cout << "\n=============================================\n";
        cout << "            VEHICLE RENTAL RECEIPT           \n";
        cout << "=============================================\n";
        cout << " Transaction ID: TXN-" << (rand() % 90000 + 10000) << "\n";
        cout << " Status        : PAID / RESERVED\n";
        cout << "---------------------------------------------\n";
        cout << " Vehicle Details:\n";
        cout << "   Unit ID     : " << v.id << "\n";
        cout << "   Model       : " << v.brand << " " << v.model << " (" << v.year << ")\n";
        cout << "   Classification: " << v.category << "\n";
        cout << "---------------------------------------------\n";
        cout << " Rental Period : " << days << " Day(s)\n";
        cout << " Base Rate     : INR " << v.pricePerDay << " / day\n";
        cout << " Subtotal Cost : INR " << basePrice << "\n";
        if (discount > 0) {
            cout << " Applied Promo : -INR " << discount << "\n";
        }
        cout << "---------------------------------------------\n";
        cout << " TOTAL AMOUNT PAID: INR " << finalBill << "\n";
        cout << "=============================================\n";
        cout << "   Thank you for choosing us! Drive safe.     \n";
        cout << "=============================================\n\n";
    }

public:
    // 1. View Only Available Vehicles
    void viewAvailableVehicles() {
        cout << "\n--- Available Vehicles for Rent ---\n";
        bool found = false;
        for (const auto &v : fleet) {
            if (v.isAvailable && !v.needsMaintenance) {
                cout << "ID: " << v.id << " | " << v.brand << " " << v.model << " [" << v.category << "] | Price: INR " << v.pricePerDay << "/day\n";
                found = true;
            }
        }
        if (!found) cout << "No vehicles are currently available.\n";
    }

    // 2. Rent/Book a Vehicle (Integrated with Receipt and Promotion Engine)
    void rentVehicle() {
        int id, days;
        string coupon;
        cout << "\nEnter Vehicle ID you want to rent: "; cin >> id;
        for (auto &v : fleet) {
            if (v.id == id) {
                if (!v.isAvailable || v.needsMaintenance) {
                    cout << "Sorry, this vehicle is currently unavailable.\n";
                    return;
                }
                cout << "Enter number of days to rent: "; cin >> days;
                cout << "Enter Coupon Code (Enter 'NONE' if you don't have one): ";
                cin.ignore();
                getline(cin, coupon);

                double basePrice = v.pricePerDay * days;
                double discount = 0.0;

                if (coupon == "SAVE10") discount = 10.0;
                else if (coupon == "SUPER50") discount = 50.0;

                double bill = basePrice - discount;
                if (bill < 0) bill = 0; // Avoid billing anomalies

                v.isAvailable = false;
                totalSystemRevenue += bill;

                // Fire receipt display module
                generateReceipt(v, days, basePrice, bill, discount);
                return;
            }
        }
        cout << "Vehicle ID not found.\n";
    }

    // 3. Return a Vehicle
    void returnVehicle() {
        int id;
        cout << "\nEnter Vehicle ID you wish to return: "; cin >> id;
        for (auto &v : fleet) {
            if (v.id == id) {
                if (v.isAvailable) {
                    cout << "This vehicle is already sitting in our lot.\n";
                    return;
                }
                v.isAvailable = true;
                cout << "Vehicle returned successfully. Thank you!\n";
                return;
            }
        }
        cout << "Invalid Vehicle ID.\n";
    }

    // 4. Calculate Rental Estimation Quote
    void estimateFare() {
        int id, days;
        cout << "\nEnter Vehicle ID for Quote Estimation: "; cin >> id;
        for (const auto &v : fleet) {
            if (v.id == id) {
                cout << "Enter intended rental duration (days): "; cin >> days;
                cout << "Estimated Cost for " << days << " days: INR " << (v.pricePerDay * days) << "\n";
                return;
            }
        }
        cout << "Vehicle not found.\n";
    }

    // 5. Filter Vehicles by Category
    void filterByCategory() {
        string targetCat;
        cout << "\nEnter Category to filter (Sedan, SUV, Bike): ";
        cin.ignore();
        getline(cin, targetCat);
        
        cout << "\n--- Matching Results ---\n";
        for (const auto &v : fleet) {
            if (v.category == targetCat) {
                cout << "ID: " << v.id << " | " << v.brand << " " << v.model << " | INR " << v.pricePerDay << "/day [" << (v.isAvailable ? "Available" : "Rented") << "]\n";
            }
        }
    }

    // 6. Search Vehicle by Brand Name
    void searchByBrand() {
        string brandName;
        cout << "\nEnter Brand Name: ";
        cin.ignore();
        getline(cin, brandName);
        
        for (const auto &v : fleet) {
            if (v.brand == brandName) {
                cout << "Found -> ID: " << v.id << " | " << v.brand << " " << v.model << " (INR " << v.pricePerDay << "/day)\n";
            }
        }
    }

    // 7. Leave Feedback / Review
    void leaveReview() {
        int id;
        string reviewText;
        cout << "\nEnter Vehicle ID to drop a review: "; cin >> id;
        for (auto &v : fleet) {
            if (v.id == id) {
                cout << "Write your review: ";
                cin.ignore();
                getline(cin, reviewText);
                v.reviews.push_back(reviewText);
                cout << "Thank you for your feedback!\n";
                return;
            }
        }
        cout << "Vehicle not found.\n";
    }

    // 8. Sort Vehicles by Rental Price (Low to High)
    void sortByPrice() {
        vector<Vehicle> sortedFleet = fleet;
        sort(sortedFleet.begin(), sortedFleet.end(), [](const Vehicle &a, const Vehicle &b) {
            return a.pricePerDay < b.pricePerDay;
        });

        cout << "\n--- Vehicles Sorted by Price (Low to High) ---\n";
        for (const auto &v : sortedFleet) {
            cout << "INR " << v.pricePerDay << "/day - " << v.brand << " " << v.model << " (ID: " << v.id << ")\n";
        }
    }

    // 9. View Vehicle Specifications Detail
    void viewSpecs() {
        int id;
        cout << "\nEnter Vehicle ID for full specs: "; cin >> id;
        for (const auto &v : fleet) {
            if (v.id == id) {
                cout << "\n=== Specifications Dynamic Card ===\n";
                cout << "ID            : " << v.id << "\n";
                cout << "Make & Model  : " << v.brand << " " << v.model << "\n";
                cout << "Year Model    : " << v.year << "\n";
                cout << "Classification: " << v.category << "\n";
                cout << "Rate Value    : INR " << v.pricePerDay << " per day\n";
                return;
            }
        }
        cout << "Vehicle not found.\n";
    }

    // 10. Check Real-Time Availability of a Specific Vehicle
    void checkInstantAvailability() {
        int id;
        cout << "\nEnter specific Vehicle ID to check status: "; cin >> id;
        for (const auto &v : fleet) {
            if (v.id == id) {
                if (v.needsMaintenance) cout << "Status: Currently in garage under maintenance.\n";
                else if (!v.isAvailable) cout << "Status: Currently Rented out to another client.\n";
                else cout << "Status: Ready and waiting in the showroom lot!\n";
                return;
            }
        }
        cout << "Vehicle track records do not exist.\n";
    }

    // 11. [NEW USER FEATURE] Check Active Promotions Info
    void viewPromotions() {
        cout << "\n=== AVAILABLE RENTAL COUPONS ===\n";
        cout << "1. Code: SAVE10    -> Get a flat INR 10 off your total bill.\n";
        cout << "2. Code: SUPER50   -> Get a massive INR 50 off long-term rentals.\n";
        cout << "Apply these codes during the checkout phase under the booking window.\n";
    }

    // 12. [NEW USER FEATURE] Show Most Budget-Friendly Option Available
    void showCheapestVehicle() {
        double minPrice = 999999.0;
        int targetIdx = -1;

        for (size_t i = 0; i < fleet.size(); i++) {
            if (fleet[i].isAvailable && !fleet[i].needsMaintenance && fleet[i].pricePerDay < minPrice) {
                minPrice = fleet[i].pricePerDay;
                targetIdx = i;
            }
        }

        if (targetIdx != -1) {
            cout << "\n--- Current Most Affordable Deal ---\n";
            cout << "Vehicle: " << fleet[targetIdx].brand << " " << fleet[targetIdx].model 
                 << " runs at only INR " << fleet[targetIdx].pricePerDay << " per day! [ID: " << fleet[targetIdx].id << "]\n";
        } else {
            cout << "\nNo operational vehicles are sitting available right now.\n";
        }
    }
};

// ==========================================
// SEED DATA & ENTRYPOINT
// ==========================================
void seedData() {
    fleet.push_back(Vehicle(101, "Toyota", "Camry", 2022, 45.0, "Sedan"));
    fleet.push_back(Vehicle(102, "Ford", "Explorer", 2021, 75.0, "SUV"));
    fleet.push_back(Vehicle(103, "Honda", "CBR", 2023, 30.0, "Bike"));
    fleet.push_back(Vehicle(104, "Tesla", "Model Y", 2024, 110.0, "SUV"));
}

int main() {
    srand(time(0)); // Seed random for Receipt IDs
    seedData();
    Admin adminObj;
    User userObj;
    int choice = 0;

    while (choice != 3) {
        cout << "\n====================================";
        cout << "\n   VEHICLE MANAGEMENT SYSTEM UI";
        cout << "\n====================================";
        cout << "\n1. Admin Dashboard Interface";
        cout << "\n2. User Interface Marketplace";
        cout << "\n3. Exit System";
        cout << "\nSelect Access Node: ";
        cin >> choice;

        if (choice == 1) {
            int adminChoice = 0;
            while (adminChoice != 13) {
                cout << "\n--- Admin Command Portal ---";
                cout << "\n1. Add Fleet Unit         2. Remove Fleet Unit";
                cout << "\n3. Update Daily Price     4. Output All Inventory";
                cout << "\n5. Send to Repair         6. Release from Repair";
                cout << "\n7. View Gross Revenue     8. Global Registry Search";
                cout << "\n9. Read User Reviews     10. System Metric Report";
                cout << "\n11. Bulk Category Adjust 12. Search Fleet by Year";
                cout << "\n13. Logout to Main Menu";
                cout << "\nExecute Action (1-13): ";
                cin >> adminChoice;

                switch (adminChoice) {
                    case 1: adminObj.addVehicle(); break;
                    case 2: adminObj.removeVehicle(); break;
                    case 3: adminObj.updatePrice(); break;
                    case 4: adminObj.viewAllVehicles(); break;
                    case 5: adminObj.flagMaintenance(); break;
                    case 6: adminObj.releaseMaintenance(); break;
                    case 7: adminObj.viewRevenue(); break;
                    case 8: adminObj.searchVehicleAdmin(); break;
                    case 9: adminObj.viewVehicleFeedback(); break;
                    case 10: adminObj.generateReport(); break;
                    case 11: adminObj.updateCategoryRate(); break;
                    case 12: adminObj.searchByYear(); break;
                }
            }
        } 
        else if (choice == 2) {
            int userChoice = 0;
            while (userChoice != 13) {
                cout << "\n--- User Rental Marketplace ---";
                cout << "\n1. View Available Cars    2. Rent Vehicle & Get Receipt";
                cout << "\n3. Return Vehicle         4. Get Cost Estimation";
                cout << "\n5. Filter by Body Type    6. Search Brand Name";
                cout << "\n7. Leave Review Log       8. Sort by Price (Asc)";
                cout << "\n9. Read Technical Specs  10. Instant Status Check";
                cout << "\n11. View Active Coupons  12. Show Best Bargain Unit";
                cout << "\n13. Return to Main Menu";
                cout << "\nExecute Action (1-13): ";
                cin >> userChoice;

                switch (userChoice) {
                    case 1: userObj.viewAvailableVehicles(); break;
                    case 2: userObj.rentVehicle(); break;
                    case 3: userObj.returnVehicle(); break;
                    case 4: userObj.estimateFare(); break;
                    case 5: userObj.filterByCategory(); break;
                    case 6: userObj.searchByBrand(); break;
                    case 7: userObj.leaveReview(); break;
                    case 8: userObj.sortByPrice(); break;
                    case 9: userObj.viewSpecs(); break;
                    case 10: userObj.checkInstantAvailability(); break;
                    case 11: userObj.viewPromotions(); break;
                    case 12: userObj.showCheapestVehicle(); break;
                }
            }
        }
    }
    cout << "\nSystem safe shutdown finalized. Goodbye.\n";
    return 0;
}