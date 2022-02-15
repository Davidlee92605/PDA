#ifndef PARSE_H
#define PARSE_H
#include<bits/stdc++.h>
#include"Object.h"
using namespace std;

class Parse{
    public:
        Parse(const char * , const char *, const char *, const char *);
        ~Parse();
        void parse();
        void get_outline();
    private:
        const char* hardblocks_filename;
        const char* nets_filename;
        const char* pl_filename;
        const char* ratio_filename;
};

#endif