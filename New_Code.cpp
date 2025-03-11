#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>

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
        cout << "  HUMAN's choice? ";
        cin >> choice;
        choice = toupper(choice);
        cout << "  HUMAN chose " << (choice == 'R' ? "ROCK" : choice == 'P' ? "PAPER" : "SCISSORS") << endl;
        return choice;
    }
};

class ComputerPlayer : public Player {
protected:
    vector<char> history;
    map<string, int> frequency;
    int memorySize;
public:
    ComputerPlayer(int memSize = 5) : memorySize(memSize) {
        srand(time(0));
        loadFrequencies();
    }
    virtual char makeChoice() override = 0;
    void updateHistory(char humanChoice, char computerChoice) {
        history.push_back(humanChoice);
        history.push_back(computerChoice);
        if (history.size() > 2 * memorySize) {
            history.erase(history.begin());
            history.erase(history.begin());
        }
        
        if (history.size() == 2 * memorySize) {
            string pattern(history.begin(), history.end());
            frequency[pattern]++;
            cout << "    Updated " << pattern << " to " << frequency[pattern] << endl;
        }
    }
    void saveFrequencies() {
        ofstream file("frequencies.txt");
        for (const auto &entry : frequency) {
            file << entry.first << " " << entry.second << endl;
        }
        file.close();
        cout << "Writing frequency file freq.txt: " << frequency.size() << " records." << endl;
    }
    void loadFrequencies() {
        ifstream file("frequencies.txt");
        if (!file.is_open()) return;
        frequency.clear();
        string pattern;
        int count;
        while (file >> pattern >> count) {
            frequency[pattern] = count;
        }
        file.close();
        cout << "Reading file freq.txt: " << frequency.size() << " records." << endl;
    }
};

class SmartComputer : public ComputerPlayer {
public:
    SmartComputer(int memSize = 5) : ComputerPlayer(memSize) {}
    char makeChoice() override {
        if (history.size() < 2 * memorySize) {
            cout << "    Insufficient history to predict.\n    Computer will choose randomly." << endl;
            return randomChoice();
        }
        
        string bestPattern;
        int maxFreq = 0;
        for (const auto &[pattern, count] : frequency) {
            if (pattern.substr(0, 2 * (memorySize - 1)) == string(history.begin(), history.end() - 2) && count > maxFreq) {
                maxFreq = count;
                bestPattern = pattern;
            }
        }
        
        if (bestPattern.empty()) {
            cout << "    No matching pattern found.\n    Computer will choose randomly." << endl;
            return randomChoice();
        }
        
        char predictedMove = bestPattern[bestPattern.size() - 2];
        cout << "    Predicted human choice: " << (predictedMove == 'R' ? "ROCK" : predictedMove == 'P' ? "PAPER" : "SCISSORS") << endl;
        return counterMove(predictedMove);
    }
private:
    char randomChoice() {
        char choices[] = {'R', 'P', 'S'};
        return choices[rand() % 3];
    }
    char counterMove(char move) {
        if (move == 'R') return 'P';
        if (move == 'P') return 'S';
        return 'R';
    }
};

class GameEngine {
private:
    HumanPlayer human;
    SmartComputer *computer;
    int humanScore = 0, computerScore = 0, ties = 0;
public:
    GameEngine(SmartComputer *comp) : computer(comp) {}
    void playGame(int rounds = 20) {
        for (int i = 0; i < rounds; ++i) {
            cout << "\nRound " << i + 1 << endl;
            char humanChoice = human.makeChoice();
            char computerChoice = computer->makeChoice();
            computer->updateHistory(humanChoice, computerChoice);
            cout << "  COMPUTER chose " << (computerChoice == 'R' ? "ROCK" : computerChoice == 'P' ? "PAPER" : "SCISSORS") << endl;
            
            if ((humanChoice == 'R' && computerChoice == 'S') ||
                (humanChoice == 'P' && computerChoice == 'R') ||
                (humanChoice == 'S' && computerChoice == 'P')) {
                cout << "  The winner is: HUMAN\n";
                humanScore++;
            } else if (humanChoice == computerChoice) {
                cout << "  The winner is: TIE\n";
                ties++;
            } else {
                cout << "  The winner is: COMPUTER\n";
                computerScore++;
            }
        }
        displayResults();
        computer->saveFrequencies();
    }
    void displayResults() {
        int totalGames = humanScore + computerScore + ties;
        cout << "\nMatch stats\n-----------\n";
        cout << "   Human wins:    " << humanScore << "  " << fixed << setprecision(0) << (humanScore * 100.0 / totalGames) << "%\n";
        cout << "Computer wins:   " << computerScore << "  " << (computerScore * 100.0 / totalGames) << "%\n";
        cout << "         Ties:    " << ties << "  " << (ties * 100.0 / totalGames) << "%\n";
    }
};

int main() {
    SmartComputer computer;
    GameEngine game(&computer);
    game.playGame();
    return 0;
}
