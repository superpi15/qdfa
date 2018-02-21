#include "revsyn_ttb.hpp"
#include "qdfa.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Top_Ttb_t * Ttb_ReadSpec( char * FileName ){
	Top_Ttb_t * pRet = new Top_Ttb_t;
	std::ifstream FileIn( FileName, std::ios::in );
	std::string line, input, output;
	
	int nLine = 0;
	std::getline( FileIn, line );
	std::istringstream header(line);
	header >> nLine;
	pRet->nLine = nLine;
	while( std::getline( FileIn, line ) ){
		std::istringstream word(line);
		if( !(word >> input) ){
			printf("Error: missing input.\n");
			return NULL;
		}
		if( !(word >> output) ){
			printf("Error: missing output.\n");
			return NULL;
		}
		pRet->resize( pRet->size() +1 );
		mpz_init_set_str( pRet->back().first , input .c_str(), 2 );
		mpz_init_set_str( pRet->back().second, output.c_str(), 2 );
		//std::cout<< pRet->back().first<<"(" << input<<") " \
		<< pRet->back().second<<"("<< output<<")"<<std::endl;
	}
	Top_Ttb_t::iterator cur, next;
	for( cur = pRet->begin(); cur != pRet->end(); cur ++ ){
		next = cur+1;
		if( next == pRet->end() )
			break;
		if( mpz_cmp( next->first , cur->first ) <=0 ){
			printf("Error: input pattern should be sorted.\n");
			return NULL;
		}
	}
	return pRet;
}

tRevNtk * Top_TtbToRev_Top( Top_Ttb_t * pTtb ){
	int nState = pTtb->size();
	bool IsPowerOf2 = nState && !( nState & (nState-1));
	if( IsPowerOf2 )
		return Top_TtbToRev( pTtb );
	else
		return Top_TtbToRev_DC( pTtb );
}

tRevNtk * Top_TtbToRev_DC( Top_Ttb_t * pTtb ){
	Top_Ttb_t * pIn, * pOut;
	tRevNtk * pRevIn, * pRevOut;
	Top_Ttb_t::iterator itr;
	mpz_t num;
	std::vector<unsigned long int> MidState( pTtb->size() );
	unsigned long int count = 0;

	//Basic MidState Assignment
	for( count = 0; count < pTtb->size(); count++ )
		MidState[count] = count;

	mpz_init(num);
	pIn = new Top_Ttb_t( pTtb->nLine );
	pOut= new Top_Ttb_t( pTtb->nLine );
	
	for( count = 0, itr = pTtb->begin(); itr != pTtb->end(); itr++, count++ ){
		mpz_set_ui( num, MidState[count] );
		pIn ->insert_entry( num, itr->first  );
		pOut->insert_entry( num, itr->second );
	}
	//pIn->print(std::cout);
	//pOut->print(std::cout);
	
	pRevIn  = Top_TtbToRev( pIn  );
	pRevOut = Top_TtbToRev( pOut );
	//pRevIn->print(std::cout);
	//pRevOut->print(std::cout);
	pRevIn->reverse();
	pRevOut->splice( pRevOut->begin(), *pRevIn );

	mpz_clear(num);
	delete pIn;
	delete pOut;
	delete pRevIn;

	return pRevOut;
}
tRevNtk * Top_TtbToRev( Top_Ttb_t * pTtb ){
	Top_Ttb_t * pDup = pTtb->Duplicate();
	Top_Ttb_t::iterator itr, sub;
	tRevNtk * pRev = new tRevNtk;
	tRevNtk & Rev = * pRev;
	mpz_t mask, result;

	mpz_init(mask);
	mpz_init(result);
	for( itr = pDup->begin(); itr != pDup->end(); itr ++ ){
		if( mpz_cmp( itr->first, itr->second ) == 0 )
			continue;
		;
		for( int i=0; i<pDup->nLine; i++ ){
			if( mpz_tstbit( itr->first, i ) 
				== mpz_tstbit(itr->second, i ) )
				continue;
			mpz_combit( itr->second, i );
			mpz_set( mask, itr->second );
			mpz_clrbit( mask, i );
			//use mask to encode a gate
			Rev.push_front( tRevNtk::value_type() );
			Rev.front().resize( pDup->nLine, '-' );
			Rev.front()[i] = 'X';
			for( int j=0; j<pDup->nLine; j++ ){
				if( j==i )
					continue;
				if( mpz_tstbit( mask, j ) )
					Rev.front()[j] = '+';
			}
			//flip bit
			for( sub = itr + 1; sub != pDup->end(); sub++ ){
				mpz_and( result, mask, sub->second );
				if( mpz_cmp( result, mask ) == 0 )
					mpz_combit( sub->second, i );
			}
		}
	}
	mpz_clear(mask);
	mpz_clear(result);
	delete pDup;

	return pRev;
}

