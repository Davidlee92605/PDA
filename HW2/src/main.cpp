#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;
#define ratio 0.53


struct CELL{
    int id, size, gain, pin, lock;
    char set;
    vector<int> nets_in_cell;
};
struct NET{
    int id, size;
    vector<int> cells_in_A;
    vector<int> cells_in_B;
    vector<int> cells_in_net;
};


//////////////////global/////////////////////////////////////////////////
vector<CELL*> cells;
vector<NET*> nets;
map<int, vector<CELL*>> bucketlist;
map<int, int> cell_name_to_index;
map<int, int> net_id_to_index;
int total_cell_size = 0;
int max_cell_size = 0;
int max_gain = 0;
int max_pin = 0;
int current_A_size = 0;
vector<CELL*> move_order;
////////////////////////////////////////////////////////////////////////
void readfile(ifstream &cell_file, ifstream &net_file){
    char temp;
    int cell_id, cell_size;
    int net_id;
    while(cell_file >> temp){
        cell_file >> cell_id >> cell_size; 
        CELL *c = new CELL();
        c->id = cell_id;
        c->size = cell_size;
        c->gain = 0;
        c->set = 'B';
        total_cell_size += cell_size;
        if(cell_size > max_cell_size){
            max_cell_size = cell_size;
        }
        cells.push_back(c);
        cell_name_to_index[cell_id] = cells.size()-1;
    }
    string str;
    while(net_file >> str){
        net_file >> temp >> net_id >> temp;
        NET *n = new NET();
        n->id = net_id;
        int size = 0;
        while(net_file >> temp){
            int c_id;
            if(temp == 'c'){
                net_file >> c_id;
                cells[cell_name_to_index[c_id]]->pin++;
                cells[cell_name_to_index[c_id]]->nets_in_cell.push_back(net_id);
                
                n->cells_in_net.push_back(c_id);
                
                size++;
            }else if(temp == '}'){
                break;
            }
        }
        n->size = size;
        nets.push_back(n);
        net_id_to_index[net_id] = nets.size() - 1;
    }
}

bool is_balance(int size, bool init){//// check whether is balance
    if(init){
        if(size >  ratio*(total_cell_size - max_cell_size)  && size < ratio* (total_cell_size + max_cell_size) ){
            return true;
        }else{
            return false;
        }
    }else{
        if((double)abs(total_cell_size - 2*size) < (double)total_cell_size/10){
            return true;
        }else{
            return false;
        }
    }
    
}

void initial_set(){//// for initial partition
    ///////////////set A from net until balance ///////////////////////
    int A_size = 0 ;
    int count = 0;
    CELL* cell_ptr =NULL;
    for(auto n : nets){
        for(auto c_id : n->cells_in_net){
            if(!is_balance(A_size, true)){
                if(cells[cell_name_to_index[c_id]]->set == 'B'){
                    cells[cell_name_to_index[c_id]]->set = 'A';
                    n->cells_in_A.push_back(c_id);
                    A_size += cells[cell_name_to_index[c_id]]->size;
                }else{
                    n->cells_in_A.push_back(c_id);
                }
            }else{
                if(cells[cell_name_to_index[c_id]]->set == 'A'){
                    n->cells_in_A.push_back(c_id);
                }else{
                    n->cells_in_B.push_back(c_id);
                }
            }
        }
    }
    //cout << "A_size: " << A_size << endl;
    current_A_size = A_size;
}

int  get_cutsize(vector<NET*> nets){
    int cutsize = 0;
    for(auto n : nets){
        if(n->cells_in_A.size() > 0 && n->cells_in_B.size() > 0){
            cutsize++;
        }
    }
    return cutsize;
}

void gain_init(){
    for(auto n : nets){
        if(n->cells_in_A.size() == 0){
            for(auto c_id : n->cells_in_B){
                cells[cell_name_to_index[c_id]]->gain--;
            }
        }
        if(n->cells_in_B.size() == 0){
            for(auto c_id : n->cells_in_A){
                cells[cell_name_to_index[c_id]]->gain--;
            }
        }
        if(n->cells_in_A.size() == 1 ){
            cells[cell_name_to_index[n->cells_in_A.back()]]->gain++;
        }
        if(n->cells_in_B.size() == 1 ){
            cells[cell_name_to_index[n->cells_in_B.back()]]->gain++;
        }
    }
}

void set_bucket_list(){
    for(auto c : cells){
        if(max_pin < c->pin){
            max_pin = c->pin;
        }
        if(max_gain < c->gain){
            max_gain = c->gain;
        }
        bucketlist[c->gain].push_back(c);
    }
}

void print_cells(){
    int count = 0;
    for(int i=0; i<cells.size(); i++){
        cout << count++ << " id: " << cells[i]->id << " size: " << cells[i]->size << " pin: " << cells[i]->pin << " set: " << cells[i]->set <<" gain: " << cells[i]->gain << endl;
    }
}

