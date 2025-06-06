#undef _GLIBCXX_DEBUG                // disable run-time bound checking, etc
#pragma GCC optimize("Ofast,inline") // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens

#pragma GCC target("bmi,bmi2,lzcnt,popcnt")                      // bit manipulation
#pragma GCC target("movbe")                                      // byte swap
#pragma GCC target("aes,pclmul,rdrnd")                           // encryption
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2") // SIMD

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>
#include <string.h>
#include <cfloat>
#include <set>
#include <chrono>

using namespace std;

 struct Pac 
 {
    int pacId;
    bool mine;
    int x;
    int y;
    string typeId;
    int speedTurnsLeft;
    int abilityCooldown;
 };

static vector<vector<int>> super_pellets;
static vector<vector<int>> pellets;
static vector<Pac> my_pac;
static vector<Pac> my_last_pac;
static vector<Pac> op_pac;
static int width; // size of the grid
static int height; // top left corner is (x=0, y=0)
static vector<int> unvisited;
static vector<int> mapa;
static int actual;
static int target1[2];
static int target2[2];
static int my_pac_size;
static int pellets_size;

int distance_val(int k, int x, int y)
{
    return abs(my_pac[k].x-x)+abs(my_pac[k].y-y);
}

int isBeatable(string my, string op)
{
    if (my=="PAPER" && op=="ROCK") return 1;
    else if (my=="ROCK" && op=="SCISSORS") return 1;
    else if (my=="SCISSORS" && op=="PAPER") return 1;

    return 0;
}

string destroy(string typ)
{
    if (typ=="PAPER") return "SCISSORS";
    else if (typ=="ROCK") return "PAPER";
    else if (typ=="SCISSORS") return "ROCK";
    else
    {
        cerr<<"unknown type to destroy "<<typ<<endl;
        return "ROCK";
    }
}

string antitrap(string typ)
{
    if (typ=="PAPER") return "ROCK";
    else if (typ=="ROCK") return "SCISSORS";
    else if (typ=="SCISSORS") return "PAPER";
    else
    {
        cerr<<"unknown type to antitrap "<<typ<<endl;
        return "ROCK";
    }
}

////////////////////// POCZATEK A*
// Creating a shortcut for int, int pair type 
typedef pair<int, int> Pair; 
  
// Creating a shortcut for pair<int, pair<int, int>> type 
typedef pair<double, pair<int, int>> pPair; 
  
// A structure to hold the neccesary parameters 
struct cell 
{ 
    // Row and Column index of its parent 
    // Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1 
    int parent_i, parent_j; 
    // f = g + h 
    double f, g, h; 
}; 
  
// A Utility Function to check whether given cell (row, col) is a valid cell or not. 
bool isValid(int row, int col) 
{ 
    // Returns true if row number and column number is in range 
    return (row >= 0) && (row < height) && (col >= 0) && (col < width); 
} 
  
// A Utility Function to check whether the given cell is blocked or not 
bool isUnBlocked(int row, int col) 
{ 
    // Returns true if the cell is not blocked else false
    for (int k = 0;k<op_pac.size();++k)
    {
        if (op_pac[k].x==col && op_pac[k].y==row) 
        {
            if(isBeatable(my_pac[actual].typeId,op_pac[k].typeId) && op_pac[k].abilityCooldown>0)
                return(true);
            else
                return (false);
        }
    }
    // for (int k = 0;k<my_pac.size();++k)
    for (int k = 0;k<my_pac_size;++k)
    {
        if (my_pac[actual].pacId!=my_pac[k].pacId && my_pac[k].x==col && my_pac[k].y==row) 
        {
            return (false);
        }
        else if (my_pac[actual].pacId==my_pac[k].pacId && my_pac[k].x==col && my_pac[k].y==row) 
        {
            return (true);
        }
    } 
    
    if (mapa[col+row*width]==1)
        return (true); 
    else
        return (false); 
} 
  
// A Utility Function to check whether destination cell has been reached or not 
bool isDestination(int row, int col, Pair dest) 
{ 
    if (row == dest.first && col == dest.second) 
        return (true); 
    else
        return (false); 
} 
  
