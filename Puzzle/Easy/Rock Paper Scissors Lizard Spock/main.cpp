#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int typ (string t)
{
    if (t=="R") return 0;
    else if (t=="P") return 1;
    else if (t=="C") return 2;
    else if (t=="L") return 3;
    else if (t=="S") return 4;
    else
    {
        cerr<<"unknown type "<<t<<endl;
        return -1;
    }
}

int duel (pair<int,string> g1,pair<int,string> g2)
{
    int wyniki[5][5] = {{0,-1,1,1,-1},
                        {1,0,-1,-1,1},
                        {-1,1,0,1,-1},
                        {-1,1,-1,0,1},
                        {1,-1,1,-1,0}};
    
    return wyniki[typ(g1.second)][typ(g2.second)];
}

int main()
{
    int N;
    int res;
    cin >> N; cin.ignore();
    vector<pair<int,string>> gracze;
    vector<vector<int>> oppos(N);
    for (int i = 0; i < N; i++) {
        int NUMPLAYER;
        string SIGNPLAYER;
        cin >> NUMPLAYER >> SIGNPLAYER; cin.ignore();
        gracze.push_back(make_pair(NUMPLAYER,SIGNPLAYER));
    }
    while(gracze.size()>1)
    {
        for(int i = 1;i<gracze.size();++i)
        {
            res = duel(gracze[i-1],gracze[i]);
            if (res==-1) 
            {
                oppos[i].push_back(gracze[i-1].first);
                oppos.erase(oppos.begin()+i-1);
                gracze.erase(gracze.begin()+i-1);
            }
            else if (res==1) 
            {
                oppos[i-1].push_back(gracze[i].first);
                oppos.erase(oppos.begin()+i);
                gracze.erase(gracze.begin()+i);
            }
            else if (res==0) 
            {
                if (gracze[i-1].first>gracze[i].first) 
                {
                    oppos[i].push_back(gracze[i-1].first);
                    oppos.erase(oppos.begin()+i-1);
                    gracze.erase(gracze.begin()+i-1);
                }
                else
                {
                    oppos[i-1].push_back(gracze[i].first);
                    oppos.erase(oppos.begin()+i);
                    gracze.erase(gracze.begin()+i);
                }
            }
            else
                cerr<<"something went wrong "<<res<<endl;
        }
    }
    cout << gracze[0].first << endl;
    for (int i = 0;i<oppos[0].size();++i)
    {
        cout<<oppos[0][i];
        if (i!=oppos[0].size()-1)
            cout<<" ";
    }
    cout<<endl;
}