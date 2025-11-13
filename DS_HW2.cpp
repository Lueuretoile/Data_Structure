#include <iostream>
#include <string>
#include <fstream>
using namespace std;
void findOneGoal();
void findNGoals();
void countAllGoals();
void findShortestPath();

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
  // 取得檔名並開啟指定的迷宮檔案
  string filename = getfile();
  ifstream inputFile(filename);
  if (!inputFile.is_open()) {
    cout << endl
         << filename << " does not exist!" << endl;
    return;
  }

  // 讀取迷宮尺寸
  int width = 0;
  int height = 0;
  inputFile >> width >> height;

  // 建立迷宮並載入內容
  Maze maze(width, height);
  for (int row = 0; row < height; ++row) {
    string line;
    inputFile >> line;
    for (int col = 0; col < width; ++col) {
      maze.setbox(col, row, line[col]);
    }
  }
  inputFile.close();

  // 定義四個方向（右、下、左、上）
  int dx[4] = {1, 0, -1, 0};
  int dy[4] = {0, 1, 0, -1};

  // 準備深度優先搜尋
  Stack path;
  int startX = 0;
  int startY = 0;


  // 建立訪問紀錄陣列
  bool **visitedRecord = new bool*[height];
  for (int row = 0; row < height; ++row) {
    visitedRecord[row] = new bool[width];
    for (int col = 0; col < width; ++col) {
      visitedRecord[row][col] = false;
    }
  }

  // 將起點標記為已訪問並push into stack
  bool found = false;
  maze.setbox(startX, startY, 'V');
  visitedRecord[startY][startX] = true;
  path.push(startX, startY, 0);

  // main loop
  while (!path.isEmpty()) {
    StackNode *topNode = path.getTop();
    int x = topNode->x;
    int y = topNode->y;
    int dir = topNode->dir;

    bool moved = false;

    // 先檢查四周是否有目標，確保能優先找到最近的 'G'
    for (int i = 0; i < 4; ++i) {
      int nx = x + dx[i];
      int ny = y + dy[i];

      if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
        continue;
      }

      if (maze.getbox(nx, ny) == 'G') {
        topNode->dir = i + 1;
        path.push(nx, ny, 0);
        found = true;
        moved = true;
        break;
      }
    }

    if (found) {
      break;
    }

    // 若鄰近沒有 'G'，依照既定方向尋找可走的空地
    for (int i = dir; i < 4; ++i) {
      int nx = x + dx[i];
      int ny = y + dy[i];

      if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
        continue;
      }

      char cell = maze.getbox(nx, ny);
      if (cell == 'E') {
        topNode->dir = i + 1;
        maze.setbox(nx, ny, 'V');
        visitedRecord[ny][nx] = true;
        path.push(nx, ny, 0);
        moved = true;
        break;
      }
    }

    // 若四周皆不可走，回退並清除標記
    if (!moved) {
      StackNode *deadEnd = path.getTop();
      int deadX = deadEnd->x;
      int deadY = deadEnd->y;
      path.pop();
      if (!(deadX == startX && deadY == startY)) {
        maze.setbox(deadX, deadY, 'E');
      }
    }
  }

  // result
  if (found) {
    // 搜尋時的訪問狀態
    maze.display();
    cout << endl;

    // 建立結果地圖並還原非路徑的 'V'
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

    // 將stack中的路徑標記為 'R'
    Stack tempStack;
    while (!path.isEmpty()) {
      StackNode *node = path.getTop();
      result.setbox(node->x, node->y, 'R');
      tempStack.push(node->x, node->y);
      path.pop();
    }

    // 保留迷宮中的目標符號
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
    // 未找到目標時還原所有訪問過的空地
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        if (visitedRecord[row][col] && maze.getbox(col, row) == 'E') {
          maze.setbox(col, row, 'V');
        }
      }
    }
    maze.display();
    cout << endl << endl;
  }

  // 釋放訪問紀錄陣列
  for (int row = 0; row < height; ++row) {
    delete[] visitedRecord[row];
  }
  delete[] visitedRecord;
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

