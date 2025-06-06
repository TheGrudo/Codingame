#undef _GLIBCXX_DEBUG                // disable run-time bound checking, etc
#pragma GCC optimize("Ofast,inline") // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>
#include <cfloat>
#include <set>
#include <string.h>
#include <chrono>
#include <unordered_set>

using namespace std;

struct Stock {
    int A;
    int B;
    int C;
    int D;

    Stock () {};
    Stock(int a, int b, int c, int d):A(a),B(b),C(c),D(d) {};
};

struct Entity {
    int x;                  // Współrzędna x
    int y;                  // Współrzędna y
    std::string type;       // Typ: WALL, ROOT, BASIC, TENTACLE, etc.
    int owner;              // Właściciel: 1 - Twój, 0 - Wróg, -1 - Żaden
    int organ_id;           // ID organu, jeśli to organ; 0 w przeciwnym wypadku
    std::string organ_dir;  // Kierunek organu: N, E, S, W lub X, jeśli brak
    int organ_parent_id;    // ID rodzica organu
    int organ_root_id;      // ID korzenia organu, proteina:-1 ja wydobywam, -2 przeciwnik wydobywa

    Entity () {};

    // Konstruktor
    Entity(int x, int y, const std::string& type, int owner, int organ_id, 
         const std::string& organ_dir, int organ_parent_id, int organ_root_id)
        : x(x), y(y), type(type), owner(owner), organ_id(organ_id),
          organ_dir(organ_dir), organ_parent_id(organ_parent_id), organ_root_id(organ_root_id) {};

    // Metoda do wyświetlania informacji o obiekcie
    void display() const {
        std::cout << "Cell(" << x << ", " << y << ") ["
                  << "Type: " << type << ", "
                  << "Owner: " << owner << ", "
                  << "Organ ID: " << organ_id << ", "
                  << "Organ Dir: " << organ_dir << ", "
                  << "Organ Parent ID: " << organ_parent_id << ", "
                  << "Organ Root ID: " << organ_root_id << "]\n";
    };
};

struct Move {
    int src_x;
    int src_y;
    int src_id;
    int dst_x;
    int dst_y;
    string dir;
    int cost;
    int value;
    string type;
};

void printMoves(const std::vector<Move> &moves)
{
    for (const auto &move : moves)
    {
        cerr << move.src_x << " "
                  << move.src_y << " "
                  << move.src_id << " "
                  << move.dst_x << " "
                  << move.dst_y << " "
                  << move.dir << " "
                  << move.cost << " "
                  << move.value << " "
                  << move.type << endl;
    }
    cerr<<endl;
}

std::string getRandomDirection(int x, int y, std::vector<std::vector<int>> &mapToff)
{
    static const std::string directions[] = {"N", "E", "S", "W"};
    std::vector<std::string> allowedDirections;

    int height = mapToff.size();
    int width = mapToff[0].size();
    
    // Sprawdź North (N)
    if (y > 0 && mapToff[y - 1][x] == 0)
    {
        allowedDirections.push_back("N");
    }

    // Sprawdź East (E)
    if (x < width - 1 && mapToff[y][x + 1] == 0)
    {
        allowedDirections.push_back("E");
    }

    // Sprawdź South (S)
    if (y < height - 1 && mapToff[y + 1][x] == 0)
    {
        allowedDirections.push_back("S");
    }

    // Sprawdź West (W)
    if (x > 0 && mapToff[y][x - 1] == 0)
    {
        allowedDirections.push_back("W");
    }

    // Jeśli są dostępne kierunki, wybierz losowy spośród dozwolonych
    if (!allowedDirections.empty())
    {
        return allowedDirections[rand() % allowedDirections.size()];
    }

    // Jeśli brak dozwolonych kierunków, zwróć losowy kierunek
    return directions[rand() % 4];
}

bool isMoveAllowed(int x, int y, const std::string &direction, const std::vector<std::vector<int>> &mapToff)
{
    int height = mapToff.size();
    int width = mapToff[0].size();

    if (direction == "N")
    {
        // Sprawdź, czy kierunek N (North) jest dozwolony
        if (y > 0 && mapToff[y - 1][x] == 0)
        {
            return true; // Dozwolony
        }
    }
    else if (direction == "E")
    {
        // Sprawdź, czy kierunek E (East) jest dozwolony
        if (x < width - 1 && mapToff[y][x + 1] == 0)
        {
            return true; // Dozwolony
        }
    }
    else if (direction == "S")
    {
        // Sprawdź, czy kierunek S (South) jest dozwolony
        if (y < height - 1 && mapToff[y + 1][x] == 0)
        {
            return true; // Dozwolony
        }
    }
    else if (direction == "W")
    {
        // Sprawdź, czy kierunek W (West) jest dozwolony
        if (x > 0 && mapToff[y][x - 1] == 0)
        {
            return true; // Dozwolony
        }
    }

    // Jeśli żadna z powyższych opcji nie została spełniona, ruch jest niedozwolony
    return false;
}

struct Target {
    int x;
    int y;
    string type;
    string dir;
    int value;

    bool operator==(const Target &other) const
    {
        return x == other.x && y == other.y && type == other.type;
    }
};

struct TargetHash
{
    size_t operator()(const Target &target) const
    {
        size_t h1 = std::hash<int>()(target.x);
        size_t h2 = std::hash<int>()(target.y);
        size_t h3 = std::hash<std::string>()(target.type);
        return h1 ^ (h2 << 1) ^ (h3 << 2); // Kombinacja hashy
    }
};

void removeDuplicates(std::vector<Target> &targets)
{
    // Kontener do przechowywania unikalnych elementów
    std::unordered_set<Target, TargetHash> unique_targets;

    // Przenieś elementy z wektora do unordered_set, usuwając duplikaty
    auto it = std::remove_if(targets.begin(), targets.end(), [&](const Target &target)
                             { return !unique_targets.insert(target).second; });

    // Usuń nadmiarowe elementy z wektora
    targets.erase(it, targets.end());
}

struct Root {
    unordered_map<int, Entity> organs;
    vector<int> organs_id;
    vector<Move> moves;
    Root() {};
};

static Stock my_stock;
static Stock op_stock;
static unordered_map<string, Stock> move_cost;
static unordered_map<string, int> my_gains;
static vector<pair<int, int>> bottlenecks;
static int turn;
static int num_roots;

////////////////////// POCZATEK A*
// Creating a shortcut for int, int pair type
typedef pair<int, int>
    Pair;

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
bool isValid(int row, int col, int height, int width) 
{ 
    // Returns true if row number and column number is in range 
    return (row >= 0) && (row < height) && (col >= 0) && (col < width); 
} 
  