// A Utility Function to calculate the 'h' heuristics. 
double calculateHValue(int row, int col, Pair dest) 
{ 
    // Return using the distance formula 
    return ((double)(abs(row-dest.first) + abs(col-dest.second))); 
} 
  
// A Utility Function to trace the path from the source to destination 
// void tracePath(cell cellDetails[][width], Pair dest) 
int tracePath(vector<vector<cell>> cellDetails, Pair dest) 
{ 
    // cerr<<"The Path is "; 
    int row = dest.first; 
    int col = dest.second; 
    int lon = 0;
  
    stack<Pair> Path; 
  
    while (!(cellDetails[row][col].parent_i == row && cellDetails[row][col].parent_j == col )) 
    { 
        Path.push (make_pair (row, col)); 
        int temp_row = cellDetails[row][col].parent_i; 
        int temp_col = cellDetails[row][col].parent_j; 
        row = temp_row; 
        col = temp_col; 
    } 
  
    Path.push (make_pair (row, col)); 
    while (!Path.empty()) 
    { 
        pair<int,int> p = Path.top(); 
        Path.pop(); 
        //  cerr<<"-> ("<<p.first<<","<<p.second; 
        if (lon==1)
        {
            target1[0]=p.first;
            target1[1]=p.second;
        }
        else if (lon==2)
        {
            target2[0]=p.first;
            target2[1]=p.second;
        }
        lon++;
    } 
    return lon; 
} 
  
