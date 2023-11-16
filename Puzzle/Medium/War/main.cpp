#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;
class Card
{
    public:
    int value;
    friend ostream& operator<< (ostream &out, const Card &c);
    friend bool operator== (const Card& c1, const Card& c2);
    friend bool operator!= (const Card& c1, const Card& c2);
    friend bool operator< (const Card& c1, const Card& c2);
    friend bool operator> (const Card& c1, const Card& c2);
    
    Card(string s)
    {
        if (s.substr(0,s.size()-1)=="J") value = 11;
        else if (s.substr(0,s.size()-1)=="Q") value = 12;
        else if (s.substr(0,s.size()-1)=="K") value = 13;
        else if (s.substr(0,s.size()-1)=="A") value = 14;
        else value = stoi(s.substr(0,s.size()-1));
    }
};

ostream& operator<<(ostream& out, const Card& c) {
    return (out << c.value);
}
bool operator== (const Card& c1, const Card& c2) {
    return c1.value == c2.value;
}
bool operator!= (const Card& c1, const Card& c2) {
    return c1.value != c2.value;
}
bool operator< (const Card& c1, const Card& c2) {
    return c1.value < c2.value;
}
bool operator> (const Card& c1, const Card& c2) {
    return c1.value > c2.value;
}

void battle(queue<Card*> &p1, queue<Card*> &p2, queue<Card*> &p1war, queue<Card*> &p2war);

void war (queue<Card*> &p1, queue<Card*> &p2, queue<Card*> &p1war, queue<Card*> &p2war)
{
    for (int i = 0; i < 4; ++i)
    {
        p1war.push(p1.front());
        p1.pop();
        p2war.push(p2.front());
        p2.pop();
        if (p1.empty() || p2.empty()) return;
    }
    battle(p1,p2,p1war,p2war);
}

void battle(queue<Card*> &p1, queue<Card*> &p2, queue<Card*> &p1war, queue<Card*> &p2war)
{
    if (*p1.front() > *p2.front())
    {
        while (!p1war.empty())
        {
            p1.push(p1war.front());
            p1war.pop();
        }
        p1.push(p1.front());
        p1.pop();
        while (!p2war.empty())
        {
            p1.push(p2war.front());
            p2war.pop();
        }
        p1.push(p2.front());
        p2.pop();
    }
    else if ((*p2.front()>*p1.front()))
    {
        while (!p1war.empty())
        {
            p2.push(p1war.front());
            p1war.pop();
        }
        p2.push(p1.front());
        p1.pop();
        while (!p2war.empty())
        {
            p2.push(p2war.front());
            p2war.pop();
        }
        p2.push(p2.front());
        p2.pop();
    }
    else if (*p1.front() == *p2.front()) 
        war(p1,p2,p1war,p2war);
    else
    {
        cerr<<"Wrong cards in battle "<<*p1.front()<<" "<<*p2.front()<<endl;
    }
}

int main()
{
    queue<Card*> p1,p2,p1war,p2war;
    int result, turns = 0;
    int n; // the number of cards for player 1
    cin >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        string cardp_1; // the n cards of player 1
        cin >> cardp_1; cin.ignore();
        p1.push(new Card(cardp_1));
    }
    int m; // the number of cards for player 2
    cin >> m; cin.ignore();
    for (int i = 0; i < m; i++) {
        string cardp_2; // the m cards of player 2
        cin >> cardp_2; cin.ignore();
        p2.push(new Card(cardp_2));
    }

    while(!p1.empty() && !p2.empty())
    {
        turns++;
        battle(p1,p2,p1war,p2war);
    }

    if ((p1.empty() || p2.empty()) && !p1war.empty() && !p2war.empty()) cout << "PAT" << endl;
    else if (p1.empty()) cout<<2<<" "<<turns<<endl;
    else if (p2.empty()) cout<<1<<" "<<turns<<endl;
    else
        cout << "PAT" << endl;
}