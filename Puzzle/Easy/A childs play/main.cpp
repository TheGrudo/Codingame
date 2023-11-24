#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
  int posX, posY, sX, sY;
  bool moved = false;
  int dir = 0;
  int siz;
  int w;
  int h;
  cin >> w >> h;
  cin.ignore();
  int map[h][w];
  long long n;
  cin >> n;
  cerr<<n<<endl;
  cin.ignore();
  for (int i = 0; i < h; ++i) {
    string line;
    getline(cin, line);
    siz = line.size();
    for (int j = 0; j < siz; ++j) {
      if (line[j] == '.')
        map[i][j] = 1;
      else if (line[j] == '#')
        map[i][j] = 2;
      else if (line[j] == 'O') {
        map[i][j] = 1;
        posX = j;
        posY = i;
        sX=j;
        sY=i;
      }
    }
  }
  for (long long i = 0; i < n; ++i) {
    if (sX==posX && sY==posY && i>0) 
    {
        cerr<<"loop"<<endl;
        i=n-n%i;
    }
    cerr<<i<<" "<<posX<<" "<<posY<<" "<<dir<<endl;
    while (!moved) {
      if (dir == 0) {
        if (map[posY - 1][posX] == 1) {
          --posY;
          moved = true;
        } else {
          dir = 1;
        }
      } else if (dir == 1) {
        if (map[posY][posX + 1] == 1) {
          ++posX;
          moved = true;
        } else {
          dir = 2;
        }
      } else if (dir == 2) {
        if (map[posY + 1][posX] == 1) {
          ++posY;
          moved = true;
        } else {
          dir = 3;
        }
      } else if (dir == 3) {
        if (map[posY][posX - 1] == 1) {
          --posX;
          moved = true;
        } else {
          dir = 0;
        }
      }
    }
    moved = false;
  }
  cout << posX << " " << posY << endl;
}