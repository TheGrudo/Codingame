#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    int N;
    cin >> N; cin.ignore();
    vector<int> P;
    int Pi;
    for (int i = 0; i < N; i++) {
        cin >> Pi; cin.ignore();
        P.push_back(Pi);
    }
    sort(P.begin(),P.end());
    int dif = 999999;
    int dlg = P.size();
    for (int i = 1;i<dlg;++i)
    {
        if (P[i]-P[i-1]<dif)
            dif = P[i]-P[i-1];
    }

    cout << dif << endl;
}