int RevNtkVerify( tRevNtk * pRev, Top_Ttb_t * pTtb ){
	Top_Ttb_t::iterator itr;
	bool equi = true;
	tRevNtk::reverse_iterator gate;
	mpz_t num;
	mpz_init(num);
	for( itr = pTtb->begin(); itr != pTtb->end(); itr ++ ){
		mpz_set( num, itr->second );
		gate = pRev->rbegin();
		for( ; gate != pRev->rend(); gate++ ){
			int flip = -1;
			bool cond = true;
			for( int i=0; i<gate->size(); i++ ){
				char op = (*gate)[i];
				if( op == 'X' )
					flip = i;
				if( op == '+' )
					if( mpz_tstbit( num, i ) == 0 ){
						cond = false;
						break;
					}
			}
			if( cond && (flip != -1) )
				mpz_combit(num,flip);
		}
		//std::cout<< itr->first<<" "<< itr->second\
			<< " "<< num<< std::endl;
		if( mpz_cmp( itr->first, num ) != 0 ){
			equi = false;
			break;
		}
	}
	mpz_clear(num);
	return equi==true;
}

tQdfa * RdfaToRevNtk_Ttb( tRdfa * pRdfa, bool UseDC ){
	tQdfa * pQdfa =new tQdfa;
	std::vector<int> StateEncode( pRdfa->nState, 0 );
	for( int i = 0; i<StateEncode.size(); i++ )
		StateEncode[i] = i;
	//DecideEncode( pRdfa, StateEncode );
	
	int nQgate = 0;
	int nSelfTrans = 0;
	std::set<int>::iterator symbol;
	int nState = pRdfa->nState;
	bool IsPowerOf2 = nState && !( nState & (nState-1));
	int nLine = (IsPowerOf2?0:1)+log(pRdfa->nState)/log(2);
	for( symbol = pRdfa->AlphaBet.begin(); symbol != pRdfa->AlphaBet.end();
		symbol++ ){
		Top_Ttb_t Ttb;
		Ttb.nLine = nLine;
		for( int i=0; i<pRdfa->nState; i++ ){
			
			assert( pRdfa->adj.find(i) != pRdfa->adj.end() );
			assert( pRdfa->adj[i].find(*symbol) != pRdfa->adj[i].end() );
			if( i == pRdfa->adj[i][*symbol] )
				nSelfTrans ++;
			int EncodedSrc, EncodedDes;
			EncodedSrc = StateEncode[i];
			EncodedDes = StateEncode[ pRdfa->adj[i][*symbol] ];
			//Trans[i] = std::pair<int,int>( EncodedSrc, EncodedDes );

			if( UseDC ){
				if( pRdfa->real_path.find(
					std::pair<int,int>(i,*symbol) )
					!= pRdfa->real_path.end() ){
					Ttb.insert_entry( EncodedSrc, EncodedDes );
				}
				//else\
					printf("not real:(%d,%d)\n",i,*symbol);
			} else {
				Ttb.insert_entry( EncodedSrc, EncodedDes );
			}
		}
		tRevNtk * pRevNtk = Top_TtbToRev_Top( &Ttb );
		/**
		if( UseDC )
			pRevNtk = DCBasic(Spec);
		else
			pRevNtk = ReversibleBasic(Spec,false);
		/**/
		printf("Symbol \'%d\'\n", *symbol );
		pRevNtk->print( std::cout );
		pQdfa->OpMap[*symbol] = pRevNtk;
		nQgate += pRevNtk->size();	
		/**/
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


