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
  lastFilename = filename;
  hasLoadedFile = true;
  int width = 0, height = 0;
  inputFile >> width >> height;
  Maze maze(width, height);
  for (int row = 0; row < height; ++row) {
    string line; inputFile >> line;
    for (int col = 0; col < width; ++col) maze.setbox(col, row, line[col]);
  }
  inputFile.close();

  int dx[4] = {1, 0, -1, 0};   // 0:右 1:下 2:左 3:上 (順時針)
  int dy[4] = {0, 1, 0, -1};

  Stack path;
  int startX = 0, startY = 0;
  bool found = false;

  bool **visitedRecord = new bool*[height];
  for (int r = 0; r < height; ++r) {
    visitedRecord[r] = new bool[width];
    for (int c = 0; c < width; ++c) visitedRecord[r][c] = false;
  }

  maze.setbox(startX, startY, 'V');
  visitedRecord[startY][startX] = true;
  path.push(startX, startY, 0); // 初始偏好方向：右(0)

  while (!path.isEmpty()) {
    StackNode* topNode = path.getTop();
    int x = topNode->x;
    int y = topNode->y;
    int lastDir = topNode->dir; // 上一次選擇方向 (作為起點)

    bool moved = false;
    // 從「上一次方向」開始，順時針：lastDir, (lastDir+1)%4, (lastDir+2)%4, (lastDir+3)%4
    for (int k = 0; k < 4; ++k) {
      int tryDir = (lastDir + k) % 4;
      int nx = x + dx[tryDir];
      int ny = y + dy[tryDir];
      if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
      char cell = maze.getbox(nx, ny);

      if (cell == 'G') {
        // 抵達目標，仍更新父節點的 dir 以保持一致邏輯
        topNode->dir = tryDir;
        path.push(nx, ny, tryDir);
        found = true;
        moved = true;
        break;
      }
      if (cell == 'E' && !visitedRecord[ny][nx]) {
        maze.setbox(nx, ny, 'V');
        visitedRecord[ny][nx] = true;
        topNode->dir = tryDir;      // 更新父節點偏好方向
        path.push(nx, ny, tryDir);  // 新節點記錄本次方向
        moved = true;
        break;
      }
    }

    if (found) break;
    if (!moved) {
      path.pop(); // 回溯
    }
  }

  if (found) {
    maze.display();
    cout << endl;
    Maze result(width, height);
    for (int r = 0; r < height; ++r)
      for (int c = 0; c < width; ++c)
        result.setbox(c, r, (maze.getbox(c, r) == 'V' ? 'E' : maze.getbox(c, r)));

    Stack temp;
    while (!path.isEmpty()) {
      StackNode* node = path.getTop();
      result.setbox(node->x, node->y, 'R');
      temp.push(node->x, node->y, node->dir);
      path.pop();
    }
    // 保留所有 G
    for (int r = 0; r < height; ++r)
      for (int c = 0; c < width; ++c)
        if (maze.getbox(c, r) == 'G') result.setbox(c, r, 'G');

    result.display();
    cout << endl << endl;
  } else {
    maze.display();
    cout << endl << endl;
  }

  for (int r = 0; r < height; ++r) delete[] visitedRecord[r];
  delete[] visitedRecord;
}


void findNGoals() {
  if (!hasLoadedFile || lastFilename.empty()) {
    cout << endl <<"### Execute command 1 to load a maze! ###" << endl << endl;
    return;
  }
  cout << endl << "Number of G (goals): ";
  int N = 0;
  if (!(cin >> N) || N <= 0) return;

  ifstream inputFile(lastFilename);
  if (!inputFile.is_open()) {
    cout << endl << lastFilename << " does not exist!" << endl;
    return;
  }
  int width = 0, height = 0;
  inputFile >> width >> height;
  Maze maze(width, height);

  int totalGoals = 0;
  for (int r = 0; r < height; ++r) {
    string line; inputFile >> line;
    for (int c = 0; c < width; ++c) {
      maze.setbox(c, r, line[c]);
      if (line[c] == 'G') totalGoals++;
    }
  }
  inputFile.close();

  int dx[4] = {1, 0, -1, 0}; // 0:右 1:下 2:左 3:上
  int dy[4] = {0, 1, 0, -1};

  bool **visitedRecord = new bool*[height];
  for (int r = 0; r < height; ++r) {
    visitedRecord[r] = new bool[width];
    for (int c = 0; c < width; ++c) visitedRecord[r][c] = false;
  }

  Stack path;
  int startX = 0, startY = 0;
  int goalsFound = 0;

  maze.setbox(startX, startY, 'V');
  visitedRecord[startY][startX] = true;
  path.push(startX, startY, 0); // 初始偏好方向：右

  while (!path.isEmpty() && goalsFound < N) {
    StackNode* topNode = path.getTop();
    int x = topNode->x;
    int y = topNode->y;
    int lastDir = topNode->dir;

    bool moved = false;
    for (int k = 0; k < 4; ++k) {
      int tryDir = (lastDir + k) % 4;
      int nx = x + dx[tryDir];
      int ny = y + dy[tryDir];
      if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
      char cell = maze.getbox(nx, ny);

      if (cell == 'G' && !visitedRecord[ny][nx]) {
        visitedRecord[ny][nx] = true;
        topNode->dir = tryDir;      // 更新父節點方向
        path.push(nx, ny, tryDir);
        goalsFound++;
        moved = true;
        break;
      }
      if (cell == 'E' && !visitedRecord[ny][nx]) {
        maze.setbox(nx, ny, 'V');
        visitedRecord[ny][nx] = true;
        topNode->dir = tryDir;
        path.push(nx, ny, tryDir);
        moved = true;
        break;
      }
    }

    if (!moved) {
      path.pop();
    }
  }

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
    for (int r = 0; r < height; ++r)
      for (int c = 0; c < width; ++c)
        result.setbox(c, r, (maze.getbox(c, r) == 'V' ? 'E' : maze.getbox(c, r)));

    Stack temp;
    while (!path.isEmpty()) {
      StackNode* node = path.getTop();
      result.setbox(node->x, node->y, 'R');
      temp.push(node->x, node->y, node->dir);
      path.pop();
    }
    for (int r = 0; r < height; ++r)
      for (int c = 0; c < width; ++c)
        if (maze.getbox(c, r) == 'G') result.setbox(c, r, 'G');

    result.display();
    cout << endl << endl;
  } else {
    cout << endl << endl;
  }

  for (int r = 0; r < height; ++r) delete[] visitedRecord[r];
  delete[] visitedRecord;
}

void countAllGoals() { // task 3
  cout << "3" << endl;
}

void findShortestPath() { // task 4
  cout << "4" << endl;
}