// A Utility Function to check whether the given cell is blocked or not 
bool isUnBlocked(int row, int col, std::vector<std::vector<int>> &map) 
{ 
    // Returns true if the cell is not blocked else false
    if (map[col][row]==0)
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
int tracePath(vector<vector<cell>> cellDetails, Pair dest, int *mx, int *my)
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
            *mx=p.first;
            *my=p.second;
        }
        lon++;
    } 
    // cerr << endl;
    return lon; 
} 
  
// A Function to find the shortest path between a given source cell to a destination cell according to A* Search Algorithm 
int aStarSearch(Pair src, Pair dest, int height, int width, vector<vector<int>> &map, int *mx, int *my) 
{ 
    // cerr<<"src dst "<<src.first<<" "<< src.second<<" "<<dest.first<<" "<< dest.second<<endl;
    // If the source is out of range 
    if (isValid (src.first, src.second, height, width) == false) 
    { 
        // cerr<<"valid s"<<endl;
        return 900; 
    } 
  
    // If the destination is out of range 
    if (isValid (dest.first, dest.second, height, width) == false) 
    { 
        // cerr<<"valid d"<<endl;
        return 900; 
    } 
  
    // Either the source or the destination is blocked 
    // if (isUnBlocked(src.first, src.second, map) == false || isUnBlocked(dest.first, dest.second, map) == false) 
    // { 
    //     cerr<<"block"<<endl;
    //     return 900; 
    // } 
    if (isUnBlocked(dest.first, dest.second, map) == false) 
    { 
        // cerr<<"block"<<endl;
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
  
    set<pPair> openList; 

    openList.insert(make_pair (0.0, make_pair (i, j))); 

    bool foundDest = false; 
  
    while (!openList.empty()) 
    { 
        pPair p = *openList.begin(); 
        openList.erase(openList.begin()); 
  
        i = p.second.first; 
        j = p.second.second; 
        closedList[i][j] = true; 

        double gNew, hNew, fNew; 
  
        //----------- 1st Successor (North) ------------ 
        if (isValid(i-1, j, height, width) == true) 
        { 
            if (isDestination(i-1, j, dest) == true) 
            { 
                cellDetails[i-1][j].parent_i = i; 
                cellDetails[i-1][j].parent_j = j; 
                lon = tracePath (cellDetails, dest, mx, my); 
                foundDest = true; 
                return lon; 
            } 
            else if (closedList[i-1][j] == false && isUnBlocked(i-1, j, map) == true) 
            { 
                gNew = cellDetails[i][j].g + 1.0; 
                hNew = calculateHValue (i-1, j, dest); 
                fNew = gNew + hNew; 

                if (cellDetails[i-1][j].f == FLT_MAX || cellDetails[i-1][j].f > fNew) 
                { 
                    openList.insert( make_pair(fNew, make_pair(i-1, j))); 

                    cellDetails[i-1][j].f = fNew; 
                    cellDetails[i-1][j].g = gNew; 
                    cellDetails[i-1][j].h = hNew; 
                    cellDetails[i-1][j].parent_i = i; 
                    cellDetails[i-1][j].parent_j = j; 
                } 
            } 
        } 
  
        //----------- 2nd Successor (South) ------------ 
        if (isValid(i+1, j, height, width) == true) 
        { 
            if (isDestination(i+1, j, dest) == true) 
            {  
                cellDetails[i+1][j].parent_i = i; 
                cellDetails[i+1][j].parent_j = j; 
                lon = tracePath(cellDetails, dest, mx, my); 
                foundDest = true; 
                return lon; 
            } 
            else if (closedList[i+1][j] == false && isUnBlocked(i+1, j, map) == true) 
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
        if (isValid (i, j+1, height, width) == true) 
        { 
            if (isDestination(i, j+1, dest) == true) 
            { 
                cellDetails[i][j+1].parent_i = i; 
                cellDetails[i][j+1].parent_j = j; 
                lon = tracePath(cellDetails, dest, mx, my); 
                foundDest = true; 
                return lon; 
            } 
            else if (closedList[i][j+1] == false && isUnBlocked (i, j+1, map) == true) 
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
        if (isValid(i, j-1, height, width) == true) 
        { 
            if (isDestination(i, j-1, dest) == true) 
            { 
                cellDetails[i][j-1].parent_i = i; 
                cellDetails[i][j-1].parent_j = j; 
                lon = tracePath(cellDetails, dest, mx, my); 
                foundDest = true; 
                return lon; 
            } 
            else if (closedList[i][j-1] == false && isUnBlocked(i, j-1, map) == true) 
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
    return 800; 
} 
///////////////////// KONIEC A*

// Funkcja obliczająca wartość minimalnego pozostałego zasobu po wykonaniu ruchu
int calculateMinRemaining(const Stock &available, const Stock &cost)
{
    // Sprawdź, czy zasoby wystarczą na wykonanie ruchu
    if (cost.A > available.A || cost.B > available.B || cost.C > available.C || cost.D > available.D)
    {
        return -1; // Ruch niemożliwy
    }
    // Oblicz minimalną wartość pozostałych zasobów
    int remainingA = available.A - cost.A;
    int remainingB = available.B - cost.B;
    int remainingC = available.C - cost.C;
    int remainingD = available.D - cost.D;

    return std::min({remainingA, remainingB, remainingC, remainingD});
}

// Znajduje najlepszy ruch z faworyzacją "BASIC" w przypadku remisu
std::string findBestMove()
{
    std::string bestMove;
    int maxMinRemaining = -1;

    for (const auto &[move, cost] : move_cost)
    {
        int minRemaining = calculateMinRemaining(my_stock, cost);
        if (move == "WAIT" || move == "SPORE")
            minRemaining = -1;
        // Aktualizuj najlepszy ruch na podstawie maksymalnej wartości minimalnego zasobu
        if (minRemaining > maxMinRemaining || (minRemaining == maxMinRemaining && move == "BASIC"))
        {
            maxMinRemaining = minRemaining;
            bestMove = move;
        }
    }

    return bestMove;
}

void calculate_move(vector<Target> &targets, std::unordered_map<int, Root> &my_roots, vector<int> my_roots_id, vector<vector<int>> &map, vector<vector<int>> &mapToff, vector<vector<int>> &mapT1)
{
    int dist_min = 900;
    int id_min = -1;
    int max_range = 10;
    int height = map[0].size();
    int width = map.size();

    for (int j = 0; j < my_roots_id.size(); ++j)
    {
        cerr << "targets.size() " << targets.size() << "\n";
        for (int targId = 0; targId < targets.size(); ++targId)
        {
            int mx, my;
            int bmx, bmy;
            dist_min=900;
            for (int i = 0; i < my_roots[my_roots_id[j]].organs_id.size(); ++i)
            {
                int id = my_roots[my_roots_id[j]].organs_id[i];
                int range = abs(my_roots[my_roots_id[j]].organs[id].x - targets[targId].x + my_roots[my_roots_id[j]].organs[id].y - targets[targId].y);
                if (range > max_range) 
                    continue;
                int dist;
                if (targets[targId].type!="TENTACLE") 
                    dist = aStarSearch({my_roots[my_roots_id[j]].organs[id].x, my_roots[my_roots_id[j]].organs[id].y}, {targets[targId].x, targets[targId].y}, height, width, map, &mx, &my);
                else if (targets[targId].type == "TENTACLE")
                    dist = aStarSearch({my_roots[my_roots_id[j]].organs[id].x, my_roots[my_roots_id[j]].organs[id].y}, {targets[targId].x, targets[targId].y}, height, width, mapT1, &mx, &my);
                if (dist < dist_min)
                {
                    dist_min = dist;
                    id_min = id;
                    bmx = mx;
                    bmy = my;
                }
            }

            if (dist_min < 100)
            {
                Move mov;
                mov.src_x = my_roots[my_roots_id[j]].organs[id_min].x;
                mov.src_y = my_roots[my_roots_id[j]].organs[id_min].y;
                mov.src_id = my_roots[my_roots_id[j]].organs[id_min].organ_id;
                mov.dst_x = bmx;
                mov.dst_y = bmy;
                mov.dir = getRandomDirection(mov.dst_x, mov.dst_y, mapToff);
                mov.cost = dist_min;
                if (targets[targId].type == "TENTACLE")
                    mov.cost *= mov.cost;
                mov.type = findBestMove();
                mov.value = targets[targId].value;
                if (dist_min <= 2 || (targets[targId].type == "TENTACLE" && dist_min <= 5))
                {
                    if (isMoveAllowed(mov.dst_x, mov.dst_y, targets[targId].dir, mapToff))
                    {
                        mov.dir = targets[targId].dir;
                    }
                    else 
                    {
                        mov.dir = getRandomDirection(mov.dst_x, mov.dst_y, mapToff);
                    }
                    mov.type = targets[targId].type;
                    mov.value = targets[targId].value;
                }
                my_roots[my_roots_id[j]].moves.push_back(mov);
            }
        }
    }
}

void dummy_move(std::unordered_map<int, Root> &my_roots, int idr, vector<vector<int>> &map, vector<vector<int>> &mapToff)
{
    int dist_min = 900;
    int id_min = -1;
    int height = map[0].size();
    int width = map.size();

    for (int y = 0; y < width; ++y)
    {
        for (int x = 0; x < height; ++x)
        {
            if (map[y][x] == 0)
            {
                // cerr<<"sprawdzam "<<x<<" "<<y<<" "<<idr<<" "<<my_roots[idr].organs_id.size()<<endl;
                for (int i = 0; i < my_roots[idr].organs_id.size(); ++i)
                {
                    int mx, my;
                    int id = my_roots[idr].organs_id[i];
                    int dist = aStarSearch({my_roots[idr].organs[id].x, my_roots[idr].organs[id].y}, {x, y}, height, width, map, &mx, &my);
                    // cerr<<"dist "<<dist<<endl;
                    if (dist < 800)
                    {
                        dist_min = dist;
                        id_min = id;
                        Move mov;
                        mov.src_x = my_roots[idr].organs[id_min].x;
                        mov.src_y = my_roots[idr].organs[id_min].y;
                        mov.src_id = my_roots[idr].organs[id_min].organ_id;
                        mov.dst_x = mx;
                        mov.dst_y = my;
                        // mov.dir = getRandomDirection();
                        mov.dir = getRandomDirection(mov.dst_x, mov.dst_y, mapToff);
                        mov.cost = dist_min;
                        mov.type = findBestMove();
                        mov.value = 0;
                        if (dist_min < 100)
                        {
                            my_roots[idr].moves.push_back(mov);
                            cerr << "found dummy move"<<endl;
                            return;
                        }
                    }
                }
            }
        }
    }
    cerr<<"no moves "<<idr<<endl;
    Move mov;
    mov.src_x = 0;
    mov.src_y = 0;
    mov.src_id = 0;
    mov.dst_x = 0;
    mov.dst_y = 0;
    mov.dir = "N";
    mov.cost = 100;
    mov.type = "WAIT";
    mov.value = 0;
    my_roots[idr].moves.push_back(mov);
}

void protein_analyzer(unordered_map<int, Entity> &protein_sources, vector<int> protein_id, vector<vector<int>> &map, unordered_map<int, Entity> &entities, vector<vector<int>> &mapT0, vector<vector<int>> &mapT1)
{
    for (int j = 0; j < protein_id.size(); ++j)
    {
        int entId;
        if (protein_sources[protein_id[j]].y + 1 < map.size())
        {
            entId = map[protein_sources[protein_id[j]].y + 1][protein_sources[protein_id[j]].x];
            if (entId != 0 && protein_sources[protein_id[j]].organ_root_id == 0 && entities[entId].type == "HARVESTER" && entities[entId].organ_dir == "N")
            {
                if (entities[entId].owner == 1)
                {
                    protein_sources[protein_id[j]].organ_root_id = -1;
                    my_gains[protein_sources[protein_id[j]].type]++;
                }
                else if (entities[entId].owner == 0)
                {
                    protein_sources[protein_id[j]].organ_root_id = -2;
                    mapT0[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
                    mapT1[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
                }
            }
        }
        if (protein_sources[protein_id[j]].y - 1 >= 0)
        {
            entId = map[protein_sources[protein_id[j]].y - 1][protein_sources[protein_id[j]].x];
            if (entId != 0 && protein_sources[protein_id[j]].organ_root_id == 0 && entities[entId].type == "HARVESTER" && entities[entId].organ_dir == "S")
            {
                if (entities[entId].owner == 1)
                {
                    protein_sources[protein_id[j]].organ_root_id = -1;
                    my_gains[protein_sources[protein_id[j]].type]++;
                }
                else if (entities[entId].owner == 0)
                {
                    protein_sources[protein_id[j]].organ_root_id = -2;
                    mapT0[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
                    mapT1[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
                }
            }
        }
        if (protein_sources[protein_id[j]].x + 1 < map[0].size())
        {
            entId = map[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x + 1];
            if (entId != 0 && protein_sources[protein_id[j]].organ_root_id == 0 && entities[entId].type == "HARVESTER" && entities[entId].organ_dir == "W")
            {
                if (entities[entId].owner == 1)
                {
                    protein_sources[protein_id[j]].organ_root_id = -1;
                    my_gains[protein_sources[protein_id[j]].type]++;
                }
                else if (entities[entId].owner == 0)
                {
                    protein_sources[protein_id[j]].organ_root_id = -2;
                    mapT0[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
                    mapT1[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
                }
            }
        }
        if (protein_sources[protein_id[j]].x - 1 >= 0)
        {
            entId = map[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x - 1];
            if (entId != 0 && protein_sources[protein_id[j]].organ_root_id == 0 && entities[entId].type == "HARVESTER" && entities[entId].organ_dir == "E")
            {
                if (entities[entId].owner == 1)
                {
                    protein_sources[protein_id[j]].organ_root_id = -1;
                    my_gains[protein_sources[protein_id[j]].type]++;
                }
                else if (entities[entId].owner == 0)
                {
                    protein_sources[protein_id[j]].organ_root_id = -2;
                    mapT0[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
                    mapT1[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
                }
            }
        }
        if ((protein_sources[protein_id[j]].type == "C" && my_stock.C == 0) || (protein_sources[protein_id[j]].type == "D" && my_stock.D == 0))
        {
            mapT0[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
            mapT1[protein_sources[protein_id[j]].y][protein_sources[protein_id[j]].x] = 0;
        }
    }
}

void find_harvests(unordered_map<int, Entity> &protein_sources, vector<int> protein_id, vector<Target> &targets, vector<Target> &spore_targets)
{
    for (int j = 0; j < protein_id.size(); ++j)
    {
        if (protein_sources[protein_id[j]].organ_root_id==0)
        {
            int multi = 1;
            if (my_gains[protein_sources[protein_id[j]].type]==0) 
                multi = 5;
            if (my_gains[protein_sources[protein_id[j]].type] > 5)
                multi = 0;
            targets.push_back({protein_sources[protein_id[j]].x, protein_sources[protein_id[j]].y + 1, "HARVESTER", "N", 30*multi});
            targets.push_back({protein_sources[protein_id[j]].x, protein_sources[protein_id[j]].y - 1, "HARVESTER", "S", 30 * multi});
            targets.push_back({protein_sources[protein_id[j]].x + 1, protein_sources[protein_id[j]].y, "HARVESTER", "W", 30 * multi});
            targets.push_back({protein_sources[protein_id[j]].x - 1, protein_sources[protein_id[j]].y, "HARVESTER", "E", 30 * multi});
            spore_targets.push_back({protein_sources[protein_id[j]].x, protein_sources[protein_id[j]].y + 2, "SPORE", "E", 30 * multi});
            spore_targets.push_back({protein_sources[protein_id[j]].x, protein_sources[protein_id[j]].y - 2, "SPORE", "S", 30 * multi});
            spore_targets.push_back({protein_sources[protein_id[j]].x + 2, protein_sources[protein_id[j]].y, "SPORE", "W", 30 * multi});
            spore_targets.push_back({protein_sources[protein_id[j]].x - 2, protein_sources[protein_id[j]].y, "SPORE", "E", 30 * multi});
            spore_targets.push_back({protein_sources[protein_id[j]].x + 1, protein_sources[protein_id[j]].y + 1, "SPORE", "E", 30 * multi});
            spore_targets.push_back({protein_sources[protein_id[j]].x + 1, protein_sources[protein_id[j]].y - 1, "SPORE", "S", 30 * multi});
            spore_targets.push_back({protein_sources[protein_id[j]].x - 1, protein_sources[protein_id[j]].y + 1, "SPORE", "W", 30 * multi});
            spore_targets.push_back({protein_sources[protein_id[j]].x - 1, protein_sources[protein_id[j]].y - 1, "SPORE", "E", 30 * multi});
        }
    }
}

void find_proteins(unordered_map<int, Entity> &protein_sources, vector<int> protein_id, vector<Target> &targets, vector<Target> &spore_targets)
{
    for (int j = 0; j < protein_id.size(); ++j)
    {
        if ((my_stock.C == 0 && protein_sources[protein_id[j]].type == "C") || (my_stock.D == 0 && protein_sources[protein_id[j]].type == "D"))
        {
            targets.push_back({protein_sources[protein_id[j]].x, protein_sources[protein_id[j]].y, findBestMove(), "N", 100});
        }
    }
}

void target_opp_proteins(unordered_map<int, Entity> &protein_sources, vector<int> protein_id, vector<Target> &targets)
{
    for (int j = 0; j < protein_id.size(); ++j)
    {
        if (protein_sources[protein_id[j]].organ_root_id==-2)
        {
            targets.push_back({protein_sources[protein_id[j]].x, protein_sources[protein_id[j]].y, findBestMove(), "N", 30});
        }
    }
}

void find_op_roots(std::unordered_map<int, Root> &op_roots, vector<int> op_roots_id, vector<Target> &targets, unordered_map<int, Entity> &entities)
{
    for (int j = 0; j < op_roots_id.size(); ++j)
    {
        targets.push_back({op_roots[op_roots_id[j]].organs[op_roots_id[j]].x, op_roots[op_roots_id[j]].organs[op_roots_id[j]].y + 1, "TENTACLE", "N", 100});
        targets.push_back({op_roots[op_roots_id[j]].organs[op_roots_id[j]].x, op_roots[op_roots_id[j]].organs[op_roots_id[j]].y - 1, "TENTACLE", "S", 100});
        targets.push_back({op_roots[op_roots_id[j]].organs[op_roots_id[j]].x + 1, op_roots[op_roots_id[j]].organs[op_roots_id[j]].y, "TENTACLE", "W", 100});
        targets.push_back({op_roots[op_roots_id[j]].organs[op_roots_id[j]].x - 1, op_roots[op_roots_id[j]].organs[op_roots_id[j]].y, "TENTACLE", "E", 100});
    }
}

void find_op_organs(std::unordered_map<int, Root> &op_roots, vector<int> op_roots_id, vector<Target> &targets, unordered_map<int, Entity> &entities, vector<Target> &spore_targets)
{
    for (int j = 0; j < op_roots_id.size(); ++j)
    {
        for (int i = 0; i < op_roots[op_roots_id[j]].organs_id.size(); ++i)
        {
            int id = op_roots[op_roots_id[j]].organs_id[i];
            int val = (op_roots_id[j] == op_roots[op_roots_id[j]].organs_id[i])? 500 : 400;
            targets.push_back({op_roots[op_roots_id[j]].organs[id].x, op_roots[op_roots_id[j]].organs[id].y + 1, "TENTACLE", "N", val});
            targets.push_back({op_roots[op_roots_id[j]].organs[id].x, op_roots[op_roots_id[j]].organs[id].y - 1, "TENTACLE", "S", val});
            targets.push_back({op_roots[op_roots_id[j]].organs[id].x + 1, op_roots[op_roots_id[j]].organs[id].y, "TENTACLE", "W", val});
            targets.push_back({op_roots[op_roots_id[j]].organs[id].x - 1, op_roots[op_roots_id[j]].organs[id].y, "TENTACLE", "E", val});
            if (op_roots_id[j] == op_roots[op_roots_id[j]].organs_id[i])
            {
                spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x, op_roots[op_roots_id[j]].organs[id].y + 2, "SPORE", "E", 20 * val});
                spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x, op_roots[op_roots_id[j]].organs[id].y - 2, "SPORE", "S", 20 * val});
                spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x + 2, op_roots[op_roots_id[j]].organs[id].y, "SPORE", "W", 20 * val});
                spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x - 2, op_roots[op_roots_id[j]].organs[id].y, "SPORE", "E", 20 * val});
                spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x + 1, op_roots[op_roots_id[j]].organs[id].y + 1, "SPORE", "E", 100 * val});
                spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x + 1, op_roots[op_roots_id[j]].organs[id].y - 1, "SPORE", "S", 100 * val});
                spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x - 1, op_roots[op_roots_id[j]].organs[id].y + 1, "SPORE", "W", 100 * val});
                spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x - 1, op_roots[op_roots_id[j]].organs[id].y - 1, "SPORE", "E", 100 * val});
            }
            spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x, op_roots[op_roots_id[j]].organs[id].y + 2, "SPORE", "E", 2 * val});
            spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x, op_roots[op_roots_id[j]].organs[id].y - 2, "SPORE", "S", 2 * val});
            spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x + 2, op_roots[op_roots_id[j]].organs[id].y, "SPORE", "W", 2 * val});
            spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x - 2, op_roots[op_roots_id[j]].organs[id].y, "SPORE", "E", 2 * val});
            spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x + 1, op_roots[op_roots_id[j]].organs[id].y + 1, "SPORE", "E", 2 * val});
            spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x + 1, op_roots[op_roots_id[j]].organs[id].y - 1, "SPORE", "S", 2 * val});
            spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x - 1, op_roots[op_roots_id[j]].organs[id].y + 1, "SPORE", "W", 2 * val});
            spore_targets.push_back({op_roots[op_roots_id[j]].organs[id].x - 1, op_roots[op_roots_id[j]].organs[id].y - 1, "SPORE", "E", 2 * val});
        }
    }
}

