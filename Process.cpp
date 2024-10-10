class Process {
private:
    int pid;
    String screenName;
    int curLines;
    int maxLines;
    int core;
public:
    enum ProcessState {
        READY,
        RUNNING,
        WAITING,
        FINISHED
    };

    Process(int pid, std::string screenName, int core;) : pid(pid), screenName(screenName), state(READY), isFinished(false) {}

    ProcessState getState() const {
        return state;
    }

    void switchState(ProcessState newState) {
        state = newState;
        std::cout << "Process " << screenName << " is now in state: "
            << (state == READY ? "READY" :
                state == RUNNING ? "RUNNING" :
                state == WAITING ? "WAITING" : "FINISHED")
            << std::endl;
    }

    bool hasFinished() const {
        return isFinished;
    }

    void executeCommand(std::function<void()> command) {
        if (state == RUNNING) {
            for (curLines = 0; curLines < maxLines; ++curLines) {
                 //tentative
            }
            
            switchState(FINISHED);
            isFinished = true;
        }
        else {
            std::cout << "Cannot execute command." << std::endl;
        }
    }

    return 0;
}