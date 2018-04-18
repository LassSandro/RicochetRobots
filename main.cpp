#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>

#define SIZER 4
#define SIZEC 4

#define ROBOTS 3

// Input grid
/* Each 1 is a wall tile
 * Each 0 is a free moving tile
 * Each -1 is a tile in which the player has already been

  {0, 1, 0, 0}
  {0, 1, 0, 0}
  {0, 0, 0, 1}
  {1, 0, 0, 0}

  . / . .
  . / . .
  . . . /
  / . . .
*/
int grid[SIZER][SIZEC] = {
  /*{0, 0, 1, 1},
  {1, 0, 1, 1},
  {1, 0, 0, 0},
  {1, 0, 0, 0}*/
  /*{0, 0, 1, 0, 0},
  {1, 0, 1, 0, 1},
  {0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0},
  {1, 0, 1, 1, 0}*/
  {0, 0, 0, 1},
  {0, 0, 1, 0},
  {0, 0, 0, 0},
  {0, 1, 0, 0}
};

// Struct for a position in the grid
// r (row) and c (column) coordinates
struct pos {
    int r;
    int c;
};

// Pair grid/position
struct gridPos {
    int grid[SIZER][SIZEC];/* = {
        {0, 0, 0, 0},
        {1, 0, 1, 0},
        {1, 1, 0, 0}
      };*/
    pos position;
};

struct moving {
    std::vector<gridPos> moves;
    std::vector<std::string> dir;
};

struct dirMove {
    gridPos moves;
    std::string direction;
    int who;
};

// Player staring position in the grid
pos staring;

// Winning position in the grid
pos ending;

// Store initial robots position
pos robots[ROBOTS];

// Final moves data
std::vector<dirMove> minMoves;

// Final min moves result of the algorithm
int min = 9999999;


void findMin(gridPos gp, gridPos gpRobot[], int layer, int whichRobot, std::vector<dirMove> storeMoves, bool singleMove, int printLayerSpace);
void changeRobotPosition(gridPos gp, gridPos gpRobot[], int layer, int whichRobot, std::vector<dirMove> storeMoves, int printLayerSpace);


/* PRINTS */
void printLayer(int layer) {
    for (int l = 0; l < layer; ++l) {
        std::cout << "  ";
    }
}

// Print the grid matrix
void printMatrix(int grid[][SIZEC], int layer) {
    int robots = 1;
    for (int i = 0; i < SIZER; ++i) {
        printLayer(layer);

        for (int j = 0; j < SIZEC; ++j) {
            if (grid[i][j] == 0)
                std::cout << std::setw(2) << ".";
            else if (grid[i][j] == -1)
                std::cout << std::setw(2) << "x";
            else if (grid[i][j] == 2) {
                std::cout << std::setw(2) << robots;
                ++robots;
            }
            else
                std::cout << std::setw(2) << "-";
        }
        std::cout << std::endl;
    }
}

// Print pos object
void printPos(pos position) {
    std::cout << "Pos: (" << position.r << ", " << position.c << ")" << std::endl;
}

// Print directions
void printDirs(const std::vector<std::string>& dirs) {
    for (size_t i = 0; i < dirs.size(); ++i) {
        std::cout << dirs[i] << " ";
    }

    std::cout << std::endl;
}

/* SIDE FUNCTIONS */
// copied = toCopy values
void copyVector(const std::vector<gridPos>& toCopy, std::vector<gridPos>& copied) {
    for (size_t i = 0; i < toCopy.size(); ++i) {
        copied.push_back(toCopy[i]);
    }
}

void copyMatrix(const int toCopy[][SIZEC], int copied[][SIZEC]) {
    for (int r = 0; r < SIZER; ++r) {
        for (int c = 0; c < SIZEC; ++c) {
            copied[r][c] = toCopy[r][c];
        }
    }
}

void copyGridPosArray(const gridPos toCopy[], gridPos copied[]) {
    for (int rbt = 0; rbt < ROBOTS; ++rbt) {
        copied[rbt] = toCopy[rbt];
    }
}

