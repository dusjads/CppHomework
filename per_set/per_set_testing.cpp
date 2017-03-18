#include <iostream>
#include "per_set.h"

using namespace std;

int main() {
	persistent_set pr;

	//pr.insert(0);
	pr.insert(6);
	pr.insert(4);
	pr.insert(5);
	pr.insert(2);
	pr.insert(3);
	pr.insert(1);
	pr.insert(10);
	pr.insert(8);
	pr.insert(9);
	pr.insert(7);
	pr.insert(12);
	pr.insert(11);
	pr.insert(13);
	persistent_set::iterator a = pr.find(6);
	pr.erase(a);

	persistent_set::iterator i = pr.begin();
	for (; i != pr.end(); i++) {
		cout << *i << endl;
	}
}