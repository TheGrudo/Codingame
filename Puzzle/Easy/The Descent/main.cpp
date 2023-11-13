#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * The while loop represents the game.
 * Each iteration represents a turn of the game
 * where you are given inputs (the heights of the mountains)
 * and where you have to print an output (the index of the mountain to fire on)
 * The inputs you are given are automatically updated according to your last actions.
 **/

int main()
{
    int gory[8];
    int maxv = 0;
    int maxi = 0;
    // game loop
    while (1) {
        maxv=0;
        maxi=0;
        for (int i = 0; i < 8; i++) {
            int mountainH; // represents the height of one mountain.
            cin >> mountainH; cin.ignore();
            gory[i]=mountainH;
        }

        for (int j = 0; j<8; ++j)
        {
            if (gory[j]>maxv)
            {
                maxv = gory[j]; 
                maxi=j;
            }
        }
        cout << maxi << endl; // The index of the mountain to fire on.
    }
}