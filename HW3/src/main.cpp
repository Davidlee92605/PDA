#include<bits/stdc++.h>
#include "parse.h"
using namespace std;
using namespace chrono;
//////Global/////////////
vector<BLOCK*> Blocks;
vector<TERMINAL*> Terminals;
vector<NET*> Nets;
int total_blocks;
int total_terminals;
int total_nets;
int total_pins;
map<int , int> terminal_id_to_index;
map<int , int> block_id_to_index;
vector<BLOCK*> sorted_list;
vector<BLOCK*> sorted_list_nd;
NODE* tree_head;
vector <NODE*> node_in_tree;
int outline;
double space_ratio;
int blockarea = 0;
Contour_Line* Contour_line;
///// print.cpp//////////
void printBlocks();
void printTermianls();
void printNets();
////Global////////////////
void pre_order(NODE* cur){ //pre_order
    if(cur == nullptr){
        return;
    }
    cout <<  cur->block->getId() << endl;
    pre_order(cur->right);
    cout << " ============== " << endl;
    pre_order(cur->left);
}

int count(NODE *root){ //count node
    if(root == NULL){
        return 0;
    }
    else{
        return 1 + count(root->left) + count(root->right);
    }
}

void CONTOUR(NODE* node, int x_cor){
    int max_y = 0;
    Contour_Line* pre = nullptr; 
    Contour_Line* cur = Contour_line;
    while(cur != nullptr){
        if(cur->from < x_cor + node->block->getWidth() && x_cor + node->block->getWidth() < cur->end){ //if node right in other node x1 -> x2
            if(cur->y > max_y){
                max_y = cur->y;
            }
            Contour_Line* temp = new Contour_Line();
            temp->from = x_cor + node->block->getWidth(); //no update line (cur - node)
            temp->end = cur->end;
            temp->y = cur->y;
            temp->next = cur->next;
			pre->next = temp;
			Contour_Line* del = cur;
			delete del; //del cur
			cur = temp; 
        }
        if (cur->from < x_cor && x_cor < cur->end) {//if node left within cur x1 -> x2
			if(cur->y > max_y){
                max_y = cur->y;
            }
			Contour_Line* temp = new Contour_Line();
			temp->from = cur->from; //no update line (node - cur)
            temp->end = x_cor;
            temp->y = cur->y;
            temp->next = cur->next;
			if (pre != nullptr){
				pre->next = temp;
            }
			else{
				Contour_line = temp;
            }
			Contour_Line* del = cur;
			delete del; //del cur
			cur = temp;
		}
        if (x_cor <= cur->from && cur->from <= x_cor + node->block->getWidth() &&
			x_cor <= cur->end && cur->end <= x_cor + node->block->getWidth()) { //  
			if(cur->y > max_y){
                max_y = cur->y;
            }
			Contour_Line* del = cur;
			if (pre != nullptr){
				pre->next = cur->next;
            }
			cur = pre;
			delete del;
		}
        if (x_cor + node->block->getWidth() <= cur->from){
			break;
        }
        pre = cur;
		cur = cur->next;
    }
    Contour_Line* temp = new Contour_Line(); //add new node contour
	temp->y = max_y + node->block->getHeight();
	temp->from = x_cor;
	temp->end = x_cor + node->block->getWidth();
    node->block->setLeft_down(temp->from, temp->y - node->block->getHeight());
    //node->block->left_down.first = temp->from;
    //node->block->left_down.second = temp->y - node->block->height;

    cur = Contour_line;
	while (cur != nullptr) { //將他串在一起
		if (cur->end == temp->from) {
			temp->next = cur->next;
			cur->next = temp;
		}
		pre = cur;
		cur = cur->next;
	}

}

void DFS(NODE* node){//construct contour and retraverse tree 
    if(node->parent == nullptr){ //record the left down block
        Contour_line = new Contour_Line();
        Contour_line->from = -1; //0 will segamentation fault
        Contour_line->end = 0;
        Contour_line->y = 0;
        Contour_Line* root = new Contour_Line();
        root->from = 0;
        root->end = node->block->getWidth();
        root->y = node->block->getHeight();
        Contour_line->next = root;
        /// set coordinate
        node->block->setLeft_down(0, 0);
        // node->block->left_down.first = 0;
        // node->block->left_down.second = 0;
    }
    if(node->left != nullptr){ //update contour 
        CONTOUR(node->left, (node->block->getWidth() + node->block->getLeft_down_x()));
        DFS(node->left);
    }
    if(node->right != nullptr){ //update contour 
        CONTOUR(node->right, (node->block->getLeft_down_x()));
        DFS(node->right);
    }
}

