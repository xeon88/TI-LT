#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include "random.h"

using namespace std;

int test_main(int argc, char * argv[])
{
int i,k=100,n=150,d;
double c=0.01,delta=0.01;
char opt;

while( (opt=getopt( argc, argv, "hc:d:n:k:"))>=0) {
	switch (opt) {
		case 'h':
			cout << "Usage:\n ./test -c c -d delta -k k -n n\n" << endl;
			return 0;
		case 'k':
			k=atoi(optarg);
			cout << "Using k=" << k << endl;
			break;
		case 'n':
			n=atoi(optarg);
			cout << "Using n=" << n << endl;
			break;
		case 'c':
			c=atof(optarg);
			cout << "Using c=" << c << endl;
			break;
		case 'd':
			delta=atof(optarg);
			cout << "Using delta=" << delta << endl;
			break;
		default:
			cout << "Warning -" << opt << " flag is undefined" << endl;
			return -1;
	}
}

Random rndgen;

rndgen.init_rsd(k,delta,c);
for(i=0;i<n;i++) {
	d=rndgen.rsd();
	cout << d << endl;
}

return 1;

}
