#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Account {
    string site;
    string username;
    string password;
};

class PasswordManager {
private:
    vector<Account> accounts;
    string adminPassword;
    const string vaultFile = "vault.txt";

    // Simple XOR encryption for demo purposes
    string encrypt(const string& text) const {
        string enc = text;
        char key = 'K';
        for (auto &c : enc) c ^= key;
        return enc;
    }

    string decrypt(const string& text) const {
        return encrypt(text);
    }

public:
    bool authenticate() {
        ifstream file(vaultFile);
        string storedHash;
        if (file.good()) {
            getline(file, storedHash);
            string input;
            cout << "Enter admin password: ";
            getline(cin, input);
            if (encrypt(input) == storedHash) {
                adminPassword = input;
                loadFromFile();
                return true;
            } else {
                cout << "❌ Incorrect admin password!\n";
                return false;
            }
        } else {
            cout << "Set a new admin password: ";
            getline(cin, adminPassword);
            ofstream out(vaultFile);
            out << encrypt(adminPassword) << "\n";
            out.close();
            cout << "✅ Admin password created successfully!\n";
            return true;
        }
    }

    void addAccount() {
        Account acc;
        cout << "Enter site name: ";
        getline(cin, acc.site);
        cout << "Enter username: ";
        getline(cin, acc.username);
        cout << "Enter password: ";
        getline(cin, acc.password);
        accounts.push_back(acc);
        saveToFile();
        cout << "✅ Account saved successfully!\n";
    }

    void viewAccounts() const {
        if (accounts.empty()) {
            cout << "No accounts saved yet.\n";
            return;
        }
        cout << "\n--- STORED ACCOUNTS ---\n";
        for (const auto& acc : accounts) {
            cout << "🌐 Site: " << acc.site
                 << " | 👤 User: " << acc.username
                 << " | 🔑 Pass: " << acc.password << "\n";
        }
    }

    void deleteAccount() {
        string site;
        cout << "Enter site to delete: ";
        getline(cin, site);
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            if (it->site == site) {
                accounts.erase(it);
                saveToFile();
                cout << "🗑️ Account deleted successfully!\n";
                return;
            }
        }
        cout << "❌ No account found for that site.\n";
    }

private:
    void saveToFile() {
        ofstream file(vaultFile);
        file << encrypt(adminPassword) << "\n";
        for (const auto& acc : accounts) {
            string line = acc.site + "," + acc.username + "," + acc.password;
            file << encrypt(line) << "\n";
        }
    }

    void loadFromFile() {
        ifstream file(vaultFile);
        string line;
        getline(file, line); // skip admin password hash
        accounts.clear();
        while (getline(file, line)) {
            string dec = decrypt(line);
            stringstream ss(dec);
            string site, user, pass;
            getline(ss, site, ',');
            getline(ss, user, ',');
            getline(ss, pass, ',');
            if (!site.empty())
                accounts.push_back({site, user, pass});
        }
    }
};

int main() {
    PasswordManager pm;
    if (!pm.authenticate())
        return 0;

    while (true) {
        cout << "\n===== PASSWORD MANAGER =====\n";
        cout << "1. Add Account\n";
        cout << "2. View Accounts\n";
        cout << "3. Delete Account\n";
        cout << "4. Exit\n";
        cout << "Choose option: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") pm.addAccount();
        else if (choice == "2") pm.viewAccounts();
        else if (choice == "3") pm.deleteAccount();
        else if (choice == "4") {
            cout << "👋 Exiting... Have a great day!\n";
            break;
        }
        else cout << "Invalid choice! Try again.\n";
    }

    return 0;
}
