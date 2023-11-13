#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * You're capable of such beautiful dreams, and such horrible nightmares.
 * You feel so lost, so cut off, so alone, only you're not.
 * See, in all our searching, the only thing we've found that makes the emptiness bearable, is each other.
 * (a quote from "Contact")
 **/

class Spiral
{
    public:
    int sideSize;
    vector<int> startPoint;
    int direction;
    int dirChanges = 0;
    vector<vector<int>> moves = {{0,1},{1,0},{0,-1},{-1,0}};
    vector<int> bounds = {0,0,0,0};
    int clockwise;
    int startLetter;
    int letterInc;
    int startCount;
    int countInc;
    int currLetter;
    vector<int> currPosition;
    int currCount;
    int letterCount = 0;
    vector<vector<int>> square;
    
    Spiral(string s)
    {
        startPoint.resize(2);
        vector<string> input;
        string delimiter = " ";
        size_t pos = 0;
        while ((pos = s.find(delimiter)) != string::npos) 
        {
            input.push_back(s.substr(0, pos));
            s.erase(0, pos + delimiter.length());
        }
        input.push_back(s);

        sideSize = stoi(input[0]);
        square.resize(sideSize, vector<int>(sideSize));

        if (input[1]=="topLeft") {startPoint = {0,0}; direction = 0;}
        else if (input[1]=="topRight") {startPoint = {0,sideSize-1}; direction = 1;}
        else if (input[1]=="bottomRight") {startPoint = {sideSize-1,sideSize-1}; direction = 2;}
        else if (input[1]=="bottomLeft") {startPoint = {sideSize-1,0}; direction = 3;}
        else
            cerr<<"Wrong starting point "<<input[1]<<endl;
        currPosition = startPoint;

        if (input[2]=="clockwise") clockwise = 1;
        else if (input[2]=="counter-clockwise") {clockwise = -1; direction = (direction+1)%4;}
        else
            cerr<<"Wrong direction "<<input[2]<<endl;

        startLetter = currLetter = int(input[3][0]);
        letterInc = input[4][0] - startLetter;
        startCount = currCount = stoi(input[3].substr(1,input[3].size()-1));
        countInc = stoi(input[4].substr(1,input[4].size()-1)) - startCount;
    }

    vector<int> nextPosision()
    {
        vector<int> next;
        next.resize(2);
        std::transform (currPosition.begin(), currPosition.end(), moves[direction].begin(), next.begin(), std::plus<int>());
        if (next[0]<0+bounds[1] || next[0] >= sideSize-bounds[3] || next[1]<0+bounds[0] || next[1]>= sideSize-bounds[2])
        {
            direction = (4+(direction+clockwise))%4;
            bounds[direction]+=2;
            std::transform (currPosition.begin(), currPosition.end(), moves[direction].begin(), next.begin(), std::plus<int>());
        }  
        return next;
    }

    void build()
    {
        while(currLetter>='A' && currLetter<='Z' && inBounds() && currCount > 0)
        {
            square[currPosition[0]][currPosition[1]] = currLetter;
            letterCount++;
            if (letterCount>=currCount)
            {
                letterCount = 0;
                currCount += countInc;
                currLetter += letterInc;
            }
            currPosition = nextPosision();
        }
    }

    bool inBounds()
    {
        if (bounds[0]+bounds[2]>sideSize || bounds[1]+bounds[3]>sideSize)
            return false;
        else
            return true;
    }

    void print()
    {
        int printSize = (sideSize>31)?31:sideSize;
        for (int i = 0; i<printSize;++i)
        {
            for (int j = 0; j<printSize;++j)
            {
                if (square[i][j]>='A' && square[i][j]<='Z')
                    cout<<(char)square[i][j];
                else
                    cout<<" ";
            }
            cout<<endl;
        }
    }
};

int main()
{
    string cryptic_instructions;
    getline(cin, cryptic_instructions);
    cerr<<cryptic_instructions<<endl;
    
    Spiral spiral(cryptic_instructions);
    spiral.build();
    spiral.print();
}