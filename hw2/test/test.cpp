#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
	int a[4] = {1,2,3,4};
	int* p = a;
	int* e = a+4;
	cout << (p+3) << " " << e << " " << (int)(e-(p+3)) << endl;
}
