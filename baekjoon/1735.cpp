#include <iostream>
using namespace std;

void swap(int& x, int& y) {
	int temp;
	temp = x;
	x = y;
	y = temp;
}

int main() {
	int fa2,count = 1;
	int a1, a2, b1, b2,c1,c2;
	int result;
	cin >> a1 >> a2 >> b1 >> b2;
	if (a2 < b2) {
		swap(a1, b1);
		swap(a2, b2);
	}
	fa2 = a2;
	while (a2%b2!=0)
	{
		a2 = fa2 * count;
		count += 1;
	}
	result = a2 / b2;
	b1 *= result;
	b2 *= result;
	if (count == 1) {
		a1 = a1 * count;
	}
	else {
		a1 *= (count - 1);
	}
	c1 = a1 + b1;
	c2 = a2;
	cout << a1 << ' ' << b1 << endl;
	cout << c1 << ' '<< c2 << endl;
	if (c1 == c2) {
		c1 = 1;
		c2 = 1;
	}
	else if (c2 % c1 == 0 || c1 % c2 == 0) {
		if (c2 > c1) {
			c2 /= c1;
			c1 /= c1;
		}
		else {
			c2 /= c2;
			c1 /= c2;
		}
	}
	cout << c1 << ' ' << c2;
	// 최대공약수 구하는 식 구할것
	return 0;
}