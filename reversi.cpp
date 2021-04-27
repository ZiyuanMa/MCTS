#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <array>
#include <map>
#include <assert.h>
#include "reversi.h"

// define 8 directions, used for direction check on board
const std::array<std::pair<int, int>, 8> directions = 
                    {std::make_pair(-1, -1), std::make_pair(-1, 0), std::make_pair(-1, 1),
                    std::make_pair(0, -1), std::make_pair(0, 1),
                    std::make_pair(1, -1), std::make_pair(1, 0), std::make_pair(1, 1)};


Environment::Environment() {
    // init board
    board.fill(0);
    board[27] = static_cast<int>(Player::white);
    board[36] = static_cast<int>(Player::white);
    board[28] = static_cast<int>(Player::black);
    board[35] = static_cast<int>(Player::black);
}

Environment::~Environment() {}

bool Environment::is_done() {
    return this->done;
}

void Environment::step(int action) {
    std::vector<int>::iterator iter = find(legal_actions.begin(), legal_actions.end(), action);

    assert(iter != legal_actions.end());

    // action 65 means no legal action currently, so skip this tern
    if (*iter == 65) {

        assert(legal_actions.size() == 1);

        this->player = this->player == Player::white ? Player::black : Player::white;
        this->update_legal_actions();

    } else {
        Player opponent = this->player == Player::white ? Player::black : Player::white;

        bool token = false;
        int row = action / 8;
        int column = action % 8;
        for (std::pair<int, int> direction : directions) {

            int next_row = row + direction.first;
            int next_column = column + direction.second;
            int step = 0;

            while (next_row >= 0 && next_column >= 0 && next_row <= 7 && next_column <= 7
                    && this->board[next_row*8+next_column] == static_cast<int>(opponent)) {
                next_row += direction.first;
                next_column += direction.second;
                ++step;
            }

            if (step != 0 && next_row >= 0 && next_column >= 0 && next_row <= 7 && next_column <= 7
                    && this->board[next_row*8+next_column] == static_cast<int>(this->player)) {
                for (int i = 0; i < step; ++i) {
                    next_row -= direction.first;
                    next_column -= direction.second;
                    assert (this->board[next_row*8+next_column] == static_cast<int>(opponent));
                    this->board[next_row*8+next_column] = static_cast<int>(this->player);
                    ++this->scores[this->player];
                    --this->scores[opponent];
                    token = true;
                }
            }
        }

        // assert (token);
        if (!token) {
            print_board();
            std::cout << action << std::endl;
            assert (token);
        }

        this->board[action] = static_cast<int>(this->player);
        ++this->scores[this->player];

        ++this->steps;

        this->update_possible_actions(action);

        if (this->steps == 60 || this->scores[Player::white] == 0 || this->scores[Player::black] == 0) {
            this->done = true;
        } else {
            this->player = this->player == Player::white ? Player::black : Player::white;
            this->update_legal_actions();
        }
    }
}

void Environment::update_possible_actions(int action) {
    int row = action / 8, column = action % 8;
    int around_row, around_column;
    for (const std::pair<int, int> &direction : directions) {
        around_row = row + direction.first;
        around_column = column + direction.second;
        if (around_row >= 0 && around_column >= 0 && around_row <= 7 && around_column <= 7) {
            this->possible_actions.insert(around_row * 8 + around_column);
        }
    }
    this->possible_actions.erase(action);
}

void Environment::update_legal_actions() {

    this->legal_actions.clear();

    if (this->steps == 60) {
        return;
    }

    // for each possible action, check it along 8 directions on board, if the action 
    // can change the board alone at least 1 direction, then its a legal action
    for (const int &action : this->possible_actions) {
        for (std::pair<int, int> direction : directions) {
            if (this->check_direction(action, direction.first, direction.second)) {
                this->legal_actions.push_back(action);
                break;
            }
        }
    }

    // no legal action, insert skip-the-round action
    if (this->legal_actions.size() == 0) {

        this->legal_actions.push_back(65);
    }

}

bool Environment::check_direction(int action, int row_direc, int col_direc){

    int opponent = this->player == Player::white ? -1 : 1;
    int row = action / 8 + row_direc;
    int column = action % 8 + col_direc;
    int steps = 0;

    while (row >= 0 && row <= 7 && column >= 0 && column <= 7 && board[row*8+column] == opponent) {
        steps += 1;
        row += row_direc;
        column += col_direc;
    }

    if (steps != 0 && row >= 0 && row <= 7 && column >= 0 && column <= 7
            && board[row*8+column] == static_cast<int>(this->player)) {
        return true;
    }else{
        return false;
    }

}

std::vector<int> Environment::get_legal_actions() {
    return this->legal_actions;
}

float Environment::get_score() {

    assert (this->done);

    Player opponent = this->player == Player::white ? Player::black : Player::white;
    int player_score = 0;
    int opponent_score = 0;

    for(const int &i : this->board){ 
        if (i == static_cast<int>(this->player)) {
            ++player_score;
        } else if (i == static_cast<int>(opponent)) {
            ++opponent_score;
        }               
    }


    if (player_score > opponent_score)
        return 1;
    else if (player_score < opponent_score)
        return 0;
    else
        return 0.5;

}


void Environment::print_board() {
    std::cout << "\n\n        0      1      2      3      4      5      6      7\n" << std::endl;
    for(int i = 0; i != 8; i++){
        char c = '0' + i;
        std::cout << "     ---------------------------------------------------------" << std::endl;
        std::cout << "     |      |      |      |      |      |      |      |      |" << std::endl;
        std::cout << "  " << c << "  "; 
        for(int j = 0; j != 8; ++j){
            char piece;
            if (board[i*8+j] == 1) {
                piece = 'O';
            } else if (board[i*8+j] == -1) {
                piece = 'X';
            } else {
                piece = ' ';
            }
            std::cout << "|  " << piece << "   ";
        }
            std::cout << "|" << std::endl;
            std::cout << "     |      |      |      |      |      |      |      |      |" << std::endl;
    }
    std::cout << "     ---------------------------------------------------------\n\n" << std::endl;

}