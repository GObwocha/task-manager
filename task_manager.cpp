#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>

using namespace std;

class Task {
    chrono::year_month_day date_saved = chrono::year_month_day{floor<chrono::days>(chrono::system_clock::now())};
    chrono::year_month_day deadline;
    string title;
    string description;

public:
    Task() {};
    ~Task() {};
    Task(chrono::year_month_day deadline, string title, string description) {
        this->deadline = deadline;
        this->title = title;
        this->description = description;
    }

    Task(chrono::year_month_day date_saved, chrono::year_month_day deadline, string title, string description) {
        this->date_saved = date_saved;
        this->deadline = deadline;
        this->title = title;
        this->description = description;
    }

    void setDateSaved(string date_saved_str) {
        istringstream ss(date_saved_str);
        ss >> std::chrono::parse("%F", date_saved);
    }

    void setDeadline(string deadline_str) {
        istringstream ss(deadline_str);
        ss >> std::chrono::parse("%F", deadline);
    }

    void setTitle(string title) {
        this->title = title;
    }

    void setDescription(string description) {
        this->description = description;
    }

    int getDeadline() {
        chrono::sys_days today = floor<chrono::days>(chrono::system_clock::now());
        chrono::sys_days deadline_sys = deadline;
        chrono::days days_remaining = deadline_sys - today;
        return days_remaining.count();
    }

    void getDetails() {
        cout << "Title: " << title << endl;
        cout << "Description\n------------" << endl;
        cout << description << endl;
    }

    string getTitle() {
        return title;
    }

    // Notice we pass by const reference now since we are using values
    ofstream& writeTask(ofstream& out) const {
        string eot = "end of task";
        // To write chrono types to file cleanly in C++20
        out << format("{:%F}", date_saved) << endl;
        out << format("{:%F}", deadline) << endl;
        out << title << endl;
        out << description << endl;
        out << eot << endl;
        return out;
    }
};

class User {
    string username;
    string password;
    vector<Task> tasks; // Storing by value, no pointers!

public:
    User() {};
    User(string username, string password) {
        this->username = username;
        this->password = password;
    }
    ~User() {};

    bool isPassword(string password) {
        return this->password == password;
    }

    void addTask(Task task) {
        tasks.push_back(task);
        cout << "Task has been added successfully!" << endl;
    }

    void setUsername(string username) {
        this->username = username;
    }

    void setPassword(string password) {
        this->password = password;
    }

    void setTask(Task task) {
        tasks.push_back(task);
    }

    string getUsername() {
        return username;
    }

    void getNotifications();
    string writeUser();
    void deleteTask(string title);
    void display_tasks();
};

string User::writeUser() {
    string eou = "end of user";
    string filename = username + ".txt";
    ofstream out(filename, ios::out);
    out << username << endl;
    out << password << endl;
    for (const auto& task : tasks) {
        task.writeTask(out);
    }
    out << eou << endl;
    out.close();
    return filename;
}

void User::deleteTask(string title) {
    // Modern C++20 way to delete from a vector
    auto initial_size = tasks.size();
    std::erase_if(tasks, [&](Task& t) { return t.getTitle() == title; });
    
    if (tasks.size() < initial_size) {
        cout << "Task deleted successfully!" << endl;
    } else {
        cout << "Task not found" << endl;
    }
}

void User::display_tasks() {
    for (auto& task : tasks) {
        task.getDetails();
    }
    cout << endl;
}

void User::getNotifications() {
    string separator = "                              ";
    cout << "Task Title" << separator << "Deadline" << endl;
    for (auto& task : tasks) {
        int days_left = task.getDeadline();
        if (days_left > 0)
            cout << task.getTitle() << separator << days_left << " days left" << endl;
        else if (days_left == 0)
            cout << task.getTitle() << separator << "Due today!" << endl;
        else
            cout << task.getTitle() << separator << "Overdue by " << (days_left * -1) << " days" << endl;
    }
}

class App {
    vector<User> users; // Storing by value

public:
    App() {};
    ~App() {};
    
    void registerUser(User user) {
        users.push_back(user);
        cout << "User registered successfully!" << endl;
    }

    int isUser(string username);
    void deleteUser(string username, string password);
    User* login();
    ofstream& writeUsers(ofstream& out);
};

int App::isUser(string username) {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].getUsername() == username) {
            return i;
        }
    }
    return -1; // Return -1 to clearly indicate "not found"
}

void App::deleteUser(string username, string password) {
    auto initial_size = users.size();
    std::erase_if(users, [&](User& u) { 
        return u.getUsername() == username && u.isPassword(password); 
    });
    
    if (users.size() < initial_size) {
        cout << "User deleted successfully!" << endl;
    } else {
        cout << "User not found or incorrect password!" << endl;
    }
}

User* App::login() {
    string username, password;
    cout << "Enter your username: ";
    cin >> username; 
    cout << "Enter password: ";
    cin >> password; 
    
    int index = isUser(username);
    if (index != -1 && users[index].isPassword(password)) {
        return &users[index]; // Return pointer to the user in the vector
    }
    return nullptr;
}

ofstream& App::writeUsers(ofstream& out) {
    for (auto& user : users) {
        string filename = user.writeUser();
        out << filename << endl;
    }
    return out;
}