// A Function to find the shortest path between a given source cell to a destination cell according to A* Search Algorithm 
// void aStarSearch(int grid[][COL], Pair src, Pair dest) 
int aStarSearch(Pair src, Pair dest) 
{ 
    // If the source is out of range 
    if (isValid (src.first, src.second) == false) 
    { 
        // cerr<<"Source is invalid"<<endl; 
        return 900; 
    } 
  
    // If the destination is out of range 
    if (isValid (dest.first, dest.second) == false) 
    { 
        // cerr<<"Destination is invalid"<<endl; 
        return 900; 
    } 
  
    // Either the source or the destination is blocked 
    if (isUnBlocked(src.first, src.second) == false || isUnBlocked(dest.first, dest.second) == false) 
    { 
        // cerr<<"Source or the destination is blocked"<<endl; 
        return 900; 
    } 
  
    // If the destination cell is the same as source cell 
    if (isDestination(src.first, src.second, dest) == true) 
    { 
        cerr<<"We are already at the destination"<<endl; 
        return 0; 
    } 
  
    // Create a closed list and initialise it to false which means that no cell has been included yet 
    // This closed list is implemented as a boolean 2D array 
    bool closedList[height][width]; 
    memset(closedList, false, sizeof(closedList)); 
  
    // Declare a 2D array of structure to hold the details of that cell 
    // cell cellDetails[height][width]; 
    vector<vector<cell>> cellDetails(height);
    for (int i = 0;i<cellDetails.size();++i)
    {
        cellDetails[i].resize(width);
    }
  
    int i, j;
    int lon = 0; 
  
    for (i=0; i<height; i++) 
    { 
        for (j=0; j<width; j++) 
        { 
            cellDetails[i][j].f = FLT_MAX; 
            cellDetails[i][j].g = FLT_MAX; 
            cellDetails[i][j].h = FLT_MAX; 
            cellDetails[i][j].parent_i = -1; 
            cellDetails[i][j].parent_j = -1; 
        } 
    } 
  
    // Initialising the parameters of the starting node 
    i = src.first, j = src.second; 
    cellDetails[i][j].f = 0.0; 
    cellDetails[i][j].g = 0.0; 
    cellDetails[i][j].h = 0.0; 
    cellDetails[i][j].parent_i = i; 
    cellDetails[i][j].parent_j = j; 
  
    /* 
     Create an open list having information as- 
     <f, <i, j>> 
     where f = g + h, 
     and i, j are the row and column index of that cell 
     Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1 
     This open list is implenented as a set of pair of pair.*/
    set<pPair> openList; 
  
    // Put the starting cell on the open list and set its 
    // 'f' as 0 
    openList.insert(make_pair (0.0, make_pair (i, j))); 
  
    // We set this boolean value as false as initially 
    // the destination is not reached. 
    bool foundDest = false; 
  
    while (!openList.empty()) 
    { 
        pPair p = *openList.begin(); 
  
        // Remove this vertex from the open list 
        openList.erase(openList.begin()); 
  
        // Add this vertex to the closed list 
        i = p.second.first; 
        j = p.second.second; 
        closedList[i][j] = true; 
       
       /* 
        Generating all the 4 successor of this cell 
  
            N.W   N   N.E 
              \   |   / 
               \  |  / 
            W----Cell----E 
                 / | \ 
               /   |  \ 
            S.W    S   S.E 
  
        Cell-->Popped Cell (i, j) 
        N -->  North       (i-1, j) 
        S -->  South       (i+1, j) 
        E -->  East        (i, j+1) 
        W -->  West        (i, j-1)*/
  
        // To store the 'g', 'h' and 'f' of the 8 successors 
        double gNew, hNew, fNew; 
  
        //----------- 1st Successor (North) ------------ 
        // Only process this cell if this is a valid one 
        if (isValid(i-1, j) == true) 
        { 
            // If the destination cell is the same as the 
            // current successor 
            if (isDestination(i-1, j, dest) == true) 
            { 
                // Set the Parent of the destination cell 
                cellDetails[i-1][j].parent_i = i; 
                cellDetails[i-1][j].parent_j = j; 
                // cerr<<"The destination cell is found"<<endl; 
                lon = tracePath (cellDetails, dest); 
                foundDest = true; 
                return lon; 
            } 
            // If the successor is already on the closed 
            // list or if it is blocked, then ignore it. 
            // Else do the following 
            else if (closedList[i-1][j] == false && isUnBlocked(i-1, j) == true) 
            { 
                gNew = cellDetails[i][j].g + 1.0; 
                hNew = calculateHValue (i-1, j, dest); 
                fNew = gNew + hNew; 
  
                // If it isn’t on the open list, add it to 
                // the open list. Make the current square 
                // the parent of this square. Record the 
                // f, g, and h costs of the square cell 
                //                OR 
                // If it is on the open list already, check 
                // to see if this path to that square is better, 
                // using 'f' cost as the measure. 
                if (cellDetails[i-1][j].f == FLT_MAX || cellDetails[i-1][j].f > fNew) 
                { 
                    openList.insert( make_pair(fNew, make_pair(i-1, j))); 
  
                    // Update the details of this cell 
                    cellDetails[i-1][j].f = fNew; 
                    cellDetails[i-1][j].g = gNew; 
                    cellDetails[i-1][j].h = hNew; 
                    cellDetails[i-1][j].parent_i = i; 
                    cellDetails[i-1][j].parent_j = j; 
                } 
            } 
        } 
  
        //----------- 2nd Successor (South) ------------ 
        if (isValid(i+1, j) == true) 
        { 
            if (isDestination(i+1, j, dest) == true) 
            {  
                cellDetails[i+1][j].parent_i = i; 
                cellDetails[i+1][j].parent_j = j; 
                // cerr<<"The destination cell is found"<<endl; 
                lon = tracePath(cellDetails, dest); 
                foundDest = true; 
                return lon; 
            } 
            else if (closedList[i+1][j] == false && isUnBlocked(i+1, j) == true) 
            { 
                gNew = cellDetails[i][j].g + 1.0; 
                hNew = calculateHValue(i+1, j, dest); 
                fNew = gNew + hNew; 
  
                if (cellDetails[i+1][j].f == FLT_MAX || cellDetails[i+1][j].f > fNew) 
                { 
                    openList.insert( make_pair (fNew, make_pair (i+1, j))); 

                    cellDetails[i+1][j].f = fNew; 
                    cellDetails[i+1][j].g = gNew; 
                    cellDetails[i+1][j].h = hNew; 
                    cellDetails[i+1][j].parent_i = i; 
                    cellDetails[i+1][j].parent_j = j; 
                } 
            } 
        } 
  
        //----------- 3rd Successor (East) ------------ 
        if (isValid (i, j+1) == true) 
        { 
            if (isDestination(i, j+1, dest) == true) 
            { 
                cellDetails[i][j+1].parent_i = i; 
                cellDetails[i][j+1].parent_j = j; 
                // cerr<<"The destination cell is found"<<endl; 
                lon = tracePath(cellDetails, dest); 
                foundDest = true; 
                return lon; 
            } 
            else if (closedList[i][j+1] == false && isUnBlocked (i, j+1) == true) 
            { 
                gNew = cellDetails[i][j].g + 1.0; 
                hNew = calculateHValue (i, j+1, dest); 
                fNew = gNew + hNew; 

                if (cellDetails[i][j+1].f == FLT_MAX || cellDetails[i][j+1].f > fNew) 
                { 
                    openList.insert( make_pair(fNew, make_pair (i, j+1))); 
  
                    cellDetails[i][j+1].f = fNew; 
                    cellDetails[i][j+1].g = gNew; 
                    cellDetails[i][j+1].h = hNew; 
                    cellDetails[i][j+1].parent_i = i; 
                    cellDetails[i][j+1].parent_j = j; 
                } 
            } 
        } 
  
        //----------- 4th Successor (West) ------------  
        if (isValid(i, j-1) == true) 
        { 
            if (isDestination(i, j-1, dest) == true) 
            { 
                cellDetails[i][j-1].parent_i = i; 
                cellDetails[i][j-1].parent_j = j; 
                // cerr<<"The destination cell is found"<<endl; 
                lon = tracePath(cellDetails, dest); 
                foundDest = true; 
                return lon; 
            } 
            else if (closedList[i][j-1] == false && isUnBlocked(i, j-1) == true) 
            { 
                gNew = cellDetails[i][j].g + 1.0; 
                hNew = calculateHValue(i, j-1, dest); 
                fNew = gNew + hNew; 

                if (cellDetails[i][j-1].f == FLT_MAX || cellDetails[i][j-1].f > fNew) 
                { 
                    openList.insert( make_pair (fNew, make_pair (i, j-1))); 
  
                    cellDetails[i][j-1].f = fNew; 
                    cellDetails[i][j-1].g = gNew; 
                    cellDetails[i][j-1].h = hNew; 
                    cellDetails[i][j-1].parent_i = i; 
                    cellDetails[i][j-1].parent_j = j; 
                } 
            } 
        }  
    } 
  
    // When the destination cell is not found and the open 
    // list is empty, then we conclude that we failed to 
    // reach the destiantion cell. This may happen when the 
    // there is no way to destination cell (due to blockages) 

    // if (foundDest == false) 
    //     cerr<<"Failed to find the Destination Cell"<<endl;
  
    return 900; 
} 
///////////////////// KONIEC A*


