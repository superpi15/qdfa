#include "revsyn_ttb.hpp"
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
		std::cout<< itr->first<<" "<< itr->second<< " "<< num<< std::endl;
		if( mpz_cmp( itr->first, num ) != 0 ){
			equi = false;
			break;
		}
	}
	mpz_clear(num);
	return equi==true;
}

int main( int argc, char * argv[] ){
	if( argc<2 )
		return 0;
	Top_Ttb_t * pTtb = Ttb_ReadSpec( argv[1] );
	pTtb->print(std::cout);
	tRevNtk * pRev = Top_TtbToRev(pTtb);
	pRev->print(std::cout);
	if( ! RevNtkVerify( pRev, pTtb ) )
		printf("The RevNtk doesn't implement Spec.\n");
	else
		printf("Spec is correctly implemented.\n");
	delete pTtb;
	delete pRev;
}
