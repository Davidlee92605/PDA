#include<bits/stdc++.h>
#include "parse.h"

using namespace std;

Parse::Parse(const char * hardblocks_filename, const char * nets_filename, const char * pl_filename, const char * ratio_filename){
    this->hardblocks_filename = hardblocks_filename;
    this->nets_filename = nets_filename;
    this->pl_filename =  pl_filename;
    this->ratio_filename = ratio_filename;
    parse();
}

Parse::~Parse() {}

void Parse::parse(){
    ifstream hardblocks(this->hardblocks_filename);
    ifstream nets(this->nets_filename);
    ifstream pl(this->pl_filename);
    //// read hardblocks file ///////
    string temp;
    hardblocks >> temp >> temp >> total_blocks;
    hardblocks >> temp >> temp >> total_terminals;
    int count = total_blocks;
    while(count--){
        char s , b;
        hardblocks >> s >> b; //sb
        BLOCK* block = new BLOCK();
        int _id;
        hardblocks >> _id >> temp; //hardrectilinear
        block->setId(_id);
        int num; 
        hardblocks >> num; //4
        char c;
        int zero;
        int x , y;
        hardblocks >> c; // (
        hardblocks >> x;
        //hardblocks >> block->left_down.first;
        hardblocks >> c; // ,
        hardblocks >> y;
        //hardblocks >> block->left_down.second;
        hardblocks >> c; // )
        block->setLeft_down(x,y);
        ////////////////////////
        hardblocks >> c; // (
        hardblocks >> zero;
        hardblocks >> c; // ,
        hardblocks >> y;
        hardblocks >> c; // )
        ////////////////////////
        hardblocks >> c; // (
        hardblocks >> x;
        hardblocks >> c; // ,
        hardblocks >> y;
        hardblocks >> c; // )
        ////////////////////////
        hardblocks >> c; // (
        hardblocks >> x;
        hardblocks >> c; // ,
        hardblocks >> zero;
        hardblocks >> c; // )
        block->setHeight(y);
        block->setWidth(x);
        //block->height = y;
        //block->width = x;
        int area;
        area = y * x;
        block->setArea(area);
        //block->area = block->height * block->width;
        block_id_to_index[block->getId()] = total_blocks - count - 1;
        blockarea += block->getArea();
        Blocks.push_back(block);
    }
    char c;
    count = total_terminals;
    while(count--){
        hardblocks >> c; //p
        int id;
        hardblocks >> id; // id
        TERMINAL* terminal = new TERMINAL();
        terminal->id = id;
        hardblocks >> temp;
        terminal_id_to_index[terminal->id] = total_terminals - count - 1;
        Terminals.push_back(terminal);
    }

    //// read nets file //////////////
    nets >> temp >> temp >> total_nets; 
    nets >> temp >> temp >> total_pins;
    count = total_nets;
    while(count--){
        NET* net = new NET();
        net->id = total_nets - count;
        nets >> temp >> temp  >> net->degree;
        char c;
        int id;
        for(int i = 0; i < net->degree; i++){
            nets >> c;
            if(c == 'p'){
                nets >> id;
                net->pins.push_back(Terminals[terminal_id_to_index[id]]);
            }else if(c == 's'){
                char b;
                nets >> b;
                nets >> id;
                net->blocks.push_back(Blocks[block_id_to_index[id]]);
            }
        }
        Nets.push_back(net);
    }
    //// read pl file //////////////
    count = total_terminals;
    while(count--){
        pl >> c ;
        int p_id;
        pl >> p_id ;
        pl >> Terminals[terminal_id_to_index[p_id]]->x >> Terminals[terminal_id_to_index[p_id]]->y ; 
    }
    
    //// read ratio //////////////////////////
    space_ratio = std::stod(this->ratio_filename);
    get_outline();
}

void Parse::get_outline(){
    outline = sqrt(blockarea * (1.0 + space_ratio));
}