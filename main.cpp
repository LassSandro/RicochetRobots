#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#define SIZER 5
#define SIZEC 5

#define ROBOTS 1

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
  {0, 0, 1, 0, 0},
  {1, 0, 1, 0, 1},
  {0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0},
  {1, 0, 1, 1, 0}
};

// Struct for a position in the grid
// r (row) and c (column) coordinates
struct pos {
    int r;
    int c;
};

// Pair grid/position
struct gridPos {
    int grid[SIZER][SIZEC];
    pos position;
};

struct moving {
    std::vector<gridPos> moves;
    std::vector<std::string> dir;
};

struct dirMove {
    std::string direction;
    int who;
};

// Player staring position in the grid
pos staring;

// Winning position in the grid
pos ending;

pos robots[ROBOTS];

moving robotMoved[ROBOTS];

// Final min moves result of the algorithm
int min = 9999999;

// Final moves: grids and directions
moving allMoved;

//std::vector<gridPos> allMoves;
//std::vector<std::string> allMovesDir;


void findMin(gridPos gp, gridPos gpRobot, int layer, moving moved, moving robotsMoved, bool singleMove);
void changeRobotPosition(gridPos gp, gridPos gpRobot, int layer, moving moved, moving robotsMoved);


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

void saveMove(const gridPos& gp, int dir, moving& moved) {
    moved.moves.push_back(gp);

    switch(dir) {
    case 1:
        moved.dir.push_back("RIGHT");
        break;
    case 2:
        moved.dir.push_back("BOTTOM");
        break;
    case 3:
        moved.dir.push_back("LEFT");
        break;
    case 4:
        moved.dir.push_back("TOP");
        break;
    }
}

void setVisited(gridPos& gp, const pos& newPos) {
    // Update current tile as already visited
    gp.grid[newPos.r][newPos.c] = -1;
}