bool sort_module(BLOCK* a, BLOCK* b){ //sort width
    if(a->getHeight() != b->getHeight()){
        return a->getHeight() > b->getHeight();
    }
    return a->getWidth() > b->getWidth();
}

void initial_tree(){ //construct tree
    bool* block_move = new bool[Blocks.size()]();
    for(auto n : Nets){
        for(auto i : n->blocks){
            if(block_move[block_id_to_index[i->getId()]] == false){
                if(i->getWidth() > i ->getHeight()){ 
                    i->setRotated(!i->getRotated());
                }
                sorted_list.push_back(i);
                block_move[block_id_to_index[i->getId()]] = true;
            }
            else{
                continue;
            }
        }
    }
    // for(auto i : Blocks){
    //     if(i->getHeight() > i -> getWidth()){ 
    //         i->setRotated(!i->getRotated());
    //     }
    //     sorted_list.push_back(i);
    // }
    sort(sorted_list.begin(), sorted_list.end(), sort_module);
    // for(auto i : sorted_list){
    //     cout << "Blocks id: " << i->id << endl;
    //     cout << "Blocks area: " << i->area << endl;
    //     cout << "Rotate: " << i->rotated << endl;
    // }
    //// creat intitial ////
    tree_head = new NODE();
    NODE* root = tree_head;
    bool* move = new bool[sorted_list.size()]();
    
    int now_h = 0; // same column height
    int now_w = 0; // same column width
    int flag = 0;
    while(1){
        flag = 1;
        NODE* temp = root;
        ///  build one column fits height ///
        for(int i = 0 ; i < sorted_list.size() ; i++){ // column base block
            if(move[i] == false){
                move[i] = true;
                temp->block = sorted_list[i];
                now_h += sorted_list[i]->getHeight();
                now_w = sorted_list[i]->getWidth();
                break;
            }
        }
        for(int i = 0 ; i < sorted_list.size() ; i++){
            if(move[i] == false){
                if(sorted_list[i]->getWidth() + now_w <= outline && sorted_list[i]->getHeight()  <= now_h){ //fits Height 
                    move[i] = true;
                    now_w += sorted_list[i]->getWidth();
                    temp->left = new NODE();
                    temp->left->block = sorted_list[i];
                    temp->left->parent = temp;
                    temp = temp->left;
                }
            }
        }
        /// check stop ///
        for(int i = 0 ; i < sorted_list.size() ; i++){
            if(move[i] == false){
                flag = 0;
                break; // means not all block inserted
            }
        }
        if(flag == 1){
            break;
        }
        /// next loop initialize ////
        now_h = 0;
        root->right = new NODE();
        root->right->parent = root;
        root = root->right;
    }
    
    //pre_order(tree_head);
    // int c;
    // c = count(tree_head);
    // cout << c << endl;

    //////////////construct contour ////////////// 
    DFS(tree_head);   
}

int HPWL(){
    int wirelength = 0;
    for(auto n : Nets){
        int left = INT_MAX, right = 0, high = 0, low = INT_MAX;
        int length = 0;
        for(auto p : n->pins){
            if(p == nullptr) break;
            if(p->x < left){
                left = p->x;
            }
            if(p->x > right){
                right = p->x;
            }
            if(p->y < low){
                low = p->y;
            }
            if(p->y > high){
                high = p->y;
            }
        }
        for(auto b : n->blocks){
            // cout << "b id : " << b->getId() << endl;
            // cout << "b x : " << b->getLeft_down_x() << endl;
            // cout << "b y : " << b->getLeft_down_y() << endl;
            
            if(b == nullptr) break;
            int x, y;
            x = b->getLeft_down_x() + (0.5) * b->getWidth();
            y = b->getLeft_down_y() + (0.5) * b->getHeight();
            if(x < left){
                left = x;
            }
            if(x > right){
                right = x;
            }
            if(y < low){
                low = y;
            }
            if(y > high){
                high = y;
            }
        }
        length = (right - left) + (high - low);
        // cout << "right: " << right << endl;
        // cout << "length : " << length << endl;
        wirelength += length;
    }
    return wirelength;
}