bool samePos(const pos& pos1, const pos& pos2) {
    if (pos1.r == pos2.r && pos1.c == pos2.c) {
        return true;
    }

    return false;
}

/* CHECK POSITIONS
 * do not go outside the grid
 * do not collide with a wall or another robot or player
 * stop if you step on another already visited cell (not the starting one) because there is a faster way to reach the same position
*/
// Check position: column + 1
void toRight(pos& newPos, const gridPos& gp) {
    while (newPos.c + 1 < SIZEC &&
           (gp.grid[newPos.r][newPos.c + 1] == 0 || gp.grid[newPos.r][newPos.c + 1] == -1) &&
           (gp.grid[newPos.r][newPos.c] != -1 || samePos(newPos, gp.position))) {
        ++newPos.c;
    }
}

// Check position: column - 1
void toLeft(pos& newPos, const gridPos& gp) {
    while (newPos.c - 1 >= 0 &&
           (gp.grid[newPos.r][newPos.c - 1] == 0 || gp.grid[newPos.r][newPos.c - 1] == -1) &&
           (gp.grid[newPos.r][newPos.c] != -1 || samePos(newPos, gp.position))) {
        --newPos.c;
    }
}

// Check position: row + 1
void toBottom(pos& newPos, const gridPos& gp) {
    while (newPos.r + 1 < SIZER &&
           (gp.grid[newPos.r + 1][newPos.c] == 0 || gp.grid[newPos.r + 1][newPos.c] == -1) &&
           (gp.grid[newPos.r][newPos.c] != -1 || samePos(newPos, gp.position))) {
        ++newPos.r;
    }
}

// Check position: row - 1
void toTop(pos& newPos, const gridPos& gp) {
    while (newPos.r - 1 >= 0 &&
           (gp.grid[newPos.r - 1][newPos.c] == 0 || gp.grid[newPos.r - 1][newPos.c] == -1) &&
           (gp.grid[newPos.r][newPos.c] != -1 || samePos(newPos, gp.position))) {
        --newPos.r;
    }
}

void toDir(pos& newPos, int dir, const gridPos& gp) {
    newPos = gp.position;

    switch(dir) {
    case 1:
        toRight(newPos, gp);
        break;
    case 2:
        toBottom(newPos, gp);
        break;
    case 3:
        toLeft(newPos, gp);
        break;
    case 4:
        toTop(newPos, gp);
        break;
    }
}

void saveMove(const gridPos& gp, int dir, std::vector<dirMove>& storeMoves, int nowIndex) {
    dirMove toStore;

    toStore.moves = gp;
    toStore.who = nowIndex;

    switch(dir) {
    case 1:
        toStore.direction = "RIGHT";
        break;
    case 2:
        toStore.direction = "BOTTOM";
        break;
    case 3:
        toStore.direction = "LEFT";
        break;
    case 4:
        toStore.direction = "TOP";
        break;
    }

    storeMoves.push_back(toStore);
}

void setVisited(gridPos& gp, const pos& newPos) {
    // Update current tile as already visited
    gp.grid[newPos.r][newPos.c] = -1;
}

void updateMin(int layer, int dir, gridPos& gp, std::vector<dirMove>& storeMoves) {
    if (layer <= min) {
        min = layer;

        setVisited(gp, gp.position);
        saveMove(gp, dir, storeMoves, 0);

        minMoves = storeMoves;

        // Print result
        printLayer(layer);
        std::cout << "FOUND MIN: " << layer << std::endl;
        std::cout << std::endl;
    }
}

void updatePos(gridPos& gp, const pos& newPos) {
    setVisited(gp, newPos);

    // Update current position of the player
    gp.position = newPos;
}