void print_nets(){
    int count = 0;
    for(auto n : nets){
        cout << "nets id: " << n->id << " nets size: " << n->size <<" ";
        for(auto i : n->cells_in_A){
            cout << "cells in A :" << i << " ";
        }
        cout << endl;
        for(auto i : n->cells_in_B){
            cout << "cells in B :" <<  i << " ";
        }
        cout << endl;
    }
} 

void update_gain(CELL* movedcell , bool fromAtoB , bool is_moved){
    CELL* cell_ptr= NULL;
    vector<int>*nets_ptr = NULL;
    vector<CELL*> *vec_ptr = NULL;
    vector<CELL*> *vec_ptr_new = NULL;
    if(!is_moved){
        if(fromAtoB){//from A to B
            for(auto n_id : movedcell->nets_in_cell){
                nets_ptr = &(nets[net_id_to_index[n_id]]->cells_in_B);
                if((*nets_ptr).size() == 0){
                    for(auto c_id : nets[net_id_to_index[n_id]]->cells_in_A){
                        cell_ptr = cells[cell_name_to_index[c_id]];
                        if(cell_ptr->lock == 0){
                            vec_ptr = &bucketlist[cell_ptr->gain];
                            for(int i = (*vec_ptr).size()-1 ; i >=0 ; i--){                                
                                if( (*vec_ptr)[i] == cell_ptr){
                                    (*vec_ptr).erase( (*vec_ptr).begin() + i);
                                    cell_ptr->gain++;
                                    bucketlist[cell_ptr->gain].push_back(cell_ptr);
                                    break;
                                }
                            }
                            
                        }else{
                            continue;
                        }
                        
                    }
                }else if((*nets_ptr).size() == 1){
                    int c_id = (*nets_ptr).back();
                    cell_ptr = cells[cell_name_to_index[c_id]];
                    if(cell_ptr->lock == 0){
                        vec_ptr = &bucketlist[cell_ptr->gain];
                        for(int i = (*vec_ptr).size()-1 ; i >=0 ; i--){                                
                            if( (*vec_ptr)[i] == cell_ptr){
                                (*vec_ptr).erase( (*vec_ptr).begin() + i);
                                cell_ptr->gain--;
                                bucketlist[cell_ptr->gain].push_back(cell_ptr);
                                break;
                            }
                        }
                                
                    }else{
                        continue;
                    }
                    
                }else{
                    continue;
                }
            }
        }else{ //from B to A
            for(auto n_id : movedcell->nets_in_cell){
                nets_ptr = &(nets[net_id_to_index[n_id]]->cells_in_A);
                if((*nets_ptr).size() == 0){
                    for(auto c_id : nets[net_id_to_index[n_id]]->cells_in_B){
                        cell_ptr = cells[cell_name_to_index[c_id]];
                        if(cell_ptr->lock == 0){
                            vec_ptr = &bucketlist[cell_ptr->gain];
                            for(int i = (*vec_ptr).size()-1 ; i >=0 ; i--){                                
                                if( (*vec_ptr)[i] == cell_ptr){
                                    (*vec_ptr).erase( (*vec_ptr).begin() + i);
                                    cell_ptr->gain++;
                                    bucketlist[cell_ptr->gain].push_back(cell_ptr);
                                    break;
                                }
                            }
                            
                        }else{
                            continue;
                        }
                        
                        
                    }
                }else if((*nets_ptr).size() == 1){
                    int c_id = (*nets_ptr).back();
                    cell_ptr = cells[cell_name_to_index[c_id]];
                    if(cell_ptr->lock == 0){
                        vec_ptr = &bucketlist[cell_ptr->gain];
                        for(int i = (*vec_ptr).size()-1 ; i >=0 ; i--){                                
                            if( (*vec_ptr)[i] == cell_ptr){
                                (*vec_ptr).erase( (*vec_ptr).begin() + i);
                                cell_ptr->gain--;
                                bucketlist[cell_ptr->gain].push_back(cell_ptr);
                                break;
                            }
                        }
                        
                    }else{
                        continue;
                    }
                    
                }else{
                    continue;
                }
            }
        }
    }else{
        if(fromAtoB){
            for(auto n_id : movedcell->nets_in_cell){
                nets_ptr = &(nets[net_id_to_index[n_id]]->cells_in_A);
                if((*nets_ptr).size() == 0){
                    for(auto c_id : nets[net_id_to_index[n_id]]->cells_in_B){
                        cell_ptr = cells[cell_name_to_index[c_id]];
                        if(cell_ptr->lock == 0){
                            vec_ptr = &bucketlist[cell_ptr->gain];
                            for(int i = (*vec_ptr).size()-1 ; i >=0 ; i--){                                
                                if( (*vec_ptr)[i] == cell_ptr){
                                    (*vec_ptr).erase( (*vec_ptr).begin() + i);
                                    cell_ptr->gain--;
                                    bucketlist[cell_ptr->gain].push_back(cell_ptr);
                                    break;
                                }
                            }
                            
                        }else{
                            continue;
                        }
                        
                    }
                }else if((*nets_ptr).size() == 1){
                    int c_id = (*nets_ptr).back();
                    cell_ptr = cells[cell_name_to_index[c_id]];
                    if(cell_ptr->lock == 0){
                        vec_ptr = &bucketlist[cell_ptr->gain];
                        for(int i = (*vec_ptr).size()-1 ; i >=0 ; i--){                                
                            if( (*vec_ptr)[i] == cell_ptr){
                                (*vec_ptr).erase( (*vec_ptr).begin() + i);
                                cell_ptr->gain++;
                                bucketlist[cell_ptr->gain].push_back(cell_ptr);
                                break;
                            }
                        }
                        
                    }else{
                        continue;
                    }
                    
                }else{
                    continue;
                }
            }
        }else{ //from B to A
            for(auto n_id : movedcell->nets_in_cell){
                nets_ptr = &(nets[net_id_to_index[n_id]]->cells_in_B);
                if((*nets_ptr).size() == 0){
                    for(auto c_id : nets[net_id_to_index[n_id]]->cells_in_A){
                        cell_ptr = cells[cell_name_to_index[c_id]];
                        if(cell_ptr->lock == 0){
                            vec_ptr = &bucketlist[cell_ptr->gain];
                            for(int i = (*vec_ptr).size()-1 ; i >=0 ; i--){                                
                                if( (*vec_ptr)[i] == cell_ptr){
                                    (*vec_ptr).erase( (*vec_ptr).begin() + i);
                                    cell_ptr->gain--;
                                    bucketlist[cell_ptr->gain].push_back(cell_ptr);
                                    break;
                                }
                            }

                        }else{
                            continue;
                        }
                        
                    }
                }else if((*nets_ptr).size() == 1){
                    int c_id =  (*nets_ptr).back();
                    cell_ptr = cells[cell_name_to_index[c_id]];
                    if(cell_ptr->lock == 0){
                        vec_ptr = &bucketlist[cell_ptr->gain];
                        for(int i = (*vec_ptr).size()-1 ; i >=0 ; i--){                                
                            if( (*vec_ptr)[i] == cell_ptr){
                                (*vec_ptr).erase( (*vec_ptr).begin() + i);
                                cell_ptr->gain++;
                                bucketlist[cell_ptr->gain].push_back(cell_ptr);
                                break;
                            }
                        }
                           
                    }else{
                        continue;
                    }
                    
                }else{
                    continue;
                }
            }
        }
    }
      
}