void empty_unvisited()
{
    int tmp_x;
    int tmp_y;
    int tmp;
    // for (int k = 0;k<my_pac.size();++k)
    for (int k = 0;k<my_pac_size;++k)
    {
        tmp_x = my_pac[k].x;
        tmp_y = my_pac[k].y;
        while(tmp_x<width)
        {
            tmp=0;
            tmp_x++;
            if (mapa[tmp_x+tmp_y*width]==0) break;
            if (pellets_size==0)
            {
                vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), (tmp_x + tmp_y*width));
                if (it != unvisited.end())
                {
                    // cerr<<"remove "<<tmp_x<<" "<<tmp_y<<endl;
                    unvisited.erase(it);
                }
            }
            for (int j = 0;j<pellets_size;++j)
            {
                if (pellets[j][0]==tmp_x && pellets[j][1]==tmp_y)
                {
                    tmp=1;
                    break;
                }
            }
            if (tmp==0)
            {
                vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), (tmp_x + tmp_y*width));
                if (it != unvisited.end())
                {
                    //cerr<<"remove "<<my_pac[k].pacId<<" "<<tmp_x<<" "<<tmp_y<<" "<<pellets[j][0]<<" "<<pellets[j][1]<<endl;
                    unvisited.erase(it);
                }
            }
        }
        tmp_x = my_pac[k].x;
        tmp_y = my_pac[k].y;
        while(tmp_x>=0)
        {
            tmp=0;
            tmp_x--;
            if (mapa[tmp_x+tmp_y*width]==0) break;
            if (pellets.size()==0)
            {
                vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), (tmp_x + tmp_y*width));
                if (it != unvisited.end())
                {
                    // cerr<<"remove "<<tmp_x<<" "<<tmp_y<<endl;
                    unvisited.erase(it);
                }
            }
            for (int j = 0;j<pellets_size;++j)
            {
                if (pellets[j][0]==tmp_x && pellets[j][1]==tmp_y)
                {
                    tmp=1;
                    break;
                }
            }
            if (tmp==0)
            {
                vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), (tmp_x + tmp_y*width));
                if (it != unvisited.end())
                {
                    //cerr<<"remove "<<my_pac[k].pacId<<" "<<tmp_x<<" "<<tmp_y<<" "<<pellets[j][0]<<" "<<pellets[j][1]<<endl;
                    unvisited.erase(it);
                }
            }
        }
        tmp_x = my_pac[k].x;
        tmp_y = my_pac[k].y;
        while(tmp_y>=0)
        {
            tmp=0;
            tmp_y--;
            if (mapa[tmp_x+tmp_y*width]==0) break;
            if (pellets_size==0)
            {
                vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), (tmp_x + tmp_y*width));
                if (it != unvisited.end())
                {
                    // cerr<<"remove "<<tmp_x<<" "<<tmp_y<<endl;
                    unvisited.erase(it);
                }
            }
            for (int j = 0;j<pellets_size;++j)
            {
                if (pellets[j][0]==tmp_x && pellets[j][1]==tmp_y)
                {
                    tmp=1;
                    break;
                }
            }
            if (tmp==0)
            {
                vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), (tmp_x + tmp_y*width));
                if (it != unvisited.end())
                {
                    // cerr<<"remove "<<my_pac[k].pacId<<" "<<tmp_x<<" "<<tmp_y<<" "<<pellets[j][0]<<" "<<pellets[j][1]<<endl;
                    unvisited.erase(it);
                }
            }
        }
        tmp_x = my_pac[k].x;
        tmp_y = my_pac[k].y;
        while(tmp_y<height)
        {
            tmp=0;
            tmp_y++;
            if (mapa[tmp_x+tmp_y*width]==0) break;
            if (pellets_size==0)
            {
                vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), (tmp_x + tmp_y*width));
                if (it != unvisited.end())
                {
                    unvisited.erase(it);
                }
            }
            for (int j = 0;j<pellets_size;++j)
            {
                if (pellets[j][0]==tmp_x && pellets[j][1]==tmp_y)
                {
                    tmp=1;
                    break;
                }
            }
            if (tmp==0)
            {
                vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), (tmp_x + tmp_y*width));
                if (it != unvisited.end())
                {
                    //cerr<<"remove "<<my_pac[k].pacId<<" "<<tmp_x<<" "<<tmp_y<<" "<<pellets[j][0]<<" "<<pellets[j][1]<<endl;
                    unvisited.erase(it);
                }
            }
        }
    }
}

