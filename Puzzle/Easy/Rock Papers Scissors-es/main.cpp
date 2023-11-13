#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

unordered_map<string, int> conv = {
    {"Rock",0},
    {"Paper",1},
    {"Scissors",2}
};

unordered_map<string, string> toWin = {
    {"Rock","Paper"},
    {"Paper","Scissors"},
    {"Scissors","Rock"}
};

vector<vector<int>> res = {
    {0,-1,1},
    {1,0,-1},
    {-1,1,0}
};

int main()
{
    int n, best_start = 0, best_score = 0;
    vector<string> opp;
    cin >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        string a;
        getline(cin, a);
        opp.push_back(a);
    }

    for (int i = 0; i<n;++i)
    {
        int result = 0, score = 0, j = i;
        while(result>=0)
        {
            result = res[conv[toWin[opp[i]]]][conv[opp[j]]];
            score += result;
            ++j;
            if (j==i) break;
            if (j>=n) j = 0;
        }
        if (score > best_score)
        {
            best_score = score;
            best_start = i;
        }
    }

    cout << toWin[opp[best_start]] << endl;
    cout << best_start << endl;
}