#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <vector>
using namespace std;
void findOneGoal();
void findNGoals();
void countAllGoals();
void findShortestPath();

class StackNode {
 public:
  int x;
  int y;
  int preferredDir;
  int nextIndex;
  StackNode* next;

  StackNode(int xVal, int yVal, int preferredVal = 0, int nextVal = 0)
    : x(xVal), y(yVal), preferredDir(preferredVal), nextIndex(nextVal), next(nullptr) {}
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
  
  void push(int x, int y, int preferredDir = 0) {
    StackNode* newNode = new StackNode(x, y, preferredDir, 0);
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
  char** map;

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

  char getbox(int x, int y) {
    return map[y][x];
  }

  void setbox(int x, int y, char val) {
    map[y][x] = val;
  }
};

string getfile() {  // get filename from user
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

  while (true) {
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
    cout << endl
         << filename << " does not exist!" << endl;
    return;
  }

  int width = 0;
  int height = 0;
  inputFile >> width >> height;

  Maze maze(width, height);
  for (int row = 0; row < height; ++row) {
    string line;
    inputFile >> line;
    for (int col = 0; col < width; ++col) {
      maze.setbox(col, row, line[col]);
    }
  }
  inputFile.close();

  const int dx[4] = {1, 0, -1, 0};
  const int dy[4] = {0, 1, 0, -1};
  const int baseOrder[4] = {0, 1, 2, 3};

  Stack path;
  int startX = 0;
  int startY = 0;

  vector<vector<bool>> visited(height, vector<bool>(width, false));
  bool found = false;

  auto computeDirection = [&](int preferred, int orderIndex) {
    int order[4];
    order[0] = preferred;
    int idx = 1;
    for (int dir : baseOrder) {
      if (dir == preferred) {
        continue;
      }
      order[idx++] = dir;
    }
    return order[orderIndex];
  };

  visited[startY][startX] = true;
  path.push(startX, startY, 0);

  while (!path.isEmpty()) {
    StackNode* topNode = path.getTop();
    int x = topNode->x;
    int y = topNode->y;

    if (maze.getbox(x, y) == 'G') {
      found = true;
      break;
    }

    if (topNode->nextIndex >= 4) {
      path.pop();
      continue;
    }

    int direction = computeDirection(topNode->preferredDir, topNode->nextIndex);
    topNode->nextIndex++;

    int nx = x + dx[direction];
    int ny = y + dy[direction];

    if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
      continue;
    }

    char cell = maze.getbox(nx, ny);
    if ((cell == 'E' || cell == 'G' || cell == 'S') && !visited[ny][nx]) {
      visited[ny][nx] = true;
      path.push(nx, ny, direction);
      if (cell == 'G') {
        found = true;
        break;
      }
    }
  }

  if (found) {
    maze.display();
    cout << endl;

    Maze result(width, height);
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        result.setbox(col, row, maze.getbox(col, row));
      }
    }

    vector<pair<int, int>> pathCells;
    while (!path.isEmpty()) {
      StackNode* node = path.getTop();
      pathCells.push_back({node->x, node->y});
      path.pop();
    }

    for (auto it = pathCells.rbegin(); it != pathCells.rend(); ++it) {
      char current = result.getbox(it->first, it->second);
      if (current != 'G' && current != 'S') {
        result.setbox(it->first, it->second, 'R');
      }
    }

    result.display();
    cout << endl << endl;
  } else {
    maze.display();
    cout << endl << endl;
  }
}


void findNGoals() { // task 2
  cout << "2" << endl;
}

void countAllGoals() { // task 3
  cout << "3" << endl;
}

void findShortestPath() { // task 4
  cout << "4" << endl;
}