CELL* get_max_gain_cell(){
    int found = 0;
    map<int, vector<CELL*>>::reverse_iterator iter;
    for(iter = bucketlist.rbegin(); iter != bucketlist.rend(); iter++){
        for(int i = iter->second.size()-1 ; i >=0  ; i--){
            if(!iter->second[i]->lock){
                if(iter->second[i]->set == 'A'){
                    if(is_balance(current_A_size - iter->second[i]->size , false)){
                        return iter->second[i];
                    }else{
                        continue;
                    }
                }
                else{
                    if(is_balance(current_A_size + iter->second[i]->size , false)){
                        return iter->second[i];
                    }else{
                        continue;
                    }
                }
            }else{
                continue;
            }
        }
    }
    return nullptr;
}

void FM(){
    CELL* movedcell = get_max_gain_cell();
    int greatpartialsum = 0 , currentpartialsum = 0, targetstep = 0 , count = 0;
    int countdown = 10;
    vector<int> *vec_ptr = NULL;
    vector<int> *net_ptr = NULL;
    while(movedcell != nullptr){
        movedcell->lock = 1;
        move_order.push_back(movedcell);
        currentpartialsum = currentpartialsum + movedcell->gain;
        if(movedcell->set == 'A'){
            update_gain(movedcell , true , false);
            movedcell->set = 'B';
            current_A_size -= movedcell->size;
            for(auto n : movedcell->nets_in_cell){
                vec_ptr = &(nets[net_id_to_index[n]]->cells_in_A);
                for(int i = 0 ; i < (*vec_ptr).size() ; i++){
                    if((*vec_ptr)[i] == movedcell->id){
                        (*vec_ptr).erase((*vec_ptr).begin() + i);
                        nets[net_id_to_index[n]]->cells_in_B.push_back(movedcell->id);
                        break;
                    }
                }
            }
            update_gain(movedcell , true , true);
        }else{
            update_gain(movedcell , false , false);
            movedcell->set = 'A';
            current_A_size += movedcell->size;
            for(auto n : movedcell->nets_in_cell){
                vec_ptr = &(nets[net_id_to_index[n]]->cells_in_B);
                for(int i = 0 ; i < (*vec_ptr).size() ; i++){
                    if((*vec_ptr)[i] == movedcell->id){
                        (*vec_ptr).erase((*vec_ptr).begin() + i);
                        nets[net_id_to_index[n]]->cells_in_A.push_back(movedcell->id);
                        break;
                    }
                }
            }
            update_gain(movedcell , false , true);
        }

        if(greatpartialsum <= currentpartialsum){
            greatpartialsum = currentpartialsum;
            targetstep = move_order.size();
            count = 10;
        }else{
            count--;
        }

        if(count == 0){
            break;
        }
        
        movedcell = get_max_gain_cell();
    }

    for(int i = move_order.size()-1 ; i > targetstep-1 ; i--){
        if(move_order[i]->set == 'A'){//move A to B
            move_order[i]->set = 'B';
            for(auto n : move_order[i]->nets_in_cell){
                net_ptr = &(nets[net_id_to_index[n]]->cells_in_A);
                for(int j = 0 ; j < (*net_ptr).size() ; j++){
                    if((*net_ptr)[j] == move_order[i]->id){
                        (*net_ptr).erase((*net_ptr).begin() + j);
                        nets[net_id_to_index[n]]->cells_in_B.push_back(move_order[i]->id);
                        break;
                    }
                }
            }
        }else{//move B to A
            move_order[i]->set = 'A';
            for(auto n : move_order[i]->nets_in_cell){
                net_ptr = &(nets[net_id_to_index[n]]->cells_in_B);
                for(int j = 0 ; j < (*net_ptr).size() ; j++){
                    if((*net_ptr)[j] == move_order[i]->id){
                        (*net_ptr).erase((*net_ptr).begin() + j);
                        nets[net_id_to_index[n]]->cells_in_A.push_back(move_order[i]->id);
                        break;
                    }
                }
            }
        }
    }

    //cout << "partialsum: " << greatpartialsum << endl;
    return ; 
}


