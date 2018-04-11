#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#define SIZER 7
#define SIZEC 7

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
  {0, 1, 1, 1, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0},
  {1, 0, 1, 0, 1, 1, 0},
  {0, 0, 1, 0, 0, 1, 0},
  {0, 1, 1, 1, 0, 1, 0},
  {0, 0, 1, 0, 0, 1, 0},
  {1, 0, 0, 0, 1, 0, 0},
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

// Final min moves result of the algorithm
int min = 9999999;

// Final moves: grids and directions
std::vector<gridPos> allMoves;
std::vector<std::string> allMovesDir;

/* PRINTS */
// Print the grid matrix
void printMatrix(int grid[][SIZEC]) {
    for (int i = 0; i < SIZER; ++i) {
        for (int j = 0; j < SIZEC; ++j) {
            if (grid[i][j] == 0)
                std::cout << std::setw(2) << ".";
            else if (grid[i][j] ==-1)
                std::cout << std::setw(2) << "x";
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
           gp.grid[newPos.r][newPos.c + 1] != 1) {
        ++newPos.c;
    }
}

// Check position: column - 1
void toLeft(pos& newPos, const gridPos& gp) {
    while (newPos.c - 1 >= 0 &&
           gp.grid[newPos.r][newPos.c - 1] != 1) {
        --newPos.c;
    }
}

// Check position: row + 1
void toBottom(pos& newPos, const gridPos& gp) {
    while (newPos.r + 1 < SIZER &&
           gp.grid[newPos.r + 1][newPos.c] != 1) {
        ++newPos.r;
    }
}

// Check position: row - 1
void toTop(pos& newPos, const gridPos& gp) {
    while (newPos.r - 1 >= 0 &&
           gp.grid[newPos.r - 1][newPos.c] != 1) {
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

        return nullReturn;
    }

    // Update current position and push back moves and dirs
    updatePos(gp, newPos);
    saveMove(gp, dir, moves, dirs);


    return gp;
}

/*gridPos goRight(gridPos gp, int layer) {
    gridPos nullReturn;
    nullReturn.position.r = -1;

    pos newPos = gp.position;

    while (newPos.c + 1 < SIZE &&
           gp.grid[newPos.r][newPos.c + 1] != 1) {
        ++newPos.c;
    }

    if (newPos.c == gp.position.c ||
        gp.grid[newPos.r][newPos.c] == -1) {
        return nullReturn;
    }

    if (newPos.r == ending.r && newPos.c == ending.c) {
        if (layer < min) {
            min = layer;
            std::cout << "!!! GOT MIN !!!" << std::endl;
            std::cout << "Layer: " << layer << std::endl;
            printMatrix(gp.grid);
            printPos(gp.position);
            std::cout << std::endl;
            return nullReturn;
        }
    }

    gp.grid[newPos.r][newPos.c] = -1;
    gp.position = newPos;

    //printMatrix(gp.grid);
    //printPos(newPos);

    return gp;
}

gridPos goLeft(gridPos gp, int layer) {
    gridPos nullReturn;
    nullReturn.position.r = -1;

    pos newPos = gp.position;

    while (newPos.c - 1 >= 0 &&
           gp.grid[newPos.r][newPos.c - 1] != 1) {
        --newPos.c;
    }

    if (newPos.c == gp.position.c ||
        gp.grid[newPos.r][newPos.c] == -1) {
        return nullReturn;
    }

    if (newPos.r == ending.r && newPos.c == ending.c) {
        if (layer < min) {
            min = layer;
            std::cout << "!!! GOT MIN !!!" << std::endl;
            std::cout << "Layer: " << layer << std::endl;
            printMatrix(gp.grid);
            printPos(gp.position);
            std::cout << std::endl;
            return nullReturn;
        }
    }

    gp.grid[newPos.r][newPos.c] = -1;
    gp.position = newPos;

    //printMatrix(gp.grid);
    //printPos(newPos);

    return gp;
}

gridPos goTop(gridPos gp, int layer) {
    gridPos nullReturn;
    nullReturn.position.r = -1;

    pos newPos = gp.position;

    while (newPos.r - 1 >= 0 &&
           gp.grid[newPos.r - 1][newPos.c] != 1) {
        --newPos.r;
    }

    if (newPos.r == gp.position.r ||
        gp.grid[newPos.r][newPos.c] == -1) {
        return nullReturn;
    }

    if (newPos.r == ending.r && newPos.c == ending.c) {
        if (layer < min) {
            min = layer;
            std::cout << "!!! GOT MIN !!!" << std::endl;
            std::cout << "Layer: " << layer << std::endl;
            printMatrix(gp.grid);
            printPos(gp.position);
            std::cout << std::endl;
            return nullReturn;
        }
    }

    gp.grid[newPos.r][newPos.c] = -1;
    gp.position = newPos;

    //printMatrix(gp.grid);
    //printPos(newPos);

    return gp;
}

gridPos goBottom(gridPos gp, int layer) {
    gridPos nullReturn;
    nullReturn.position.r = -1;

    pos newPos = gp.position;

    while (newPos.r + 1 < SIZE &&
           gp.grid[newPos.r + 1][newPos.c] != 1) {
        ++newPos.r;
    }

    if (newPos.r == gp.position.r ||
        gp.grid[newPos.r][newPos.c] == -1) {
        return nullReturn;
    }

    if (newPos.r == ending.r && newPos.c == ending.c) {
        if (layer < min) {
            min = layer;
            std::cout << "!!! GOT MIN !!!" << std::endl;
            std::cout << "Layer: " << layer << std::endl;
            printMatrix(gp.grid);
            printPos(gp.position);
            std::cout << std::endl;
            return nullReturn;
        }
    }

    gp.grid[newPos.r][newPos.c] = -1;
    gp.position = newPos;

    //printMatrix(gp.grid);
    //printPos(newPos);

    return gp;
}*/

void findMin(gridPos gp, int layer, std::vector<gridPos> moves, std::vector<std::string> dirs) {
    gridPos right = goDir(gp, layer + 1, 1, moves, dirs);
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
    }
}

int main() {
    // Set player starting position
    staring.r = 0;
    staring.c = 0;
    grid[staring.r][staring.c] = -1;

    // Set grid end position
    ending.r = 6;
    ending.c = 5;

    // Initialize grid to pass to the function
    gridPos setup;
    for (int i = 0; i < SIZER; ++i) {
        for (int j = 0; j < SIZEC; ++j) {
            setup.grid[i][j] = grid[i][j];
        }
    }

    // Initialize starting position and layer = 0
    setup.position = staring;
    int layer = 0;

    // Initialize vector to get moves
    std::vector<gridPos> moves;
    std::vector<std::string> dirs;

    // Find solution
    findMin(setup, layer, moves, dirs);

    // Print solution
    std::cout << "Min is: " << min << std::endl;

    printDirs(allMovesDir);

    for (size_t m = 0; m < allMoves.size(); ++m) {
        printMatrix(allMoves[m].grid);
        std::cout << std::endl;
    }

    return 0;
}
