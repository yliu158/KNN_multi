#include <thread>
#include <iostream>
#include "Pool.cpp"

using namespace std;

void hello(int x, int y) {
  cout << x << endl;
  cout << y << endl;
}

int main(int argc, char const *argv[]) {
  thread t1(hello, 10, 20);
  thread t2(hello, 30, 40);
  t1.join();
  t2.join();
  return 0;
}
