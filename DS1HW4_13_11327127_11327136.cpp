// 資訊二甲 11327136 陳彥均 11327127 林煒博
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <chrono> // 計時用
using namespace std;

// define functions
void task1();
void task2();
void task3();
void task4();

struct Order {
  int OID;
  int arrival;
  int duration;
  int timeOut;
};

struct LogEntry {
  int OID;
  int CID;        // 廚師編號 (Chef ID)
  int Time;       // 取消時刻 (Abort) 或 完成時間 (Departure)
  int Delay;
};

class myQueue {
 private:
  struct Node {
    Order data;
    Node* next;
  };

  Node* front;
  Node* rear;
  int currentSize;
  int capacity;

 public:
  myQueue() {
    front = nullptr;
    rear = nullptr;
    currentSize = 0;
    capacity = 3;
  }

  ~myQueue() {
    Node* current = front;
    while (current != nullptr) {
      Node* nextNode = current->next;
      delete current;
      current = nextNode;
    }

    front = nullptr;
    rear = nullptr;
    currentSize = 0;
  }

  bool isEmpty() const {return currentSize == 0; }
  bool isFull() const {return currentSize >= capacity; }
  int size() const {return currentSize; }

  bool enqueue(Order item) {
    if (isFull()) {
      return false;
    }
    
    Node* newNode = new Node;
    newNode->data = item;
    newNode->next = nullptr;

    if (isEmpty()) {
      front = newNode;
      rear = newNode;
    } else {
      rear->next = newNode;
      rear = newNode;
    }
    currentSize++;
    return true;
  }

  bool dequeue(Order& item) {
    if (isEmpty()) {
      return false;
    }
    
    Node* temp = front;
    item = temp->data;

    front = front->next;

    if (front == nullptr) {
      rear = nullptr;
    }
    
    delete temp;
    currentSize--;

    return true;
  }

  bool peek(Order& item) {
    if (isEmpty()) {
      return false;
    }
    item = front->data;
    return true;
  }
};

void showMenu() {
  cout << "*** (^_^) Data Structure (^o^) ***" << endl;
  cout << "** Simulate FIFO Queues by SQF ***" << endl;
  cout << "* 0. Quit                        *" << endl;
  cout << "* 1. Sort a file                 *" << endl;
  cout << "* 2. Simulate one FIFO queue     *" << endl;
  cout << "* 3. Simulate two queues by SQF  *" << endl;
  cout << "* 4. Simulate some queues by SQF *" << endl;
  cout << "**********************************" << endl;
};

void ReadInput(int &command) {
  cout << "Input a command(0, 1, 2, 3, 4): ";
  if (!(cin >> command)) {
    cin.clear();
    string dummy;
    getline(cin, dummy);
    cout << endl << "Command does not exist!" << endl << endl;
    return;
  }

  if (command < 0 || command > 4) {
    cout << endl << "Command does not exist!" << endl << endl;
    return;
  }
};

string getInputID() {  
  string id;
  cout << endl << "Input a file number (e.g., 401, 402, 403, ...): ";
  cin >> id;
  return id;
}

void shellSort();

int main() {
  int command = 0;

  do {
    showMenu();
    ReadInput(command);
    switch (command) {
      case 1:
        task1();
        break;
      case 2:
        task2();
        break;
      case 3:
        task3();
        break;
      case 4:
        task4();
        break;
      case 0:
        return 0;
        break;
    }
  } while (command != 0);
  
  return 0;
}
void shellSort(Order* array, int n) {
  for (int gap = n / 2; gap > 0; gap /= 2) {
    for (int i = gap; i < n; i++) {
      Order temp = array[i];
      int j;
      for (j = i; j >= gap; j -= gap) {
        bool needShift = false;
        if (array[j - gap].arrival > temp.arrival) {
          needShift = true;
        } else if (array[j - gap].arrival == temp.arrival) {
          if (array[j - gap].OID > temp.OID) {
            needShift = true;
          }
        }

        if (needShift) {
          array[j] = array[j - gap];
        } else {
          break;
        }
      }
      array[j] = temp;
    }
  }
}

void task1() {
  string fileID = getInputID(); 
  string inputFileName = "input" + fileID + ".txt";
  string outputFileName = "sorted" + fileID + ".txt";

  auto startRead = chrono::high_resolution_clock::now(); // timer start

  ifstream inFile(inputFileName);
  if (!inFile) {
    cout << endl << "### " << inputFileName << " does not exist! ###" << endl << endl;
    return;
  }

  // 第一次讀取 計算資料筆數 (Two-pass approach)
  string headerLine;
  getline(inFile, headerLine); // 跳過第一行標題

  int totalOrders = 0;
  int temp; 
  // 嘗試讀取每一行的四個整數，讀成功一次計數加一
  while (inFile >> temp >> temp >> temp >> temp) {
    totalOrders++;
  }

  // 配置動態空間
  Order* orderList = new Order[totalOrders];

  // 第二次讀取真正讀入資料
  inFile.clear();
  inFile.seekg(0); // 游標回到檔案開頭
  getline(inFile, headerLine); // 再次跳過標題

  for (int i = 0; i < totalOrders; i++) {
    inFile >> orderList[i].OID 
           >> orderList[i].arrival 
           >> orderList[i].duration 
           >> orderList[i].timeOut;
  }
  inFile.close();

  auto endRead = chrono::high_resolution_clock::now(); // timer end
  auto readTime = chrono::duration_cast<chrono::microseconds>(endRead - startRead).count();

  cout << endl;
  cout << "\tOID\tArrival\tDuration\tTimeOut" << endl;
  for (int i = 0; i < totalOrders; i++) {
      cout << "(" << (i + 1) << ")\t" 
           << orderList[i].OID << "\t" 
           << orderList[i].arrival << "\t" 
           << orderList[i].duration << "\t\t" 
           << orderList[i].timeOut << endl;
  }
    
  // sort
  auto stratSort = chrono::high_resolution_clock::now();
  shellSort(orderList, totalOrders);
  auto endSort = chrono::high_resolution_clock::now();

  auto sortTime = chrono::duration_cast<chrono::microseconds>(endSort - stratSort).count();

  // write in
  auto startWrite = chrono::high_resolution_clock::now();
  ofstream outFile(outputFileName);

  outFile << "OID\tArrival\tDuration\tTimeOut" << endl;
  for (int i = 0; i < totalOrders; i++) {
    outFile << orderList[i].OID << "\t"
            << orderList[i].arrival << "\t"
            << orderList[i].duration << "\t"
            << orderList[i].timeOut << endl;
  }
  outFile.close();

  auto endWrite = chrono::high_resolution_clock::now();
  auto writeTime = chrono::duration_cast<chrono::microseconds>(endWrite - startWrite).count();

  cout << "\nReading data: " << readTime << " us." << endl;
  cout << "\nSorting data: " << sortTime << " us." << endl;
  cout << "\nWriting data: " << writeTime << " us." << endl;
  cout << endl;
  delete[] orderList;
}

void task2() {

}

void task3() {

}

void task4() {

}
