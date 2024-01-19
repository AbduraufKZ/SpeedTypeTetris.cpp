#include "raylib.h"//library for 3d and 2d games
#include <string>
#include <vector>
#include <ctime>
#include <cctype>
#include <fstream>//file handling
#include <iostream>// ithhought it would not be needed but.....

using namespace std;

struct Word {
    string text;
    Vector2 position;
    bool active;
};

struct UserData {
    string name;
    int highScore;
};
void LoadDataFromFile(UserData& currentUser) {
    //C:\\Users\\Asus\\Desktop\\2nd year\\cosc2\\final007\\final007
    ifstream file("UserData.txt");
    if (file.is_open()) {
        file >> currentUser.name;
        file >> currentUser.highScore; // Read high score separately
        file.close();

        cout << "Loaded user data: Name - " << currentUser.name << ", High Score - " << currentUser.highScore << endl;
    }
    else {
        cout << "No previous user data found." << endl;
    }
}

//void function to save the userdata to file 
void SaveUserDataToFile(const UserData& currentUser) {
//C:\\Users\\Asus\\Desktop\\2nd year\\cosc2\\final007\\final007
    ofstream file("UserData.txt");
    if (file.is_open()) {
        file << currentUser.name << " " << currentUser.highScore;
        file.close();

        cout << "User data saved: Name - " << currentUser.name << ", High Score - " << currentUser.highScore << endl;
    }
    else {
        cout << "Unable to save !" << endl;
    }
}

// to check weather the falling word typed correctly or not 
int IsTypedCorrectly(const string& typed, const vector<Word>& words) {
    for (int i = 0; i < words.size(); i++) {
        if (words[i].active && typed == words[i].text) {
            return i;
        }
    }
    return -1;
}



enum GameState {

    Start,
    NewUser,
    Instructions,
    Gameplay,
    Results,
};


