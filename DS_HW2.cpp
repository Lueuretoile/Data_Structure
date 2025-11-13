#include <iostream>
#include <string>
#include <fstream>
using namespace std;
void findOneGoal();
void findNGoals();
void countAllGoals();
void findShortestPath();

// Global state to remember last loaded maze file from task 1
static string lastFilename = "";
static bool hasLoadedFile = false;

class StackNode {
 public:
  int x;
  int y;
  int dir;
  StackNode* next;

  StackNode(int xVal, int yVal, int dirVal = 0) 
    : x(xVal), y(yVal), dir(dirVal), next(nullptr) {}
};

class Stack {
 private:
  StackNode* top;
  int size;
  
 public:
  Stack() : top(nullptr), size(0) {}
  
  ~Stack() {
    while (top != nullptr) {
      StackNode* temp = top;
      top = top->next;
      delete temp;
    }
  }
  
  void push(int x, int y, int dir = 0) {
    StackNode* newNode = new StackNode(x, y, dir);
    newNode->next = top;
    top = newNode;
    size++;
  }
  
  void pop() {
    if (top != nullptr) {
      StackNode* temp = top;
      top = top->next;
      delete temp;
      size--;
    }
  }
  
  StackNode* getTop() {
    return top;
  }
  
  bool isEmpty() {
    return top == nullptr;
  }
  
  int getSize() {
    return size;
  }
};

class Maze {
 private:
    int width;
    int height;
    char **map;
 public:
    Maze(int w, int h) : width(w), height(h) {
        map = new char*[height];
        for (int i = 0; i < height; ++i) {
            map[i] = new char[width];
        }
    }

    ~Maze() {
        for (int i = 0; i < height; ++i) {
            delete[] map[i];
        }
        delete[] map;
    }

    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }
    void display() const {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                cout << map[i][j];
            }
            cout << endl;
        }
    }
    //取單格的值
    char getbox(int x, int y) {
        return map[y][x];
    }
    //設單格的值
    void setbox(int x, int y, char val) {
        map[y][x] = val;
    }
};

string getfile() {// get filename from user
    string filename;
    cout << endl << "Input a file number: ";
    cin >> filename;
    return "input" + filename + ".txt";
}

void showMenu() {
  cout << "*** (^_^) Data Structure (^o^) ***" << endl;
  cout << "*** Find the Goal(s) in a Maze ***" << endl;
  cout << "* 0. Quit                        *" << endl;
  cout << "* 1. Find one goal               *" << endl;
  cout << "* 2. Find goal(s) as requested   *" << endl;
  cout << "* 3. How many goals?             *" << endl;
  cout << "* 4. Shortest path to one goal   *" << endl;
  cout << "**********************************" << endl;
}

int main() {
  Maze* currentMaze = nullptr;
  int command;

  while(true) {
    showMenu();
    cout << "Input a command(0, 1, 2, 3, 4): ";

    if (!(cin >> command)) {
      cin.clear();
      string dummy;
      getline(cin, dummy);
      cout << endl << "Command does not exist!" << endl << endl;
      continue;
    }

    if (command < 0 || command > 4) {
      cout << endl << "Command does not exist!" << endl << endl;
      continue;
    }

    if (command == 1) {
      findOneGoal();
    } else if (command == 2) {
      findNGoals();
    } else if (command == 3) {
      countAllGoals();
    } else if (command == 4) {
      findShortestPath();
    } else if (command == 0) {
      return 0;
    }
    
  }
 return 0;
}

