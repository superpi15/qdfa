#include <stdio.h>
#include "revsyn_ttb.hpp"

int main( int argc, char * argv[] ){
	if( argc<2 )
		return 0;
	tDfa Dfa;
	ReadDfa( argv[1], Dfa );
	Dfa.print( std::cout );
	tDfaToRdfa Man;
	Man.scan( Dfa );
	tRdfa * pRdfa = Man.convert();
	printf("Rdfq\n");
	pRdfa->print( std::cout );
	printf("AlphaBet# %d\n", Man.AlphaBet.size() );
	printf("maxSrcNum %d\n", Man.maxSrcNum );
	bool UseDC = true;
	if( argc>=3 )
		UseDC = false;
	RdfaToRevNtk_Ttb( pRdfa, UseDC );
	//tRevNtk RevNtk;
	//ReversibleBasic( Spec, RevNtk );
	//RevNtk.print(std::cout);
}


