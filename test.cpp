// test environment, function is still not complete
#include <iostream>
#include "reversi.h"
#include "mcts.h"

int main() {
    Environment reversi;
    int action, row, column;

    while (!reversi.is_done())
    {
        action = search(reversi, 200);
        std::cout << action << std::endl;
        reversi.step(action);
        reversi.print_board();

        std::cout << "input action row and column: " << std::endl;
        std::cin >> row >> column;
        reversi.step(row*8+column);
        reversi.print_board();
    }
}