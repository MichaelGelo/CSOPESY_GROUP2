#include <iostream>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <windows.h>

using namespace std;

struct Screen {
    string processName;
    int currentLine;
    int totalLines;
    time_t creationTime;
    vector<string> outputHistory;
    int textColor;

    Screen() : processName("main"), currentLine(0), totalLines(100), creationTime(time(0)), textColor(7) {} 

    Screen(const string& name, int color = 7) : processName(name), currentLine(0), totalLines(100), creationTime(time(0)), textColor(color) {}

    void addOutput(const string& output) {
        outputHistory.push_back(output);
    }
};

unordered_map<string, Screen> screens;
string currentScreen = "";
Screen mainMenuScreen;


void color(int n);
void menu();
void enter();
void clear();
void screenCreate(const string& name, int color);
void screenRestore(const string& name);
void handleScreenCommands(const string& input);
void listScreens();
void displayMainMenu();
void displayScreen(const Screen& screen);
void displayHistory(const Screen& screen);


void color(int n) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n);
}

// Main Menu
void menu() {
    color(13);
    cout << R"(
===============================================================
 _______  _______  _______  _______  _______  _______  __   __ 
|       ||       ||       ||       ||       ||       ||  | |  |
|       ||  _____||   _   ||    _  ||    ___||  _____||  |_|  |
|       || |_____ |  | |  ||   |_| ||   |___ | |_____ |       |
|      _||_____  ||  |_|  ||    ___||    ___||_____  ||_     _|
|     |_  _____| ||       ||   |    |   |___  _____| |  |   |  
|_______||_______||_______||___|    |_______||_______|  |___|  
AGUSTINES    --   DEPASUCAT     --     ESTEBAN     --  PADILLA

HELLO, WELCOME TO GROUP 2'S CSOPESY COMMANDLINE!
TYPE 'exit' TO QUIT, 'clear' TO CLEAR THE SCREEN
===============================================================
    )" << endl;
    color(7);
}

void enter() {
    color(7);
    if (currentScreen.empty()) {
        cout << "Enter a command: ";
    }
    else {
        cout << "[" << currentScreen << "]$ ";
    }
}

// Clear Screen
void clear() {
    system("cls");
    if (currentScreen.empty()) {
        menu();
        displayHistory(mainMenuScreen);
    }
    else {
        displayScreen(screens[currentScreen]);
    }
}

// Display Main Menu
void displayMainMenu() {
    cout << "\nAvailable commands:\n";
    cout << " - initialize\n";
    cout << " - screen -s <name> (create new screen)\n";
    cout << " - screen -r <name> (restore screen)\n";
    cout << " - screen -ls (list all screens)\n";
    cout << " - scheduler-test\n";
    cout << " - scheduler-stop\n";
    cout << " - report-util\n";
    cout << " - clear\n";
    cout << " - exit\n";
    cout << endl;
}

// Create Screen
void screenCreate(const string& name, int color = 7) {
    if (screens.find(name) != screens.end()) {
        cout << "Screen with name '" << name << "' already exists.\n";
        return;
    }
    screens[name] = Screen(name, color);
    currentScreen = name;
    displayScreen(screens[name]);
}

// Display Screen History
void displayHistory(const Screen& screen) {
    for (const string& output : screen.outputHistory) {
        cout << output << endl;
    }
}

// Display Screen
void displayScreen(const Screen& screen) {
    system("cls");
    color(screen.textColor); 
    cout << "Screen Name: " << screen.processName << "\n";
    cout << "Current Line: " << screen.currentLine << " / " << screen.totalLines << "\n";

    tm localTime;
    localtime_s(&localTime, &screen.creationTime);
    ostringstream oss;
    oss << put_time(&localTime, "%m/%d/%Y, %I:%M:%S %p");
    cout << "Created At: " << oss.str() << "\n\n";


    displayHistory(screen);
}

// Display Exisitng Screen
void screenRestore(const string& name) {
    auto it = screens.find(name);
    if (it == screens.end()) {
        string error = "Screen '" + name + "' does not exist.\n";

        if (currentScreen.empty()) {
            mainMenuScreen.addOutput(error);
            color(4);
            cout << error;
            color(7);
        }
        else {
            screens[currentScreen].addOutput(error);
            color(4);
            cout << error;
            color(7);
        }
        return;
    }

    currentScreen = name;
    displayScreen(it->second);
}

// Display All Screens
void listScreens() {
    if (screens.empty()) {
        cout << "No active screens.\n";
    }
    else {
        cout << "Active screens:\n";
        for (const auto& screen : screens) {
            if (screen.first == currentScreen) {
                cout << screen.first << " (attached)\n";
            }
            else {
                cout << screen.first << "\n";
            }
        }
    }
}

// Screen Commands
void handleScreenCommands(const string& input) {
    istringstream iss(input);
    string command, option, name;
    iss >> command >> option >> name;

    if (command == "screen") {
        if (option == "-s" && !name.empty()) {
            screenCreate(name);
        }
        else if (option == "-r" && !name.empty()) {
            screenRestore(name);
        }
        else if (option == "-ls") {
            listScreens();
        }
        else {
            string error = "Invalid screen command.\n\n";
            mainMenuScreen.addOutput(error);
            color(4);
            cout << error;

            color(7);
            cout << "  Usage:\n";
            cout << "  screen -s <name> (create new screen)\n";
            cout << "  screen -r <name> (restore screen)\n";
            cout << "  screen -ls (list all screens)\n\n";
        }
    }
    else {
        string error = "Unknown command. Try again.\n\n";
        mainMenuScreen.addOutput(error);
        color(4);
        cout << error;
        color(7);
    }
}

// Main Menu Command
void readCommand(const string& command) {
    if (currentScreen.empty()) {
        mainMenuScreen.addOutput("Enter a command: " + command);
    }
    else {
        screens[currentScreen].addOutput("[" + currentScreen + "]$ " + command);
    }

    if (command == "exit") {
        if (!currentScreen.empty()) {
            currentScreen.clear();
            clear();
        }
        else {
            exit(0);
        }
    }
    else if (command.find("screen") == 0) {
        handleScreenCommands(command);
    }
    else if (command == "initialize") {
        cout << "Initialize command recognized. Doing something.\n\n";
    }
    else if (command == "scheduler-test") {
        cout << "Scheduler test command recognized. Doing something.\n\n";
    }
    else if (command == "scheduler-stop") {
        cout << "Scheduler stop command recognized. Doing something.\n\n";
    }
    else if (command == "report-util") {
        cout << "Report util command recognized. Doing something.\n\n";
    }
    else if (command == "clear") {
        clear();
    }
    else {
        handleScreenCommands(command);
    }
}

// Main command loop
void mainCommandLoop() {
    clear();
    while (true) {
        enter();
        string input;
        getline(cin, input);
        readCommand(input);
    }
}

void printStatement(const string& statement, int coreID/*, screen_name???*/) {
    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now);

    ostringstream timeStream;
    timeStream << put_time(&localtm, "(%m/%d/%Y %I:%M:%S %p)");

    cout << timeStream.str() << " Core:" << coreID << " " << statement /*<< screen_name???*/ << endl;
}

int main() {
    menu();
    mainCommandLoop();
    return 0;
}
