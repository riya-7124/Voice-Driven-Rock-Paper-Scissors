#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <algorithm>

using namespace std;

map<string, int> leaderboard;

void loadLeaderboard(const string& filename) {
    ifstream infile(filename);
    string name;
    int wins;
    while (infile >> name >> wins) {
        leaderboard[name] = wins;
    }
    infile.close();
}

void saveLeaderboard(const string& filename) {
    ofstream outfile(filename);
    for (const auto& entry : leaderboard) {
        outfile << entry.first << " " << entry.second << endl;
    }
    outfile.close();
}

string getComputerChoice() {
    string choices[] = {"rock", "paper", "scissors"};
    return choices[rand() % 3];
}

int determineWinner(const string& user, const string& computer) {
    if (user == computer) return 0;
    if ((user == "rock" && computer == "scissors") ||
        (user == "paper" && computer == "rock") ||
        (user == "scissors" && computer == "paper")) {
        return 1;
    }
    return -1;
}

string getVoiceInput() {
    string last_line;
    array<char, 128> buffer;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen("python get_voice_input.py", "r"), pclose);
    if (!pipe) {
        cerr << "Error: Failed to open pipe to Python script!" << endl;
        return "invalid";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        last_line = buffer.data();  // Update last_line with each new line
    }

    // Clean the last line (remove newlines, carriage returns, and extra spaces)
    last_line.erase(remove(last_line.begin(), last_line.end(), '\n'), last_line.end());
    last_line.erase(remove(last_line.begin(), last_line.end(), '\r'), last_line.end());
    last_line.erase(remove_if(last_line.begin(), last_line.end(), ::isspace), last_line.end());

    // Debug output
    cout << "Parsed voice input: '" << last_line << "'" << endl;

    if (last_line.empty() || last_line == "invalid" ||
        (last_line != "rock" && last_line != "paper" && last_line != "scissors")) {
        return "invalid";
    }
    return last_line;
}

void displayLeaderboard() {
    cout << "\n🏆 Leaderboard:\n";
    for (const auto& entry : leaderboard) {
        cout << entry.first << ": " << entry.second << " wins\n";
    }
    cout << endl;
}

int main() {
    srand(time(0));
    const string leaderboardFile = "leaderboard.txt";
    loadLeaderboard(leaderboardFile);

    string username;
    cout << "Enter your username: ";
    cin >> username;

    if (leaderboard.find(username) == leaderboard.end()) {
        leaderboard[username] = 0;
        cout << "Welcome, " << username << "! Your score will be tracked.\n";
    } else {
        cout << "Welcome back, " << username << "! Current wins: " << leaderboard[username] << "\n";
    }

    char playAgain;
    do {
        cout << "\n🎮 Choose your move (say 'rock', 'paper', or 'scissors' into your microphone)...\n";
        string userChoice;
        int max_attempts = 3;
        for (int attempt = 0; attempt < max_attempts; attempt++) {
            userChoice = getVoiceInput();
            if (userChoice != "invalid") {
                cout << "You chose: " << userChoice << endl;
                break;  // Valid input received
            }
            cout << "Invalid input. Please speak clearly and say 'rock', 'paper', or 'scissors' (" 
                 << (max_attempts - attempt - 1) << " attempts left).\n";
            if (attempt == max_attempts - 1) {
                cout << "Voice input failed. Enter rock, paper, or scissors manually: ";
                cin >> userChoice;
            }
        }

        if (userChoice != "rock" && userChoice != "paper" && userChoice != "scissors") {
            cout << "Invalid input. Skipping this round.\n";
            continue;
        }

        string computerChoice = getComputerChoice();
        cout << "Computer chose: " << computerChoice << endl;

        int result = determineWinner(userChoice, computerChoice);
        if (result == 1) {
            cout << "You win this round!\n";
            leaderboard[username]++;
        } else if (result == -1) {
            cout << "Computer wins this round.\n";
        } else {
            cout << "It's a tie.\n";
        }

        cout << "Play again? (y/n): ";
        cin >> playAgain;
    } while (playAgain == 'y' || playAgain == 'Y');

    saveLeaderboard(leaderboardFile);
    displayLeaderboard();

    return 0;
}