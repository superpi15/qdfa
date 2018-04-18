#ifndef REVSYN_HPP
#define REVSYN_HPP

#include <iostream> 
#include <vector>
#include <list>
#include <fstream>
#include <string>
#include <set>

class tRevNtk;

class tSpec: public  std::vector<std::vector<char> > {
public:
	int nBit;
	std::set<int> CareLine;
	tSpec():nBit(0){}
	void print( std::ostream& ostr ){
		for( int i=0; i<size(); i++ ){
			for( int j=0; j<(*this)[i].size(); j++ ){
				ostr<< ((int)(*this)[i][j]);
			}
			ostr<<"\n";
		}
	}
	int verify( tRevNtk*);
};

class tRevNtk: public 	std::list<std::vector<char> >{
public:
	void print( std::ostream& ostr ){
		if( empty() )
			return ;
		int nBit = front().size();
		char buff[256];
		for( int i=0; i<nBit; i++ ){
			sprintf( buff, "|%d>", i );
			printf("%7s ", buff );
			for( iterator itr = begin(); itr!=end(); itr++ )
				printf("%c",(*itr)[i]);
			printf("\n");
		}
	}
	/*
	 * dot
	 * wdot
	 * not
	*/
	long long QCost();
private:
	long long QCost( std::vector<char>& col );
};
tRevNtk * DCBasic( tSpec& Spec );
tRevNtk * ReversibleBasic( tSpec& Spec, bool UseDC = false );
int ReadSpec( const char * FileName, tSpec& Spec );

#endif

