#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int dirX(string dir)
{
    if (dir == "UR" || dir == "R" || dir == "DR") return 1;
    else if (dir == "UL" || dir == "L" || dir == "DL") return -1;
    else return 0;
}

int dirY(string dir)
{
    if (dir == "UR" || dir == "U" || dir == "UL") return -1;
    else if (dir == "DR" || dir == "D" || dir == "DL") return 1;
    else return 0;
}

int main()
{
    int W; // width of the building.
    int H; // height of the building.
    cin >> W >> H; cin.ignore();
    int N; // maximum number of turns before game over.
    cin >> N; cin.ignore();
    int X0;
    int Y0;
    cin >> X0 >> Y0; cin.ignore();
    int posX = X0;
    int posY = Y0;
    int x,y;
    int xl=0, xr=W-1, yu=0, yd=H-1;

    // game loop
    while (1) {
        string bombDir; // the direction of the bombs from batman's current location (U, UR, R, DR, D, DL, L or UL)
        cin >> bombDir; cin.ignore();
        cerr<<bombDir<<" "<<posX<<" "<<posY<<" "<<xl<<xr<<yu<<yd<<endl;

        if (dirX(bombDir)==-1)
        {
            xr = posX;
            x = (xr+xl)/2;
            if (x==posX) x--;
        } 
        else if (dirX(bombDir)==1)
        {
            xl = posX;
            x = (xr+xl)/2;
            if (x==posX) x++;
        }
        else 
        {
            x = posX;
        }

        if (dirY(bombDir)==-1)
        {
            yd = posY;
            y = (yd+yu)/2;
            if (y==posY) y--;
        } 
        else if (dirY(bombDir)==1)
        {
            yu = posY;
            y = (yd+yu)/2;
            if (y==posY) y++;
        }
        else 
        {
            y = posY;
        }

        posX=x;
        posY=y;

        // the location of the next window Batman should jump to.
        cout <<x<< " "<<y << endl;
    }
}