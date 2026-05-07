#include "GRAPH_SEARCH.h"
#include "data_types.h"
#include <stdio.h>
#include <stdlib.h>

/* ============================================================================
 * KNOWN_LIMITATIONS — bugs detected in PDF-locked files (NOT modified here).
 *
 * B1) Standart_Search.c :: Remove_Node_From_Frontier
 *      - prev_queue is read on the first non-match iteration before being
 *        initialized → undefined behaviour when removing a non-head node.
 *      - After matching, the function does not return, so the loop keeps
 *        iterating with curr_queue dangling.
 *      - The matched queue cell is never free()'d → memory leak.
 *
 * B2) Standart_Search.c :: Compare_States
 *      - strcmp() is called with unsigned char[] keys → signedness warning.
 *
 * B3) Standart_Search.c :: DepthType_Search_TREE
 *      - Number_Searched_Nodes / Number_Generated_Nodes / Number_Allocated_Nodes
 *        are declared `static`. Iterative-Deepening calls this function once
 *        per depth limit, so the counters accumulate across calls and the
 *        reported numbers are not per-iteration counts.
 *
 * B4) GRAPH_SEARCH.c :: main
 *      - Max_Level is read uninitialized when method != DepthLimitedSearch
 *        and is then passed to DepthType_Search_TREE for IterativeDeepening
 *        is fine (overwritten in the loop) but for DepthFirstSearch it is
 *        passed unread → harmless but raises a warning.
 *
 * Multiple [-Wpointer-sign] warnings remain in HashTable.c (ht_insert,
 * ht_insert_key, ht_search, Resize_Hash_Table) and Standart_Search.c
 * (Compare_States, Insert_FIFO empty body) for the same locked-file reason.
 * ========================================================================= */

int MAZE[MAZE_ROWS_50][MAZE_COLS_50];
int MAZE_ROWS;
int MAZE_COLS;
int CURRENT_MAZE;

static const int MAZE_1_DATA[MAZE_ROWS_20][MAZE_COLS_20] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

static int MAZE_2_DATA[MAZE_ROWS_50][MAZE_COLS_50];

static void Build_Maze_2(void)
{
    int r, c;
    for(r=0; r<MAZE_ROWS_50; r++)
        for(c=0; c<MAZE_COLS_50; c++)
            MAZE_2_DATA[r][c] = 0;

    for(c=0; c<MAZE_COLS_50; c++){
        MAZE_2_DATA[0][c] = 1;
        MAZE_2_DATA[MAZE_ROWS_50-1][c] = 1;
    }
    for(r=0; r<MAZE_ROWS_50; r++){
        MAZE_2_DATA[r][0] = 1;
        MAZE_2_DATA[r][MAZE_COLS_50-1] = 1;
    }

    for(r=2; r<=46; r+=2){
        for(c=1; c<MAZE_COLS_50-1; c++)
            MAZE_2_DATA[r][c] = 1;
        if((r/2) % 2 == 1){
            MAZE_2_DATA[r][8]  = 0;
            MAZE_2_DATA[r][16] = 0;
            MAZE_2_DATA[r][24] = 0;
            MAZE_2_DATA[r][32] = 0;
            MAZE_2_DATA[r][40] = 0;
            MAZE_2_DATA[r][48] = 0;
        } else {
            MAZE_2_DATA[r][1]  = 0;
            MAZE_2_DATA[r][12] = 0;
            MAZE_2_DATA[r][20] = 0;
            MAZE_2_DATA[r][28] = 0;
            MAZE_2_DATA[r][36] = 0;
            MAZE_2_DATA[r][44] = 0;
        }
    }

    for(r=3; r<=45; r+=4){
        MAZE_2_DATA[r][6]  = 1;
        MAZE_2_DATA[r][14] = 1;
        MAZE_2_DATA[r][22] = 1;
        MAZE_2_DATA[r][30] = 1;
        MAZE_2_DATA[r][38] = 1;
    }
    for(r=5; r<=45; r+=4){
        MAZE_2_DATA[r][10] = 1;
        MAZE_2_DATA[r][18] = 1;
        MAZE_2_DATA[r][26] = 1;
        MAZE_2_DATA[r][34] = 1;
        MAZE_2_DATA[r][42] = 1;
    }
}