void random_unvisited_move(int k)
{
    // cerr<<"RANDOM UNVISITED MOVE"<<endl;
    if (unvisited.size()>0)
    {
        int rnd = rand()%unvisited.size();
        cout << "MOVE "<<my_pac[k].pacId<<" "<<unvisited[rnd]%width<<" "<<unvisited[rnd]/width<<"|"; // MOVE <pacId> <x> <y>
    }
    else
        cerr<<"0 unvisited, something wrong"<<endl;
}

void unvisited_move(int k)
{
    // cerr<<"UNVISITED MOVE"<<endl;
    if (unvisited.size()>0)
    {
        int dist=999;
        int closest=999;
        int targ[2];
        int unv_size = unvisited.size();
        for (int i = 0; i<unv_size;++i)
        {
            //int distance =sqrt(pow(my_pac[k].x-pellets[i][0],2)+pow(my_pac[k].y-pellets[i][1],2));
            int distance = aStarSearch({my_pac[k].y,my_pac[k].x},{unvisited[i]/width,unvisited[i]%width});
            // int distance =distance_val(k,unvisited[i]%width,unvisited[i]/width); 
            if (distance<dist)
            {
                dist = distance;
                closest = i;
                if(dist>=3)
                {
                    targ[0]=target2[0];
                    targ[1]=target2[1];
                    if (my_pac[k].speedTurnsLeft>0 && dist ==3) break;
                }
                else
                {
                    targ[0]=target1[0];
                    targ[1]=target1[1];
                    break;
                }
            }
        }
        // cout << "MOVE "<<my_pac[k].pacId<<" "<<unvisited[closest]%width<<" "<<unvisited[closest]/width<<" U"<<unvisited[closest]%width<<","<<unvisited[closest]/width<<"|"; // MOVE <pacId> <x> <y>
        cout << "MOVE "<<my_pac[k].pacId<<" "<<targ[1]<<" "<<targ[0]<<"|"; // MOVE <pacId> <x> <y>

    }
    else
        cerr<<"0 unvisited, something wrong"<<endl;
}

