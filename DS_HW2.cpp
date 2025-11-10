#include <iostream>

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
                std::cout << map[i][j];
            }
            std::cout << std::endl;
        }
    }
    //取單格的值
    char getbox(int x, int y) {
        return map[y][x];
    }
    //設單格的值
    char setbox(int x, int y, char val) {
        map[y][x] = val;
    }
};



int main() {



}
