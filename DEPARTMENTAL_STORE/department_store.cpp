#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define MAX 100

/* ---------- PRODUCT CLASS ---------- */
class Product {
public:
    int id;
    string name;
    float price;
    int qty;
};

/* ---------- ARRAY ---------- */
Product productArray[MAX];
int productCount = 0;

/* ---------- LINKED LIST ---------- */
struct Node {
    Product p;
    Node* next;
};
Node* head = NULL;

/* ---------- STACK (CART) ---------- */
Product cart[MAX];
int top = -1;

/* ---------- QUEUE (ORDERS) ---------- */
Product orderQueue[MAX];
int front = 0, rear = -1;

/* ---------- FILE HANDLING ---------- */
void loadFromFile() {
    ifstream in("products.txt");
    productCount = 0;

    while (in >> productArray[productCount].id
              >> productArray[productCount].name
              >> productArray[productCount].price
              >> productArray[productCount].qty) {
        productCount++;
    }
    in.close();
}

void saveToFile() {
    ofstream out("products.txt");
    for (int i = 0; i < productCount; i++) {
        out << productArray[i].id << " "
            << productArray[i].name << " "
            << productArray[i].price << " "
            << productArray[i].qty << endl;
    }
    out.close();
}

/* ---------- ADMIN FUNCTIONS ---------- */
void addProduct() {
    Product p;
    cout << "Enter ID Name Price Quantity: ";
    cin >> p.id >> p.name >> p.price >> p.qty;
    productArray[productCount++] = p;
    saveToFile();
}

void viewProducts() {
    cout << "\nID\tName\tPrice\tQty\n";
    for (int i = 0; i < productCount; i++) {
        cout << productArray[i].id << "\t"
             << productArray[i].name << "\t"
             << productArray[i].price << "\t"
             << productArray[i].qty << endl;
    }
}

void searchProduct() {
    int id;
    cout << "Enter Product ID: ";
    cin >> id;
    for (int i = 0; i < productCount; i++) {
        if (productArray[i].id == id) {
            cout << "Found: " << productArray[i].name
                 << " Price: " << productArray[i].price
                 << " Qty: " << productArray[i].qty << endl;
            return;
        }
    }
    cout << "Product Not Found\n";
}

void updateProduct() {
    int id;
    cout << "Enter Product ID to Update: ";
    cin >> id;
    for (int i = 0; i < productCount; i++) {
        if (productArray[i].id == id) {
            cout << "Enter New Price and Qty: ";
            cin >> productArray[i].price >> productArray[i].qty;
            saveToFile();
            return;
        }
    }
    cout << "Product Not Found\n";
}

void deleteProduct() {
    int id;
    cout << "Enter Product ID to Delete: ";
    cin >> id;
    for (int i = 0; i < productCount; i++) {
        if (productArray[i].id == id) {
            for (int j = i; j < productCount - 1; j++)
                productArray[j] = productArray[j + 1];
            productCount--;
            saveToFile();
            return;
        }
    }
}

/* ---------- STACK (CART) ---------- */
void pushCart(Product p) {
    if (top == MAX - 1) {
        cout << "Cart Full\n";
        return;
    }
    cart[++top] = p;
}

void popCart() {
    if (top == -1) {
        cout << "Cart Empty\n";
        return;
    }
    top--;
}

void viewCart() {
    for (int i = top; i >= 0; i--) {
        cout << cart[i].name << " " << cart[i].price << endl;
    }
}

/* ---------- QUEUE (ORDER) ---------- */
void enqueueOrder(Product p) {
    orderQueue[++rear] = p;
}

void viewOrders() {
    for (int i = front; i <= rear; i++) {
        cout << orderQueue[i].name << " " << orderQueue[i].price << endl;
    }
}

/* ---------- ADMIN MENU ---------- */
void adminMenu() {
    int ch;
    do {
        cout << "\n--- ADMIN MENU ---\n";
        cout << "1.Add Product\n2.View Products\n3.Search Product\n";
        cout << "4.Update Product\n5.Delete Product\n6.Stock Report\n";
        cout << "7.Load File\n8.Save File\n9.View Orders\n10.Logout\n";
        cin >> ch;

        switch (ch) {
            case 1: addProduct(); break;
            case 2: viewProducts(); break;
            case 3: searchProduct(); break;
            case 4: updateProduct(); break;
            case 5: deleteProduct(); break;
            case 6: viewProducts(); break;
            case 7: loadFromFile(); break;
            case 8: saveToFile(); break;
            case 9: viewOrders(); break;
        }
    } while (ch != 10);
}

/* ---------- USER MENU ---------- */
void userMenu() {
    int ch, id;
    do {
        cout << "\n--- USER MENU ---\n";
        cout << "1.View Products\n2.Search Product\n3.Add to Cart\n";
        cout << "4.View Cart\n5.Remove Cart Item\n6.Generate Bill\n";
        cout << "7.Place Order\n8.View Orders\n9.Clear Cart\n10.Logout\n";
        cin >> ch;

        switch (ch) {
            case 1: viewProducts(); break;
            case 2: searchProduct(); break;
            case 3:
                cout << "Enter Product ID: ";
                cin >> id;
                for (int i = 0; i < productCount; i++)
                    if (productArray[i].id == id)
                        pushCart(productArray[i]);
                break;
            case 4: viewCart(); break;
            case 5: popCart(); break;
            case 7:
                while (top != -1)
                    enqueueOrder(cart[top--]);
                break;
            case 8: viewOrders(); break;
            case 9: top = -1; break;
        }
    } while (ch != 10);
}

/* ---------- MAIN ---------- */
int main() {
    int choice;
    loadFromFile();

    do {
        cout << "\n==== DEPARTMENT STORE MANAGEMENT SYSTEM ====\n";
        cout << "1.Admin\n2.User\n3.Exit\n";
        cin >> choice;

        if (choice == 1) adminMenu();
        else if (choice == 2) userMenu();

    } while (choice != 3);

    return 0;
}
