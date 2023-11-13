#include<iostream>
using namespace std;main(){int a,b,c,d;cin>>a>>b>>c>>d;for(;;){if(d>b){cout<<"N";d--;}if(d<b){cout<<"S";d++;}if(c>a){cout<<"W";c--;}if(c<a){cout<<"E";c++;}cout<<endl;}}