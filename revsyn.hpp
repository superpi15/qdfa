#ifndef REVSYN_HPP
#define REVSYN_HPP

#include <iostream> 
#include <vector>
#include <list>
#include <fstream>
#include <string>

class tSpec: public  std::vector<std::vector<char> > {
public:
	int nBit;
	tSpec():nBit(0){}
	void print( std::ostream& ostr ){
		for( int i=0; i<size(); i++ ){
			for( int j=0; j<(*this)[i].size(); j++ ){
				ostr<< ((int)(*this)[i][j]);
			}
			ostr<<"\n";
		}
	}
};

class tRevNtk: public 	std::list<std::vector<char> >{
public:
	void print( std::ostream& ostr ){
		if( empty() )
			return ;
		int nBit = front().size();
		for( int i=0; i<nBit; i++ ){
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
	;
};

int ReversibleBasic( tSpec& Spec, tRevNtk& RevNtk );
int ReadSpec( const char * FileName, tSpec& Spec );

#endif