void findOneGoal() {
  string filename = getfile();
  ifstream inputFile(filename);
  if (!inputFile.is_open()) {
    cout << endl << filename << " does not exist!" << endl;
    return;
  }

  // Record loaded file for task 2 usage
  lastFilename = filename;
  hasLoadedFile = true;

  // Read maze size
  int width = 0;
  int height = 0;
  inputFile >> width >> height;

  // Create maze and load content
  Maze maze(width, height);
  for (int row = 0; row < height; ++row) {
    string line;
    inputFile >> line;
    for (int col = 0; col < width; ++col) {
      maze.setbox(col, row, line[col]);
    }
  }
  inputFile.close();

  // Directions: right, down, left, up
  int dx[4] = {1, 0, -1, 0};
  int dy[4] = {0, 1, 0, -1};

  Stack path;
  int startX = 0;
  int startY = 0;
  bool found = false;

  // Visited record
  bool **visitedRecord = new bool*[height];
  for (int row = 0; row < height; ++row) {
    visitedRecord[row] = new bool[width];
    for (int col = 0; col < width; ++col) {
      visitedRecord[row][col] = false;
    }
  }

  // Start point
  maze.setbox(startX, startY, 'V');
  visitedRecord[startY][startX] = true;
  path.push(startX, startY, 0);

  while (!path.isEmpty()) {
    StackNode *topNode = path.getTop();
    int x = topNode->x;
    int y = topNode->y;
    int dir = topNode->dir; // preferred direction

    // Try preferred direction first
    int nx = x + dx[dir];
    int ny = y + dy[dir];
    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
      char cell = maze.getbox(nx, ny);
      if (cell == 'G') {
        path.push(nx, ny, dir);
        found = true;
        break;
      } else if (cell == 'E') {
        maze.setbox(nx, ny, 'V');
        visitedRecord[ny][nx] = true;
        path.getTop()->dir = dir;
        path.push(nx, ny, dir);
        continue;
      }
    }

    // If preferred direction not possible, try other directions
    bool moved = false;
    for (int i = 0; i < 4; ++i) {
      if (i == dir) continue;
      int tx = x + dx[i];
      int ty = y + dy[i];
      if (tx < 0 || tx >= width || ty < 0 || ty >= height) continue;
      char cell = maze.getbox(tx, ty);
      if (cell == 'G') {
        path.push(tx, ty, i);
        found = true;
        moved = true;
        break;
      } else if (cell == 'E') {
        maze.setbox(tx, ty, 'V');
        visitedRecord[ty][tx] = true;
        path.getTop()->dir = i;
        path.push(tx, ty, i);
        moved = true;
        break;
      }
    }
    if (found) break;
    if (!moved) {
      path.pop();
    }
  }

  // Output result
  if (found) {
    maze.display();
    cout << endl;
    // Create result map and restore non-path 'V'
    Maze result(width, height);
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        char current = maze.getbox(col, row);
        if (current == 'V') {
          result.setbox(col, row, 'E');
        } else {
          result.setbox(col, row, current);
        }
      }
    }
    // Mark path in stack as 'R'
    Stack tempStack;
    while (!path.isEmpty()) {
      StackNode *node = path.getTop();
      result.setbox(node->x, node->y, 'R');
      tempStack.push(node->x, node->y, node->dir);
      path.pop();
    }
    // Keep 'G' in maze
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        if (maze.getbox(col, row) == 'G') {
          result.setbox(col, row, 'G');
        }
      }
    }
    result.display();
    cout << endl << endl;
  } else {
    // No goal found, only show search state
    maze.display();
    cout << endl << endl;
  }

  for (int row = 0; row < height; ++row) {
    delete[] visitedRecord[row];
  }
  delete[] visitedRecord;
}


