#include <iostream>
#include <string>
#include <fstream>
#include <limits>
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
      cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
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

void findOneGoal() { // task 1
  string filename = getfile();
  ifstream inputFile(filename);
  if (!inputFile.is_open()) {
    cout << endl << filename << " does not exist!" << endl << endl;
    return;
  }
  // record loded file
  lastFilename = filename;
  hasLoadedFile = true;
  // read maze size
  int width = 0, height = 0;
  inputFile >> width >> height;
  // create maze
  Maze maze(width, height);
  for (int row = 0; row < height; ++row) {
    string line; inputFile >> line;
    for (int col = 0; col < width; ++col) {
      maze.setbox(col, row, line[col]);
    }
  }
  inputFile.close();

  int dx[4] = {1, 0, -1, 0};   // 0:右 1:下 2:左 3:上
  int dy[4] = {0, 1, 0, -1};

  Stack path;
  int startX = 0, startY = 0;
  bool found = false;

  // visited record
  bool **visitedRecord = new bool*[height];
  for (int r = 0; r < height; ++r) {
    visitedRecord[r] = new bool[width];
    for (int c = 0; c < width; ++c) {
      visitedRecord[r][c] = false;
    }
  }

  // start point
  maze.setbox(startX, startY, 'V');
  visitedRecord[startY][startX] = true;
  path.push(startX, startY, 0); // 初始偏好方向：右(0)

  while (!path.isEmpty()) {
    StackNode* topNode = path.getTop();
    int x = topNode->x;
    int y = topNode->y;
    int lastDir = topNode->dir; // preferred direction

    bool moved = false;
    // Try preferred direction first
    for (int k = 0; k < 4; ++k) {
      int tryDir = (lastDir + k) % 4;
      int nx = x + dx[tryDir];
      int ny = y + dy[tryDir];
      if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
        continue;
      }
      char cell = maze.getbox(nx, ny);

      if (cell == 'G') {
        topNode->dir = tryDir;
        path.push(nx, ny, tryDir);
        found = true;
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

    if (found) {
      break;
    }

    if (!moved) {
      path.pop();
    }
  }

  if (found) {
    maze.display();
    cout << endl;
    Maze result(width, height);
    for (int r = 0; r < height; ++r)
      for (int c = 0; c < width; ++c) {
        if (maze.getbox(c, r) == 'V') {
          result.setbox(c, r, 'E');
        } else {
          result.setbox(c, r, maze.getbox(c, r));
        }
      }
    Stack temp;
    while (!path.isEmpty()) {
      StackNode* node = path.getTop();
      result.setbox(node->x, node->y, 'R');
      temp.push(node->x, node->y, node->dir);
      path.pop();
    }
    // keep all 'G'
    for (int r = 0; r < height; ++r) {
      for (int c = 0; c < width; ++c) {
        if (maze.getbox(c, r) == 'G') {
          result.setbox(c, r, 'G');
        }
      }
    }
    result.display();
    cout << endl << endl;
  } else {
    // no goal found, only display
    maze.display();
    cout << endl << endl;
  }

  for (int r = 0; r < height; ++r) {
    delete[] visitedRecord[r];
  }
  delete[] visitedRecord;
}


void findNGoals() { // task 2
  if (!hasLoadedFile || lastFilename.empty()) {
    cout << endl <<"### Execute command 1 to load a maze! ###" << endl << endl;
    return;
  }
  int N = 0;
  while (true) {
    cout << endl << "Number of G (goals): ";
    if (!(cin >> N)) {
      cin.clear();
      string dummy;
      getline(cin, dummy);
      continue;
    }
    if (N == 0) {
      cout << endl << "### The number must be in [1,100] ###" << endl;
    }
    if (N > 0 && N <= 100) {
      break;
    }
  }
  ifstream inputFile(lastFilename);
  if (!inputFile.is_open()) {
    cout << endl << lastFilename << " does not exist!" << endl << endl;
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
      if (line[c] == 'G') {
        totalGoals++;
      }
    }
  }
  inputFile.close();

  int dx[4] = {1, 0, -1, 0}; // 0:右 1:下 2:左 3:上
  int dy[4] = {0, 1, 0, -1};

  bool **visitedRecord = new bool*[height];
  for (int r = 0; r < height; ++r) {
    visitedRecord[r] = new bool[width];
    for (int c = 0; c < width; ++c) {
      visitedRecord[r][c] = false;
    }
  }

  // 記錄所有已確定為「通往任何已找到 G 的路徑」之座標
  bool **onPath = new bool*[height];
  for (int r = 0; r < height; ++r) {
    onPath[r] = new bool[width];
    for (int c = 0; c < width; ++c) {
      onPath[r][c] = false;
    }
  }

  Stack path;
  int startX = 0, startY = 0;
  int goalsFound = 0;

  maze.setbox(startX, startY, 'V');
  visitedRecord[startY][startX] = true;
  path.push(startX, startY, 0);

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
      if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
        continue;
      }
      char cell = maze.getbox(nx, ny);

      if (cell == 'G' && !visitedRecord[ny][nx]) {
        visitedRecord[ny][nx] = true;
        topNode->dir = tryDir;
        path.push(nx, ny, tryDir);
        goalsFound++;
        // 將目前堆疊中的所有節點都標記為結果路徑的一部分
        for (StackNode* p = path.getTop(); p != nullptr; p = p->next) {
          onPath[p->y][p->x] = true;
        }
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
      for (int c = 0; c < width; ++c) {
        if (maze.getbox(c, r) == 'V') {
          result.setbox(c, r, 'E');
        } else {
          result.setbox(c, r, maze.getbox(c, r));
        }
      }
    // 將累積的所有路徑節點標記為 'R'
    for (int r = 0; r < height; ++r) {
      for (int c = 0; c < width; ++c) {
        if (onPath[r][c]) {
          result.setbox(c, r, 'R');
        }
      }
    }
    for (int r = 0; r < height; ++r) {
      for (int c = 0; c < width; ++c) {
        if (maze.getbox(c, r) == 'G') {
          result.setbox(c, r, 'G');
        }
      }
    }
    result.display();
    cout << endl << endl;
  } else {
    cout << endl << endl;
  }

  for (int r = 0; r < height; ++r) {
    delete[] visitedRecord[r];
  }
  delete[] visitedRecord;
  for (int r = 0; r < height; ++r) {
    delete[] onPath[r];
  }
  delete[] onPath;
}