void target_bottlenecs(vector<Target> &targets)
{
    for (const auto &[y, x] : bottlenecks)
    {
        // cerr << "(" << x << ", " << y << ") ";
        targets.push_back({x, y + 1, "TENTACLE", "N", 20});
        targets.push_back({x, y - 1, "TENTACLE", "S", 20});
        targets.push_back({x + 1, y, "TENTACLE", "W", 20});
        targets.push_back({x - 1, y, "TENTACLE", "E", 20});
    }
    // cerr << endl;
}

void find_possible_spores(vector<Target> &spore_targets, unordered_map<int, Root> &my_roots, vector<int> my_roots_id, vector<vector<int>> &map, vector<vector<int>> &mapT0)
{
    int height = map[0].size();
    int width = map.size();
    for (int j = 0; j < my_roots_id.size(); ++j)
    {
        for (int targId = 0; targId<spore_targets.size(); ++targId)
        {
            bool found = false;
            for (int i = 0; i < my_roots[my_roots_id[j]].organs_id.size(); ++i)
            {
                int id = my_roots[my_roots_id[j]].organs_id[i];
                if (my_roots[my_roots_id[j]].organs[id].type=="SPORER" && (my_roots[my_roots_id[j]].organs[id].x == spore_targets[targId].x || my_roots[my_roots_id[j]].organs[id].y == spore_targets[targId].y) 
                    && ((my_roots[my_roots_id[j]].organs[id].x > spore_targets[targId].x && my_roots[my_roots_id[j]].organs[id].organ_dir == "W")||(my_roots[my_roots_id[j]].organs[id].x < spore_targets[targId].x && my_roots[my_roots_id[j]].organs[id].organ_dir == "E")
                    || (my_roots[my_roots_id[j]].organs[id].y > spore_targets[targId].y && my_roots[my_roots_id[j]].organs[id].organ_dir == "N") || (my_roots[my_roots_id[j]].organs[id].y < spore_targets[targId].y && my_roots[my_roots_id[j]].organs[id].organ_dir == "S")))
                {
                    int dist = abs(my_roots[my_roots_id[j]].organs[id].x - spore_targets[targId].x + my_roots[my_roots_id[j]].organs[id].y - spore_targets[targId].y);
                    if (dist < 4)
                        break;
                    bool accessible = true;
                    int xstep = (spore_targets[targId].x - my_roots[my_roots_id[j]].organs[id].x) / dist;
                    int ystep = (spore_targets[targId].y - my_roots[my_roots_id[j]].organs[id].y) / dist;
                    for (int steps = 1; steps <= dist; steps++)
                    {
                        if (my_roots[my_roots_id[j]].organs[id].y + steps * ystep < 0 || my_roots[my_roots_id[j]].organs[id].y + steps * ystep >= map.size() 
                            || my_roots[my_roots_id[j]].organs[id].x + steps * xstep < 0 || my_roots[my_roots_id[j]].organs[id].x + steps * xstep >= map[0].size()
                            || (map[my_roots[my_roots_id[j]].organs[id].y + steps * ystep][my_roots[my_roots_id[j]].organs[id].x + steps * xstep] == 1))
                        {
                            accessible = false;
                            break;
                        }
                    }
                    if (accessible)
                    {
                        Move mov;
                        mov.src_x = my_roots[my_roots_id[j]].organs[id].x;
                        mov.src_y = my_roots[my_roots_id[j]].organs[id].y;
                        mov.src_id = my_roots[my_roots_id[j]].organs[id].organ_id;
                        mov.dst_x = spore_targets[targId].x;
                        mov.dst_y = spore_targets[targId].y;
                        mov.dir = "N";
                        mov.cost = 1;
                        mov.type = "SPORE";
                        mov.value = spore_targets[targId].value;
                        my_roots[my_roots_id[j]].moves.push_back(mov);
                        found = true;
                        break;
                    }
                }
                else if (my_roots[my_roots_id[j]].organs[id].x == spore_targets[targId].x || my_roots[my_roots_id[j]].organs[id].y == spore_targets[targId].y) 
                {
                    int dist = abs(my_roots[my_roots_id[j]].organs[id].x - spore_targets[targId].x + my_roots[my_roots_id[j]].organs[id].y - spore_targets[targId].y);
                    if (dist < 5) 
                        break;
                    bool accessible = true;
                    int xstep = (spore_targets[targId].x - my_roots[my_roots_id[j]].organs[id].x) / dist;
                    int ystep = (spore_targets[targId].y - my_roots[my_roots_id[j]].organs[id].y) / dist;
                    for (int steps = 1; steps <= dist; steps++)
                    {
                        if (my_roots[my_roots_id[j]].organs[id].y + steps * ystep < 0 || my_roots[my_roots_id[j]].organs[id].y + steps * ystep >= map.size() 
                            || my_roots[my_roots_id[j]].organs[id].x + steps * xstep < 0 || my_roots[my_roots_id[j]].organs[id].x + steps * xstep >= map[0].size() 
                            || (map[my_roots[my_roots_id[j]].organs[id].y + steps * ystep][my_roots[my_roots_id[j]].organs[id].x + steps * xstep] == 1))
                        {
                            accessible = false;
                            break;
                        }
                    }
                    if (accessible && !mapT0[my_roots[my_roots_id[j]].organs[id].y + ystep][my_roots[my_roots_id[j]].organs[id].x + xstep])
                    {
                        Move mov;
                        mov.src_x = my_roots[my_roots_id[j]].organs[id].x;
                        mov.src_y = my_roots[my_roots_id[j]].organs[id].y;
                        mov.src_id = my_roots[my_roots_id[j]].organs[id].organ_id;
                        mov.dst_x = my_roots[my_roots_id[j]].organs[id].x+xstep;
                        mov.dst_y = my_roots[my_roots_id[j]].organs[id].y+ystep;
                        if (xstep < 0)
                            mov.dir = "W";
                        else if (xstep > 0)
                            mov.dir = "E";
                        else if (ystep < 0)
                            mov.dir = "N";
                        else
                            mov.dir = "S";

                        mov.cost = 1;
                        mov.type = "SPORER";
                        
                        if (num_roots<2) mov.value = 200;
                        else mov.value = 10;
                        my_roots[my_roots_id[j]].moves.push_back(mov);
                        found = true;
                        break;
                    }
                }
            }
        }
    }
}

