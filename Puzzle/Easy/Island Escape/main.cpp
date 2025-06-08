#include <iostream>
#include <vector>

using namespace std;

vector<vector<int>> adj;
vector<bool> visited;
vector<vector<int>> grid;
int n;
bool found = false;

void dfs(int u)
{
    if (found)
        return;

    visited[u] = true;

    int i = u / n;
    int j = u % n;

    if (grid[i][j] == 0)
    {
        found = true;
        return;
    }

    for (int v : adj[u])
    {
        if (!visited[v])
        {
            dfs(v);
        }
    }
}

int main()
{
    cin >> n;
    cin.ignore();
    adj.resize(n * n);
    visited.resize(n * n);
    grid.resize(n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int elevation;
            cin >> elevation;
            cin.ignore();
            grid[i].push_back(elevation);
        }
    }

    for (const auto &row : grid)
    {
        for (int val : row)
        {
            cerr << val << " ";
        }
        cerr << endl;
    }

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            int u = i * n + j;

            for (int dir = 0; dir < 4; ++dir)
            {
                int ni = i + dx[dir];
                int nj = j + dy[dir];

                if (ni >= 0 && ni < n && nj >= 0 && nj < n)
                {
                    if (abs(grid[i][j] - grid[ni][nj]) <= 1)
                    {
                        int v = ni * n + nj;
                        adj[u].push_back(v);
                    }
                }
            }
        }
    }

    dfs(n / 2 + n * (n / 2));

    if (!found)
    {
        cout << "no" << endl;
    }
    else
    {
        cout << "yes" << endl;
    }
}