// Main function, check possible moves in four directions
// 1: Right
// 2: Bottom
// 3: Left
// 4: Top
gridPos goDir(gridPos gp, gridPos& gpRobot, int layer, int dir, std::vector<dirMove>& storeMoves) {
    // Create 'NULL' pos to return when end reached (base case)
    gridPos nullReturn;
    nullReturn.position.r = -1;

    // If the moves exceed the current min, return directly
    if (layer > min) {
        printLayer(layer);
        std::cout << "Ply Out of moves" << std::endl;
        return nullReturn;
    }

    // newPos = coordinates where the player is after he moves
    pos newPos;
    toDir(newPos, dir, gp);

    // Exit without success
    /* IF
     * Still in the same tile as before you moved (no moves allowed)
     * Get to an already visited tile
    */
    if (samePos(newPos, gp.position) ||
         gp.grid[newPos.r][newPos.c] == -1) {
        return nullReturn;
    }

    // Exit with success
    /* IF
     * Position reached is the end tile
    */
    if (samePos(newPos, ending)) {
        updatePos(gp, newPos);

        // If you used the least possible moves found save as new best min
        updateMin(layer, dir, gp, storeMoves);

        printLayer(layer);
        printPos(gp.position);
        printMatrix(gp.grid, layer);

        //nullReturn.position.r = -2;

        return nullReturn;
    }

    // Update player's position in the robot gird
    gpRobot.grid[gp.position.r][gp.position.c] = 0;
    gpRobot.grid[newPos.r][newPos.c] = 2;


    // Update current position and push back moves and dirs
    updatePos(gp, newPos);
    saveMove(gp, dir, storeMoves, 0);

    // gp with the new position and -1 in that position,
    // as well as -1 in all the previous visited positions
    return gp;
}

gridPos goDirRobot(gridPos gpRobot, gridPos& gp, int layer, int dir, std::vector<dirMove>& storeMoves) {
    // Create 'NULL' pos to return when end reached (base case)
    gridPos nullReturn;
    nullReturn.position.r = -1;

    // If the moves exceed the current min, return directly
    if (layer > min) {
        printLayer(layer);
        std::cout << "Rbt Out of moves" << std::endl;
        return nullReturn;
    }

    // Create and assign value to temp pos newPos based on the direction in which to go
    pos newPos;
    toDir(newPos, dir, gpRobot);


    // Exit without success
    /* IF
     * Still in the same tile as before you moved (no moves allowed)
     * Get to an already visited tile
    */
    if (samePos(newPos, gpRobot.position) ||
         gpRobot.grid[newPos.r][newPos.c] == -1) {
        return nullReturn;
    }

    // Update robot's position in the player gird
    gp.grid[gpRobot.position.r][gpRobot.position.c] = 0;
    gp.grid[newPos.r][newPos.c] = 2;

    /*std::cout << "Got here" << std::endl;
    printPos(newPos);
    printMatrix(gp.grid, 0);*/

    // Update current position and push back moves and dirs
    updatePos(gpRobot, newPos);
    saveMove(gpRobot, dir, storeMoves, 1);


    return gpRobot;
}

gridPos moveDir(std::vector<gridPos>& gpRobot, gridPos& gpPlayer, int layer, int playerIndex, int nowIndex, int dir, std::vector<dirMove>& storeMoves) {
    // Create 'NULL' pos to return when end reached (base case)
    gridPos nullReturn;
    nullReturn.position.r = -1;

    // If the moves exceed the current min, return directly
    if (layer > min) {
        //printLayer(layer);
        //std::cout << "Rbt Out of moves" << std::endl;
        return nullReturn;
    }

    // Create and assign value to temp pos newPos based on the direction in which to go to the first robot
    pos newPos;
    toDir(newPos, dir, gpRobot[0]);


    // Exit without success
    /* IF
     * Still in the same tile as before you moved (no moves allowed)
     * Get to an already visited tile
    */
    if (samePos(newPos, gpRobot[0].position) ||
         gpRobot[0].grid[newPos.r][newPos.c] == -1) {
        return nullReturn;
    }

    // Exit with success
    /* IF
     * Position reached is the end tile
    */
    if (samePos(newPos, ending) && playerIndex == nowIndex) {
        updatePos(gpRobot[0], newPos);

        // If you used the least possible moves found save as new best min
        updateMin(layer, dir, gpRobot[0], storeMoves);

        printLayer(layer);
        printPos(gpRobot[0].position);
        printMatrix(gpRobot[0].grid, layer);

        nullReturn.position.r = 1;
        return nullReturn;
    }

    // Update robot's position in the other robots gird
    for (size_t others = 1; others < gpRobot.size(); ++others) {
        gpRobot[others].grid[gpRobot[0].position.r][gpRobot[0].position.c] = 0;
        gpRobot[others].grid[newPos.r][newPos.c] = 2;
    }

    if (playerIndex != nowIndex) {
        gpPlayer.grid[gpRobot[0].position.r][gpRobot[0].position.c] = 0;
        gpPlayer.grid[newPos.r][newPos.c] = 2;
    }


    // Update current position and push back moves and dirs
    updatePos(gpRobot[0], newPos);
    saveMove(gpRobot[0], dir, storeMoves, nowIndex);

    // Success return
    nullReturn.position.r = 0;
    return nullReturn;
}

