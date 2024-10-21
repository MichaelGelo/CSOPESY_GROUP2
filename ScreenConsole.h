#pragma once
#include "AConsole.h"
#include <string>
#include <vector>

class ScreenConsole : public AConsole {
public:
    ScreenConsole(const std::string& processName, int currentLine, int totalLines);

    void onEnabled() override;
    void display() override;
    void process() override;

private:
    std::string getCurrentTimestamp() const;
    std::string processName;
    int currentLine;
    int totalLines;

    std::vector<std::string> commandHist; // Store command history
    std::string timestamp; // Timestamp variable
    bool isActive; // State variable to track if the screen is active
    std::string lastCommand;
};