pair<int, int> cal_boundary(){
    pair<int, int> ans;
    int cur_width = 0;
    int cur_height = 0;
    Contour_Line* temp = Contour_line->next;
    while(temp != nullptr){
        if(temp->end > cur_width){
            cur_width = temp->end;
        }
        if(temp->y > cur_height){
            cur_height = temp->y;
        }
        temp = temp->next;
    }
    ans.first = cur_width;
    ans.second = cur_height;
    return ans;
}

int Area(){
    int cur_width = 0;
    int cur_height = 0;
    Contour_Line* temp = Contour_line->next;
    while(temp != nullptr){
        if(temp->end > cur_width){
            cur_width = temp->end;
        }
        if(temp->y > cur_height){
            cur_height = temp->y;
        }
        temp = temp->next;
    }
    //cout << "cur_width: " << cur_width << endl;
    return cur_width*cur_height;
}

void update_tree(NODE* root){ //convert tree with vector type
    if(root == nullptr) return;
    node_in_tree.push_back(root);
    update_tree(root->right);
    update_tree(root->left);
}

NODE* cpy_tree(NODE* parent, NODE* self){ //(nullptr, tree_head)
    NODE* n = new NODE();
    n->block = self->block;
    n->parent = parent;
    if(self->left) n->left = cpy_tree(n, self->left);
    if(self->right) n->right = cpy_tree(n, self->right);
    return n;
}

////////////////////// operations //////////////////////
void rotate(){
    int r, temp;
    r = rand() % total_blocks;
    node_in_tree[r]->block->setRotated(!(node_in_tree[r]->block->getRotated()));
}

void move(){ //choose leaf to move
    NODE* from, *to;
    int f, t;
    int flag = 0;
    while(1){ 
        f = rand() % total_blocks;
        t = rand() % total_blocks;
        if(f != t ){
            break;
        }
    }
    from = node_in_tree[f];
    to = node_in_tree[t];
    if(from->left == nullptr && from->right == nullptr){
        
        if(from->parent->left == from){
            from->parent->left = nullptr;
            from->left = to->left;
            if(to->left != nullptr){
                to->left->parent = from;
            }
            to->left = from;
        }
        if(from->parent->right == from){
            from->parent->right = nullptr;
             from->right = to->right;
            if(to->right != nullptr){
                to->right->parent = from;
            }
            to->right = from;
        }
        from->parent = to; //connect from and to
        // int side = rand() % 2; //
        // if(side == 0){ //insert at to left
        //     from->left = to->left;
        //     if(to->left != nullptr){
        //         to->left->parent = from;
        //     }
        //     to->left = from;
        // }else if(side == 1){
        //     from->right = to->right;
        //     if(to->right != nullptr){
        //         to->right->parent = from;
        //     }
        //     to->right = from;
        // }

    }
    
    else if (from->left != nullptr || from->right != nullptr){
        NODE* cur = from;
        
        //cout << "1: "<< cur->left << " " << cur->right << endl;
        while(cur->left != nullptr || cur->right != nullptr){
            bool move_left;
            if(cur->left != nullptr && cur->right != nullptr){
                move_left = rand() % 2 == 0;
                //cout << "1: "<< cur->left << " " << cur->right << endl;
            }
            else if(cur->left != nullptr){
                move_left = true;
                //cout << "1: "<< cur->left << " " << cur->right << endl;
            }
            else{
                move_left = false;
            }
            if(move_left){
                cur = cur->left;
            }
            else{
                cur = cur->right;
            }
        }
        if(cur == to){
            return;
        }
        //cout << "stoppp" << endl;
        
        if(cur->parent->left == cur){ //del cur
            cur->parent->left = nullptr;
            cur->left = to->left;
            if(to->left != nullptr){
                to->left->parent = cur;
            }
            to->left = cur;
        }
        if(cur->parent->right == cur){
            cur->parent->right = nullptr;
             cur->right = to->right;
            if(to->right != nullptr){
                to->right->parent = cur;
            }
            to->right = cur;
        }
        
        BLOCK* temp = from->block;
        from->block = cur->block;
        //cout << "temp: " << temp->getId() << " from: " << from->block->getId()<< endl;
        cur->block = temp;
       // cout << "cur: " << cur->block->getId() << endl;
        cur->parent = to; //connect from and to
        //cout << "from: " << from->block->getId() << "cur: " << cur->block->getId() << endl;
        // int side = rand() % 2; //
        // if(side == 0){ //insert at to left
        //     cur->left = to->left;
        //     if(to->left != nullptr){
        //         to->left->parent = cur;
        //     }
        //     to->left = cur;
        // }else if(side == 1){
        //     cur->right = to->right;
        //     if(to->right != nullptr){
        //         to->right->parent = cur;
        //     }
        //     to->right = cur;
        // }
        
    }

    
}