void normal_move(int k)
{
    // cerr<<"NORMAL MOVE"<<endl;
    int dist=999;
    int closest=999;
    int targ[2];
    if (my_pac[k].speedTurnsLeft>0)
    {
        for (int i = 0; i<pellets_size;++i)
        {
            if(my_pac[k].x==pellets[i][0] || my_pac[k].y==pellets[i][1])
            {
                int distance = aStarSearch({my_pac[k].y,my_pac[k].x},{pellets[i][1],pellets[i][0]});
                if (distance==3)
                {
                    if (my_pac[k].speedTurnsLeft>0)
                    {
                        dist = distance;
                        closest = i;
                        targ[0]=target2[0];
                        targ[1]=target2[1];
                        // cerr<<my_pac[k].pacId<<" t2 "<<target2[1]<<" "<<target2[0]<<" "<<pellets[closest][0]<<","<<pellets[closest][1]<<endl;
                        break;
                    }
                }
            }
        }
    }
    if (dist>900)
    {
        for (int i = 0; i<pellets_size;++i)
        {
            if(my_pac[k].x==pellets[i][0] || my_pac[k].y==pellets[i][1])
            {
                int distance = aStarSearch({my_pac[k].y,my_pac[k].x},{pellets[i][1],pellets[i][0]});
                if (distance<dist)
                {
                    dist = distance;
                    closest = i;
                    targ[0]=target1[0];
                    targ[1]=target1[1];
                    // cerr<<my_pac[k].pacId<<" t1 "<<target1[1]<<" "<<target1[0]<<endl;
                    if (distance==2) break;
                }
            }
        }
    }
    if (pellets_size>0 && dist<(width+height)/5)
        cout << "MOVE "<<my_pac[k].pacId<<" "<<targ[1]<<" "<<targ[0]<<"|"; // MOVE <pacId> <x> <y>

    else
        unvisited_move(k);
}

void super_move(int k)
{
    // cerr<<"SUPER MOVE"<<endl;
    int dist=999;
    int closest=999;
    int targ[2];
    for (int i = 0; i<super_pellets.size();++i)
    {
        //int distance =sqrt(pow(my_pac[k].x-super_pellets[i][0],2)+pow(my_pac[k].y-super_pellets[i][1],2)); 
        int distance = aStarSearch({my_pac[k].y,my_pac[k].x},{super_pellets[i][1],super_pellets[i][0]});
        // int distance = abs(my_pac[k].x-super_pellets[i][0])+abs(my_pac[k].y-super_pellets[i][1]); 
        // cerr<<" distance "<<distance<<endl;
        if (distance<dist)
        {
            dist = distance;
            closest = i;
            if (dist>=3)
            {
                targ[0]=target2[0];
                targ[1]=target2[1];
            }
            else 
            {
                targ[0]=target1[0];
                targ[1]=target1[1];
                break;
            }
        }
    }
    if (super_pellets.size()>0)
    {
        cout << "MOVE "<<my_pac[k].pacId<<" "<<targ[1]<<" "<<targ[0]<<" "<<"S"<<targ[1]<<","<<targ[0]<<"|"; // MOVE <pacId> <x> <y>
    }

    else
        normal_move(k);
}