void findNGoals() { // task 2
  if (!hasLoadedFile || lastFilename.empty()) {
    cout << "### Execute command 1 to load a maze! ###" << endl;
    return;
  }

  cout << endl << "Number of G (goals): ";
  int N = 0;
  if (!(cin >> N) || N <= 0) {
    return;
  }

  // Load maze from the previously selected file
  ifstream inputFile(lastFilename);
  if (!inputFile.is_open()) {
    cout << endl << lastFilename << " does not exist!" << endl;
    return;
  }

  int width = 0;
  int height = 0;
  inputFile >> width >> height;
  Maze maze(width, height);

  int totalGoals = 0;
  for (int row = 0; row < height; ++row) {
    string line;
    inputFile >> line;
    for (int col = 0; col < width; ++col) {
      maze.setbox(col, row, line[col]);
      if (line[col] == 'G') {
        totalGoals++;
      }
    }
  }
  inputFile.close();

  int dx[4] = {1, 0, -1, 0};
  int dy[4] = {0, 1, 0, -1};

  // Visited record (separate from maze content to avoid mutating 'G')
  bool **visitedRecord = new bool*[height];
  for (int r = 0; r < height; ++r) {
    visitedRecord[r] = new bool[width];
    for (int c = 0; c < width; ++c) visitedRecord[r][c] = false;
  }

  Stack path;
  int startX = 0;
  int startY = 0;
  int goalsFound = 0;

  maze.setbox(startX, startY, 'V');
  visitedRecord[startY][startX] = true;
  path.push(startX, startY, 0);

  while (!path.isEmpty() && goalsFound < N) {
    StackNode *topNode = path.getTop();
    int x = topNode->x;
    int y = topNode->y;
    int dir = topNode->dir;

    // Try continuing in the preferred direction first
    // 2
    int nx = x + dx[dir];
    int ny = y + dy[dir];
    bool moved = false;
    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
      char cell = maze.getbox(nx, ny);
      if ((cell == 'E' && !visitedRecord[ny][nx]) || (cell == 'G' && !visitedRecord[ny][nx])) {
        if (cell == 'G') {
          visitedRecord[ny][nx] = true;
          path.push(nx, ny, dir);
          goalsFound++;
        } else if (cell == 'E') { // 'E'
          maze.setbox(nx, ny, 'V');
          visitedRecord[ny][nx] = true;
          path.getTop()->dir = dir;
          path.push(nx, ny, dir);
        }
        moved = true;
      }
    }

    // If preferred dir not possible, try in order right, down, left, up (excluding preferred)
    if (!moved && goalsFound < N) {
      for (int i = 0; i < 4; ++i) {
        if (i == dir) continue;
        int tx = x + dx[i];
        int ty = y + dy[i];
        if (tx < 0 || tx >= width || ty < 0 || ty >= height) continue;
        char cell = maze.getbox(tx, ty);
        if ((cell == 'E' && !visitedRecord[ty][tx]) || (cell == 'G' && !visitedRecord[ty][tx])) {
          if (cell == 'G') {
            visitedRecord[ty][tx] = true;
            path.push(tx, ty, i);
            goalsFound++;
          } else { // 'E'
            maze.setbox(tx, ty, 'V');
            visitedRecord[ty][tx] = true;
            path.getTop()->dir = i;
            path.push(tx, ty, i);
          }
          moved = true;
          break;
        }
      }
    }

    if (!moved) {
      // Dead-end, backtrack
      path.pop();
    }
  }

  // Output
  maze.display();
  cout << endl;

  if (N > totalGoals) {
    for (int r = 0; r < height; ++r) delete[] visitedRecord[r];
    delete[] visitedRecord;
    cout << endl;
    return;
  }

  if (goalsFound >= N) {
    Maze result(width, height);
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        char current = maze.getbox(col, row);
        if (current == 'V') {
          result.setbox(col, row, 'E');
        } else {
          result.setbox(col, row, current);
        }
      }
    }

    Stack tempStack;
    while (!path.isEmpty()) {
      StackNode *node = path.getTop();
      result.setbox(node->x, node->y, 'R');
      tempStack.push(node->x, node->y, node->dir);
      path.pop();
    }

    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        if (maze.getbox(col, row) == 'G') {
          result.setbox(col, row, 'G');
        }
      }
    }

    result.display();
    cout << endl << endl;
  } else {
    cout << endl << endl;
  }

  
  for (int row = 0; row < height; ++row) {
    delete[] visitedRecord[row];
  }
  delete[] visitedRecord;
  
}

void countAllGoals() { // task 3
  cout << "3" << endl;
}

void findShortestPath() { // task 4
  cout << "4" << endl;
}