void findMin(gridPos gp, gridPos gpRobot[], int layer, int whichRobot, std::vector<dirMove> storeMoves, bool singleMove, int printLayerSpace) {
    // Visit all the four directions (1, 2, 3, 4)
    for (int d = 1; d <= 4; ++d) {
        gridPos tempArray[ROBOTS];
        copyGridPosArray(gpRobot, tempArray);

        gridPos temp = tempArray[whichRobot];
        gridPos direction = goDir(gp, temp, layer + 1, d, storeMoves);

        // Found where to go
        if (direction.position.r != -1) {
            // If is a single move (recursive loop)
            /*std::cout << "Pause player before singleMove";
            //std::cin.get();*/

            if(singleMove) {
                /** Print passed grid **/
                /*std::cout << "Print passed singleMove grid" << std::endl;
                printPos(gp.position);
                printMatrix(gp.grid, 0);
                printPos(gpRobot[0].position);
                printMatrix(gpRobot[0].grid, 0);
                printPos(gpRobot[1].position);
                printMatrix(gpRobot[1].grid, 0);*/

                gridPos newGp;
                copyMatrix(grid, newGp.grid);
                newGp.position = gp.position;
                newGp.grid[gp.position.r][gp.position.c] = 2;
                // Move the robot with the pervious player position
                changeRobotPosition(newGp, gpRobot, layer, whichRobot, storeMoves, printLayerSpace + 1);

                std::cout << "NEXT PLAYER GOES" << std::endl;
                // Matrix with -1 only on the new position of the player
                /*gridPos newPlayerGp;
                copyMatrix(grid, newPlayerGp.grid);
                newPlayerGp.grid[direction.position.r][direction.position.c] = -1;
                newPlayerGp.position = direction.position;
                std::cout << "\n" << std::endl;*/
            }

            /* PRINTS */
            printLayer(printLayerSpace);
            std::cout << "Player with Robot " << whichRobot << ": d, " << d << " - m, " << (layer + 1) << std::endl;
            printLayer(printLayerSpace);
            printPos(direction.position);
            printMatrix(direction.grid, printLayerSpace);

            // Next move of the player
            findMin(direction, tempArray, layer + 1, whichRobot, storeMoves, singleMove, printLayerSpace);

            storeMoves.pop_back();
        }
    }
}

