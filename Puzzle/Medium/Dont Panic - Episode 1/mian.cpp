#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

int main()
{
    int nb_floors; // number of floors
    int width; // width of the area
    int nb_rounds; // maximum number of rounds
    int exit_floor; // floor on which the exit is found
    int exit_pos; // position of the exit on its floor
    int nb_total_clones; // number of generated clones
    int nb_additional_elevators; // ignore (always zero)
    int nb_elevators; // number of elevators
    unordered_map<int,int> elevators;
    unordered_map<string,int> directions ={{"LEFT",-1},{"RIGHT",1}};
    vector<bool> done;
    cin >> nb_floors >> width >> nb_rounds >> exit_floor >> exit_pos >> nb_total_clones >> nb_additional_elevators >> nb_elevators; cin.ignore();
    for (int i = 0; i < nb_elevators; i++) {
        int elevator_floor; // floor on which this elevator is found
        int elevator_pos; // position of the elevator on its floor
        cin >> elevator_floor >> elevator_pos; cin.ignore();
        elevators.insert({elevator_floor,elevator_pos});
        done.push_back(false);
    }
    elevators.insert({exit_floor,exit_pos});
    done.push_back(false);
    // game loop
    while (1) {
        int clone_floor; // floor of the leading clone
        int clone_pos; // position of the leading clone on its floor
        string direction; // direction of the leading clone: LEFT or RIGHT
        cin >> clone_floor >> clone_pos >> direction; cin.ignore();
        if (done[clone_floor])
            cout << "WAIT" << endl; // action: WAIT or BLOCK
        else
        {
            done[clone_floor] = true;
            if ((clone_pos - elevators[clone_floor])*directions[direction]<=0)
                cout << "WAIT" << endl;
            else
                cout << "BLOCK" << endl;
        }
    }
}