#ifndef OBJECT_H
#define OBJECT_H
#include<bits/stdc++.h>
using namespace std;
class BLOCK{
    private:
        int id;
        int height, width;
        int area;
        bool rotated = 0;
        pair<int, int> left_down;
    public:
        BLOCK() {};
        ~BLOCK();
        int getHeight(){
            if(rotated){
                return width;
            }else{
                return height;
            }
        }
        int getWidth(){
            if(rotated){
                return height;
            }else{
                return width;
            }
        }
        int getId(){return id;};
        int getArea(){return area;};
        bool getRotated(){return rotated;};
        int getLeft_down_x(){return left_down.first;};
        int getLeft_down_y(){return left_down.second;};
        void setId(int _id) {id = _id;};
        void setHeight(int _height) {height = _height;};
        void setWidth(int _width) {width = _width;};
        void setArea(int _area) {area = _area;};
        void setRotated(bool _rotated) {rotated = _rotated;};
        void setLeft_down(int x, int y) {left_down.first = x; left_down.second = y;};
};

struct TERMINAL{
    int id;
    int x, y;
};

struct NET{
    int id;
    int degree;
    vector<BLOCK*> blocks;
    vector<TERMINAL*> pins;
};

struct NODE{
    BLOCK* block;
    NODE* left = nullptr;
    NODE* right = nullptr;
    NODE* parent = nullptr;
};

struct Contour_Line {
    Contour_Line* next = nullptr;
    int from;
    int end;
    int y;
};

extern vector<BLOCK*> Blocks; //all blocks
extern vector<TERMINAL*> Terminals;//all terminals
extern vector<NET*> Nets;//all nets
extern vector<BLOCK*> sorted_list;
extern map<int , int> terminal_id_to_index;
extern map<int , int> block_id_to_index;
extern NODE* tree_array;
extern vector <NODE*> node_in_tree;
extern int total_blocks;
extern int total_terminals;
extern int total_nets;
extern int total_pins;
extern int outline;
extern double space_ratio;
extern int blockarea;
extern Contour_Line* Contour_line;
#endif