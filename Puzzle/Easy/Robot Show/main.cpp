#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    vector<int> bots;
    int l, n;
    cin >> l >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        int b;
        cin >> b; cin.ignore();
        bots.push_back(b);
    }
    std::sort(bots.begin(), bots.end());
    cout << ((l-bots[0]>bots[n-1])?(l-bots[0]):bots[n-1]) << endl;
}