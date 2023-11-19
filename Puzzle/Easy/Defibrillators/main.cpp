#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

int main()
{
    int found = 0;
    int ind = 0;
    double lat, lon, d;
    double min = 99999;
    string LON;
    cin >> LON; cin.ignore();
    string LAT;
    cin >> LAT; cin.ignore();
    int N;
    cin >> N; cin.ignore();
    string DEFIB[N];
    double def[N][2];
    for (int i = 0; i < N; i++) {
        getline(cin, DEFIB[i]);
    }

    found = LON.find(',');
    LON.replace(found,1,".");
    found = LAT.find(',');
    LAT.replace(found,1,".");
    lon = stod(LON)*M_PI/180;
    lat = stod(LAT)*M_PI/180;
    cerr<<lon<<" "<<lat<<endl;

    for (int i = 0; i<N; ++i)
    {
        found = 0;
        int num = count(DEFIB[i].begin(),DEFIB[i].end(),',');
        for (int j = 0;j<num;++j)
        {
            found = DEFIB[i].find(',',found+1);
            DEFIB[i].replace(found,1,".");
        }
        found = DEFIB[i].find_last_of(';');
        def[i][1]=stod(DEFIB[i].substr(found+1))*M_PI/180;
        found = DEFIB[i].substr(0,found-1).find_last_of(";");
        def[i][0]=stod(DEFIB[i].substr(found+1))*M_PI/180;
    }
    
    for (int i = 0; i<N;++i)
    {
        d=sqrt(pow((lon-def[i][0])*cos((lat+def[i][1])/2),2)+pow(lat-def[i][1],2))*6371;
        if (d<min)
        {
            ind = i;
            min = d;
        }
    }
    found = DEFIB[ind].find(';');
    cout << DEFIB[ind].substr(found+1,DEFIB[ind].find(';',found+1)-found-1) << endl;
}