bool hasSufficientResourcesForTarget(const std::string &type)
{
    auto it = move_cost.find(type);
    if (it == move_cost.end())
    {
        cerr << "Typ nie istnieje w kosztach"<<endl;
        return false; // Typ nie istnieje w kosztach
    }

    const Stock &cost = it->second;

    // Sprawdź tylko zasoby, które faktycznie są wymagane (koszt > 0)
    if ((cost.A > 0 && my_stock.A < cost.A) ||
        (cost.B > 0 && my_stock.B < cost.B) ||
        (cost.C > 0 && my_stock.C < cost.C) ||
        (cost.D > 0 && my_stock.D < cost.D))
    {
        return false; // Zasoby są niewystarczające
    }
    return true; // Zasoby są wystarczające
}

// Funkcja do usunięcia targetów, które nie mają wystarczających zasobów w my_stock
void removeTargetsWithInsufficientResources(vector<Target> &targets)
{
    // Użycie std::remove_if i erase do usunięcia elementów
    targets.erase(std::remove_if(targets.begin(), targets.end(),
                                 [](const Target &target)
                                 {
                                     return !hasSufficientResourcesForTarget(target.type); // Jeśli nie ma wystarczających zasobów, usuń
                                 }),
                  targets.end());
}

void filter_targets(vector<Target> &targets)
{
    removeDuplicates(targets);
}

const vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

bool isValidB(int x, int y, const vector<vector<int>> &map)
{
    return x >= 0 && x < map.size() && y >= 0 && y < map[0].size() && map[x][y] == 0;
}

void dfs(int x, int y, const vector<vector<int>> &map, vector<vector<bool>> &visited)
{
    visited[x][y] = true;
    for (const auto &[dx, dy] : directions)
    {
        int nx = x + dx, ny = y + dy;
        if (isValidB(nx, ny, map) && !visited[nx][ny])
        {
            dfs(nx, ny, map, visited);
        }
    }
}

int countComponents(const vector<vector<int>> &map)
{
    int components = 0;
    vector<vector<bool>> visited(map.size(), vector<bool>(map[0].size(), false));

    for (int i = 0; i < map.size(); ++i)
    {
        for (int j = 0; j < map[0].size(); ++j)
        {
            if (map[i][j] == 0 && !visited[i][j])
            {
                dfs(i, j, map, visited);
                components++;
            }
        }
    }
    return components;
}

vector<pair<int, int>> findBottlenecks(vector<vector<int>> map)
{
    vector<pair<int, int>> bottlenecks;
    int originalComponents = countComponents(map);

    for (int i = 0; i < map.size(); ++i)
    {
        for (int j = 0; j < map[0].size(); ++j)
        {
            if (map[i][j] == 0)
            {
                // Tymczasowo blokuj punkt
                map[i][j] = 1;
                int newComponents = countComponents(map);
                if (newComponents > originalComponents)
                {
                    bottlenecks.emplace_back(i, j);
                }
                // Przywróć punkt
                map[i][j] = 0;
            }
        }
    }

    return bottlenecks;
}

