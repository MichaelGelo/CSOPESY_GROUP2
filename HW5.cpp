#include <iostream>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <windows.h>

#include "MarqueeConsole.h"
using namespace std;

struct Screen {
    string processName;
    int currentLine;
    int totalLines;
    time_t creationTime;
    vector<string> outputHistory;
    int textColor;
    bool isVisible;

    Screen() : processName("main"), currentLine(0), totalLines(100), creationTime(time(0)), textColor(7), isVisible(true) {}

    Screen(const string& name, int color = 7, bool visible = true)
        : processName(name), currentLine(0), totalLines(100), creationTime(time(0)), textColor(color), isVisible(visible) {}

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
void screenCreate(const string& name, int color, bool visible);
void screenRestore(const string& name);
void handleScreenCommands(const string& input);
void listScreens();
void displayMainMenu();
void displayScreen(const Screen& screen);
void displayHistory(const Screen& screen);
void hiddenScreenCreate(const string& name);
void nvidia_Page(const Screen& screen);
void marquee_Page(const Screen& screen);

void color(int n) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n);
}

void marquee_Console() {
    MarqueeConsole consoleApp;
    consoleApp.run();
}


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

void printStatement(const string& statement, int coreID/*, screen_name???*/) {
    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now);

    ostringstream timeStream;
    timeStream << put_time(&localtm, "(%m/%d/%Y %I:%M:%S %p)");

    cout << timeStream.str() << " Core:" << coreID << " " << statement /*<< screen_name???*/ << endl;
}

void exportTxt(const std::string& command) {
    if (command == "start") {
        outputFile.open("output.txt");
        if (!outputFile.is_open()) {
            std::cerr << "Unable to open output file" << std::endl;
        }
    } else if (command == "end") {
        if (outputFile.is_open()) {
            outputFile.close();
            std::cout << "Content has been exported to output.txt" << std::endl;
        }
    }
}

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

void screenCreate(const string& name, int color = 7, bool visible = true) {
    if (screens.find(name) != screens.end()) {
        cout << "Screen with name '" << name << "' already exists.\n";
        return;
    }
    screens[name] = Screen(name, color, visible);
    currentScreen = name;
    if (name == "nvidia-smi") {
        nvidia_Page(screens[name]);
    }else  if (name == "marquee") {
        marquee_Page(screens[name]);
    }
    else {
        displayScreen(screens[name]);
    }
}

void displayHistory(const Screen& screen) {
    for (const string& output : screen.outputHistory) {
        cout << output << endl;
    }
}

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

void listScreens() {
    bool hasVisibleScreens = false;
    for (const auto& screen : screens) {
        if (screen.second.isVisible) {
            if (!hasVisibleScreens) {
                cout << "Active screens:\n";
                hasVisibleScreens = true;
            }
            if (screen.first == currentScreen) {
                cout << screen.first << " (attached)\n";
            }
            else {
                cout << screen.first << "\n";
            }
        }
    }
    if (!hasVisibleScreens) {
        cout << "No active screens.\n";
    }
}

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

void displayNvidiaSmiOutput(const Screen& screen) {
    for (const string& output : screen.outputHistory) {
        cout << output;
    }
}

void displayMarqueeOutput(const Screen& screen) {
    for (const string& output : screen.outputHistory) {
        cout << output;
    }
}

void nvidia_Page(const Screen& screen) {
    system("cls");
    color(screen.textColor);
    displayNvidiaSmiOutput(screen);
}

void marquee_Page(const Screen& screen) {
    system("cls");
    color(screen.textColor);
    displayMarqueeOutput(screen);
    currentScreen.clear();
    clear();
}


void hiddenScreenCreate(const string& name) {
    screenCreate(name, 7, false);
    if (name == "nvidia-smi") {
        screens[name].outputHistory.clear(); 

        int visibleScreens = 0;
        for (const auto& screen : screens) {
            if (screen.second.isVisible) {
                visibleScreens++;
            }
        }

        tm localTime;
        localtime_s(&localTime, &screens[name].creationTime);
        ostringstream oss;
        oss << put_time(&localTime, "%a %b %d %H:%M:%S %Y");

        screens[name].addOutput(oss.str() + "\n");
        screens[name].addOutput("+--------------------------------------------------------------------------------------+\n");
        screens[name].addOutput("| NVIDIA-SMI 551.86                Driver Version: 551.86       CUDA Version: 12.4     |\n");
        screens[name].addOutput("|----------------------------------------+----------------------+----------------------+\n");
        screens[name].addOutput("| GPU  Name                     TCC/WDDM | Bus-Id        Disp.A | Volatile Uncorr. ECC |\n");
        screens[name].addOutput("| Fan  Temp  Perf           Pwr:Usage/Cap|         Memory-Usage | GPU-Util  Compute M. |\n");
        screens[name].addOutput("|                                        |                      |               MIG M. |\n");
        screens[name].addOutput("|========================================+======================+======================|\n");
        screens[name].addOutput("|   0  NVIDIA GeForce GTX 1080     WDDM  | 00000000:26:00.0  On |                  N/A |\n");
        screens[name].addOutput("| 28%   45C    P8             12W / 170W |    701MiB /  8192MiB |      0%      Default |\n");
        screens[name].addOutput("|                                        |                      |                  N/A |\n");
        screens[name].addOutput("+----------------------------------------+----------------------+----------------------+\n");
        screens[name].addOutput("                                                                                        \n");
        screens[name].addOutput("+--------------------------------------------------------------------------------------+\n");
        screens[name].addOutput("| Processes:                                                                           |\n");
        screens[name].addOutput("|  GPU   GI   CI        PID   Type   Process name                           GPU Memory |\n");
        screens[name].addOutput("|        ID   ID                                                            Usage      |\n");
        screens[name].addOutput("|======================================================================================|\n");

        if (visibleScreens == 0) {
            screens[name].addOutput("|                                No processes available                                |\n");
        }
        else {
            for (const auto& screen : screens) {
                if (screen.second.isVisible) {
                    ostringstream processLine;
                    processLine << "|  0     N/A  N/A       0     C+G    " << setw(39) << left << screen.second.processName << "N/A        |\n";
                    screens[name].addOutput(processLine.str());
                }
            }
        }

        screens[name].addOutput("+--------------------------------------------------------------------------------------+\n");
        nvidia_Page(screens[name]);

    }else if (name == "marquee") {
        screens[name].outputHistory.clear();

        int visibleScreens = 0;
        for (const auto& screen : screens) {
            if (screen.second.isVisible) {
                visibleScreens++;
            }
        }

        marquee_Console();


       
        marquee_Page(screens[name]);
    }
    else {
        screens[name].addOutput("Running " + name + "...\n");
        displayScreen(screens[name]);
    }
}

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
    else if (command == "nvidia-smi") {
        hiddenScreenCreate("nvidia-smi");
    }
    else if (command == "marquee") {
        hiddenScreenCreate("marquee");
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

void mainCommandLoop() {
    clear();
    while (true) {
        enter();
        string input;
        getline(cin, input);
        readCommand(input);
    }
}

int main() {
    menu();
    mainCommandLoop();
    return 0;
}
