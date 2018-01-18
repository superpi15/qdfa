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

void DecideEncode( tRdfa * pRdfa, std::vector<int>& StateEncode ){
	std::vector<std::pair<int,int> > SelfTransNum( pRdfa->nState );
	for( int i=0; i<SelfTransNum.size(); i++ )
		SelfTransNum[i] = std::pair<int,int>( 0, i );
	std::set<int>::iterator symbol;
	for( symbol = pRdfa->AlphaBet.begin(); symbol != pRdfa->AlphaBet.end();
		symbol++ ){
		std::vector<std::pair<int,int> > Trans;
		Trans.resize( pRdfa->nState );
		for( int i=0; i<pRdfa->nState; i++ ){
			
			assert( pRdfa->adj.find(i) != pRdfa->adj.end() );
			assert( pRdfa->adj[i].find(*symbol) != pRdfa->adj[i].end() );
			if( i == pRdfa->adj[i][*symbol] )
				SelfTransNum[i].first++;
			Trans[i] = std::pair<int,int>( 
				StateEncode[i]
				, StateEncode[ pRdfa->adj[i][*symbol] ] );
		}
	}
	std::sort( SelfTransNum.begin(), SelfTransNum.end() );
	StateEncode.clear();
	StateEncode.resize( SelfTransNum.size() );
	for( int i=0; i<SelfTransNum.size(); i++ ){
		StateEncode[i] = SelfTransNum[i].second;
	}
	std::reverse( StateEncode.begin(), StateEncode.end() );
}

tQdfa * RdfaToRevNtk( tRdfa * pRdfa, bool UseDC=false ){
	tQdfa * pQdfa =new tQdfa;
	std::vector<int> StateEncode( pRdfa->nState, 0 );
	for( int i = 0; i<StateEncode.size(); i++ )
		StateEncode[i] = i;
	//DecideEncode( pRdfa, StateEncode );
	
	int nQgate = 0;
	int nSelfTrans = 0;
	std::set<int>::iterator symbol;
	for( symbol = pRdfa->AlphaBet.begin(); symbol != pRdfa->AlphaBet.end();
		symbol++ ){
		std::vector<std::pair<int,int> > Trans;
		std::vector<int> CareLine( pRdfa->nState, 0 );
		Trans.resize( pRdfa->nState );
		for( int i=0; i<pRdfa->nState; i++ ){
			
			assert( pRdfa->adj.find(i) != pRdfa->adj.end() );
			assert( pRdfa->adj[i].find(*symbol) != pRdfa->adj[i].end() );
			if( i == pRdfa->adj[i][*symbol] )
				nSelfTrans ++;
			int EncodedSrc, EncodedDes;
			EncodedSrc = StateEncode[i];
			EncodedDes = StateEncode[ pRdfa->adj[i][*symbol] ];
			Trans[i] = std::pair<int,int>( EncodedSrc, EncodedDes );
			if( UseDC )
				if( pRdfa->real_path.find(
					std::pair<int,int>(i,*symbol) )
					!= pRdfa->real_path.end() )
					CareLine[EncodedSrc] = 1;
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
			//int StateID = StateEncode[Trans[i].first];
			if( UseDC )
				if( !CareLine[i] )
					continue;
			assert( i==Trans[i].first );
			for( int j=0; j<width; j++ )
				Spec[ Trans[i].first ][j]= (Trans[i].second & ( 1<<j))? 1: 0 ;
		}
		tRevNtk * pRevNtk = ReversibleBasic(Spec);
		printf("Symbol \'%d\'\n", *symbol );
		pRevNtk->print( std::cout );
		pQdfa->OpMap[*symbol] = pRevNtk;
		nQgate += pRevNtk->size();	
	}
	double ave = (double) nQgate/pRdfa->AlphaBet.size();
	double var = 0;
	for( tQdfa::tOpMap::iterator itr = pQdfa->OpMap.begin();
		itr != pQdfa->OpMap.end();
		itr++ ){
		double diff = (double) itr->second->size() - ave;
		var += diff*diff;
	}
	var /= (double)pRdfa->AlphaBet.size();
	//printf("Qgate # %d\n",nQgate);
	//printf("QgateAve %8.3f\n",ave);
	//printf("QgateVar %8.3f\n",var);
	printf("Summary: ");
	printf("%d %d %d %8.3f %8.3f\n", pRdfa->AlphaBet.size(), nQgate, nSelfTrans, ave, var );
	printf("AlphaBet#, Qgate, nSelfTrans, QgateAve, QgateVar\n");
	return pQdfa;
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
	RdfaToRevNtk( pRdfa, 1 );
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


