#include <iostream>

using namespace std;

int main()
{
    int n, min = 5555;
    cin >> n; cin.ignore();
    
    for (int i = 0; i < n; i++) {
        int t; // a temperature expressed as an integer ranging from -273 to 5526
        cin >> t; cin.ignore();
        
        if (abs(t)<abs(min)) min = t;
        else if (abs(t)==abs(min) && (t>=0)) min = t;
    }

    if (min ==5555) cout << "0" <<endl;
    else cout << min << endl;
}