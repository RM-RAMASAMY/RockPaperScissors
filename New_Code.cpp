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

class freq_filehander{
    public:
    map<string, int> frequency;
    void saveFrequencies() {
        ofstream file("frequencies.txt");
        for (const auto &entry : frequency) {
            file << entry.first << " " << entry.second << endl;
        }
        file.close();
        cout << "Writing frequency file frequency.txt: " << frequency.size() << " records." << endl;
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
    void updateFrequency(vector<char> pattern) {
        string patternStr(pattern.begin(), pattern.end());
        frequency[patternStr]++;
        cout << "    Updated " << patternStr << " to " << frequency[patternStr] << endl;
    }
};

class ComputerPlayer : public Player, public freq_filehander {
protected:
    vector<char> history;
    int memorySize;
    int mode;
    
public:
    ComputerPlayer(int memSize = 5, int Mode=1) : memorySize(memSize), mode(Mode) {
        srand(time(0));
        loadFrequencies();
    }
    virtual char makeChoice() override = 0;
    void updateHistory(char choice) {
        history.emplace_back(choice);
        if(history.size() > memorySize) {
            history.erase(history.begin());
        }
        printHist();
        if (history.size() == memorySize) {
            updateFrequency(history);
        }
    }
    char randomChoice() {
        char choices[] = {'R', 'P', 'S'};
        return choices[rand() % 3];
    }
    char counterMove(char move) {
        if (move == 'R') return 'P';
        if (move == 'P') return 'S';
        return 'R';
    }
    void printHist()
    {
        cout << "  History: "<<string(history.begin(), history.end()) << endl;
        
    }
};

class SmartComputer : public ComputerPlayer {
public:
    SmartComputer(int MemSize = 5) {
        ComputerPlayer::memorySize = MemSize;
        ComputerPlayer::mode = 1;
    }
    char makeChoice() override {
        if (history.size() < memorySize) {
            cout << "    Insufficient history to predict.\n    Computer will choose randomly." << endl;
            return randomChoice();
        }
        
        string bestPattern;
        int maxFreq = 0;
        vector<char> options = {'R', 'P', 'S'};
        string currentPattern(history.begin(), history.end());
        currentPattern.erase(currentPattern.begin());
        cout<< "Checked: \n";
        for(char option : options){
            string potentialPattern = currentPattern+option;
            if(frequency.find(potentialPattern) == frequency.end()){
                cout<<potentialPattern<<": 0"<<endl;
            }
            else{
                cout<<potentialPattern<<": "<<frequency[potentialPattern]<<endl;
                if(frequency[potentialPattern]>maxFreq){
                    maxFreq = frequency[potentialPattern];
                    bestPattern = potentialPattern;
                }
            }
        }

        if (bestPattern.empty()) {
            cout << "    No matching pattern found.\n    Computer will choose randomly." << endl;
            return randomChoice();
        }
        cout << "Best Pattern: " << bestPattern << endl;
        
        char predictedMove = bestPattern[bestPattern.size() - 1];
        cout << "    Predicted human choice: " << (predictedMove == 'R' ? "ROCK" : predictedMove == 'P' ? "PAPER" : "SCISSORS") << endl;
        return counterMove(predictedMove);
    }
};

class RandomComputer : public ComputerPlayer {
public:
    RandomComputer(int MemSize = 5) {
        ComputerPlayer::memorySize = MemSize;
        ComputerPlayer::mode = 2;
}
    char makeChoice() override {
        cout << "    Computer will choose randomly." << endl;
        return randomChoice();
    }
};

class GameEngine {
private:
    HumanPlayer human;
    ComputerPlayer *computer;
    int humanScore = 0, computerScore = 0, ties = 0;
public:
    GameEngine(ComputerPlayer *const comp){
        computer = comp;
    }
    void playGame(int rounds = 5) {
        for (int i = 0; i < rounds; ++i) {
            cout << "\nRound " << i + 1 << endl;
            computer->printHist();
            char humanChoice = human.makeChoice();
            char computerChoice = computer->makeChoice();
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
            
            computer->updateHistory(humanChoice);
            computer->updateHistory(computerChoice);
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
    int choice;
    do{
        cout<<"Choose which Strategy that the computer should employ in the game: \n";
        cout<<"1. Smart Computer\n";
        cout<<"2. Random Computer\n";
        cout<<"Enter your choice (1 or 2): ";
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
    }while (choice!=1 && choice!=2);
 
    int patlen = 5;
    ComputerPlayer* computer;
    if (choice == 1) {
        computer = new SmartComputer(patlen);
    } else {
        computer = new RandomComputer(patlen);
    }
    GameEngine game(computer);
    game.playGame();
    return 0;
}
