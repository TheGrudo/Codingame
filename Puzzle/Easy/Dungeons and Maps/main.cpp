#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cstdio>

using namespace std;

int main()
{
    int w;
    int h;
    cin >> w >> h; cin.ignore();
    int startRow;
    int startCol;
    cin >> startRow >> startCol; cin.ignore();
    int n;
    cin >> n; cin.ignore();
    int *dlg = new int[n]; 
    int **mapy = new int*[h];
    int posX = startCol;
    int posY = startRow;
    int steps;
    int min_val = 9999;
    int min_id = 0;
    for (int i = 0; i < h; ++i)
    {
        mapy[i]= new int[w];
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < h; j++) {
            string mapRow;
            getline(cin, mapRow);
            for (int k = 0; k<w;++k)
            {
                if ((char)mapRow[k]=='.') mapy[j][k]=4;
                else if ((char)mapRow[k]=='#') mapy[j][k]=4;
                else if ((char)mapRow[k]=='^') mapy[j][k]=0;
                else if ((char)mapRow[k]=='>') mapy[j][k]=1;
                else if ((char)mapRow[k]=='v') mapy[j][k]=2;
                else if ((char)mapRow[k]=='<') mapy[j][k]=3;
                else if ((char)mapRow[k]=='T') mapy[j][k]=9;
            } 
        }
        steps=0;
        posX = startCol;
        posY = startRow;

        while(1)
        {
            steps++;
            if (steps>=min_val)
            {
                dlg[i]=9999;
                break;
            }
            if (posX>=w || posX<0 || posY >=h || posY<0 || mapy[posY][posX]==4)
            {
                dlg[i]=9999;
                break;
            }
            if (mapy[posY][posX]==9)
            {
                dlg[i]=steps;
                min_val=steps;
                min_id = i;
                break;
            }
            if (mapy[posY][posX]==0) posY--;
            else if (mapy[posY][posX]==1) posX++;
            else if (mapy[posY][posX]==2) posY++;
            else if (mapy[posY][posX]==3) posX--;
        }
    }

    if (dlg[min_id]==9999)
        cout << "TRAP" << endl;
    else 
        cout << min_id << endl;
}