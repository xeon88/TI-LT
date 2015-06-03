#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "random.h"
#include <limits.h>
#include <list>

struct encoded_pkt
{
    unsigned long int value ;
    int * indexes;
    int degree;
    encoded_pkt(unsigned long int p, int *a,int d);
};

typedef encoded_pkt* pkts;


pkts * encoder(int n, int k,unsigned long int * payloads);

/*
    DECODER

*/

typedef struct index_struct
{
    unsigned int value ;
    bool closed;
    int ix;
    std::list<pkts>heap;
    index_struct(unsigned int p, bool cl , int i);
};

typedef struct index_struct * index_state;


void update_pkts(pkts p, int * in_input , index_state * decoder_info);
bool compare_degree(const pkts &a, const pkts &b);
index_state * decoder(int k, int n , pkts * buffer);
