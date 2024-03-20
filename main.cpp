#include <kipr/wombat.h>
#include <ctime>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <thread>


int motorLeft = 0;
int motorRight = 1;

int armBottom = 2;
int armTop = 3;


//servos
int clawFront = 0;
int clawArm = 1;

int StartLight = 1;
int frontLightLeft = 0;
int frontLightRight = 2;

// Corrected enum definition to have consistent naming and added missing state
enum ArmState{
    Down,
    Up1,
    Up2,
    Up3
};


// Redesigned to be a singleton class for managing arm state
class ArmStateManager {
private:
    ArmState state;
    static ArmStateManager* instance;
public:

    ArmStateManager() : state(Down) {}

    // Deleted copy constructor and assignment operator to prevent copying
    ArmStateManager(const ArmStateManager&) = delete;
    ArmStateManager& operator=(const ArmStateManager&) = delete;

    static ArmStateManager* getInstance() {
        if (instance == nullptr) {
            instance = new ArmStateManager();
        }
        return instance;
    }

    void ArmStateManagerLoop(){
        while(true){
            switch(state){
                case(Down):
                    break;
                case(Up1):
                    break;
                case(Up2):
                    break;
                case(Up3):
                    break;
            }

        }
    }

    ArmState getState() const {
        return state;
    }

    void setState(ArmState newState) {
        state = newState;
    }
};

ArmStateManager* ArmStateManager::instance = nullptr; // Initialize instance

/*
 *
  ___                                    _        _ __        _    _
 / __| ___  _ __   _ __   __ _  _ _   __| |      | '_ \ __ _ | |_ | |_  ___  _ _  _ _
| (__ / _ \| '  \ | '  \ / _` || ' \ / _` |      | .__// _` ||  _||  _|/ -_)| '_|| ' \
 \___|\___/|_|_|_||_|_|_|\__/_||_||_|\__/_|      |_|   \__/_| \__| \__|\___||_|  |_||_|

 */


// Command template
class Command {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
};



class ArmUp1 : public Command {
public:
    void execute() override {
        // Implement motor control to lift the arm
        mrp(armBottom, 200, 50);
        ArmStateManager::getInstance()->setState(Up1);
    }

    void undo() override {

    }
};

class ArmUp2 : public Command {
public:
    void execute() override {
        // Implement motor control to lift the arm
        mrp(armBottom, 200, 50);
        ArmStateManager::getInstance()->setState(Up1);
    }

    void undo() override {

    }
};


class ArmUp3 : public Command {
public:
    void execute() override {
        // Implement motor control to lift the arm
        mrp(armBottom, 200, 50);
        ArmStateManager::getInstance()->setState(Up1);
    }

    void undo() override {

    }
};


class ArmDown : public Command {
public:
    void execute() override {
        // Implement motor control to lower the arm
        // Example: mav(motor_port, -speed);
        ArmStateManager::getInstance()->setState(Down);
    }

    void undo() override {

    }
};

class OpenClawCommand : public Command {
public:
    void execute() override {
        set_servo_position(clawFront, 100)
    }

    void undo() override {

    }
};

class CloseClawCommand : public Command {
public:
    void execute() override {
        set_servo_position(clawFront, 0)
    }

    void undo() override {

    }
};

class OpenArmClawCommand : public Command {
public:
    void execute() override {
        set_servo_position(clawArm, 100)
    }

    void undo() override {

    }
};

class CloseArmClawCommand : public Command {
public:
    void execute() override {
        set_servo_position(clawFront, 0)
    }

    void undo() override {

    }
};

class TurnCommand : public Command {
private:
    int speed;
    int angle;

public:
    TurnCommand(int speed, int angle){
        this->speed = speed;
        this->angle = angle;
    }

    void execute() override {
        mrp(0, speed, angle);
        mrp(1, speed, -angle);
        bmd(0);
        bmd(1);
    }

    void undo() override {
        mrp(0, speed, -angle);
        mrp(1, speed, angle);
        bmd(0);
        bmd(1);
    }
};

class DriveCommand : public Command {
private:
    int speed;
    int distance;


public:
    DriveCommand(int speed, int distance){
        this->speed = speed;
        this->distance = distance;
    }

    void execute() override {
        mrp(0, speed, distance);
        mrp(1, speed, distance);
        bmd(0);
        bmd(1);
    }

    void undo() override {
        mrp(0, speed, -distance);
        mrp(1, speed, -distance);
        bmd(0);
        bmd(1);
    }
};

class CommandController {
private:
    std::vector<std::unique_ptr<Command>> commands;
    std::vector<std::unique_ptr<Command>> history;

public:
    void addCommand(Command* command) {
        commands.push_back(std::unique_ptr<Command>(command));
    }

    void executeCommands() {
        for (auto& command : commands) {
            command->execute();
            history.push_back(std::move(command));
        }
        commands.clear(); // Clear the command list after execution
    }

    void undoCommands() {
        std::reverse(history.begin(), history.end());
        for(auto& command : history){
            command->undo();
        }
        history.clear(); // Clear the history after undoing
    }
};




/*
                                        _
 __  ___  _ _  ___        __  ___  __| | ___
/ _|/ _ \| '_|/ -_)      / _|/ _ \/ _` |/ -_)
\__|\___/|_|  \___|      \__|\___/\__/_|\___|

 */

enum TaskState {
    Aufgabe1State,
    Aufgabe2State,
    Aufgabe3State,
    FinalState,
};

// Task interface
class Task {
public:
    virtual void start() = 0;
    virtual bool isCompleted() = 0;
    virtual void returnToStart() = 0;
};