void changeRobotPosition(gridPos gp, gridPos gpRobot[], int layer, int whichRobot, std::vector<dirMove> storeMoves, int printLayerSpace) {
    for (int d = 1; d <= 4; ++d) {
        // Using a temp variable
        // In the next run of the cycle I need the original gp variable, I use temp to run the recursive
        gridPos tempArray[ROBOTS];
        copyGridPosArray(gpRobot, tempArray);

        gridPos temp = gp;
        gridPos direction = goDirRobot(gpRobot[whichRobot], temp, layer + 1, d, storeMoves);

        // Found where to go
        if (direction.position.r != -1) {
            // GREAT CHANGING
            // If there are still robots to move recursively call them
            if (whichRobot < ROBOTS - 1) {
                /** Print passed grid **/
                /*std::cout << "Print passed < which grid" << std::endl;
                printPos(gp.position);
                printMatrix(gp.grid, 0);
                printPos(gpRobot[0].position);
                printMatrix(gpRobot[0].grid, 0);
                printPos(gpRobot[1].position);
                printMatrix(gpRobot[1].grid, 0);*/

                changeRobotPosition(gp, gpRobot, layer, whichRobot + 1, storeMoves, printLayerSpace + 1);
            }

            /* PRINT */
            printLayer(printLayerSpace);
            std::cout << "Robot " << whichRobot << ": d, " << d << " - m, " << (layer + 1) << std::endl;
            printLayer(printLayerSpace);
            printPos(direction.position);
            printMatrix(direction.grid, printLayerSpace);

            printLayer(printLayerSpace);
            std::cout << "---" << std::endl;
            printLayer(printLayerSpace);
            printPos(temp.position);
            printMatrix(temp.grid, printLayerSpace);
            std::cout << "\n" << std::endl;

            // 4) SEARCH ALL THE POSSIBLE POSITIONS OF THE PLAYER WITH THE NEW ROBOT POSITION
            tempArray[whichRobot] = direction;

            /** Print passed grid **/
            /*std::cout << "Print passed all possible grid" << std::endl;
            printPos(temp.position);
            printMatrix(temp.grid, 0);
            printPos(tempArray[0].position);
            printMatrix(tempArray[0].grid, 0);
            printPos(tempArray[1].position);
            printMatrix(tempArray[1].grid, 0);*/

            findMin(temp, tempArray, layer + 1, whichRobot, storeMoves, false, printLayerSpace + 1);

            // Next recursive robot position
            changeRobotPosition(temp, tempArray, layer + 1, whichRobot, storeMoves, printLayerSpace);

            storeMoves.pop_back();
        }
    }
}

void moveRobot(std::vector<gridPos> gpRobot, gridPos gpPlayer, int layer, int playerIndex, int nowIndex, std::vector<dirMove> storeMoves, int printLayerSpace, bool findAll) {
    // Remove robots until there is only one, then call findAll moves of the player
    if (!findAll) {
        if (gpRobot.size() > 1) {
            std::vector<gridPos> nextCall = gpRobot;
            nextCall.erase(nextCall.begin());
            moveRobot(nextCall, gpPlayer, layer, playerIndex, nowIndex + 1, storeMoves, printLayerSpace + 1, findAll);
        } else {
            // FIND ALL MOVES LEFT BY MOVING ONLY THE PLAYER
            /*printLayer(printLayerSpace);
            std::cout << "ALL MOVES " << gpRobot.size() << std::endl;*/
            moveRobot(gpRobot, gpPlayer, layer, 0, 0, storeMoves, printLayerSpace + 1, true);
        }
    }

    for (int d = 1; d <= 4; ++d) {
        // Using a temp variable
        // In the next run of the cycle I need the original gp variable, I use temp to run the recursive
        std::vector<gridPos> temp;
        gridPos findMove;
        gridPos tempPlayer;

        if (!findAll) {
            temp = gpRobot;
            tempPlayer = gpPlayer;
            findMove = moveDir(temp, tempPlayer, layer + 1, playerIndex, nowIndex, d, storeMoves);
        } else {
            temp.push_back(gpPlayer);
            findMove = moveDir(temp, gpPlayer, layer + 1, 0, 0, d, storeMoves);
        }

        // Found where to go
        if (findMove.position.r != -1) {
            /* PRINT */
            /*printLayer(printLayerSpace);
            if (playerIndex == nowIndex || findAll) {
                std::cout << "Player";
            } else {
                std::cout << "Robot " << nowIndex;
            }
            std::cout << ": d, " << d << " - m, " << (layer + 1) << std::endl;

            for (size_t i = 0; i < temp.size(); ++i) {
                printLayer(printLayerSpace);
                printPos(temp[i].position);
                printMatrix(temp[i].grid, printLayerSpace);
                std::cout << "\n" << std::endl;
            }*/

            // Next recursive robot position
            if (!findAll) {
                moveRobot(temp, tempPlayer, layer + 1, playerIndex, nowIndex, storeMoves, printLayerSpace, findAll);
            } else {
                moveRobot(temp, temp[0], layer + 1, playerIndex, nowIndex, storeMoves, printLayerSpace, findAll);
            }

            storeMoves.pop_back();
        }
    }
}

