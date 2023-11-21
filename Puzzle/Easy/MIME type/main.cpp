#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

int main()
{
    int pos;
    int N; // Number of elements which make up the association table.
    cin >> N; cin.ignore();
    int Q; // Number Q of file names to be analyzed.
    cin >> Q; cin.ignore();
    // vector<pair<string,string>> conv;
    unordered_map<string,string> conv;
    for (int i = 0; i < N; i++) {
        string EXT; // file extension
        string MT; // MIME type.
        cin >> EXT >> MT; cin.ignore();
        transform(EXT.begin(), EXT.end(), EXT.begin(), [](unsigned char c){ return tolower(c);});
        conv[EXT]=MT;
    }
    for (int i = 0; i < Q; i++) {
        string FNAME;
        getline(cin, FNAME); // One file name per line.
        //cerr<<FNAME<<endl;
        pos = FNAME.find_last_of(".");
        if (pos!=string::npos)
        {
            FNAME = FNAME.substr(pos+1);
            transform(FNAME.begin(), FNAME.end(), FNAME.begin(), [](unsigned char c){ return tolower(c);});
            auto it = conv.find(FNAME);
            if (it!=conv.end())
                cout<<conv[FNAME]<<endl;
            else
            {
                cout<<"UNKNOWN"<<endl;
            }
        }
        else
            cout<<"UNKNOWN"<<endl;
    }
}