#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Bot
{
    public:
    int pos;
    int dir;

    void move()
    {
        pos +=dir;
    }

    void changeDir()
    {
        dir *= -1;
    }
    Bot(int p)
    {
        pos = p;
        dir = 1;
    }
};

void collisions(vector<Bot> &vec)
{
    int s = vec.size();
    for (int i = 0; i<s;++i)
    {
        for (int j = i+1; j<s;++j)
        {
            if (vec[i].pos == vec[j].pos)
            {
                vec[i].changeDir();
                vec[j].changeDir();
            }
        }
    }
}

void outed(vector<Bot> &vec, int l)
{
    int s = vec.size();
    for (int i = 0; i<s;++i)
    {
        if (vec[i].pos < 0 || vec[i].pos > l)
        {
            vec.erase(vec.begin()+i);
            --i;
            --s;
        }
    }
}

void moveAll(vector<Bot> &vec)
{   
    int s = vec.size();
    for (int i = 0; i<s;++i)
    {
        vec[i].move();
    }
}

int main()
{
    vector<Bot> bots;
    int l, n, best_t, t;
    cin >> l >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        int b;
        cin >> b; cin.ignore();
        bots.push_back(Bot(b));
    }
    for (int k = 0; k < n; ++k)
    {
        for (int i = k; i < n; ++i)
        {
            int tmpt = 0;
            t = 0;
            vector<Bot> tmp = bots;
            while (!tmp.empty())
            {
                ++t;
                collisions(tmp);
                moveAll(tmp);
                outed(tmp,l);
            } 
            if (best_t<t) best_t = t;

            for (int j = 1; j < n; ++j)
            {
                if (i%j==0) bots[j].changeDir();
            }
        }
    }
    cout << best_t-1 << endl;
}