void removeBottleneck(int x, int y)
{
    auto it = std::remove_if(bottlenecks.begin(), bottlenecks.end(),
                             [x, y](const pair<int, int> &point)
                             {
                                 return point.first == x && point.second == y;
                             });
    if (it != bottlenecks.end())
    {
        bottlenecks.erase(it, bottlenecks.end());
    }
}

bool containsCoordinates(const vector<pair<int, int>> &this_round_dest, int x, int y)
{
    return std::any_of(this_round_dest.begin(), this_round_dest.end(),
                       [x, y](const pair<int, int> &point)
                       {
                           return point.first == x && point.second == y;
                       });
}

int main()
{
    std::srand(std::time(nullptr)); // Inicjalizacja generatora losowego
    bool blocked = false;
    turn = 0;
    int width; // columns in the game grid
    int height; // rows in the game grid
    cin >> width >> height; cin.ignore();
    move_cost.emplace("BASIC",Stock(1,0,0,0));
    move_cost.emplace("HARVESTER", Stock(0, 0, 1, 1));
    move_cost.emplace("TENTACLE", Stock(0, 1, 1, 0));
    move_cost.emplace("SPORER", Stock(0, 1, 0, 1));
    move_cost.emplace("SPORE", Stock(1, 1, 1, 1));
    move_cost.emplace("WAIT", Stock(0, 0, 0, 0));

    std::vector<std::vector<int>> map(height, std::vector<int>(width, 0));
    std::vector<std::vector<int>> mapT0(height, std::vector<int>(width, 0));
    std::vector<std::vector<int>> mapT1(height, std::vector<int>(width, 0));
    std::vector<std::vector<int>> mapToff(height, std::vector<int>(width, 0));
    std::vector<std::vector<int>> mapTwall(height, std::vector<int>(width, 0));
    vector<int> my_roots_id;
    vector<int> op_roots_id;
    vector<int> protein_id;
    std::unordered_map<int, Entity> entities;
    std::unordered_map<int, Entity> protein_sources;
    std::unordered_map<int, Root> my_roots;
    std::unordered_map<int, Root> op_roots;
    vector<Target> targets;
    vector<Target> spore_targets;
    vector<Target> dummy_targets;
    vector<pair<int,int>> this_round_dest;

    bool boottlenecki = false;

    // game loop
    while (1) {
        ++turn;
        chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        int wait_cnt = 0;
        int entity_count;
        cin >> entity_count; cin.ignore();
        entities.clear();
        protein_sources.clear();
        my_roots.clear();
        op_roots.clear();
        my_roots_id.clear();
        op_roots_id.clear();
        protein_id.clear();
        targets.clear();
        spore_targets.clear();
        dummy_targets.clear();
        my_gains.clear();
        my_gains.emplace("A", 0);
        my_gains.emplace("B", 0);
        my_gains.emplace("C", 0);
        my_gains.emplace("D", 0);
        this_round_dest.clear();

            for (auto &row : map)
        {
            std::fill(row.begin(), row.end(), -1);
        }
        for (auto& row : mapT0) {
            std::fill(row.begin(), row.end(), 0);
        }
        for (auto& row : mapT1) {
            std::fill(row.begin(), row.end(), 0);
        }
        for (auto &row : mapToff)
        {
            std::fill(row.begin(), row.end(), 0);
        }
        for (auto &row : mapTwall)
        {
            std::fill(row.begin(), row.end(), 0);
        }

        for (int i = 0; i < entity_count; i++) {
            int x;
            int y; // grid coordinate
            string type; // WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER, A, B, C, D
            int owner; // 1 if your organ, 0 if enemy organ, -1 if neither
            int organ_id; // id of this entity if it's an organ, 0 otherwise
            string organ_dir; // N,E,S,W or X if not an organ
            int organ_parent_id;
            int organ_root_id;
            cin >> x >> y >> type >> owner >> organ_id >> organ_dir >> organ_parent_id >> organ_root_id; cin.ignore();
            entities.emplace(i,Entity(x, y, type, owner, organ_id, organ_dir, organ_parent_id, organ_root_id));
            map[y][x] = i;
            if (type =="WALL")
            {
                mapT0[y][x]=1;
                mapT1[y][x]=1;
                mapToff[y][x] = 1;
                mapTwall[y][x] = 1;
            }
            if (type=="A"||type=="B"||type=="C"||type=="D")
            {
                mapT0[y][x]=1;
                protein_sources.emplace(i,Entity(x, y, type, owner, organ_id, organ_dir, organ_parent_id, organ_root_id));
                protein_id.push_back(i);
            }
            if (type == "ROOT" || type == "BASIC" || type == "TENTACLE" || type == "HARVESTER" || type == "SPORER")
            {

                mapT0[y][x]=1;
                mapT1[y][x]=1;
                if (owner == 1)
                {
                    mapToff[y][x] = 1;
                    if (type == "TENTACLE")
                    {
                        if (organ_dir == "N" && y > 0)
                        {
                            removeBottleneck(y - 1, x);
                        }
                        else if (organ_dir == "S" && y < height - 1)
                        {
                            removeBottleneck(y + 1, x);
                        }
                        else if (organ_dir == "W" && x > 0)
                        {
                            removeBottleneck(y, x - 1);
                        }
                        else if (organ_dir == "E" && x < width - 1)
                        {
                            removeBottleneck(y, x + 1);
                        }
                    }
                    my_roots.emplace(organ_root_id,Root());
                    my_roots[organ_root_id].organs.emplace(organ_id,Entity(x, y, type, owner, organ_id, organ_dir, organ_parent_id, organ_root_id));
                    if (std::find(my_roots_id.begin(), my_roots_id.end(), organ_root_id) == my_roots_id.end()) {
                        my_roots_id.push_back(organ_root_id);
                    }
                    my_roots[organ_root_id].organs_id.push_back(organ_id);
                }
                else if (owner == 0)
                {
                    if (type == "TENTACLE")
                    {
                        if (organ_dir=="N" && y > 0)
                        {
                            mapT0[y-1][x] = 1;
                            mapT1[y-1][x] = 1;
                        }
                        else if (organ_dir == "S" && y < height-1)
                        {
                            mapT0[y + 1][x] = 1;
                            mapT1[y + 1][x] = 1;
                        }
                        else if (organ_dir == "W" && x > 0)
                        {
                            mapT0[y][x - 1] = 1;
                            mapT1[y][x - 1] = 1;
                        }
                        else if (organ_dir == "E" && x < width - 1)
                        {
                            mapT0[y][x + 1] = 1;
                            mapT1[y][x + 1] = 1;
                        }
                    }
                    op_roots.emplace(organ_root_id,Root());
                    op_roots[organ_root_id].organs.emplace(organ_id,Entity(x, y, type, owner, organ_id, organ_dir, organ_parent_id, organ_root_id));
                    if (std::find(op_roots_id.begin(), op_roots_id.end(), organ_root_id) == op_roots_id.end()) {
                        op_roots_id.push_back(organ_root_id);
                    }
                    op_roots[organ_root_id].organs_id.push_back(organ_id);
                }
            }
        }
        cin >> my_stock.A >> my_stock.B >> my_stock.C >> my_stock.D; cin.ignore();
        cin >> op_stock.A >> op_stock.B >> op_stock.C >> op_stock.D; cin.ignore();
        int required_actions_count; // your number of organisms, output an action for each one in any order
        cin >> required_actions_count; cin.ignore();
        num_roots = required_actions_count;
        // for (const auto &row : mapToff)
        // {
        //     for (const auto &cell : row)
        //     {
        //         cerr << cell << " ";
        //     }
        //     cerr << std::endl;
        // }

        boottlenecki = true;
        bottlenecks = findBottlenecks(mapTwall);

        // cerr << "Wąskie gardła: ";
        // for (const auto &[x, y] : bottlenecks)
        // {
        //     cerr << "(" << x << ", " << y << ") ";
        // }
        // cerr << endl;

        // cerr<< "proteiny" <<endl;
        protein_analyzer(protein_sources, protein_id, map, entities, mapT0, mapT1);
        // cerr << "my gains " << my_gains["A"] << " " << my_gains["B"] << " " << my_gains["C"] << " " << my_gains["D"] <<endl;
        // cerr << "harvest" << endl;
        find_harvests(protein_sources, protein_id, targets, spore_targets);
        find_proteins(protein_sources, protein_id, targets, spore_targets);
        // find_op_roots(op_roots, op_roots_id, targets, entities);
        // cerr << "opp" << endl;
        find_op_organs(op_roots, op_roots_id, targets, entities, spore_targets);
        // cerr << "targetowanie bottleneckow"<<endl;
        target_bottlenecs(targets);
        target_opp_proteins(protein_sources, protein_id, targets);
        cerr << "redukcja targetow" << endl;
        cerr << "1targets.size() " << targets.size() << "\n";
        removeTargetsWithInsufficientResources(spore_targets);
        cerr << "2targets.size() " << targets.size() << "\n";
        removeTargetsWithInsufficientResources(targets);
        cerr << "3targets.size() " << targets.size() << "\n";
        filter_targets(spore_targets);
        filter_targets(targets);
        // cerr << "sporki" << endl;
        find_possible_spores(spore_targets, my_roots, my_roots_id, mapT1, mapT0);
        cerr << "ruchy" << endl;
        if (my_stock.C == 0 || my_stock.D == 0)
            blocked = true;
        if (!blocked)
        {
            calculate_move(targets, my_roots, my_roots_id, mapT0, mapToff, mapT1);
        }
        else
        {
            calculate_move(targets, my_roots, my_roots_id, mapT1, mapToff, mapT1);
        }
        // cerr << "po calc"<<endl;

        for (int i = 0; i < required_actions_count; i++) {
            if (my_roots[my_roots_id[i]].moves.size()==0)
            {
                cerr << "no move " << my_roots_id[i]<< endl;
                if (!blocked)
                {
                    dummy_move(my_roots, my_roots_id[i], mapT0, mapToff);
                }
                else
                {
                    dummy_move(my_roots, my_roots_id[i], mapT1, mapToff);
                }
                // dummy_move(my_roots, my_roots_id[i], mapT0);
            }
            std::sort(my_roots[my_roots_id[i]].moves.begin(), my_roots[my_roots_id[i]].moves.end(), [](const Move& a, const Move& b) {
                double ratioA = static_cast<double>(a.value) / a.cost;
                double ratioB = static_cast<double>(b.value) / b.cost;
                return ratioA > ratioB;  // Malejąco
            });
        }
        // cerr << "przed ruchami" << endl;
        blocked = false;
        for (int i = 0; i < required_actions_count; i++) {
            printMoves(my_roots[my_roots_id[i]].moves);
            for (int movId = 0; movId < my_roots[my_roots_id[i]].moves.size(); ++movId)
            {
                // cerr << my_roots[my_roots_id[i]].moves[movId].value << " " << my_roots[my_roots_id[i]].moves[movId].type << " " << my_roots[my_roots_id[i]].moves[movId].dst_x << " " << my_roots[my_roots_id[i]].moves[movId].dst_y << endl;
                if (my_stock.A - move_cost[my_roots[my_roots_id[i]].moves[movId].type].A >= 0 && my_stock.B - move_cost[my_roots[my_roots_id[i]].moves[movId].type].B >= 0 && my_stock.C - move_cost[my_roots[my_roots_id[i]].moves[movId].type].C >= 0 && my_stock.D - move_cost[my_roots[my_roots_id[i]].moves[movId].type].D >= 0 
                    && (!containsCoordinates(this_round_dest, my_roots[my_roots_id[i]].moves[movId].dst_x, my_roots[my_roots_id[i]].moves[movId].dst_y) && my_roots[my_roots_id[i]].moves[movId].type != "WAIT"))
                {
                    my_stock.A -= move_cost[my_roots[my_roots_id[i]].moves[movId].type].A;
                    my_stock.B -= move_cost[my_roots[my_roots_id[i]].moves[movId].type].B;
                    my_stock.C -= move_cost[my_roots[my_roots_id[i]].moves[movId].type].C;
                    my_stock.D -= move_cost[my_roots[my_roots_id[i]].moves[movId].type].D;
                    if (my_roots[my_roots_id[i]].moves[movId].type == "WAIT")
                    {
                        cout << "WAIT " << endl;
                        wait_cnt++;
                    }
                    else if (my_roots[my_roots_id[i]].moves[movId].type == "SPORE")
                    {
                        cout << "SPORE " << my_roots[my_roots_id[i]].moves[movId].src_id << " " << my_roots[my_roots_id[i]].moves[movId].dst_x << " " << my_roots[my_roots_id[i]].moves[movId].dst_y << endl;
                        this_round_dest.push_back({my_roots[my_roots_id[i]].moves[movId].dst_x, my_roots[my_roots_id[i]].moves[movId].dst_y});
                    }
                    else
                    {
                        cout << "GROW " << my_roots[my_roots_id[i]].moves[movId].src_id << " " << my_roots[my_roots_id[i]].moves[movId].dst_x << " " << my_roots[my_roots_id[i]].moves[movId].dst_y << " " << my_roots[my_roots_id[i]].moves[movId].type << " " << my_roots[my_roots_id[i]].moves[movId].dir << endl;
                        this_round_dest.push_back({my_roots[my_roots_id[i]].moves[movId].dst_x, my_roots[my_roots_id[i]].moves[movId].dst_y});
                    }
                    break;
                }
                if (movId == my_roots[my_roots_id[i]].moves.size()-1)
                {
                    cout << "WAIT " << endl;
                    wait_cnt++;
                }
            }
        }
        if (wait_cnt == required_actions_count) 
            blocked = true;
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        cerr << "Time = " << chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000 << "[ms]" << endl;
    }
}