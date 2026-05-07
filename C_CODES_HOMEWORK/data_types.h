#ifndef DATA_TYPES_H
#define DATA_TYPES_H

enum ACTIONS
{
    Move_Up, Move_Down, Move_Left, Move_Right
};

typedef struct State
{
    int row;
    int col;
    float h_n;
}State;

enum METHODS
{
	BreastFirstSearch = 1,   UniformCostSearch = 2,        DepthFirstSearch = 3,
	DepthLimitedSearch= 4,   IterativeDeepeningSearch = 5, GreedySearch = 6,
    AStarSearch = 7, GeneralizedAStarSearch = 8
};

typedef struct Transition_Model
{
    State new_state;
    float step_cost;
}Transition_Model;

typedef struct Node
{
    State state;
    float path_cost;
    enum ACTIONS action;
    struct Node *parent;
    int Number_of_Child;
}Node;

typedef struct Queue
{
    Node *node;
    struct Queue *next;
}Queue;

#endif
