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

vector<int> min(const vector<int>& a, const vector<int>& b)
{
	vector<int> ret_val;

	vector<int>::const_iterator cia;
    vector<int>::const_iterator cib;

	for (cia = a.begin(), cib = b.begin(); cia != a.end(); cia++, cib++)
	{
		ret_val.push_back(min(*cia, *cib));
	}

	return ret_val;
}

int main()
{
	vector<int> a = {5, 9, 12, 7, 3, 1, 28, 16};
	vector<int> b = {7, 3, 6, 12, 9, 5, 42, 8};

	vector<int> c = min(a, b);

	vector<int>::iterator it;

	for (it = c.begin(); it != c.end(); it++)
		cout << *it << " ";
	cout << endl;

    return 0;
}
