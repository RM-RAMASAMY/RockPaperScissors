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
        while (true) {
            cout << "  HUMAN's choice? ";
            cin >> choice;
            choice = toupper(choice);
            if (choice == 'R' || choice == 'P' || choice == 'S') {
                cout << "  HUMAN chose " << (choice == 'R' ? "ROCK" : choice == 'P' ? "PAPER" : "SCISSORS") << endl;
                return choice;
            } else {
                cout << "Invalid choice. Please enter R, P, or S: " << endl;
            }
        }
    }
};

class ComputerPlayer : public Player {
protected:
    vector<pair<char, char>> history;
    map<string, int> frequency;
    int memorySize;
    int mode;
public:
    ComputerPlayer(int memSize = 5, int Mode=1) : memorySize(memSize), mode(Mode) {
        srand(time(0));
        loadFrequencies();
    }
    virtual char makeChoice() override = 0;
    void updateHistory(char humanChoice, char computerChoice) {
        history.emplace_back(humanChoice, computerChoice);
        
        if (history.size() == ((memorySize/2)+1)) {
            string pattern;
            for (const auto& [h, c] : history) {
                pattern += h;
                pattern += c;
            }
            pattern.erase(pattern.end() - 1);
            frequency[pattern]++;
            cout << "    Updated " << pattern << " to " << frequency[pattern] << endl;
            history.erase(history.begin());
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
        cout << "Reading file frequencies.txt: " << frequency.size() << " records." << endl;
    }
};

class SmartComputer : public ComputerPlayer {
public:
    SmartComputer(int memSize = 5, int Mode=1) : ComputerPlayer(memSize,Mode) {}
    char makeChoice() override {
        if (mode==2)
        {
            cout << "    Computer will choose randomly." << endl;
            return randomChoice();
        }
        if (history.size() < (memorySize/2)) {
            cout << "    Insufficient history to predict.\n    Computer will choose randomly." << endl;
            return randomChoice();
        }
        
        string bestPattern;
        int maxFreq = 0;
        string currentPattern;
        for (const auto& [h, c] : history) {
            currentPattern += h;
            currentPattern += c;
        }
        cout<< "Matching Patterns: \n";
        for (const auto &[pattern, count] : frequency) {

            if (pattern.substr(0, currentPattern.size()) == currentPattern && count > maxFreq) {
                cout << pattern << " " << count << endl;
                maxFreq = count;
                bestPattern = pattern;
            }
        }
        cout << "Best Pattern: " << bestPattern << endl;
        
        if (bestPattern.empty()) {
            cout << "    No matching pattern found.\n    Computer will choose randomly." << endl;
            return randomChoice();
        }
        
        char predictedMove = bestPattern[bestPattern.size() - 1];
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
    GameEngine(SmartComputer *const comp) : computer(comp) {}
    void playGame(int rounds = 5) {
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
    cout<<"\n\nWelcome to Rock, Paper, Scissors Game\n";
    cout<<"--------------------------------------\n";
    cout<<"\nRules of the game:\n";
    cout<<"------------------\n";
    cout<<"Rock beats Scissors\n";
    cout<<"Scissors beats Paper\n";
    cout<<"Paper beats Rock\n";
    cout<<"--------------------------------------\n";
    cout<<"There are 20 rounds in the game\n";
    cout<<"--------------------------------------\n";
    cout<<"Choose which Strategy that the computer should employ in the game: \n";
    cout<<"1. Smart Computer\n";
    cout<<"2. Random Computer\n";
    cout<<"Enter your choice (1 or 2): ";
    int choice;
    cin>>choice;
    switch (choice)
    {
        case 1:
        cout<<"Smart Computer Strategy is chosen\n";
        break;
        case 2:
        cout<<"Random Computer Strategy is chosen\n";
        break;
        default:
        cout<<"Invalid choice\n";
        break;
    }
    int patlen = 5;
    SmartComputer computer(patlen,choice);
    GameEngine game(&computer);
    game.playGame();
    return 0;
}
