#include <iostream>
#include <libpq-fe.h>
#include <string>

using namespace std;

const string DB_CONN = "host=localhost port=5432 dbname=bank user=postgres password=your_new_password";

void create_account() {
    string name, type, address;
    long mobile;
    float balance;

    cout << "Enter Your Name: ";
    cin >> name;
    cout << "Which Type Of Account (Current/Saving): ";
    cin >> type;
    cout << "Enter Mobile Number: ";
    cin >> mobile;
    cin.ignore();
    cout << "Enter Address: ";
    getline(cin, address);
    cout << "Enter Amount to Deposit: ";
    cin >> balance;

    PGconn *conn = PQconnectdb(DB_CONN.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Connection Error: " << PQerrorMessage(conn);
        PQfinish(conn);
        return;
    }

    const char *last_query = "SELECT customer_id, account_number FROM bank ORDER BY customer_id DESC LIMIT 1;";
    PGresult *last_res = PQexec(conn, last_query);

    int last_customer_id = 3154;
    long last_account_number = 867578622952;

    if (PQresultStatus(last_res) == PGRES_TUPLES_OK && PQntuples(last_res) == 1) {
        last_customer_id = stoi(PQgetvalue(last_res, 0, 0));
        last_account_number = stol(PQgetvalue(last_res, 0, 1));
    }
    PQclear(last_res);

    int new_customer_id = last_customer_id + 1;
    long new_account_number = last_account_number + 1;

    string insert_query = "INSERT INTO bank (customer_id, account_number, customer_name, account_type, address, mobile, balance) VALUES (" +
                          to_string(new_customer_id) + ", " + to_string(new_account_number) + ", '" + name + "', '" + type + "', '" +
                          address + "', " + to_string(mobile) + ", " + to_string(balance) + ");";

    PGresult *res = PQexec(conn, insert_query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Insert Failed: " << PQerrorMessage(conn);
    } else {
        cout << "Account created successfully. Customer ID: " << new_customer_id << "\n";
    }

    PQclear(res);
    PQfinish(conn);
}

void check_balance_by_id() {
    int id;
    cout << "Enter Customer ID to check balance: ";
    cin >> id;

    PGconn *conn = PQconnectdb(DB_CONN.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "DB Error: " << PQerrorMessage(conn);
        PQfinish(conn);
        return;
    }

    string query = "SELECT customer_name, balance FROM bank WHERE customer_id = " + to_string(id) + ";";
    PGresult *res = PQexec(conn, query.c_str());

    if (PQntuples(res) == 1) {
        cout << "Hello " << PQgetvalue(res, 0, 0)
             << ", your balance is: ₹" << PQgetvalue(res, 0, 1) << "\n";
    } else {
        cout << "Customer not found.\n";
    }

    PQclear(res);
    PQfinish(conn);
}

void change_mobile_by_id() {
    int id;
    long new_mobile;
    cout << "Enter Customer ID to change mobile number: ";
    cin >> id;
    cout << "Enter New Mobile Number: ";
    cin >> new_mobile;

    PGconn *conn = PQconnectdb(DB_CONN.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Connection Failed: " << PQerrorMessage(conn);
        PQfinish(conn);
        return;
    }

    string query = "UPDATE bank SET mobile = " + to_string(new_mobile) +
                   " WHERE customer_id = " + to_string(id) + ";";
    PGresult *res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) == PGRES_COMMAND_OK && string(PQcmdTuples(res)) != "0") {
        cout << "Mobile number updated successfully.\n";
    } else {
        cout << "Update failed. Customer ID not found.\n";
    }

    PQclear(res);
    PQfinish(conn);
}
void transfer_money() {
    int sender_id, receiver_id;
    float amount;
    cout << "Enter Sender Customer ID: ";
    cin >> sender_id;
    cout << "Enter Receiver Customer ID: ";
    cin >> receiver_id;
    cout << "Enter Amount to Transfer: ";
    cin >> amount;

    PGconn *conn = PQconnectdb(DB_CONN.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Connection Failed: " << PQerrorMessage(conn);
        PQfinish(conn);
        return;
    }

    string sender_query = "SELECT balance FROM bank WHERE customer_id = " + to_string(sender_id) + ";";
    PGresult *res_sender = PQexec(conn, sender_query.c_str());

    if (PQntuples(res_sender) != 1) {
        cout << "Sender not found.\n";
        PQclear(res_sender);
        PQfinish(conn);
        return;
    }

    string receiver_query = "SELECT balance FROM bank WHERE customer_id = " + to_string(receiver_id) + ";";
    PGresult *res_receiver = PQexec(conn, receiver_query.c_str());

    if (PQntuples(res_receiver) != 1) {
        cout << "Receiver not found.\n";
        PQclear(res_sender);
        PQclear(res_receiver);
        PQfinish(conn);
        return;
    }

    float sender_balance = stof(PQgetvalue(res_sender, 0, 0));
    PQclear(res_sender);
    PQclear(res_receiver);

    if (sender_balance < amount) {
        cout << "Insufficient balance.\n";
        PQfinish(conn);
        return;
    }

    PQexec(conn, "BEGIN");

    string debit = "UPDATE bank SET balance = balance - " + to_string(amount) +
                   " WHERE customer_id = " + to_string(sender_id) + ";";
    string credit = "UPDATE bank SET balance = balance + " + to_string(amount) +
                    " WHERE customer_id = " + to_string(receiver_id) + ";";

    PGresult *res1 = PQexec(conn, debit.c_str());
    PGresult *res2 = PQexec(conn, credit.c_str());

    if (PQresultStatus(res1) == PGRES_COMMAND_OK && PQresultStatus(res2) == PGRES_COMMAND_OK) {
        PQexec(conn, "COMMIT");
        cout << "Transfer successful.\n";
    } else {
        PQexec(conn, "ROLLBACK");
        cout << "Transfer failed. Check customer IDs.\n";
    }

    PQclear(res1);
    PQclear(res2);
    PQfinish(conn);
}

int main() {
    int choice = 0;

    while (choice != 5) {
        cout << "\n====== Bank Menu ======\n";
        cout << "1. Create Account\n";
        cout << "2. Check Balance by Customer ID\n";
        cout << "3. Change Mobile Number by Customer ID\n";
        cout << "4. Transfer Money between Customers\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                create_account();
                break;
            case 2:
                check_balance_by_id();
                break;
            case 3:
                change_mobile_by_id();
                break;
            case 4:
                transfer_money();
                break;
            case 5:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Invalid option. Try again.\n";
        }
    }

    return 0;
}