int main()
{
    cin >> width >> height; cin.ignore();
    int map[height][width];
    // int visited[height][width];
    int tmp_size=0;
    // int my_pac_size = 0;

    for (int i = 0; i < height; i++) {
        string row;
        getline(cin, row); // one line of the grid: space " " is floor, pound "#" is wall
        for (int j = 0;j<row.size();++j)
        {
            if (row[j]=='#')
            {
                map[i][j]=0;
                mapa.push_back(0);
            }
            else
            {
                map[i][j]=1;
                mapa.push_back(1);
                unvisited.push_back(j+i*width);
            }
        }
    }

    // game loop
    while (1) {
        chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        super_pellets.clear();
        pellets.clear();
        my_pac.clear();
        op_pac.clear();
        int myScore;
        int opponentScore;
        cin >> myScore >> opponentScore; cin.ignore();
        int visiblePacCount; // all your pacs and enemy pacs in sight
        cin >> visiblePacCount; cin.ignore();
        for (int i = 0; i < visiblePacCount; i++) {
            int pacId; // pac number (unique within a team)
            bool mine; // true if this pac is yours
            int x; // position in the grid
            int y; // position in the grid
            string typeId; // unused in wood leagues
            int speedTurnsLeft; // unused in wood leagues
            int abilityCooldown; // unused in wood leagues
            cin >> pacId >> mine >> x >> y >> typeId >> speedTurnsLeft >> abilityCooldown; cin.ignore();
            //cerr<<"abilityCooldown "<<abilityCooldown<<endl;
            if (typeId=="DEAD") 
            {
                //cerr<<"umarty"<<endl;
                continue;
            }
            if (mine)
            {
                my_pac.push_back({pacId,mine,x,y,typeId,speedTurnsLeft,abilityCooldown});
                // visited[y][x]=1;
                vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), x+y*width);
                if (it != unvisited.end())
	                unvisited.erase(it);
                tmp_size=my_last_pac.size();
                // for (int j = 0; j<tmp_size;++j)
                // {
                //     if (!my_last_pac.empty() && (pacId == my_last_pac[j].pacId) && (abs(x-my_last_pac[j].x)+abs(y-my_last_pac[j].y)>1))
                //     {
                //         // cerr<<"after speed erase"<<endl;
                //         vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), (x+my_last_pac[j].x)/2 + (y+my_last_pac[j].y)/2 *width);
                //         if (it != unvisited.end())
	            //             unvisited.erase(it);
                //     }
                // }
            }
            else
            {
                op_pac.push_back({pacId,mine,x,y,typeId,speedTurnsLeft,abilityCooldown});
                // vector<int>::iterator it = find(unvisited.begin(), unvisited.end(), x+y*width);
                // if (it != unvisited.end())
	            //     unvisited.erase(it);
            }
        }
        chrono::steady_clock::time_point pac_init = chrono::steady_clock::now();

        // cerr<<"my "<<my_pac.size()<<" op "<<op_pac.size()<<endl;
        my_pac_size = my_pac.size();
        int visiblePelletCount; // all pellets in sight
        cin >> visiblePelletCount; cin.ignore();
        
        if (visiblePelletCount>0)
        {
            for (int i = 0; i < visiblePelletCount; i++) {
                int x;
                int y;
                int value; // amount of points this pellet is worth
                cin >> x >> y >> value; cin.ignore();
                // cerr<<"pellet "<<x<<" "<<y<<endl;
                vector<int> pellet = {x,y,value};
                if (value>1)
                {
                    super_pellets.push_back(pellet);
                }
                else
                {
                    pellets.push_back(pellet);
                }
            }
        }
        pellets_size=pellets.size();
        chrono::steady_clock::time_point pellet_init = chrono::steady_clock::now();

        empty_unvisited();
        chrono::steady_clock::time_point empty = chrono::steady_clock::now();

        // for (int k = 0;k<my_pac.size();++k)
        for (int k = 0;k<my_pac_size;++k)
        {
            // chrono::steady_clock::time_point pac_start = chrono::steady_clock::now();

            int chased = 0;
            int attacked = 0;
            for (int j = 0;j<op_pac.size();++j)
            {
                int distro = distance_val(k,op_pac[j].x,op_pac[j].y);
                // cerr<<j<<" distro "<<distro<<endl;
                if (distro<=1 || (distro==2 && op_pac[j].speedTurnsLeft>0))
                {
                    attacked = 1;
                    chased=j;
                    // cerr<<"op_pac["<<op_pac[j].pacId<<"].abilityCooldown "<<op_pac[j].abilityCooldown<<endl;
                }
            }
            actual=k;
            //  cerr<<my_pac[k].pacId<<" chased "<<chased<<endl;
            if (attacked!=0 && isBeatable(my_pac[k].typeId,op_pac[chased].typeId) && op_pac[chased].abilityCooldown>0)
            {
                // cerr<<"kill"<<endl;
                cout<<"MOVE "<<my_pac[k].pacId<<" "<<op_pac[chased].x<<" "<<op_pac[chased].y<<" kill|";
            }
            else if (attacked!=0 && !isBeatable(my_pac[k].typeId,op_pac[chased].typeId) && my_pac[k].abilityCooldown>0)
            {
                // cerr<<"run"<<endl;
                unvisited_move(k);
                // cout<<"MOVE "<<my_pac[k].pacId<<" "<<2*my_pac[k].x-op_pac[chased].x<<" "<<2*my_pac[k].y-op_pac[chased].y<<" run!|";
            }
            else if (attacked!=0 && my_pac[k].abilityCooldown==0)
            {
                if (op_pac[chased].abilityCooldown>0)
                {
                    cout<<"SWITCH "<<my_pac[k].pacId<<" "<<destroy(op_pac[chased].typeId)<<" destroy|"; 
                }
                else 
                {
                    cout<<"SWITCH "<<my_pac[k].pacId<<" "<<antitrap(my_pac[k].typeId)<<" antitrap|"; 
                }
            }
            else if (my_pac[k].abilityCooldown==0)// && op_pac.size()==0)
            {
                cout<<"SPEED "<<my_pac[k].pacId<<"| ";
            }
            else
            {
                int moved=0;
                // cerr<<"else "<<k<<endl;
                for (int j = 0; j<my_last_pac.size();++j)
                {
                    if (!my_last_pac.empty() && (my_pac[k].speedTurnsLeft!=5) && (my_pac[k].pacId == my_last_pac[j].pacId) && (my_pac[k].x == my_last_pac[j].x) && (my_pac[k].y == my_last_pac[j].y))
                    {
                        moved = 1;
                        random_unvisited_move(k);
                    }
                }
                if(moved) continue;

                if (super_pellets.size()>0)
                {
                    super_move(k);
                }
                else if (pellets_size>0)
                {
                    normal_move(k);
                }
                else
                {
                    unvisited_move(k);
                }
            }
            // chrono::steady_clock::time_point pac_end = chrono::steady_clock::now();
            // cerr << "Time "<<k<<" = " << chrono::duration_cast<std::chrono::microseconds>(pac_end - pac_start).count() << "[ms]" << endl;
        }
        chrono::steady_clock::time_point move = chrono::steady_clock::now();

        my_last_pac = my_pac;
        cout<<endl;
        cerr<<"unvisited "<<unvisited.size()<<endl;

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        cerr << "Time pac = " << chrono::duration_cast<std::chrono::microseconds>(pac_init - begin).count() << "[ms]" << endl;
        cerr << "Time pellet = " << chrono::duration_cast<std::chrono::microseconds>(pellet_init - pac_init).count() << "[ms]" << endl;
        cerr << "Time empty = " << chrono::duration_cast<std::chrono::microseconds>(empty - pellet_init).count() << "[ms]" << endl;
        cerr << "Time move = " << chrono::duration_cast<std::chrono::microseconds>(move - empty).count() << "[ms]" << endl;

        cerr << "Time = " << chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[ms]" << endl;

    }
}