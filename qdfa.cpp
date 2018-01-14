#include "qdfa.hpp"
#include "revsyn.hpp"
#include <algorithm>
#include <cmath>

int ReadDfa( const char * FileName, tDfa& Dfa ){
	std::ifstream in( FileName, std::ios::in );
	std::string line;
	int nLine;
	int MaxState = 0;
	Dfa.nState = 0;
	Dfa.adj.clear();
	for( nLine = 0; std::getline(in,line); nLine++ ){
		int src, input, des;
		std::istringstream istr(line);
		//printf("%s\n",line.c_str());
		if( !(istr>>src>>input>>des) ){
			printf("Error: read dfa failed on line %d.\n",nLine);
			return 0;
		}
		Dfa.adj[src][input] = des;
		Dfa.AlphaBet.insert(input);
		MaxState = (src>MaxState)? src: MaxState;
		MaxState = (des>MaxState)? des: MaxState;
		//printf("%d %d %d\n",src,input,des);
	}
	Dfa.nState = MaxState+1;
	return 1;
}

tRevNtk * RdfaToRevNtk( tRdfa * pRdfa ){
	std::vector<int> StateEncode( pRdfa->nState, 0 );
	for( int i = 0; i<StateEncode.size(); i++ )
		StateEncode[i] = i;
	
	std::set<int>::iterator symbol;
	for( symbol = pRdfa->AlphaBet.begin(); symbol != pRdfa->AlphaBet.end();
		symbol++ ){
		std::vector<std::pair<int,int> > Trans;
		Trans.resize( pRdfa->nState );
		for( int i=0; i<pRdfa->nState; i++ ){
			
			assert( pRdfa->adj.find(i) != pRdfa->adj.end() );
			assert( pRdfa->adj[i].find(*symbol) != pRdfa->adj[i].end() );

			Trans[i] = std::pair<int,int>( 
				StateEncode[i]
				, StateEncode[ pRdfa->adj[i][*symbol] ] );
		}
		std::sort( Trans.begin(), Trans.end() );
		tSpec Spec;
		int width = 1+log(pRdfa->nState)/log(2);
		Spec.resize( 1<<width );
		for( int i=0; i<Spec.size(); i++ ){
			Spec[i].resize( width );
			for( int j=0; j<Spec[i].size(); j++ )
				Spec[i][j] = (i&(1<<j))? 1: 0;
		}
		for( int i=0; i<Trans.size(); i++ ){
			int StateID = StateEncode[Trans[i].first];
			for( int j=0; j<width; j++ )
				Spec[StateID][j]= (Trans[i].second & ( 1<<j))? 1: 0 ;
		}
		tRevNtk * pRevNtk = ReversibleBasic(Spec);
		printf("Symbol \'%d\'\n", *symbol );
		pRevNtk->print( std::cout );
		delete pRevNtk;
	}
}
/*
void tSpec::verify( tRevNtk * pRevNtk ){
	for( int i=0; i<size(); i++ ){
		
		for( int j=0; j<nBit; j++ ){
			sprintf( buff, "|%d>", j );
			printf("%7s ", buff );
			for( iterator itr = begin(); itr!=end(); itr++ )
				printf("%c",(*itr)[j]);
			printf("\n");
		}
	}
}
*/

int main( int argc, char * argv[] ){
	if( argc!=2 )
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
	RdfaToRevNtk( pRdfa );
	//tRevNtk RevNtk;
	//ReversibleBasic( Spec, RevNtk );
	//RevNtk.print(std::cout);
	/*
	for( tRevNtk::iterator itr = RevNtk.begin(); itr!=RevNtk.end(); itr++ ){
		for( int i = 0; i<itr->size(); i++ ){
			printf("%c",(*itr)[i]);
		}
		printf("\n");
	}*/
}