void allFindMin(std::vector<gridPos> gpRobot, int layer, std::vector<dirMove> storeMoves) {
    int permutations[gpRobot.size()];
    for (size_t num = 0; num < gpRobot.size(); ++num) {
            permutations[num] = num;
    }

    std::vector<gridPos> backupSwap = gpRobot;
    do {
        int playerIndex = 0;
        for (size_t swp = 0; swp < gpRobot.size(); ++swp) {
            gpRobot[swp] = backupSwap[permutations[swp]];
            if (permutations[swp] == 0) {
                playerIndex = swp;
            }

            std::cout << permutations[swp] << " ";
        }

        std::cout << "-> " << playerIndex << std::endl;
        printMatrix(gpRobot[playerIndex].grid, 0);
        if (permutations[gpRobot.size() - 1] != 0) {
            moveRobot(gpRobot, gpRobot[playerIndex], layer, playerIndex, 0, storeMoves, 0, false);
        }
    } while (std::next_permutation(permutations, permutations + gpRobot.size()));


    // 1) Single move of the Player, calling the first robot in the array
    //findMin(gp, gpRobot, layer, 0, storeMoves, true, 0);

    // 1) SEARCH ALL THE POSSIBLE POSITIONS OF THE PLAYER WITHOUT MOVING ANYTHING
    //findMin(gp, gpRobot, layer, storeMoves, false);

    // 2) MOVE THE PLAYER ONE STEP EACH TIME
    //findMin(gp, gpRobot, layer, storeMoves, true);
}

int main() {
    /* SET INITIAL POSITIONS */
    // Set player starting position
    staring.r = 3;
    staring.c = 0;

    // Set grid end position
    ending.r = 1;
    ending.c = 3;

    // Set robots positions
    robots[0].r = 0;
    robots[0].c = 2;

    robots[1].r = 2;
    robots[1].c = 3;

    robots[2].r = 1;
    robots[2].c = 1;



    /* INITIALIZATIONS */
    // Initialize grid to pass to the function
    std::vector<gridPos> gpRobot;
    /*gridPos gp;
    gridPos gpRobots[ROBOTS + 1];*/

    //copyMatrix(grid, gp.grid);

    // Initialize robots positions (= 2)
    for (int rb = 0; rb < ROBOTS + 1; ++rb) {
        //gp.grid[robots[rb].r][robots[rb].c] = 2;
        gridPos newGpRobot;

        // Initialize default grid robot
        copyMatrix(grid, newGpRobot.grid);

        // Set to 2 position of each robot and then of the player
        for (int exRb = 0; exRb < ROBOTS; ++exRb) {
            newGpRobot.grid[robots[exRb].r][robots[exRb].c] = 2;
        }

        // Set to 2 position of the player
        newGpRobot.grid[staring.r][staring.c] = 2;

        // Initialize robot position
        if (rb == 0) {
            newGpRobot.position = staring;
        } else {
            newGpRobot.position = robots[rb - 1];
        }

        // Set default position to visited
        setVisited(newGpRobot, newGpRobot.position);

        gpRobot.push_back(newGpRobot);

        printPos(newGpRobot.position);
        printMatrix(newGpRobot.grid, 0);
        std::cout << std::endl;
    }

    // Initialize starting position and layer = 0
    //gp.position = staring;
    //setVisited(gp, gp.position);
    //gpRobots[0] = gp;

    int layer = 0;

    std::vector<dirMove> storeMoves;

    /*printPos(gp.position);
    printMatrix(gp.grid, 0);*/


    /* FIND SOLUTION */
    allFindMin(gpRobot, layer, storeMoves);



    /* PRINT SOLUTION */
    std::cout << "Moves min: " << min << std::endl;

    for (size_t m = 0; m < minMoves.size(); ++m) {
        std::cout << (m + 1) << ") ";
        if (minMoves[m].who == 0)
            std::cout << "Player";
        else
            std::cout << "Robot";

        std::cout << " - " << minMoves[m].direction << std::endl;

        printPos(minMoves[m].moves.position);
        printMatrix(minMoves[m].moves.grid, 0);
        std::cout << std::endl;
    }

    return 0;
}
