// implementation of Monte Carlo Tree Search (MCTS) with Upper
// Confidence Bounds for Trees (UCT) as tree policy
#pragma once
#include <map>
#include <vector>
#include <math.h>
#include <assert.h>

const float c_p = 1 / sqrt(2); // UCT exploration constant

template <class Game>
class Node {
public:
    Node(Node *parent_node, Game state) : parent(parent_node), current_state(state) {}

    ~Node() {
        for (const std::pair<int, Node*> &child : this->children) {
            delete child.second;
        }
    }

    Node* selection() {
        // select child recursively

        // unexpanded, return
        if (!this->expanded) {
            return this;
        }

        float max_uct_val = -1;
        int max_action = 0;
        float uct_val;
        // find child with max ucb value
        for (const std::pair<int, Node*> &child : this->children) {
            // select the child that never been visited before first
            if (child.second->num_visits == 0) {
                return child.second->selection();
            }

            uct_val = child.second->total_reward / child.second->num_visits + 
                        c_p * sqrt(2 * log(this->num_visits) / child.second->num_visits);

            if (uct_val > max_uct_val) {
                max_action = child.first;
                max_uct_val = uct_val;
            }
        }

        return this->children[max_action]->selection();
    }

    Node* expansion() {

        assert(!this->expanded);

        std::vector<int> legal_actions = this->current_state.get_legal_actions();

        //game terminated, no expansion and return itself
        if (legal_actions.size() == 0) {
            return this;
        }

        // expand child node with legal actions
        for (const int &action : legal_actions) {
            Game child_state = this->current_state;
            child_state.step(action);
            this->children.insert(std::pair<int, Node*>(action, new Node(this, child_state)));
        }

        this->expanded = true;

        int child_idx = legal_actions.at(rand() % legal_actions.size());

        return this->children.at(child_idx);
    }

    float simulation() {

        assert(!this->expanded);

        //policy: uniform random move
        Game simulate_state = this->current_state;
        std::vector<int> legal_actions;
        int action_idx;
        while (!simulate_state.is_done()) {
            legal_actions = simulate_state.get_legal_actions();
            action_idx = rand() % legal_actions.size();
            simulate_state.step(legal_actions[action_idx]);
        }

        return simulate_state.get_score();
    }


    void backpropagate(float reward) {
        ++this->num_visits;
        this->total_reward += reward;

        if (this->parent != nullptr)
            this->parent->backpropagate(-reward);
    }

    int best_action() {
        // select the best action w.r.t. the search results

        float max_value = 0;
        int best_action = 0;

        for (const std::pair<int, Node*> &child : this->children) {
            if (child.second->num_visits > max_value) {
                max_value = child.second->total_reward / child.second->num_visits;
                best_action = child.first;
            }
        }

        return best_action;
    }
    
private:
    
    Node *parent;
    std::map<int, Node*> children;
    bool expanded = false;
    Game current_state;
    float num_visits = 0;
    float total_reward = 0;
};


template <class Game> int search(Game init_state, int num_iters) {

    Node<Game> *root_node = new Node<Game>(nullptr, init_state);
    Node<Game> *leaf_node;
    float reward;

    for (int i = 0; i < num_iters; ++i) {
        leaf_node = root_node->selection();
        leaf_node = leaf_node->expansion();
        reward = leaf_node->simulation();
        leaf_node->backpropagate(reward);
    }

    int next_action = root_node->best_action();
    delete root_node;

    return next_action;
}