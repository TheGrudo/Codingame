#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

unordered_map<string,string> conv = {{"sp"," "},{"bS","\\"},{"sQ","'"},{"nl","\n"}};

int main()
{
    string t, s;
    int number;
    while(cin>>t)
    {
        number = 1;
        if (isalpha(t[t.size()-2]))
        {
            if (t.size()>2) number = stoi(t.substr(0,t.size()-2));
            s = conv[t.substr(t.size()-2,2)];
        }
        else
        {
            number = stoi(t.substr(0,t.size()-1));
            s = t[t.size()-1];
        }
        for (int i = 0; i<number;++i) cout<<s;
    }
}