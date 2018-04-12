#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#define SIZER 6
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
  {0, 0, 0, 0, 0},
  {1, 1, 0, 1, 0},
  {0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1},
  {0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0}
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

// Player staring position in the grid
pos staring;

// Winning position in the grid
pos ending;

pos robots[ROBOTS];

// Final min moves result of the algorithm
int min = 9999999;

// Final moves: grids and directions
std::vector<gridPos> allMoves;
std::vector<std::string> allMovesDir;

/* PRINTS */
// Print the grid matrix
void printMatrix(int grid[][SIZEC]) {
    int robots = 1;
    for (int i = 0; i < SIZER; ++i) {
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


// copied = toCopy values
void copyVector(const std::vector<gridPos>& toCopy, std::vector<gridPos>& copied) {
    for (size_t i = 0; i < toCopy.size(); ++i) {
        copied.push_back(toCopy[i]);
    }
}


// Check position: column + 1
void toRight(pos& newPos, const gridPos& gp) {
    while (newPos.c + 1 < SIZEC &&
           (gp.grid[newPos.r][newPos.c + 1] == 0 || gp.grid[newPos.r][newPos.c + 1] == -1)) {
        ++newPos.c;
    }
}

// Check position: column - 1
void toLeft(pos& newPos, const gridPos& gp) {
    while (newPos.c - 1 >= 0 &&
           (gp.grid[newPos.r][newPos.c - 1] == 0 || gp.grid[newPos.r][newPos.c - 1] == -1)) {
        --newPos.c;
    }
}

// Check position: row + 1
void toBottom(pos& newPos, const gridPos& gp) {
    while (newPos.r + 1 < SIZER &&
           (gp.grid[newPos.r + 1][newPos.c] == 0 || gp.grid[newPos.r + 1][newPos.c] == -1)) {
        ++newPos.r;
    }
}

// Check position: row - 1
void toTop(pos& newPos, const gridPos& gp) {
    while (newPos.r - 1 >= 0 &&
           (gp.grid[newPos.r - 1][newPos.c] == 0 || gp.grid[newPos.r - 1][newPos.c] == -1)) {
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

void saveMove(const gridPos& gp, int dir, std::vector<gridPos>& moves, std::vector<std::string>& dirs) {
    moves.push_back(gp);

    switch(dir) {
    case 1:
        dirs.push_back("RIGHT");
        break;
    case 2:
        dirs.push_back("BOTTOM");
        break;
    case 3:
        dirs.push_back("LEFT");
        break;
    case 4:
        dirs.push_back("TOP");
        break;
    }
}

void updateMin(int layer, int dir, const gridPos& gp, std::vector<gridPos>& moves, std::vector<std::string>& dirs) {
    if (layer < min) {
        min = layer;

        saveMove(gp, dir, moves, dirs);

        allMoves = moves;
        allMovesDir = dirs;

        // Print result
        std::cout << "FOUND MIN: " << layer << std::endl;
        std::cout << std::endl;
    }
}

void updatePos(gridPos& gp, const pos& newPos) {
    // Update current tile as already visited
    gp.grid[newPos.r][newPos.c] = -1;

    // Update current position of the player
    gp.position = newPos;
}

bool samePos(const pos& pos1, const pos& pos2) {
    if (pos1.r == pos2.r && pos1.c == pos2.c) {
        return true;
    }

    return false;
}

// Main function, check possible moves in four directions
// 1: Right
// 2: Bottom
// 3: Left
// 4: Top
gridPos goDir(gridPos gp, int layer, int dir, std::vector<gridPos>& moves, std::vector<std::string>& dirs) {
    // Create 'NULL' pos to return when end reached (base case)
    gridPos nullReturn;
    nullReturn.position.r = -1;

    // If the moves exceed the current min, return directly
    if (layer > min) {
        return nullReturn;
    }

    // Create and assign value to temp pos newPos based on the direction in which to go
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
        updateMin(layer, dir, gp, moves, dirs);

        printPos(gp.position);
        printMatrix(gp.grid);

        return nullReturn;
    }

    // Update current position and push back moves and dirs
    updatePos(gp, newPos);
    saveMove(gp, dir, moves, dirs);


    return gp;
}

gridPos goDirRobot(gridPos gpRobot, gridPos& gp, int layer, int dir) {
    // Create 'NULL' pos to return when end reached (base case)
    gridPos nullReturn;
    nullReturn.position.r = -1;

    // If the moves exceed the current min, return directly
    if (layer > min) {
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

    // Update current position and push back moves and dirs
    updatePos(gpRobot, newPos);

    //saveMove(gp, dir, moves, dirs);


    return gpRobot;
}

void findMin(gridPos gp, int layer, std::vector<gridPos> moves, std::vector<std::string> dirs) {
    // Visit all the four directions (1, 2, 3, 4)
    for (int d = 1; d <= 4; ++d) {
        gridPos direction = goDir(gp, layer + 1, d, moves, dirs);
        if (direction.position.r != -1) {
            std::cout << "Player find n: " << d << " - " << (layer + 1) << std::endl;
            printPos(direction.position);
            printMatrix(direction.grid);

            findMin(direction, layer + 1, moves, dirs);
            moves.pop_back();
            dirs.pop_back();
        }
    }

    /*gridPos right = goDir(gp, layer + 1, 1, moves, dirs);
    if (right.position.r != -1) {
        findMin(right, layer + 1, moves, dirs);
        moves.pop_back();
        dirs.pop_back();
    }

    gridPos bottom = goDir(gp, layer + 1, 2, moves, dirs);
    if (bottom.position.r != -1) {
        findMin(bottom, layer + 1, moves, dirs);
        moves.pop_back();
        dirs.pop_back();
    }

    gridPos left = goDir(gp, layer + 1, 3, moves, dirs);
    if (left.position.r != -1) {
        findMin(left, layer + 1, moves, dirs);
        moves.pop_back();
        dirs.pop_back();
    }

    gridPos top = goDir(gp, layer + 1, 4, moves, dirs);
    if (top.position.r != -1) {
        findMin(top, layer + 1, moves, dirs);
        moves.pop_back();
        dirs.pop_back();
    }*/
}

void changeRobotPosition(gridPos gp, gridPos gpRobot, int layer, std::vector<gridPos> moves, std::vector<std::string> dirs) {
    for (int d = 1; d <= 4; ++d) {
        gridPos temp = gp;

        std::cout << "Robot go n: " << d << std::endl;
        gridPos direction = goDirRobot(gpRobot, temp, layer + 1, d);
        if (direction.position.r != -1) {
            std::cout << "Robot find n: " << d << " - " << (layer + 1) << std::endl;
            printPos(direction.position);
            printMatrix(direction.grid);

            // A (On)
            findMin(temp, layer + 1, moves, dirs);

            // Next recursive robot position
            changeRobotPosition(temp, direction, layer + 1, moves, dirs);
        }
    }
}

void allFindMin(gridPos gp, int layer, std::vector<gridPos> moves, std::vector<std::string> dirs) {
    // A (On)
    findMin(gp, layer, moves, dirs);

    gridPos newGp = gp;
    newGp.position = robots[0];
    newGp.grid[newGp.position.r][newGp.position.c] = -1;
    newGp.grid[gp.position.r][gp.position.c] = 2;


    // B
    changeRobotPosition(gp, newGp, layer, moves, dirs);
}

int main() {
    // Set player starting position
    staring.r = 0;
    staring.c = 0;
    grid[staring.r][staring.c] = -1;

    // Set grid end position
    ending.r = 4;
    ending.c = 3;

    // Set robots positions
    robots[0].r = 0;
    robots[0].c = 1;

    // Initialize grid to pass to the function
    gridPos setup;
    for (int i = 0; i < SIZER; ++i) {
        for (int j = 0; j < SIZEC; ++j) {
            setup.grid[i][j] = grid[i][j];
        }
    }

    // Initialize robots positions (= 2)
    for (int rb = 0; rb < ROBOTS; ++rb) {
        setup.grid[robots[rb].r][robots[rb].c] = 2;
    }

    // Initialize starting position and layer = 0
    setup.position = staring;
    int layer = 0;

    // Initialize vector to get moves
    std::vector<gridPos> moves;
    std::vector<std::string> dirs;

    // Find solution
    //findMin(setup, layer, moves, dirs);
    allFindMin(setup, layer, moves, dirs);

    // Print solution
    std::cout << "Min is: " << min << std::endl;

    printDirs(allMovesDir);

    for (size_t m = 0; m < allMoves.size(); ++m) {
        printMatrix(allMoves[m].grid);
        std::cout << std::endl;
    }

    return 0;
}

/*
Robot go n: 1
Robot find n: 1 - 1
Pos: (0, 4)
 1 x . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . .
 . . - . .
Player find n: 1 - 2
Pos: (0, 3)
 x . . x 1
 - - . - .
 . . . . .
 . - - - -
 . . . . .
 . . - . .
Robot go n: 1
Robot go n: 2
Robot find n: 2 - 2
Pos: (2, 4)
 1 x . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . .
 . . - . .
Player find n: 1 - 3
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . 1
 . - - - -
 . . . . .
 . . - . .
Player find n: 2 - 4
Pos: (1, 4)
 x . . . x
 - - . - x
 . . . . 1
 . - - - -
 . . . . .
 . . - . .
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot find n: 3 - 3
Pos: (2, 0)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 . . - . .
Player find n: 1 - 4
Pos: (0, 4)
 x . . . x
 - - . - .
 1 . . . .
 . - - - -
 . . . . .
 . . - . .
Player find n: 2 - 5
Pos: (2, 4)
 x . . . x
 - - . - .
 1 . . . x
 . - - - -
 . . . . .
 . . - . .
Player find n: 3 - 6
Pos: (2, 1)
 x . . . x
 - - . - .
 1 x . . x
 . - - - -
 . . . . .
 . . - . .
Robot go n: 1
Robot go n: 2
Robot find n: 2 - 4
Pos: (5, 0)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 x . - . .
Player find n: 1 - 5
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . .
 1 . - . .
Player find n: 2 - 6
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . .
 1 . - . .
Player find n: 3 - 7
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 1 . - . .
Player find n: 2 - 8
Pos: (4, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . . .
 1 . - . .
Player find n: 1 - 9
Pos: (4, 4)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . . x
 1 . - . .
Player find n: 2 - 10
Pos: (5, 4)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . . x
 1 . - . x
Player find n: 3 - 11
Pos: (5, 3)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . . x
 1 . - x x
FOUND MIN: 12

Pos: (4, 3)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . x x
 1 . - x x
Robot go n: 1
Robot find n: 1 - 5
Pos: (5, 1)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 x x - . .
Player find n: 1 - 6
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . .
 . 1 - . .
Player find n: 2 - 7
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . .
 . 1 - . .
Player find n: 3 - 8
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 . 1 - . .
Player find n: 2 - 9
Pos: (5, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 x 1 - . .
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot go n: 4
Robot find n: 4 - 6
Pos: (4, 1)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 . x . . .
 x x - . .
Player find n: 1 - 7
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . 1 . . .
 . . - . .
Player find n: 2 - 8
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . 1 . . .
 . . - . .
Player find n: 3 - 9
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . 1 . . .
 . . - . .
Player find n: 2 - 10
Pos: (5, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . 1 . . .
 x . - . .
Player find n: 1 - 11
Pos: (5, 1)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . 1 . . .
 x x - . .
Robot go n: 1
Robot find n: 1 - 7
Pos: (4, 4)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 . x . . x
 x x - . .
Player find n: 1 - 8
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . 1
 . . - . .
Player find n: 2 - 9
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . 1
 . . - . .
Player find n: 3 - 10
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . 1
 . . - . .
Player find n: 2 - 11
Pos: (5, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . 1
 x . - . .
Player find n: 1 - 12
Pos: (5, 1)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . 1
 x x - . .
Robot go n: 1
Robot go n: 2
Robot find n: 2 - 8
Pos: (5, 4)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 . x . . x
 x x - . x
Player find n: 1 - 9
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . .
 . . - . 1
Player find n: 2 - 10
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . .
 . . - . 1
Player find n: 3 - 11
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 . . - . 1
Player find n: 2 - 12
Pos: (5, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 x . - . 1
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot find n: 3 - 9
Pos: (5, 3)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 . x . . x
 x x - x x
Player find n: 1 - 10
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . .
 . . - 1 .
Player find n: 2 - 11
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . .
 . . - 1 .
Player find n: 3 - 12
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 . . - 1 .
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot go n: 4
Robot find n: 4 - 10
Pos: (4, 3)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 . x . x x
 x x - x x
Player find n: 1 - 11
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . 1 .
 . . - . .
Player find n: 2 - 12
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . 1 .
 . . - . .
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot find n: 3 - 11
Pos: (4, 0)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 x x . x x
 x x - x x
Player find n: 1 - 12
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 1 . . . .
 . . - . .
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot go n: 4
Robot go n: 4
Robot go n: 4
Robot go n: 3
Robot find n: 3 - 8
Pos: (4, 0)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 x x . . x
 x x - . .
Player find n: 1 - 9
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 1 . . . .
 . . - . .
Player find n: 2 - 10
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 1 . . . .
 . . - . .
Player find n: 3 - 11
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 1 . . . .
 . . - . .
Player find n: 2 - 12
Pos: (3, 0)
 x . . . x
 - - . - .
 x . . . x
 x - - - -
 1 . . . .
 . . - . .
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot go n: 4
Robot go n: 4
Robot go n: 2
Robot go n: 3
Robot find n: 3 - 7
Pos: (4, 0)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 x x . . .
 x x - . .
Player find n: 1 - 8
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 1 . . . .
 . . - . .
Player find n: 2 - 9
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 1 . . . .
 . . - . .
Player find n: 3 - 10
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 1 . . . .
 . . - . .
Player find n: 2 - 11
Pos: (3, 0)
 x . . . x
 - - . - .
 x . . . x
 x - - - -
 1 . . . .
 . . - . .
Robot go n: 1
Robot find n: 1 - 8
Pos: (4, 4)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 x x . . x
 x x - . .
Player find n: 1 - 9
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . 1
 . . - . .
Player find n: 2 - 10
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . 1
 . . - . .
Player find n: 3 - 11
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . 1
 . . - . .
Player find n: 2 - 12
Pos: (5, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . 1
 x . - . .
Robot go n: 1
Robot go n: 2
Robot find n: 2 - 9
Pos: (5, 4)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 x x . . x
 x x - . x
Player find n: 1 - 10
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . .
 . . - . 1
Player find n: 2 - 11
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . .
 . . - . 1
Player find n: 3 - 12
Pos: (2, 0)
 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 . . - . 1
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot find n: 3 - 10
Pos: (5, 3)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 x x . . x
 x x - x x
Player find n: 1 - 11
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . .
 . . - 1 .
Player find n: 2 - 12
Pos: (2, 4)
 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . .
 . . - 1 .
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot go n: 4
Robot find n: 4 - 11
Pos: (4, 3)
 1 x . . x
 - - . - .
 x . . . x
 . - - - -
 x x . x x
 x x - x x
Player find n: 1 - 12
Pos: (0, 4)
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . 1 .
 . . - . .
Robot go n: 1
Robot go n: 2
Robot go n: 3
Robot go n: 4
Robot go n: 4
Robot go n: 3
Robot go n: 4
Robot go n: 2
Robot go n: 3
Robot go n: 4
Robot go n: 4
Robot go n: 2
Robot go n: 3
Robot go n: 4
Robot go n: 3
Robot go n: 4
Robot go n: 4
Robot go n: 3
Robot go n: 4
Robot go n: 2
Robot go n: 3
Robot go n: 4
Min is: 12
RIGHT BOTTOM LEFT BOTTOM RIGHT BOTTOM LEFT TOP
 x . . . x
 - - . - .
 . . . . .
 . - - - -
 . . . . .
 1 . - . .

 x . . . x
 - - . - .
 . . . . x
 . - - - -
 . . . . .
 1 . - . .

 x . . . x
 - - . - .
 x . . . x
 . - - - -
 . . . . .
 1 . - . .

 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . . .
 1 . - . .

 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . . x
 1 . - . .

 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . . x
 1 . - . x

 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . . x
 1 . - x x

 x . . . x
 - - . - .
 x . . . x
 . - - - -
 x . . x x
 1 . - x x

Press <RETURN> to close this window...

*/