void swap(){
    NODE* from, *to;
    int f, t;
    while(1){
        f = rand() % total_blocks;
        t = rand() % total_blocks;
        if(f != t){
            break;
        }
    }
    from = node_in_tree[f];
    to = node_in_tree[t];
    BLOCK* temp = from->block;
    from->block = to->block;
    to->block = temp;
}

/////////////////cost compute/////////////////////////

double COST(double area, double wirelength, pair<int, int> cur){
    double a = 1000, beta = 1;
    double widthCost = std::max((cur.first - outline) , 0);
    double heightCost = std::max((cur.second - outline) , 0);
    return a * ( widthCost + heightCost ) + beta * (wirelength);
}

void kill_tree(NODE* node){
    if(node == nullptr) return;
    kill_tree(node->left);
    kill_tree(node->right);
    delete node;
}

void SA(){
    int seed;
    if(Blocks.size() <= 100){
        seed = 74147;
    }else if(Blocks.size() > 100 && Blocks.size() <= 200){
        seed = 503890; //503890 
    }else{
        seed = 6190; //6190
    }
    //cout << "Seed: " << seed << endl;
    srand(seed);
    

    NODE *old_tree_head = cpy_tree(nullptr, tree_head);

    vector<bool> init_rotate; //store the init rotate 
    vector<bool> Best_rotate;
    for (auto node : node_in_tree) {
        init_rotate.push_back(node->block->getRotated());
        Best_rotate.push_back(node->block->getRotated());
    }

    /////// initial temperature ///////////////
    double avg_A, avg_W, avg_U, avg_Cost;
    avg_Cost = 0;
    double T_init, T;
    double p = 0.9; //init probability
    T_init = -(1e-5 / log(p));

    double Eps = 1e-5;
    int  k = 8;
    double cost = COST(Area(), HPWL(), cal_boundary());
    //cout << "cost: " << cost << endl;
    double Best_cost = cost;
    double Best_wirelength = HPWL();
    //cout << "Best cost: " << Best_cost << endl;
    double new_cost = 0;
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    T = T_init;
    int r = 1;
    NODE* Best_tree_head = cpy_tree(nullptr, tree_head);
     //store the best rotate 
    //////// SA //////////////////
    int MT, uphill, reject;
    int N = Blocks.size() * k;
    double R = 0.99;
    //cout << "T: "<< T << endl;
    while(!(duration.count() > 1000 || T < Eps)){
        //cout << "r: " << r << endl;
        MT = 0;
        uphill = 0;
        reject = 0;
        //avg_Cost = 0; //
        while(!(uphill > N || MT > 2*N)){
            double Diff = 0;
            NODE* old_tree_head = cpy_tree(nullptr, tree_head);
            init_rotate.clear();
            for (auto node : node_in_tree) {
                init_rotate.push_back(node->block->getRotated());
            }
            int op = rand() % 6 + 1;
            if(op == 1){
                rotate();
            }else if(op == 2 || op == 3 || op == 4){
                move();
            }else if(op == 5 || op == 6){
                swap();
            }
            //cout << "op: " << op << endl;
            node_in_tree.clear();
            update_tree(tree_head);
            DFS(tree_head); //update contour
            new_cost = COST(Area(), HPWL(), cal_boundary());
            //cout << "new cost: " << new_cost << endl;
            Diff = new_cost - cost;
            if(Diff <= 0){ // Good, accept it
                cost = new_cost;
                pair<int, int> cur = cal_boundary();
                if(HPWL() <= Best_wirelength && cur.first <= outline && cur.second <= outline ){ //
                    //cout << "best accept " <<endl;
                    //cout << "HPWL: " << HPWL() << endl;
                    //pre_order(tree_head);
                    Best_wirelength = HPWL();
                    kill_tree(Best_tree_head );
                    Best_tree_head = cpy_tree(nullptr, tree_head);
                    Best_rotate.clear();
                    node_in_tree.clear();
                    update_tree(tree_head);
                    for (auto node : node_in_tree) {
                        Best_rotate.push_back(node->block->getRotated());
                    }
                }
            }
            if(Diff > 0){ //Bad, prob accept it
                double probability = (double)rand() / (RAND_MAX + 1.0);
                if(probability > exp(-Diff / T)){ // no change
                    ++reject;
                    kill_tree(tree_head);
                    tree_head = cpy_tree(nullptr, old_tree_head);
                    node_in_tree.clear();
                    update_tree(tree_head);
                    for(int i = 0 ; i < node_in_tree.size() ; i++){
                        node_in_tree[i]->block->setRotated(init_rotate[i]);
                    }
                    DFS(tree_head);
                }else{
                    //cout <<"accept" << endl;
                    cost = new_cost;
                    ++uphill;
                }
            }
            MT++;
            kill_tree(old_tree_head);
        }
        T = R * T;
        //cout << "T : " << T << endl;
        r++;
        stop = high_resolution_clock::now();
        duration = duration_cast<seconds>(stop - start);
    }
    
    tree_head = cpy_tree(nullptr, Best_tree_head);
    node_in_tree.clear();
    update_tree(tree_head);
    for(int i = 0 ; i < node_in_tree.size() ; i++){
        node_in_tree[i]->block->setRotated(Best_rotate[i]);
    }
    DFS(tree_head);

    return;
}

