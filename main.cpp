#include <stdio.h>
#include <stdlib.h>
#include "revsyn_ttb.hpp"
#include <time.h>
// Reversible DFA 
/**
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
/**/

// DC aware
/**
int main( int argc, char * argv[] ){
	if( argc<2 )
		return 0;
	Top_Ttb_t * pTtb = Ttb_ReadSpec( argv[1] );
	pTtb->print(std::cout);
	tRevNtk * pRev = Top_TtbToRev_Top(pTtb);
	pRev->print(std::cout);
	if( ! RevNtkVerify( pRev, pTtb ) )
		printf("The RevNtk doesn't implement Spec.\n");
	else
		printf("Spec is correctly implemented.\n");
	delete pTtb;
	delete pRev;
}
/**/


//Comparison of Transform-based method:
// DC-aware, Bi-direction, normal
/**/
int main( int argc, char * argv[] ){
	clock_t clk_start, clk_end;
	clk_start = clock();
	int mode = 0;
	if( argc<2 )
		return 0;
	if( argc>2 )
		mode = atoi(argv[2]);
	Top_Ttb_t * pTtb = Ttb_ReadSpec( argv[1] );
	//pTtb->print(std::cout);
	printf("mode= %d\n",mode);
	tRevNtk * pRev;
	if( mode == 0 )
		pRev = Top_TtbToRev_Top(pTtb);
	else if( mode == 1 )
		pRev = Top_TtbToRev_Bi(pTtb);
	else if( mode == 2 )
		pRev = Top_TtbToRev_PseudoCare(pTtb);
	else if( mode == 3 )
		pRev = Top_TtbToRev_Bi_Core(pTtb);
	else if( mode == 4 )
		pRev = Top_GBDL(pTtb);
	else
		pRev = Top_TtbToRev(pTtb);
	//pRev->print(std::cout);
	//std::cout<<"Toffoli# "<<std::dec<< pRev->size() <<std::endl;
	if( ! RevNtkVerify( pRev, pTtb ) )\
		printf("The RevNtk doesn't implement Spec.\n");\
	else\
		printf("Spec is correctly implemented.\n");
	clk_end = clock();
	//pRev->print(std::cout);
	if( argc>3 )
		pRev->WriteReal(argv[3]);
	printf("(QCost): %15lld\n", pRev->QCost() );
	printf("(Toffli#,Time): %15d %8.3f\n"
		, pRev->size()
		, (double) (clk_end- clk_start)/CLOCKS_PER_SEC );
	//LogPrint( std::cout );
	delete pTtb;
	delete pRev;
}
/**/