// General Functions
void userDashboard(User* active_user) {
    if (!active_user) return; // Safety check

    int choice;
    string deadline_str;
    chrono::year_month_day deadline;
    string title, description;

    cout << "\nWelcome " << active_user->getUsername() << endl;
    
    while (true) {
        cout << "\nUser Dashboard\n----------------" << endl;
        cout << "Notifications\n--------------" << endl;
        active_user->getNotifications();
        cout << "\nOptions\n--------" << endl;
        cout << "1. Add a task" << endl;
        cout << "2. Delete a task" << endl;
        cout << "3. Log out" << endl;
        cout << "Enter choice: ";
        
        if (!(cin >> choice)) { // Clear bad input
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();

        switch (choice) {
        case 1:
            cout << "Enter the title of the task: ";
            getline(cin, title); 
            cout << "Enter the description of the task: ";
            getline(cin, description); 
            cout << "Enter the deadline for the task (YYYY-MM-DD): ";
            cin >> deadline_str; cin.ignore();
            {
                istringstream ss(deadline_str);
                ss >> std::chrono::parse("%F", deadline);
            }
            // Passing by value, no 'new' or 'delete' required!
            active_user->addTask(Task(deadline, title, description));
            break;
        case 2:
            cout << "Enter the title of the task to delete: ";
            getline(cin, title); 
            active_user->deleteTask(title);
            break;
        case 3:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Kindly input a valid choice!" << endl;
        }
    }
}

void save(App *app){
    string filename = "task_manager.txt";
    ofstream out(filename, ios::out); // Changed to ios::out to avoid infinite appending
    app->writeUsers(out);
    if (out.good()) {
        cout << "Data saved successfully" << endl;
    }
    out.close();
}

 void load(App& app) { 
    string filename;
    ifstream in("task_manager.txt", ios::in);
    
    if (in.fail()) {
        cout << "No previous session data found (task_manager.txt missing)." << endl;
        return;
    }

    // Loop through each user filename saved in task_manager.txt safely
    while (getline(in, filename)) {
        if (filename.empty()) continue; // Skip accidental blank lines

        ifstream user_ifstream(filename, ios::in);
        if (user_ifstream.fail()) {
            cout << "Warning: Could not open user file: " << filename << endl;
            continue;
        }

        string username, password;
        if (!getline(user_ifstream, username) || !getline(user_ifstream, password)) {
            user_ifstream.close();
            continue; // Skip malformed user files
        }

        User user(username, password);
        string line;

        // Process the rest of the user file line by line
        while (getline(user_ifstream, line)) {
            if (line == "end of user" || line.empty()) {
                break; // Stop when we hit the end marker
            }

            // If it's not the end of user, it must be the start of a Task block
            string date_saved_str = line; 
            string deadline_str;
            string title;
            string description = ""; // Reset description for every single task!
            string placeholder;

            getline(user_ifstream, deadline_str);
            getline(user_ifstream, title);

            // Accumulate multiline descriptions until we hit "end of task"
            while (getline(user_ifstream, placeholder)) {
                if (placeholder == "end of task") {
                    break;
                }
                if (!description.empty()) {
                    description += "\n"; // Preserve line breaks if description had them
                }
                description += placeholder;
            }

            // Convert string strings into proper year_month_day components
            chrono::year_month_day date_saved;
            chrono::year_month_day deadline;

            int y, m, d;
            char s1, s2;
            
            istringstream ss1(date_saved_str);
            if (ss1 >> y >> s1 >> m >> s2 >> d) {
                date_saved = chrono::year(y) / m / d;
            }

            istringstream ss2(deadline_str);
            if (ss2 >> y >> s1 >> m >> s2 >> d) {
                deadline = chrono::year(y) / m / d;
            }

            // Add the fully hydrated task object to the user
            user.addTask(Task(date_saved, deadline, title, description));
        }

        user_ifstream.close();
        
        // CRITICAL FIX: Save the populated user back into your App instance
        app.registerUser(user);
    }
    
    in.close();
    cout << "Application state loaded successfully!" << endl;
}

string getString(ifstream& in){
    string s;
    getline(in, s);
    return s;
}

int main() {
    int choice;
    User* active_user = nullptr;
    string username, password;
    App tm;
    load(tm);
    cout << "WELCOME TO THE CLI TASK MANAGER APPLICATION\n--------------------------------------------" << endl;
    
    while (true) {
        cout << "\nMain Menu\n--------" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register user account" << endl;
        cout << "3. Delete user account" << endl;
        cout << "4. Exit application" << endl;
        cout << "Enter choice: ";
        
        if (!(cin >> choice)) { // Clear bad input
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();

        switch (choice) {
        case 1:
            active_user = tm.login();
            if (active_user != nullptr) {
                userDashboard(active_user);
            } else {
                cout << "Invalid username or password.\n";
            }
            break;
        case 2:
            cout << "Enter your new username: ";
            cin >> username; 
            cout << "Enter new password: ";
            cin >> password; 
            tm.registerUser(User(username, password));
            break; 
        case 3:
            cout << "Enter your username: ";
            cin >> username; 
            cout << "Enter your password: ";
            cin >> password; 
            tm.deleteUser(username, password);
            break; 
        case 4:
            save(&tm);
            exit(0);
        default:
            cout << "Invalid choice. Kindly input a valid choice!" << endl;
        }
    }
    return 0;
}