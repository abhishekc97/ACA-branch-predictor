#include <bits/stdc++.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <sstream>
#include <fstream>
using namespace std;

int get_index_value_back(string PC_hexval, int m)
{
	unsigned tobinary;
	stringstream ss;
	ss << hex << PC_hexval;
	ss >> tobinary;
	bitset<32> bits(tobinary);
	string hexinbinary = bits.to_string();
	hexinbinary = hexinbinary.substr(32-2-m,m);
	return stoi(hexinbinary, nullptr, 2);

}

void print_bimodal_gshare(int mispredictions, int predictions, int tablesize, int table[], int x)
{
	float miss_rate = (float)((float)(100*mispredictions)/(float)(predictions));
	cout << "OUTPUT" << endl;
	cout << "number of predictions:			" << predictions << endl;
	cout << "number of mispredictions:		" << mispredictions << endl;
	cout << "misprediction rate: 			" <<  fixed<< setprecision(2)<<miss_rate << "%" << endl;
	// int x = x;
	if (x == 1)
		cout << "FINAL BIMODAL CONTENTS" << endl;
	else
		cout << "FINAL GSHARE CONTENTS" << endl;
	for(int i=0; i<tablesize; i++)
	{
		cout << i << '\t' << table[i] << endl;
	}
}
void print_hybrid(int mispredictions, int predictions, int hybridtablesize, int hybridtable[], int gsharetablesize, int gsharetable[], int bimodaltablesize, int bimodaltable[])
{
	float miss_rate = (float)((float)(100*mispredictions)/(float)(predictions));
	cout << "OUTPUT" << endl;
	cout << "number of predictions:			" << predictions << endl;
	cout << "number of mispredictions:		" << mispredictions << endl;
	cout << "misprediction rate: 			" <<  fixed<< setprecision(2)<<miss_rate << "%" << endl;
	cout << "FINAL CHOOSER CONTENTS" << endl;
	for(int i=0; i<hybridtablesize; i++)
	{
		cout << i << '\t' << hybridtable[i] << endl;
	}
	cout << "FINAL GSHARE CONTENTS" << endl;
	for(int i=0; i<gsharetablesize; i++)
	{
		cout << i << '\t' << gsharetable[i] << endl;
	}
	cout << "FINAL BIMODAL CONTENTS" << endl;
	for(int i=0; i<bimodaltablesize; i++)
	{
		cout << i << '\t' << bimodaltable[i] << endl;
	}
}

void bimodal(int a, string tracefile)
{
	int tablesize;
	int *table;
	int predictions;
	int mispredictions;
	string PC_hexval;
	string branchstatus;
	int m2 = a;
	predictions = mispredictions = 0;
	int x = 1;
	tablesize = pow(2, m2);
	table = new int[tablesize];

	for(int i=0;i<tablesize;i++)
	{
		table[i] = 4;
	}

	
	ifstream object;
	object.open(tracefile);
	while(object>>PC_hexval>>branchstatus)
	{
		predictions+=1;
		int prediction;
		int actual;
		int index;
		index = get_index_value_back(PC_hexval, m2);

		if (table[index] >= 4)
		prediction = 1;
		else if (table[index] < 4)
			prediction = 0;

		if ( branchstatus == "n")
			actual = 0;
		else if ( branchstatus == "t")
			actual = 1;

		if ( prediction != actual)
		{
			mispredictions+=1;
		}

		if ( actual == 1 )
		{
			table[index] = table[index] + 1;
			if (table[index] > 7)
				table[index] = 7;
		}

		else if( actual == 0 )
		{
			table[index] = table[index] - 1;
			if (table[index] < 0)
				table[index] = 0;
		}
		
	}
	object.close();
	print_bimodal_gshare(mispredictions, predictions, tablesize, table, x);
	
}

void gshare(int a, int b, string tracefile)
{
	int tablesize;
	int *table;
	int predictions;
	int mispredictions;
	string PC_hexval;
	string branchstatus;
	int m1 = a;
	int n = b;
	predictions = mispredictions = 0;
	int temp;
	int x = 2;
	tablesize = pow(2, m1);
	table = new int[tablesize];

	for(int i=0;i<tablesize;i++)
	{
		table[i] = 4;
	}
	bitset<32> bhr;
	
	ifstream object;
	object.open(tracefile);
	while(object>>PC_hexval>>branchstatus)
	{
		predictions+=1;
		int prediction;
		int actual;
		int index;
		index = get_index_value_back(PC_hexval, m1);
		index = index ^ (int)(bhr.to_ulong());
		if (table[index] >= 4)
		prediction = 1;
		else if (table[index] < 4)
			prediction = 0;

		if ( branchstatus == "n")
			actual = 0;
		else if ( branchstatus == "t")
			actual = 1;

		if ( prediction != actual)
		{
			mispredictions+=1;
		}

		if ( actual == 1 )
		{
			table[index] = table[index] + 1;
			if (table[index] > 7)
				table[index] = 7;
			temp = (int)(bhr.to_ulong());
			temp>>=1;
			bitset<32> tempbhr(temp);
			bhr = tempbhr;
			bhr[n-1] = 1;
		}

		else if( actual == 0 )
		{
			table[index] = table[index] - 1;
			if (table[index] < 0)
				table[index] = 0;
			temp = (int)(bhr.to_ulong());
			temp>>=1;
			bitset<32> tempbhr(temp);
			bhr = tempbhr;
			bhr[n-1] = 0;
		}
		
	}
	object.close();
	print_bimodal_gshare(mispredictions, predictions, tablesize, table, x);
	
}

