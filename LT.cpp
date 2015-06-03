#include "LT.h"

void print_usage()
{

    cout << "Codifica LT per la trasmissione di informazioni \n ";
    cout << "Richiamare l'eseguibile con opzioni : \n";
    cout << "test -h help \n";
    cout << "test k  richiama eseguibile con k intero";

}

encoded_pkt::encoded_pkt(unsigned long int p, int * a, int d) :
value(p),indexes(a),degree(d){};

/*
    Simulazione della sorgente avente probabilità di emissione
    uniforme. La sorgente forma k value come interi senza segno
*/


unsigned long int * source (int k)
{

    unsigned long int * values= new unsigned long int [k];
    for(int i=0;i<k;i++)
    {
        values[i]=rand()%LONG_MAX;
    }

    return values;
}


/*
    ENCODER
*/

pkts * encoder(int n, int k,unsigned long int * values)
{
    pkts encoded;
    int degree;
    double c,delta;
    int * indexes;
    bool * used;
    c=0.05,                             // Paramentri per la distribuzione RSD
    delta=0.05;
    Random rgen;
    pkts * to_send = new pkts[n];

    rgen.init_rsd(k,delta,c);           // inizializza RSD generator

    for(int i=0;i<n;i++)
    {
        degree=rgen.rsd();                  // scelta del grado
        indexes=new int[degree];            // vettore di indici
        unsigned long int p=0;
        int j=0;
        int rd;
        used=new bool[k];                   // vettore di controllo se il pacchetto è stato
        for(int i=0;i<k;i++)
        {
            used[i]=false;
        }
        while(j<degree)                     // già utilizzato in precendeza per eseguire lo XOR
        {                                   // x XOR x = 0
            rd=rand()%k;
            if(!used[rd])
            {
                used[rd]=true;
                indexes[j]=rd;
                p=p^values[rd];
                j++;
            }

        }
        encoded = new encoded_pkt(p,indexes,degree);
        to_send[i]= encoded;
        free(used);
    }

    return to_send;
}


/*
    DECODER
*/

index_struct::index_struct(unsigned int p, bool cl , int i):
value(p), closed(cl),ix(i) {}

/*
  aggiorna la struttura all'unterno del pkts , in base allo stato di decodifica dell'indice dk
*/

void update_pkts(pkts p, int * in_index, int dim_index , index_state * decodeder_info)
{

    int new_degree=p->degree;

    for(int i=0;i<dim_index;i++)
    {
        if(decodeder_info[in_index[i]]->closed ) // se posso risolverlo per l'indice i
        {

            // eseguo lo xor
            p->value = p->value ^ decodeder_info[in_index[i]]->value;

            // riaggiorno la struttura di indici del check node

            int j=0;

            while(j<p->degree)
            {
                if(p->indexes[j]==in_index[i])
                {
                    p->indexes[j]=-1;
                }
                j++;
            }

            new_degree--;
        }
    }

    if(new_degree>0)
    {
        int * new_indexes= new int[new_degree];
        int k=0;
        int z=0;
        while(k<p->degree)
        {
            if(p->indexes[k]!=-1)
            {
                new_indexes[z]=p->indexes[k];
                z++;
            }
            k++;
        }
        p->indexes=new_indexes;

    }

    p->degree=new_degree;
}


bool compare_degree(const pkts &a, const pkts &b)
{

    return a->degree < b->degree;

}

void print_pkts(pkts p)
{
    cout << "value  : " << p->value << "   : ";
    cout << "Degree : " << p->degree <<"   : ";
    cout << "Array  : " ;
    int s=0;
    while(s<p->degree)
    {
        cout << "[" << p->indexes[s] << "]";
        s++;
    }
    cout<<"\n";
}

void print_heap(list<pkts> heap)
{
        cout << "===============HEAP=============== \n";

        list<pkts>::iterator it2=heap.begin();
        cout << "Num elementi : " << heap.size() << "\n";
        cout << "\n";
        while(it2!=heap.end())
        {
            print_pkts(*it2);
            it2++;
        }

        cout << "\n==============HEAP===============\n";
        cout << "\n";

}