void updateMin(int layer, int dir, gridPos& gp, moving& moved, moving& robotsMoved) {
    if (layer <= min) {
        min = layer;

        setVisited(gp, gp.position);
        saveMove(gp, dir, moved);

        allMoved = moved;
        robotMoved[0] = robotsMoved;

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
gridPos goDir(gridPos gp, gridPos& gpRobot, int layer, int dir, moving& moved, moving robotsMoved) {
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
        updateMin(layer, dir, gp, moved, robotsMoved);

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
    saveMove(gp, dir, moved);

    // gp with the new position and -1 in that position,
    // as well as -1 in all the previous visited positions
    return gp;
}

gridPos goDirRobot(gridPos gpRobot, gridPos& gp, int layer, int dir, moving& robotsMoved) {
    // Create 'NULL' pos to return when end reached (base case)
    gridPos nullReturn;
    nullReturn.position.r = -1;

    // If the moves exceed the current min, return directly
    if (layer > min) {
        printLayer(layer);
        std::cout << "Rbt Out of moves" << std::endl;
        return nullReturn;
    }

    //setVisited(gpRobot, gpRobot.position);
    //saveMove(gpRobot, dir, robotsMoved);

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

    // Update current position and push back moves and dirs
    updatePos(gpRobot, newPos);
    saveMove(gpRobot, dir, robotsMoved);


    return gpRobot;
}

void findMin(gridPos gp, gridPos gpRobot, int layer, moving moved, moving robotsMoved, bool singleMove) {
    // Visit all the four directions (1, 2, 3, 4)
    for (int d = 1; d <= 4; ++d) {
        gridPos temp = gpRobot;
        gridPos direction = goDir(gp, temp, layer + 1, d, moved, robotsMoved);

        // Found where to go
        if (direction.position.r != -1) {
            /* PRINTS */
            printLayer((layer + 1));
            std::cout << "Player: d, " << d << " - m, " << (layer + 1) << std::endl;
            printLayer((layer + 1));
            printPos(direction.position);
            printMatrix(direction.grid, (layer + 1));



            // If is a single move (recursive loop)
            if(singleMove) {
                // Matrix with -1 only on the new position of the player
                gridPos newPlayerGp;
                copyMatrix(grid, newPlayerGp.grid);
                newPlayerGp.grid[direction.position.r][direction.position.c] = -1;
                newPlayerGp.position = direction.position;
                std::cout << "\n" << std::endl;


                // 3) MOVE THE ROBOT ONE STEP EACH TIME
                changeRobotPosition(newPlayerGp, temp, layer + 1, moved, robotsMoved);
            }

            // Next move of the player
            findMin(direction, temp, layer + 1, moved, robotsMoved, singleMove);

            moved.dir.pop_back();
            moved.moves.pop_back();
        }
    }
}

void changeRobotPosition(gridPos gp, gridPos gpRobot, int layer, moving moved, moving robotsMoved) {
    for (int d = 1; d <= 4; ++d) {
        // Using a temp variable
        // In the next run of the cycle I need the original gp variable, I use temp to run the recursive
        gridPos temp = gp;
        gridPos direction = goDirRobot(gpRobot, temp, layer + 1, d, robotsMoved);

        // Found where to go
        if (direction.position.r != -1) {
            /* PRINT */
            printLayer((layer + 1));
            std::cout << "Robot: d, " << d << " - m, " << (layer + 1) << std::endl;
            printLayer((layer + 1));
            printPos(direction.position);
            printMatrix(direction.grid, (layer + 1));
            printLayer((layer + 1));
            printPos(temp.position);
            printMatrix(temp.grid, (layer + 1));
            std::cout << "\n" << std::endl;


            // 4) SEARCH ALL THE POSSIBLE POSITIONS OF THE PLAYER WITH THE NEW ROBOT POSITION
            findMin(temp, direction, layer + 1, moved, robotsMoved, false);

            std::cout << "\n" << std::endl;

            // Next recursive robot position
            changeRobotPosition(temp, direction, layer + 1, moved, robotsMoved);

            robotsMoved.dir.pop_back();
            robotsMoved.moves.pop_back();
        }
    }
}

void allFindMin(gridPos gp, gridPos gpRobot, int layer, moving moved, moving robotsMoved) {
    printMatrix(gp.grid, 0);
    printMatrix(gpRobot.grid, 0);

    // 1) SEARCH ALL THE POSSIBLE POSITIONS OF THE PLAYER WITHOUT MOVING ANYTHING
    findMin(gp, gpRobot, layer, moved, robotsMoved, false);

    // 2) MOVE THE PLAYER ONE STEP EACH TIME
    findMin(gp, gpRobot, layer, moved, robotsMoved, true);
}

int main() {
    /* SET INITIAL POSITIONS */
    // Set player starting position
    staring.r = 0;
    staring.c = 0;

    // Set grid end position
    ending.r = 0;
    ending.c = 4;

    // Set robots positions
    robots[0].r = 3;
    robots[0].c = 1;



    /* INITIALIZATIONS */
    // Initialize grid to pass to the function
    gridPos gp;
    gridPos gpRobots[ROBOTS];

    copyMatrix(grid, gp.grid);

    // Initialize robots positions (= 2)
    for (int rb = 0; rb < ROBOTS; ++rb) {
        gp.grid[robots[rb].r][robots[rb].c] = 2;

        // Initialize default grid robot
        copyMatrix(grid, gpRobots[rb].grid);

        // Set to 2 position of each robot
        for (int exRb = 0; exRb < ROBOTS; ++exRb) {
            gpRobots[rb].grid[robots[exRb].r][robots[exRb].c] = 2;
        }

        // Set to 2 position of the player
        gpRobots[rb].grid[staring.r][staring.c] = 2;

        // Initialize robot position
        gpRobots[rb].position = robots[rb];

        // Set default position to visited
        setVisited(gpRobots[rb], gpRobots[rb].position);
    }

    // Initialize starting position and layer = 0
    gp.position = staring;
    setVisited(gp, gp.position);
    int layer = 0;

    // Initialize vector to get moves
    moving moved;
    moving rbtMoved;



    /* FIND SOLUTION */
    allFindMin(gp, gpRobots[0], layer, moved, rbtMoved);



    /* PRINT SOLUTION */
    std::cout << "Min is: " << min << std::endl;

    printDirs(allMoved.dir);
    printDirs(robotMoved[0].dir);


    for (size_t m = 0; m < allMoved.moves.size(); ++m) {
        printMatrix(allMoved.moves[m].grid, 0);
        std::cout << std::endl;
    }

    std::cout << std::endl;

    for (size_t m = 0; m < robotMoved[0].moves.size(); ++m) {
        printMatrix(robotMoved[0].moves[m].grid, 0);
        std::cout << std::endl;
    }

    return 0;
}