// Aufgabe1 class
class Aufgabe1 : public Task {
private:
    CommandController* controller = new CommandController();
    bool taskStarted;
    clock_t start_time;

public:
    Aufgabe1(ArmStateManager* armStateManager) : taskStarted(false) {
        controller->addCommand(new ArmUp1());
    }

    void start() override {
        if (!taskStarted) {
            controller->executeCommands();
            start_time = clock();
            taskStarted = true;
        }
    }

    bool isCompleted() override {
        if (static_cast<double>(clock() - start_time) / CLOCKS_PER_SEC > 1) {
            taskStarted = false;
            return true;
        }
        return false;
    }

    void returnToStart(){
        controller->undoCommands();

    }

    ~Aufgabe1(){
        delete controller;
    }
};

// Similar class definitions for Aufgabe2, Aufgabe3, and ReturnToHome
class Aufgabe2 : public Task {

private:
    CommandController* controller = new CommandController();
    bool taskStarted;
    clock_t start_time;

public:
    Aufgabe2(ArmStateManager* armStateManager) : taskStarted(false) {
        controller->addCommand(new DriveCommand(1000, 100));
    }

    void start() override {
        if (!taskStarted) {
            controller->executeCommands();
            start_time = clock();
            taskStarted = true;
        }
    }

    bool isCompleted() override {
        if (static_cast<double>(clock() - start_time) / CLOCKS_PER_SEC > 1) {
            taskStarted = false;
            return true;
        }
        return false;
    }

    void returnToStart(){
        controller->undoCommands();
    }

    ~Aufgabe2(){
        delete controller;
    }
};

class Aufgabe3 : public Task {

private:
    CommandController* controller = new CommandController();
    bool taskStarted;
    clock_t start_time;

public:
    Aufgabe3(ArmStateManager* armStateManager) : taskStarted(false) {
        controller->addCommand(new DriveCommand(3000, 100));

    }

    void start() override {
        if (!taskStarted) {
            controller->executeCommands();
            start_time = clock();
            taskStarted = true;
        }
    }

    bool isCompleted() override {
        if (static_cast<double>(clock() - start_time) / CLOCKS_PER_SEC > 1) {
            taskStarted = false;
            return true;
        }
        return false;
    }

    void returnToStart(){
        controller->undoCommands();
    }

    ~Aufgabe3(){
        delete controller;
    }
};

void goToNextTask(TaskState& currentState) {
    if (currentState < FinalState) {
        currentState = static_cast<TaskState>(static_cast<int>(currentState) + 1);
    }
}

bool checkTaskCompletion(Task& task, int timeout) {
    task.start(); //ensure that the task is running;

    //prevent the task from timeoting
    clock_t start_time = clock();
    while (static_cast<double>(clock() - start_time) / CLOCKS_PER_SEC < timeout) {
        if (task.isCompleted()) {
            task.returnToStart();
            return true;
        }
    }
    task.returnToStart();
    return false; // Return false if the task did not complete in time
}

/*
                           _            _ __
 __  ___  _ _  ___       | | ___  ___ | '_ \
/ _|/ _ \| '_|/ -_)      | |/ _ \/ _ \| .__/
\__|\___/|_|  \___|      |_|\___/\___/|_|

 */

void actionSequencer(TaskState& currentState, ArmStateManager* armStateManager) {
    Task* aufgabe1 = new Aufgabe1(armStateManager);
    Task* aufgabe2 = new Aufgabe2(armStateManager);
    Task* aufgabe3 = new Aufgabe3(armStateManager);

    while (currentState != FinalState) {
        switch (currentState) {
            case Aufgabe1State:

                if (checkTaskCompletion(*aufgabe1, 1)) {
                    goToNextTask(currentState);
                }
                break;
            case Aufgabe2State:

                if (checkTaskCompletion(*aufgabe2, 1)) {
                    goToNextTask(currentState);
                }
                break;
            case Aufgabe3State:

                if (checkTaskCompletion(*aufgabe3, 1)) {
                    goToNextTask(currentState);
                }
                break;
            default:
                break;
        }
    }
}



/*
  ___ __   __ _  _   ___
/ __|\ \ / /| \| | / __|
\__ \ \   / | .  || (__
|___/  |_|  |_|\_| \___|

 */

class sync {
private:
    // Assuming these are the motor ports for your drive motors
    const int leftMotor = 0;
    const int rightMotor = 1;

    // Conversion factor from encoder ticks to centimeters
    // This needs to be calibrated for your specific robot
    const double ticksPerCm = 100.0; // This is an example value

public:
    double synchronize() {
        // Reset motor positions
        cmpc(leftMotor);
        cmpc(rightMotor);

        // Drive forward 1 cm
        int targetTicks = static_cast<int>(1.0 * ticksPerCm);
        mav(leftMotor, targetTicks);
        mav(rightMotor, targetTicks);
        msleep(1000); // Adjust time as necessary

        // Read the encoder values
        int leftTicks = gmpc(leftMotor);
        int rightTicks = gmpc(rightMotor);

        // Calculate the average distance traveled
        double avgTicks = (leftTicks + rightTicks) / 2.0;
        double actualDistanceCm = avgTicks / ticksPerCm;

        // Return the actual distance traveled in cm
        return actualDistanceCm;
    }
};


int main() {

    enable_servo(clawFront);
    enable_servo(clawArm);

    //wait for start light

    /*
    while(3000 < analog(1)){
        msleep(100);
    }
    */

    ArmStateManager *armStateManager = new ArmStateManager();

    //set state & start process
    TaskState currentState = Aufgabe1State;
    actionSequencer(currentState, armStateManager);

    disable_servo(clawFront);
    disable_servo(clawArm);

    return 0;

}