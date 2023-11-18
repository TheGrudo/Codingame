#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    string MESSAGE;
    getline(cin, MESSAGE);
    string bin = "";
    string tmp="";

    int len = MESSAGE.length();
    for (int i = 0; i < len; ++i)
    {
        tmp="";
        int val = (int)(MESSAGE[i]);
        while (val > 0) 
        { 
            (val % 2)? tmp.push_back('1') : 
                       tmp.push_back('0'); 
            val /= 2; 
        }
        if (MESSAGE[i]<64)
        {
            tmp.push_back('0');
        }
        reverse(tmp.begin(), tmp.end());
        bin+=tmp;
    }
    len = bin.length();
    int j = 0;
    int n=0;
    while (j<len)
    {
        n=0;
        if (bin[j]=='0')
            cout << "00 ";
        else
            cout << "0 ";
        while (true)
        {
            cout<<"0";
            n++;
            if (bin[j]!=bin[j+n])
                break;
        }
        j+=n;
        if(j<len) cout<<" ";
    }
    cout << endl;
}