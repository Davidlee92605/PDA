#include<bits/stdc++.h>
#include "Object.h"
using namespace std;

void printBlocks(){
    cout << "Blocks size: " << Blocks.size() << endl;
    for(auto i : Blocks){
        cout << "Blocks id: " << i->getId() << endl;
        cout << "Blocks area: " << i->getArea() << endl;
        cout << "Blocks height: " << i->getHeight() << endl;
        cout << "Blocks width: " << i->getWidth() << endl;
        cout << "Rotate: " << i->getRotated() << endl;
        // cout << "left_down_x: " << i->left_down.first << endl;
        // cout << "left_down_y: " << i->left_down.second << endl;
    }
}
void printTermianls(){
    cout << "Terminals size: " << Terminals.size() << endl;
    for(auto i : Terminals){
        cout << "Terminals id: " << i->id << endl;
        cout << "X: " << i->x << "Y: " << i->y << endl;
    }
    return;
}
void printNets(){
    cout << "Nets size: " << Nets.size() << endl;
    for(auto i : Nets){
        cout << "Nets id: " << i->id << endl;
        cout << "degree: " << i->degree << endl;
        cout << "blocks: " ; 
        for(auto b : i->blocks){
            cout << b->getId() << " " ;
        }
        cout << endl;
        cout << "pins: " ; 
        for(auto p : i->pins){
            cout << p->id << " " ;
        }
        cout << endl;
    }
    return;
}