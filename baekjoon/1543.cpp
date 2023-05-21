#include <iostream>
#include <string>
using namespace std;

int main() {
	string str1,str2;
	int index;
	int count = 0;
	getline(cin, str1, '\n');
	getline(cin, str2, '\n');
	while (1) {
		if (str1.find(str2) != string::npos) {
			index = str1.find(str2);
			str1.erase(0, index+str2.length());
			count++;
		}
		else {
			break;
		}
	}
	cout << count;
	return 0;
}