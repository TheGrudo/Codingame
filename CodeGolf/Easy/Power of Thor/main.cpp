#include <iostream>
main(){int a,b,c,d;std::cin >> a >> b >> c >> d;for(;;){std::cout<<(d>b?"N":d<b?"S":"")<<(c>a?"W":c<a?"E":"")<<"\n";d+=d<b;d-=d>b;c+=c<a;c-=c>a;}}