void Init_Maze(int maze_type)
{
    int i, j;
    static int maze2_built = 0;

    if(maze_type == 1){
        MAZE_ROWS = MAZE_ROWS_20;
        MAZE_COLS = MAZE_COLS_20;
        CURRENT_MAZE = 1;
        for(i=0; i<MAZE_ROWS; i++)
            for(j=0; j<MAZE_COLS; j++)
                MAZE[i][j] = MAZE_1_DATA[i][j];
    } else {
        if(!maze2_built){
            Build_Maze_2();
            maze2_built = 1;
        }
        MAZE_ROWS = MAZE_ROWS_50;
        MAZE_COLS = MAZE_COLS_50;
        CURRENT_MAZE = 2;
        for(i=0; i<MAZE_ROWS; i++)
            for(j=0; j<MAZE_COLS; j++)
                MAZE[i][j] = MAZE_2_DATA[i][j];
    }
}

void Print_Maze_Visual(const State *const start, const State *const goal)
{
    int i, j;
    printf("\n=========== MAZE VISUAL ===========\n");
    for(i=0; i<MAZE_ROWS; i++){
        for(j=0; j<MAZE_COLS; j++){
            if(i==start->row && j==start->col)        printf("S ");
            else if(i==goal->row  && j==goal->col)    printf("G ");
            else if(MAZE[i][j]==1)                    printf("# ");
            else                                      printf(". ");
        }
        printf("\n");
    }
    printf("===================================\n");
}

void Print_Maze_With_Solution(Node *const goal, const State *const start, const State *const finish)
{
    int i, j;
    int path[MAZE_ROWS_50][MAZE_COLS_50] = {{0}};
    Node *temp;

    if(goal == FAILURE){
        printf("\nNO PATH TO DRAW (solution not found).\n");
        return;
    }

    for(temp = goal; temp != NULL; temp = temp->parent)
        path[temp->state.row][temp->state.col] = 1;

    printf("\n========== MAZE WITH SOLUTION ==========\n");
    for(i=0; i<MAZE_ROWS; i++){
        for(j=0; j<MAZE_COLS; j++){
            if(i==start->row && j==start->col)         printf("S ");
            else if(i==finish->row && j==finish->col)  printf("G ");
            else if(path[i][j]==1)                     printf("* ");
            else if(MAZE[i][j]==1)                     printf("# ");
            else                                       printf(". ");
        }
        printf("\n");
    }
    printf("========================================\n");
}

State* Create_State()
{
    State *state = (State*)malloc(sizeof(State));
    int row, col;
    int maze_choice;

    if(CURRENT_MAZE == 0){
        printf("Select maze (1 = 20x20 inner, 2 = 50x50 inner): ");
        scanf("%d", &maze_choice);
        if(maze_choice != 1 && maze_choice != 2){
            printf("Invalid maze choice. Defaulting to 1.\n");
            maze_choice = 1;
        }
        Init_Maze(maze_choice);
    }

    while(1){
        printf("Enter row (1-%d): ", MAZE_ROWS-2);
        scanf("%d", &row);
        printf("Enter col (1-%d): ", MAZE_COLS-2);
        scanf("%d", &col);

        if(row < 0 || row >= MAZE_ROWS || col < 0 || col >= MAZE_COLS){
            printf("Out of bounds. Try again.\n");
            continue;
        }
        if(MAZE[row][col] == 1){
            printf("That cell is a wall. Choose another position.\n");
            continue;
        }
        break;
    }

    state->row = row;
    state->col = col;
    state->h_n = 0;

    return state;
}

void Print_State(const State *const state)
{
    printf("(%d, %d)", state->row, state->col);
}

void Print_Action(const enum ACTIONS action)
{
    switch(action){
        case Move_Up:    printf("Move_Up");    break;
        case Move_Down:  printf("Move_Down");  break;
        case Move_Left:  printf("Move_Left");  break;
        case Move_Right: printf("Move_Right"); break;
        default:         printf("Unknown");
    }
}

int Result(const State *const parent_state,
           const enum ACTIONS action,
           Transition_Model *const trans_model)
{
    int new_row = parent_state->row;
    int new_col = parent_state->col;

    switch(action){
        case Move_Up:    new_row--; break;
        case Move_Down:  new_row++; break;
        case Move_Left:  new_col--; break;
        case Move_Right: new_col++; break;
    }

    if(new_row < 0 || new_row >= MAZE_ROWS) return FALSE;
    if(new_col < 0 || new_col >= MAZE_COLS) return FALSE;
    if(MAZE[new_row][new_col] == 1)         return FALSE;

    trans_model->new_state.row = new_row;
    trans_model->new_state.col = new_col;
    trans_model->new_state.h_n = 0;
    trans_model->step_cost     = 1.0;

    return TRUE;
}

float Compute_Heuristic_Function(const State *const state,
                                  const State *const goal)
{
    return (float)(abs(state->row - goal->row) +
                   abs(state->col - goal->col));
}

int Goal_Test(const State *const state, const State *const goal_state)
{
    if(PREDETERMINED_GOAL_STATE)
        return Compare_States(state, goal_state);
    else
        return 0;
}