void print_output(ofstream &out_file, int wirelength){
    out_file << "Wirelength " << wirelength << endl;
    out_file << "Blocks" << endl;
    for(auto b : Blocks){
        out_file << "sb" << b->getId() << " " << b->getLeft_down_x() << " " << b->getLeft_down_y() << " " << b->getRotated() << endl;
    }
}

int main(int argc, char *argv[]){
    //////////////read file///////////////////////
    ofstream out_file(argv[4]);

    auto start = high_resolution_clock::now();
    Parse* p = new Parse(argv[1], argv[2], argv[3], argv[5]); //read file
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "total_blocks: " << total_blocks << endl;
    cout << "total_terminals: " << total_terminals << endl;

    cout << "T I/O : " << duration.count() / pow(10, 6) << endl;
    ///////////////////////////////////////////////
    ///////////// build initial tree //////////////
    start = high_resolution_clock::now();
    initial_tree();
    update_tree(tree_head);
    stop = high_resolution_clock::now();
    auto duration_3 = duration_cast<microseconds>(stop - start);
    cout << "T Iniitial floorplan : " << duration_3.count() / pow(10, 6) << endl;
    ///////////////////////////////////////////////
    // Contour_Line* cur = Contour_line;
	// while(cur != nullptr) {
	// 	cout << "cur x1: " << cur->from << endl;
	// 	cout << "cur x2: " << cur->end << endl;
	// 	cout << "cur Y: " << cur->y << endl;
	// 	cur = cur->next;
	// }
    //cout << "outline: " << outline << endl;
    ////////// compute wirelength //////////
    //cout << "wirelength: " << HPWL() << endl;
    //cout << "area: " << Area() << endl;
    // pair<int, int> cur = cal_boundary();
    // cout << "X: " << cur.first << "Y: " << cur.second << endl;
    //pre_order(tree_head);
    ///////////// construct node in tree //////////////
    start = high_resolution_clock::now();
    SA();
    stop = high_resolution_clock::now();
    auto duration_2 = duration_cast<microseconds>(stop - start);
    cout << "T algo: " << duration_2.count() / pow(10, 6) << endl;
    ///////////////////////////////////////////////
    cout << "Area: " << Area() << endl;
    cout << "wirelength: " << HPWL() << endl;
    ////////// output file /////////////////
    int wirelength = HPWL();
    cout << wirelength;
    print_output(out_file, wirelength);
    ////////////////////////////////////////

}