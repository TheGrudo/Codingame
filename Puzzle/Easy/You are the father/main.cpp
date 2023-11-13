#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    string mother, child;
    getline(cin, mother);
    getline(cin, child);
    size_t found;
    int numOfChromPair = (mother.size() - mother.find(":") - mother.substr(mother.find(":")+1,mother.size()-mother.find(":")).find_first_not_of(" "))/3;
    int startChrom = mother.find(":") + mother.substr(mother.find(":")+1,mother.size()-mother.find(":")).find_first_not_of(" ")+1;
    vector<bool> mask;
    mask.resize(numOfChromPair*2,false);
    
    for (int i = 0; i<numOfChromPair*2;++i)
    {
        found = mother.substr(startChrom+3*(i/2),2).find(child[startChrom+i+i/2]);
        if (found != string::npos) mask.at(i)=true;
    }

    int num_of_possible_fathers;
    cin >> num_of_possible_fathers; cin.ignore();
    for (int j = 0; j < num_of_possible_fathers; j++) {
        string a_possible_father;
        getline(cin, a_possible_father);
        vector<bool> fatherMask;
        fatherMask.resize(numOfChromPair*2,false);
        bool pairCheck = false;
        
        for (int i = 0; i<numOfChromPair*2;++i)
        {
            if (i%2==0) pairCheck = false;
            found = a_possible_father.substr(startChrom+3*(i/2),2).find(child[startChrom+i+i/2]);
            if (found != string::npos)
            {
                fatherMask.at(i)=true;
                pairCheck = true;
            }
            if (i%2==1 && !pairCheck) break;  
        }
        
        transform(fatherMask.begin(), fatherMask.end(), mask.begin(), fatherMask.begin(), std::logical_or<>());
        
        if (std::all_of(fatherMask.begin(), fatherMask.end(), [](bool v) { return v; }))
            cout<<a_possible_father.substr(0,a_possible_father.find(":"))<<", you are the father!"<<endl;
    }
}