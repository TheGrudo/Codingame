#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    int L;
    cin >> L; cin.ignore();
    int H;
    cin >> H; cin.ignore();
    string T;
    getline(cin, T);
    string ROW[H];
    int litera=0;
    for (int i = 0; i < H; i++) {
        getline(cin, ROW[i]);
    }

    transform(T.begin(),T.end(),T.begin(),::toupper);
    for (int i = 0; i<H;++i)
    {
        for (int j=0;j<T.length();++j)
        {
            litera = T[j]-'A';
            if(litera>=0 && litera<26)
            {
                for (int k=litera*L;k<(litera+1)*L;++k)
                {
                    cout<<ROW[i][k];
                }
            }
            else
            {
                for (int k=26*L;k<27*L;++k)
                {
                    cout<<ROW[i][k];
                }
            }
        }
        cout<<endl;
    }
}