void print_bucketlist(){
    map<int, vector<CELL*>>::reverse_iterator iter;
    int size = 0;//how many bucklist
    for(iter = bucketlist.rbegin(); iter != bucketlist.rend();iter++){
        cout << "gain" << iter->first << endl;
        for(int i = 0 ; i < iter->second.size() ; i++){
            cout << iter->second[i]->id << " " ;
        }
        size += iter->second.size();
        cout << endl;
    }
    cout << "num: " << size << endl;
}

void print_ans(ofstream &output_file){
    int A_num = 0 , B_num = 0;
    vector<int> ans_A;
    vector<int> ans_B;
    for(auto c : cells){
        if(c->set ==  'A'){
            ans_A.push_back(c->id);
            A_num ++;
        }else{
            ans_B.push_back(c->id);
            B_num++;
        }
    }
    output_file << "A " << A_num << endl;
    for(auto a : ans_A){
        output_file << "c" << a << endl;
    }
    output_file << "B " << B_num << endl;
    for(auto b : ans_B){
        output_file << "c" << b << endl;
    }

}

void initialize(){
    current_A_size = 0;
    for(auto c : cells){
        c->lock = 0;
        c->gain = 0;
        if(c -> set ==  'A'){
            current_A_size += c->size;
        }
    }
    bucketlist.clear();
    move_order.clear();
}

int main(int argc, char *argv[]) {
    ifstream cell_file(argv[2]);
    ifstream net_file(argv[1]);
    ofstream output_file(argv[3]);

    auto start = high_resolution_clock::now();
    readfile(cell_file, net_file);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "T i/o: "<< duration.count() / pow(10 , 6) << endl;

    initial_set();
    for(int i = 0 ; i < 17 ; i++){
        gain_init();
        set_bucket_list();
        FM();
        initialize();
    }
    gain_init();
    set_bucket_list();
    FM();
    
    //print_cells();
    //print_bucketlist();
    // int cutsize_init = 0;
    // cutsize_init = get_cutsize(nets);
    // cout << "total:" << total_cell_size << endl;
    // cout << "cutsize_init: " << cutsize_init << endl;

    
    int cutsize = 0;
    cutsize = get_cutsize(nets); //output minimum cut
    output_file << "cut_size " << cutsize << endl;
    //auto stop = high_resolution_clock::now();
    //auto duration = duration_cast<microseconds>(stop - start);
    print_ans(output_file);
    
    auto stop_fm = high_resolution_clock::now();
    auto duration_fm = duration_cast<microseconds>(stop_fm - stop);
    cout << "T compute: "<< duration_fm.count() / pow(10 , 6)<< endl;
    // cout << "Time taken by function: "
    //      << duration.count() / pow(10 , 6) << " seconds" << endl;
    
}