int main() {//raylib library's functions
    //thankst to this guy who explained me raylib, for this programm i used the architecture of the tetris game made by//*//ttps://youtu.be/wVYKG_ch4yM?si=Ujs9pV0LL8Ofx10y// //*//
    const int screenWidth = 900;
    const int screenHeight = 900;
    // for the screen 
    InitWindow(screenWidth, screenHeight, "Typing Game");
    SetTargetFPS(60);// fps

    vector<Word> words;
    int score = 0;
    int lives = 3;
    string typedWord;
    bool newWordNeeded = true;
    int seconds = 3600;

    srand(time(nullptr));
    //location of the file with words: 
    vector<string> wordList;// to get the words from file
//C:\\Users\\Asus\\Desktop\\2nd year\\cosc2\\final007\\final007
    ifstream wordFile("words_list.txt");


    string line;

    if (wordFile.is_open()) {
        while (getline(wordFile, line)) {
            wordList.push_back(line);
        }
        wordFile.close();
    }

    int previousScore = 0;
    string playerName = "";
    GameState gameState = Start;
    UserData currentUser;



    previousScore = currentUser.highScore;


    LoadDataFromFile(currentUser);
    //1st page using switch case,

    if (currentUser.name != "") {
        playerName = currentUser.name;
        previousScore = currentUser.highScore;
    }
    playerName = "";
    while (!WindowShouldClose()) {

        switch (gameState) {
        case Start:
            ClearBackground(BLACK);

            DrawText(("Previous Username: " + currentUser.name).c_str(), 300, 150, 20, WHITE);
            DrawText(("Previous Score: " + to_string(previousScore)).c_str(), 360, 180, 20, WHITE);

            DrawText(playerName.c_str(), 400, 300, 20, GREEN);
            DrawText("Enter your name and press ENTER:", 280, 250, 20, GRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                if (!playerName.empty()) {
                    if (playerName != currentUser.name) {
                        currentUser.name = playerName;
                        currentUser.highScore = 0; // Resetting the score for a new user
                        SaveUserDataToFile(currentUser);
                    }
                    else {
                        LoadDataFromFile(currentUser);
                        previousScore = currentUser.highScore;
                    }
                    gameState = Instructions;
                }
            }
            else {
                int key = GetKeyPressed();
                if (key > 0 && key != KEY_ENTER && (IsKeyPressed(KEY_RIGHT_SHIFT) || IsKeyPressed(KEY_LEFT_SHIFT) || (key >= 32 && key <= 125))) {
                    playerName += static_cast<char>(key);
                }
                else if (IsKeyPressed(KEY_BACKSPACE) && playerName.length() > 0) {
                    playerName.pop_back();
                }
            }
            break;

            //2nd page of the programm.
        case Instructions:


            if (IsKeyPressed(KEY_ENTER)) {
                gameState = Gameplay;
                words.clear();
                score = 0;
                lives = 3;
                newWordNeeded = true;
            }
            break;
            //3rd page of the programm 
        case Gameplay:
            // Typing input handling
            if (IsKeyPressed(KEY_BACKSPACE) && !typedWord.empty()) {
                typedWord.pop_back();
            }
            else if (IsKeyPressed(KEY_ENTER)) {
                typedWord.clear();
            }
            else {
                for (int key = KEY_SPACE; key <= KEY_Z; key++) {
                    if (IsKeyPressed(key)) {

                        if (key == KEY_SPACE) {
                            typedWord += ' '; // Add a space to the typed word
                            continue;
                        }

                        // Convert typed character to lowercase if it's an alphabetical character
                        char typedChar = (IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT)) ? static_cast<char>(key) : static_cast<char>(tolower(key));
                        typedWord += typedChar;
                    }
                }
            }




            if (newWordNeeded) {
                Word word;
                // Generate a random word from the file
                if (!wordList.empty()) {
                    int randomIndex = rand() % wordList.size();
                    word.text = wordList[randomIndex];
                }
                else {
                    //  a default word if the word list is empty
                    word.text = "example";
                }
                word.position.x = rand() % (screenWidth - MeasureText(word.text.c_str(), 30));
                word.position.y = 0;
                word.active = true;
                words.push_back(word);
                newWordNeeded = false;
            }

            for (int i = 0; i < words.size(); i++) {
                if (words[i].active) {
                    words[i].position.y += 2;//speed of the falling words

                    if (words[i].position.y > screenHeight) {
                        words[i].active = false;
                        lives--;
                        newWordNeeded = true;
                    }

                    int index = IsTypedCorrectly(typedWord, words);
                    if (index != -1) {
                        words[index].active = false;
                        score++;
                        newWordNeeded = true;
                        typedWord.clear();
                    }
                }
            }

            if (seconds > 0) {
                seconds--;
            }

            if (seconds <= 0 || lives <= 0) {
                gameState = Results;
            }
            break;
            //4th page

        case Results:
            float wpm = static_cast<float>(score);
            int currentScore = static_cast<int>(wpm);

           
            DrawText(("Previous Score: " + to_string(currentUser.highScore)).c_str(), 360, 390, 20, WHITE);

            
            if (currentScore > currentUser.highScore) {
                currentUser.highScore = currentScore;
                SaveUserDataToFile(currentUser); // Save the updated high score
            }
            DrawText("Game Over", 360, 150, 40, RED);
            DrawText(("Your Score: " + to_string(score)).c_str(), 360, 250, 30, WHITE);
            DrawText("Press ENTER to Play Again", 310, 360, 20, GRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                gameState = Gameplay;
                words.clear();
                score = 0;
                lives = 3;
                seconds = 3600;
                typedWord.clear();
                newWordNeeded = true;
            }
            break;

            break;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        Color tetrisBlue = { 0, 128, 255, 255 }; // Blue, i defined the color to use

        switch (gameState) {
        case Start:
            ClearBackground(BLACK);

            DrawText("SPEED TYPE TEST", 250, 100, 40, tetrisBlue);

            break;
        case Instructions:
            DrawText("Instructions", 330, 150, 40, BLACK);
            DrawText("You have 3 lives and 1 min to type as many words as you can.", 130, 250, 20, WHITE);
            DrawText("Press ENTER to Play", 310, 400, 20, RED);
            break;

        case Gameplay:
            for (int i = 0; i < words.size(); i++) {
                if (words[i].active) {
                    DrawText(words[i].text.c_str(),
                        static_cast<int>(words[i].position.x),
                        static_cast<int>(words[i].position.y),
                        30,
                        WHITE);
                }
            }
            DrawText(typedWord.c_str(), 10, screenHeight - 40, 30, BLACK);

            DrawText(("Typed: " + typedWord).c_str(), 10, screenHeight - 90, 20, WHITE);
            DrawText(("Score: " + to_string(score)).c_str(), 10, 10, 20, WHITE);
            DrawText(("Lives " + to_string(lives)).c_str(), 10, 50, 20, RED);

            int hours = seconds / 3600;
            int minutes = (seconds % 3600) / 60;
            int remainingSeconds = seconds % 60;

            DrawText(("Time left: " + to_string(hours) + "m " + to_string(minutes) + "s " + to_string(remainingSeconds) + "ms").c_str(),
                screenWidth - 250, 10, 20, PINK);


            if (lives <= 0 || seconds <= 0) {
                gameState = Results;
            }
            break;

        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
