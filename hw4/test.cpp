#include<iostream>
#include<stdio.h>
#include <stdlib.h>
using namespace std;

class A{
private:
  int _a;
  int _b;
  int _c;
  short _d;
  // sizeof(A) = 14 à 16 Bytes
public:
  A() {}
  ~A() {}
  static void* operator new(size_t t) {
    cout << "new (inside A): " << endl;
    cout << ">> size = " << t << endl;
    A* p = (A*)malloc(t);
    cout << ">> ptr = " << p << endl;
    return p;
  }
  static void* operator new[](size_t t) {
    cout << "new[] (inside A): " << endl;
    cout << ">> size = " << t << endl;
    A* p = (A*)malloc(t);
    cout << ">> ptr = " << p << endl;
    return p;
  }
  static void operator delete(void* p) {
    cout << "delete (inside A): " << endl;
    cout << ">> ptr = " << p << endl;
    free(p);
  }
  static void operator delete[](void* p) {
    cout << "delete[] (inside A): " << endl;
    cout << "n:"<<*(size_t*)p << endl;
    cout << ">> ptr = " << p << endl;
    free(p);
  }
};

int main(){
  A* a = new A;
  cout << endl;
  cout << "new (in main): " << endl;
  cout << ">> ptr = " << a << endl;
  cout << endl;
  A* b = new A[10];
  cout << endl;
  cout << "new[] (in main): " << endl;
  cout << ">> ptr = " << b << endl;
  cout << endl;
  delete a;
  cout << endl;
  delete []b;
  cout << endl;

  // char* a = new char[10];
  // for (size_t i = 0; i < 10; i++) {
  //   *(a+i) = 48+i;
  //   cout << *(a+i) << endl;
  // }
  // char* temp1 = a;
  // cout << static_cast<const void*>(temp1) << endl;
  // temp1 += 1;
  // cout << static_cast<const void*>(temp1) << endl;
  // int *temp = (int*) a;
  // cout << temp << endl;
  // temp += 1;
  // cout << temp << endl;
  // temp = (int*) a;
  // *temp = 1024;
  // for (size_t i = 0; i < 10; i++) {
  //   cout << (int)*(a+i) << endl;
  // }
  //
  //
  return 0;

}
