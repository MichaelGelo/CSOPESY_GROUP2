#pragma once
#include "AConsole.h"
#include <string>
#include <vector>

class PlaceholderConsole : public AConsole {
public:
    PlaceholderConsole(const std::string& processName, int currentLine, int totalLines);

    void onEnabled() override;
    void display() override;
    void process() override;

private:
    std::string getCurrentTimestamp() const;
    std::string processName;
    int currentLine;
    int totalLines;

    std::vector<std::string> commandHist; // Add commandHist to store command history
    std::string timestamp; // Declare timestamp variable
};
