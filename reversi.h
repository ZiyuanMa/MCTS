#pragma once
#include <vector>
#include <map>
#include <set>

enum class Player {
    white = 1,
    black = -1
};

class Environment {
public:
    Environment();
    ~Environment();
    void step(int action);
    bool is_done();
    void print_board();
    std::vector<int> get_legal_actions();
    float get_score();

private:
    Player player = Player::black;
    std::array<int, 64> board; // 1 is white, -1 is black
    // possible actions are positions around the pieces on board, easy to update
    // and we only need to search possible actions to get legal actions, instead of search the whole board
    std::set<int> possible_actions = {18, 19, 20, 21, 26, 29, 34, 37, 42, 43, 44, 45};
    std::vector<int> legal_actions = {19, 26, 44, 37};
    bool done = false;
    int steps = 0;
    std::map<Player, int> scores = {std::make_pair(Player::white, 2), std::make_pair(Player::black, 2)};

    void update_legal_actions();
    void update_possible_actions(int action);
    bool check_action(int action);
    bool check_direction(int position, int row_direc, int col_direc);
};