index_state* decoder(int k, int n , pkts * buffer)
{
    int r=0; // numero di risolti
    int j=0; // numero pacchetti processati

    index_state * decoder_info= new index_state[k];

    for(int i=0 ; i<k;i++)
    {
        decoder_info[i]=new index_struct(0,false,i);

    }

    list<pkts> heap ;

    while( j<n && r<k)  // ci sono ancora value da decodificare
    {                   // o non ci sono più pacchetti da incodare

        // processa il pacchetto


        int in_deg = buffer[j]->degree;
        int * ind = buffer[j]->indexes;


        update_pkts(buffer[j],ind,in_deg,decoder_info);

        if(buffer[j]->degree==1) // pkts di grado 1
        {


            heap.push_front(buffer[j]);

            do
            {
                // setta il valore di grado 1 del pkts

                pkts first=heap.front();
                int resolved=first->indexes[0];

                if(!decoder_info[resolved]->closed)
                {
                    //cout << "Pacchetto di grado 1  risolvo index : " << resolved << "\n";

                    decoder_info[resolved]->value=first->value;
                    decoder_info[resolved]->ix=resolved;
                    decoder_info[resolved]->closed=true;
                    r++;

                    list<pkts>::iterator  it = decoder_info[resolved]->heap.begin();

                    if(!decoder_info[resolved]->heap.empty())
                    {
                        while(!decoder_info[resolved]->heap.empty() && it!=decoder_info[resolved]->heap.end())
                        {
                            update_pkts(*it,&resolved,1,decoder_info);
                            it++;
                        }
                    }

                }

                // per ogni pkts sulla coda dell' indice risolto risolvo il pkts
                // in base al nuovo valore

                // riordino tutti i pkts in base al grado

                //rimuovo il pacchetto di grado 1 risolto
                heap.pop_front();

                // riordino i pacchetti
                heap.sort(&compare_degree);


                list<pkts>::iterator it3=heap.begin();

                // verifico la presenza di pacchetti inutili ovvero quelli di grado 0

                if(!heap.empty() && (*it3)->degree==0 )
                {


                    while(!heap.empty() && (heap.front())->degree==0)
                    {
                        heap.pop_front();
                    }

                }



            }
            while(!heap.empty() && (heap.front())->degree==1); // continuo fino a quando non risolvo altri indici
        }

        if(buffer[j]->degree>=2)    // pkts di grado >=2
        {
            // incodo il pacchetto nell'heap
            heap.push_back(buffer[j]);

            // incodo il pacchetto alle liste di indici a cui partecipa
            int deg= buffer[j]->degree;
            for(int i=0;i<deg;i++)
            {
                decoder_info[buffer[j]->indexes[i]]->heap.push_back(buffer[j]);
            }
        }

        j++;

    }

    return decoder_info;
}





int main (int argc , char ** argv)
{

    int k;

    if(argc>2 || argc<2)
    {
        print_usage();
        return 0;
    }
    char *end;
    long val = strtol(argv[1], &end, 10);

    if (!end[0] && val >= 0)
    {
        k=val;
    }

    cout << "====TEST ENCODER ===== \n";

    int n=k;
    int test=0 ;
    int max_test=100;


    for(int i=0;i<=10;i++)
    {
        int full_decoded=0;

        for(int test=0;test<max_test;test++)
        {

            //cout << "TEST NUMERO : " << test << "\n";

            unsigned long int * pload=source(k);

            n=k+(0.1)*i*k;
            pkts * to_send=encoder(n,k,pload);

            /*
            cout <<"Trasmissioni \n";

            for(int i=0;i<k;i++)
            {

                cout << "Pacchetto :"<< i << "valore : " << pload[i] << " \n";
            }


            //cout << "\n==========DECODIFICA===========\n\n";

            */

            index_state * result=decoder(k,n,to_send);

            /*

            cout << "\n\nDecodifica pacchetti : \n";
            for(int i =0 ;i<k;i++)
            {
                cout << "Pacchetto : " << i << " valore : " << result[i]->value <<  "\n";
            }

            */
            int success=0;

            for(int i=0;i<k;i++)
            {
                if(result[i]->value!=0)
                {
                    success++;
                }

            }

            if(success==k)
            {
                full_decoded++;
            }

        }

        float full_test=(float)full_decoded/(float)max_test;
        cout << "Test : " << i << " : n = " << n << " Successo : "  << full_test << "\n";

    }

}