void countAllGoals() { // task 3
  if (!hasLoadedFile || lastFilename.empty()) {
    cout << endl <<"### Execute command 1 to load a maze! ###" << endl << endl;
    return;
  }

  ifstream inputFile(lastFilename);
  if (!inputFile.is_open()) {
    cout << endl << lastFilename << " does not exist!" << endl << endl;
    return;
  }

  int width = 0, height = 0;
  inputFile >> width >> height;
  Maze maze(width, height);

  int totalGoals = 0;
  for (int r = 0; r < height; ++r) {
    string line;
    inputFile >> line;
    for (int c = 0; c < width; ++c) {
      maze.setbox(c, r, line[c]);
      //if (line[c] == 'G') {
      //  totalGoals++;
      //}
    }
  }
  inputFile.close();

  int dx[4] = {1, 0, -1, 0};
  int dy[4] = {0, 1, 0, -1};

  bool **visitedRecord = new bool*[height];
  for (int r = 0; r < height; ++r) {
    visitedRecord[r] = new bool[width];
    for (int c = 0; c < width; ++c) {
      visitedRecord[r][c] = false;
    }
  }

  int startX = 0, startY = 0;
  int goalsVisited = 0; // 實際走到的目標數

  visitedRecord[startY][startX] = true;
  // 起點若為 'E' 改標記為 'V'; 若意外是 'G' 則保持 'G'
  if (maze.getbox(startX, startY) == 'E') {
    maze.setbox(startX, startY, 'V');
  }

  Stack path;
  path.push(startX, startY, 0);

  while (!path.isEmpty()) {
    StackNode* topNode = path.getTop();
    int x = topNode->x;
    int y = topNode->y;
    int lastDir = topNode->dir;

    bool moved = false;
    for (int k = 0; k < 4; ++k) {
      int tryDir = (lastDir + k) % 4;
      int nx = x + dx[tryDir];
      int ny = y + dy[tryDir];
      if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
        continue;
      }
      char cell = maze.getbox(nx, ny);

      if ((cell == 'E' || cell == 'G') && !visitedRecord[ny][nx]) {
        visitedRecord[ny][nx] = true;
        if (cell == 'G') {
          goalsVisited++; // 訪問到一個目標，保留 'G' 不覆蓋
          totalGoals++;
        } else {
          // 一般可走空格標記為 'V'
          maze.setbox(nx, ny, 'V');
        }
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

  cout << "The maze has " << totalGoals << " goal(s) in total." << endl << endl;


  for (int r = 0; r < height; ++r) {
    delete[] visitedRecord[r];
  }
  delete[] visitedRecord;
}



void findShortestPath() { // task 4
  string filename = getfile();
  ifstream inputFile(filename);
  if (!inputFile.is_open()) {
    cout << endl << filename << " does not exist!" << endl;
    return;
  }

  int width = 0, height = 0;
  inputFile >> width >> height;
  Maze maze(width, height);
  char **original = new char*[height];
  for (int r = 0; r < height; ++r) {
    original[r] = new char[width];
    string line; inputFile >> line;
    for (int c = 0; c < width; ++c) {
      maze.setbox(c, r, line[c]);
      original[r][c] = line[c];
    }
  }
  inputFile.close();

  // 固定方向順序：右(0)、下(1)、左(2)、上(3)
  int dx[4] = {1, 0, -1, 0};
  int dy[4] = {0, 1, 0, -1};

  struct Node { int x, y; };

  bool **visited = new bool*[height];
  pair<int,int> **parent = new pair<int,int>*[height];
  for (int i = 0; i < height; ++i) {
    visited[i] = new bool[width];
    parent[i] = new pair<int,int>[width];
    for (int j = 0; j < width; ++j) {
      visited[i][j] = false;
      parent[i][j] = {-1, -1};
    }
  }

  Node *queue = new Node[width * height];
  int front = 0, rear = 0;

  int startX = 0, startY = 0;
  visited[startY][startX] = true;
  queue[rear++] = {startX, startY};
  if (maze.getbox(startX, startY) == 'E') {
    maze.setbox(startX, startY, 'V');
  }

  bool found = false;
  int goalX = -1, goalY = -1;

  while (front < rear) {
    Node cur = queue[front++];

    // 若當前格為 'G'，找到最短路徑
    if (original[cur.y][cur.x] == 'G') {
      found = true;
      goalX = cur.x; goalY = cur.y;
        break;
    }

    // 固定順序擴展：右→下→左→上
    for (int dir = 0; dir < 4; ++dir) {
      int nx = cur.x + dx[dir];
      int ny = cur.y + dy[dir];
      
      if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

      char cell = original[ny][nx];
      if (cell != 'E' && cell != 'G') continue;
      if (visited[ny][nx]) continue;

      visited[ny][nx] = true;
      parent[ny][nx] = {cur.x, cur.y};

      // 標記探索過的格子為 'V'
      if (cell == 'E') {
        maze.setbox(nx, ny, 'V');
      }

      // 找到 'G' 則記錄並繼續（讓同層其他節點也能擴展）
      if (cell == 'G') {
        found = true;
        goalX = nx; goalY = ny;
        queue[rear++] = {nx, ny};
        // break; //找到後立即跳出此層循環
      }

      queue[rear++] = {nx, ny};
    }

    if (found) {
      // break; // 找到 G 後跳出主循環
    }
  }

  // 輸出探索地圖（含 V）
  maze.display();
  cout << endl;

  if (!found) {
    cout << endl << "### There is no path to find a goal! ###" << endl << endl;
    // 釋放資源
    for (int i = 0; i < height; ++i) { 
      delete[] visited[i];
      delete[] parent[i];
      delete[] original[i];
    }
    delete[] visited;
    delete[] parent;
    delete[] original;
    delete[] queue;
    return;
  }

  // 回溯最短路徑
  Maze result(width, height);
  for (int r = 0; r < height; ++r)
    for (int c = 0; c < width; ++c)
      result.setbox(c, r, original[r][c]);

  int pathLen = 1;
  int x = goalX, y = goalY;
  while (!(x == startX && y == startY)) {
    if (result.getbox(x, y) != 'G') {
      result.setbox(x, y, 'R');
    }
    int px = parent[y][x].first;
    int py = parent[y][x].second;
    if (px == -1 && py == -1) break;
    x = px; y = py; ++pathLen;
  }
  result.setbox(startX, startY, 'R');

  result.display();
  cout << endl;
  cout << "Shortest path length = " << pathLen << endl << endl;

  // 釋放資源
  for (int i = 0; i < height; ++i) { 
    delete[] visited[i]; 
    delete[] parent[i]; 
    delete[] original[i]; 
  }
  delete[] visited; 
  delete[] parent; 
  delete[] original; 
  delete[] queue;
}
