#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>

using namespace std;

class Player {
public:
    virtual char makeChoice() = 0;
    virtual ~Player() {}
};

class HumanPlayer : public Player {
public:
    char makeChoice() override {
        char choice;
        cout << "Enter choice (R/P/S): ";
        cin >> choice;
        return toupper(choice);
    }
};

class ComputerPlayer : public Player {
protected:
    vector<char> history;
    map<string, map<char, int>> frequency;
    int memorySize;
public:
    ComputerPlayer(int memSize = 5) : memorySize(memSize) {
        srand(time(0));
    }
    virtual char makeChoice() override = 0;
    void updateHistory(char humanChoice) {
        history.push_back(humanChoice);
        if (history.size() > memorySize) history.erase(history.begin());
        if (history.size() == memorySize) {
            string pattern(history.begin(), history.end() - 1);
            frequency[pattern][humanChoice]++;
        }
    }
};

class RandomComputer : public ComputerPlayer {
public:
    char makeChoice() override {
        char choices[] = {'R', 'P', 'S'};
        return choices[rand() % 3];
    }
};

class SmartComputer : public ComputerPlayer {
public:
    char makeChoice() override {
        if (history.size() < memorySize) return RandomComputer().makeChoice();
        string pattern(history.begin(), history.end());
        char predictedMove = 'R';
        int maxFreq = 0;
        for (auto &[move, count] : frequency[pattern]) {
            if (count > maxFreq) {
                maxFreq = count;
                predictedMove = move;
            }
        }
        if (predictedMove == 'R') return 'P';
        if (predictedMove == 'P') return 'S';
        return 'R';
    }
};

class GameEngine {
private:
    HumanPlayer human;
    ComputerPlayer *computer;
    int humanScore = 0, computerScore = 0;
public:
    GameEngine(ComputerPlayer *comp) : computer(comp) {}
    void playGame(int rounds = 20) {
        for (int i = 0; i < rounds; ++i) {
            char humanChoice = human.makeChoice();
            char computerChoice = computer->makeChoice();
            computer->updateHistory(humanChoice);
            cout << "Computer chose: " << computerChoice << endl;
            if ((humanChoice == 'R' && computerChoice == 'S') ||
                (humanChoice == 'P' && computerChoice == 'R') ||
                (humanChoice == 'S' && computerChoice == 'P')) {
                cout << "Human wins this round!\n";
                humanScore++;
            } else if (humanChoice == computerChoice) {
                cout << "It's a tie!\n";
            } else {
                cout << "Computer wins this round!\n";
                computerScore++;
            }
        }
        cout << "Final Score - Human: " << humanScore << " Computer: " << computerScore << endl;
    }
};

int main() {
    char strategy;
    cout << "Choose strategy (R for Random, S for Smart): ";
    cin >> strategy;
    ComputerPlayer *computer = nullptr;
    if(strategy == 'S') {
        computer = new SmartComputer();
    } else {
        computer = new RandomComputer();
    }
    GameEngine game(computer);
    game.playGame();
    delete computer;
    return 0;
}