void hybrid(int a, int b, int c, int d, string tracefile)
{
	// int tablesize;
	int hybridtablesize;
	int gsharetablesize;
	int bimodaltablesize;
	
	int *hybridtable;
	int *gsharetable;
	int *bimodaltable;

	int predictions;
	int mispredictions;

	string PC_hexval;
	string branchstatus;

	int k = a;
	int m1 = b;
	int n = c;
	int m2 = d;

	predictions = mispredictions = 0;
	int temp;

	//HYBRID
	hybridtablesize = pow(2, k);
	hybridtable = new int[hybridtablesize];

	for(int i=0; i<hybridtablesize; i++)
	{
		hybridtable[i] = 1;
	}

	//GSHARE
	gsharetablesize = pow(2, m1);
	gsharetable = new int[gsharetablesize];

	for(int i=0; i<gsharetablesize; i++)
	{
		gsharetable[i] = 4;
	}

	//BIMDOAL
	bimodaltablesize = pow(2, m2);
	bimodaltable = new int[bimodaltablesize];

	for(int i=0; i<bimodaltablesize; i++)
	{
		bimodaltable[i] = 4;
	}

	bitset<32> bhr;

	ifstream object;
	object.open(tracefile);
	while(object>>PC_hexval>>branchstatus)
	{
		predictions += 1;
		int gshareprediction;
		int bimodalprediction;
		int hybridprediction;

		int actual;

		int gshareindex;
		int bimodalindex;
		int hybridindex;

		if ( branchstatus == "n")
			actual = 0;
		else if ( branchstatus == "t")
			actual = 1;


		//GSHARE
		gshareindex = get_index_value_back(PC_hexval, m1);
		gshareindex = gshareindex ^ (int)(bhr.to_ulong());
		if (gsharetable[gshareindex] >= 4)
			gshareprediction = 1;
		else if (gsharetable[gshareindex] < 4)
			gshareprediction = 0;
		
		//BIMODAL
		bimodalindex = get_index_value_back(PC_hexval, m2);
		// bimodalindex = bimodalindex ^ (int)(bhr.to_ulong());
		if (bimodaltable[bimodalindex] >= 4)
			bimodalprediction = 1;
		else if (bimodaltable[bimodalindex] < 4)
			bimodalprediction = 0;

		//HYBRID
		hybridindex = get_index_value_back(PC_hexval, k);
		// hybridindex = hybridindex ^ (int)(bhr.to_ulong());

		if (hybridtable[hybridindex] >= 2)
		{
			hybridprediction = gshareprediction;
			if ( actual == 1 )
			{
				gsharetable[gshareindex] = gsharetable[gshareindex] + 1;
				if (gsharetable[gshareindex] > 7)
					gsharetable[gshareindex] = 7;
			}
			else if( actual == 0 )
			{
				gsharetable[gshareindex] = gsharetable[gshareindex] - 1;
				if (gsharetable[gshareindex] < 0)
					gsharetable[gshareindex] = 0;
			}
		}
		else
		{
			hybridprediction = bimodalprediction;
			if ( actual == 1 )
			{
				bimodaltable[bimodalindex] = bimodaltable[bimodalindex] + 1;
				if (bimodaltable[bimodalindex] > 7)
					bimodaltable[bimodalindex] = 7;
			}
			else if( actual == 0 )
			{
				bimodaltable[bimodalindex] = bimodaltable[bimodalindex] - 1;
				if (bimodaltable[bimodalindex] < 0)
					bimodaltable[bimodalindex] = 0;
			}
		}
		

		temp = (int)(bhr.to_ulong());
		temp>>=1;
		bitset<32> tempbhr(temp);
		bhr = tempbhr;

		if ( actual == 1){
			bhr[n-1] = 1;
		}
		else if ( actual == 0){
			bhr[n-1] = 0;
		}

		if(gshareprediction == actual && bimodalprediction != actual)
		{
			hybridtable[hybridindex] = hybridtable[hybridindex] + 1;
			if (hybridtable[hybridindex] > 3)
				hybridtable[hybridindex] = 3;
		}
		else if(gshareprediction != actual && bimodalprediction == actual)
		{
			hybridtable[hybridindex] = hybridtable[hybridindex] - 1;
			if (hybridtable[hybridindex] < 0)
				hybridtable[hybridindex] = 0;
		}
		if (hybridprediction != actual)
		{
			mispredictions+=1;
		}
	}
	object.close();
	print_hybrid(mispredictions, predictions, hybridtablesize, hybridtable, gsharetablesize, gsharetable, bimodaltablesize, bimodaltable);

}


int main(int argc, char *argv[])
{
	string tracefile;
	int m2, m1, n, k;
	//choose direction based on number of arguments
	//bimodal
	if(argc == 4)
	{
		m2 = atoi(argv[2]);
		tracefile = argv[3];
		cout << "COMMAND" << endl;
		cout << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3] << endl;
		bimodal(m2,tracefile);
	}
	//gshare
	else if(argc == 5)
	{
		m1 = atoi(argv[2]);
		n = atoi(argv[3]);
		tracefile = argv[4];
		cout << "COMMAND" << endl;
		cout << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3] << " "  << argv[4] << endl;
		gshare(m1, n, tracefile);
		// x = 2;
	}
	//hybrid
	else if(argc == 7)
	{
		k = atoi(argv[2]);
		m1 = atoi(argv[3]);
		n = atoi(argv[4]);
		m2 = atoi(argv[5]);
		tracefile = argv[6];
		cout << "COMMAND" << endl;
		cout << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3] << " "  << argv[4] << " "  << argv[5]<< " "  << argv[6] << endl;
		hybrid(k, m1, n, m2, tracefile);
		// x = 3;
	}

}	
