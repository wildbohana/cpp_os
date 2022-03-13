/* 
Napraviti funkciju:
	
vector<int> min(const vector<int>& a, const vector<int>& b);

Ova funkcija vraća vektor koji sadrži minimume korespodentnih elemenata vektora a i b.
Npr: r[0]=min(a[0],b[0]), r[1]=...

Podrazumeva se: a.size()==b.size()
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> min(const vector<int> &a, const vector<int> &b) {

    vector<int> temp;

    for(int i = 0; i < a.size(); i++) {
        temp.push_back(min(a[i], b[i]));
	}

    return temp;
}

int main() {

    int niz1[] = {2, 3, 4, 5, 6, 7};
    int niz2[] = {11, 33, 2, 6, 7, 8};

    vector<int> a(niz1, niz1 + sizeof(niz1) / sizeof(int));
    vector<int> b(niz2, niz2 + sizeof(niz2) / sizeof(int));

    vector<int> c = min(a,b);

	// Ispis
    for(auto it : c)
        cout << it << endl;

    return 0;
}
