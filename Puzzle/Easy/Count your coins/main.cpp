#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    bool enough = false;
    vector<int> coins, types, values;
    int value_to_reach, n, count, value, sum = 0;
    cin >> value_to_reach;
    cin >> n;

    for (int i = 0; i < n; i++)
    {
        cin >> count;
        types.push_back(count);
    }
    for (int i = 0; i < n; i++)
    {
        cin >> value;
        values.push_back(value);
    }

    int types_size = types.size();
    for (int i = 0; i < types_size; ++i)
    {
        for (int j = 0; j < types[i]; ++j)
        {
            coins.push_back(values[i]);
        }
    }

    sort(coins.begin(), coins.end(),
         [](const int a, const int b)
         { return a < b; });

    int coin_size = coins.size();
    for (int i = 0; i < coin_size; ++i)
    {
        sum += coins[i];
        if (sum >= value_to_reach)
        {
            enough = true;
            cout << i + 1 << endl;
            break;
        }
    }
    if (!enough)
        cout << "-1" << endl;
}