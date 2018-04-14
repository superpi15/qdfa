#include "revsyn_ttb.hpp"
#include "qdfa.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> //Bidirection

Top_Ttb_t * Ttb_ReadSpec( char * FileName ){
	Top_Ttb_t * pRet = new Top_Ttb_t;
	std::ifstream FileIn( FileName, std::ios::in );
	std::string line, input, output;
	
	int nLine = 0, Base = 2;
	int Temp;
	std::getline( FileIn, line );
	std::istringstream header(line);
	header >> nLine;
	if( header >> Temp )
		Base = Temp;
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
		mpz_init_set_str( pRet->back().first , input .c_str(), Base );
		mpz_init_set_str( pRet->back().second, output.c_str(), Base );
		//std::cout<< pRet->back().first<<"(" << input<<") " \
		<< pRet->back().second<<"("<< output<<")"<<std::endl;
	}
	Top_Ttb_t::iterator cur, next;
	std::sort( pRet->begin(), pRet->end(), Tte::cmptor_first() );
	for( cur = pRet->begin(); cur != pRet->end(); cur ++ ){
		next = cur+1;
		if( next == pRet->end() )
			break;
		if( mpz_cmp( next->first , cur->first ) <=0 ){
			printf("Error: input pattern should be sorted.\n");
			return NULL;
		}
	}
	//std::cout << pRet->size()<<":"<<std::endl; exit(0);
	return pRet;
}

tRevNtk * Top_TtbToRev_Top( Top_Ttb_t * pTtb ){
	/**
	if( pTtb->size() == (1<<pTtb->nLine) )
		return Top_TtbToRev( pTtb );
	else
		return Top_TtbToRev_DC( pTtb );
	/**/
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
	printf("HAHA\n");
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

	//std::cout << pIn->size()<<":"<<std::endl; exit(0);
	pRevIn  = Top_TtbToRev( pIn  );
	pRevOut = Top_TtbToRev( pOut );
	

	//printf("In half\n");\
	pRevIn->print(std::cout);\
	printf("Out half\n");\
	pRevOut->print(std::cout);
	pRevIn->reverse();
std::cout<< pRevIn->size() <<":"<< pRevOut->size()<<std::endl;	
	pRevOut->splice( pRevOut->begin(), *pRevIn );

	mpz_clear(num);
	delete pIn;
	delete pOut;
	delete pRevIn;
	//printf("Total\n");\
	pRevOut->print(std::cout);
	return pRevOut;
}

tRevNtk * Top_TtbToRev_Bi_Core( Top_Ttb_t * pTtb ){
//	if( pTtb->size() != (1<<pTtb->nLine) ){
//		printf("Error: this is a incomplete spec.\n");
//		exit(0);
//	}
	Top_Ttb_t * pDup = pTtb->Duplicate();
	std::vector<Top_Mpz_t> OG;

	tRevNtk * pRev = new tRevNtk;
	Top_Ttb_t::iterator itr, sub, itr2, itr2min;
	tRevNtk & Rev = * pRev;
	tRevNtk RevInv;
	mpz_t mask, result;
	
	OG.resize( pDup->size() );
	for( itr = pDup->begin(); itr != pDup->end(); itr++ ){
		int index = itr - pDup->begin();
		OG[index].obj = &itr->second;
	}
	std::sort( OG.begin(), OG.end(), Top_Mpz_t::cmptor() );

	mpz_init(mask);
	mpz_init(result);
	unsigned long Gmin = 0;
	for( itr = pDup->begin(); itr != pDup->end(); itr ++, Gmin ++ ){
//		if( mpz_cmp( itr->first, itr->second ) == 0 )
//			continue;
//		;
		for( itr2min = itr, itr2 = itr; itr2 != pDup->end(); itr2++ ){
			if( mpz_cmp( itr2->second, itr2min->second )<0 )
				itr2min = itr2;
		}

		//int res = mpz_cmp( itr->first, *OG[Gmin].obj );
		int res = mpz_cmp( itr->first, itr2min->second );
		bool IsForward = true;
		if( res == 0 ){
			//itr2 = pDup->find_second( \
				itr, pDup->end(), *OG[Gmin].obj );
			itr2 = itr2min;
			mp_bitcnt_t hamdist[2];
			hamdist[0] = mpz_hamdist( itr ->first, itr ->second );
			hamdist[1] = mpz_hamdist( itr2->first, itr2->second );
			IsForward = hamdist[0]<=hamdist[1]? true: false;
		} else {
			IsForward = res < 0? true: false;
		}
		//std::cout<< res <<std::endl;

		if( IsForward ){
			//std::cout<<">>1\n";\
			pDup->print(std::cout);

			int SetVal, Direction;
			SetVal = 1;
			Direction= 1;
			Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
			SetVal= 0;
			Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
			/**
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
			/**/
			//pDup->print(std::cout);\
			std::cout<<"<<1\n";
		} else {
			std::sort( itr, pDup->end(), Tte::cmptor_second() );
			//std::cout<<">>2\n";\
			pDup->print(std::cout);

			int SetVal, Direction;
			SetVal = 1;
			Direction= 0;
			Top_Synthesis( RevInv, pDup, SetVal, Direction, itr );
			SetVal= 0;
			Top_Synthesis( RevInv, pDup, SetVal, Direction, itr );
			/**
			for( int i=0; i<pDup->nLine; i++ ){
				if( mpz_tstbit( itr->first, i ) 
					== mpz_tstbit(itr->second, i ) )
					continue;
				mpz_combit( itr->first, i );
				mpz_set( mask, itr->first );
				mpz_clrbit( mask, i );
				//use mask to encode a gate
				RevInv.push_back( tRevNtk::value_type() );
				RevInv.back().resize( pDup->nLine, '-' );
				RevInv.back()[i] = 'X';
				for( int j=0; j<pDup->nLine; j++ ){
					if( j==i )
						continue;
					if( mpz_tstbit( mask, j ) )
						RevInv.back()[j] = '+';
				}
				//flip bit
				for( sub = itr + 1; sub != pDup->end(); sub++ ){
					mpz_and( result, mask, sub->first );
					if( mpz_cmp( result, mask ) == 0 )
						mpz_combit( sub->first, i );
				}
			}
			/**/
			std::sort( itr, pDup->end(), Tte::cmptor_first() );
			//std::cout<<">>\n";\
			pDup->print(std::cout);\
			std::cout<<"<<2\n";
		}
		//std::sort( OG.begin()+ Gmin, OG.end(), Top_Mpz_t::cmptor() );

	}
	mpz_clear(mask);
	mpz_clear(result);
	pRev->splice( pRev->begin(), RevInv );
	//pRev->print( std::cout );
	delete pDup;
	return pRev;
}

//Put all patterns into map of hamming weight
void ComputeHWMap(
Top_Ttb_t::iterator first, Top_Ttb_t * pTtb, mHW& HW ){
	Top_Ttb_t::iterator itr;
	for( itr = first; itr != pTtb->end(); itr ++ ){
		int hw;
		Tte& TteCur = *itr;
		int pos = itr- pTtb->begin();
		hw = mpz_popcount( TteCur.first );
		HW[hw].push_back( Pth(pos,Pth::in) );
		hw = mpz_popcount( TteCur.second );
		HW[hw].push_back( Pth(pos,Pth::out) );
	}
}

void LegalFixCand( Top_Ttb_t * pTtb, mHW& HW, vPth& Cands ){
	mHW::iterator itr;
	for( itr = HW.begin(); itr != HW.end(); itr ++ ){
		vPth& Unfix = itr->second;
		vPth::iterator jitr, kitr;
		for( jitr = Unfix.begin(); jitr != Unfix.end(); jitr++ ){
			bool is_contain = false;
			for( kitr = Cands.begin(); kitr != Cands.end(); kitr++ ){
				is_contain = Pth::Contain( pTtb )( *kitr, *jitr );
				if( is_contain )
					break;
			}
			if( !is_contain || Cands.empty() ){
				Cands.push_back( *jitr );
			}
		}
	}
}

tRevNtk * Top_GBDL( Top_Ttb_t * pTtb ){
	Top_Ttb_t::iterator itr;
	Top_Ttb_t * pDup = pTtb->Duplicate();
	tRevNtk * pRev = new tRevNtk;
	tRevNtk & Rev = * pRev;
	tRevNtk RevInv;
	for( itr = pTtb->begin(); itr != pTtb->end(); itr ++ ){	
		mHW HW;
		ComputeHWMap( itr, pTtb, HW );
		//print_mHW( std::cout, HW, pTtb );
		vPth Cands;
		LegalFixCand( pTtb, HW, Cands );
		
		int min_hdist = pTtb->nLine+1;
		vPth::iterator tarItr = Cands.end();
		for( vPth::iterator pitr = Cands.begin();
			pitr != Cands.end(); pitr++ ){
			Pth pin(pitr->pos,Pth::in), pout(pitr->pos,Pth::out);
			int hdist = mpz_hamdist(
				pin.get_mpz(pTtb), pout.get_mpz(pTtb) );
			if( hdist < min_hdist ){
				min_hdist = hdist;
				tarItr = pitr;
			}
		}
	}
	exit(0);
}

bool Determine_Pseudo_Care_Output(
	std::vector<Top_Mpz_t>::iterator first,
	std::vector<Top_Mpz_t>::iterator last, 
	mpz_t& pcout, mpz_t& pcin, 
	mpz_t& pcmin, mpz_t& pcmax, int bitcnt ){
	//This function writes the result to pcout
	Top_Mpz_t hold;
	hold.obj = &pcout;

	//lazy update
	if( mpz_cmp( pcout, pcin )<0 )
		mpz_set( pcout, pcin );
	if( !std::binary_search( first, last, hold, Top_Mpz_t::cmptor() ) )
		return true;
	
	for( int i=bitcnt-1; i>=0; i-- ){
		if( mpz_tstbit( pcout, i )==0 )
			mpz_setbit( pcout, i );
		if( mpz_cmp( pcout, pcmin )>0 
		&& !std::binary_search( first, last, hold,
		Top_Mpz_t::cmptor() ) )
			break;
	}
	mpz_t res;
	mpz_init( res );
	mpz_set( res, pcout );
	while( std::binary_search( first, last, hold,
	Top_Mpz_t::cmptor() ) ){
		//mpz_out_str( stdout, 2, pcout );\
		printf("--\n");
		mpz_sub_ui( res, pcout, 1 );
		mpz_set( pcout, res );
		assert( mpz_cmp( pcout, pcin )>0 );
	}
	//while( first != last ){\
		mpz_out_str( stdout, 2, *first->obj ); fflush( stdout );\
		printf(" ... %d\n", last- first ); fflush( stdout );\
		first ++;\
	}
	mpz_clear( res );
	//mpz_out_str( stdout, 2, pcout ); fflush( stdout );\
	printf("\n"); fflush( stdout );\
	mpz_out_str( stdout, 2, pcmin ); fflush( stdout );\
	printf("\n"); fflush( stdout );
	assert( mpz_cmp( pcout, pcin )>0 );
	assert( mpz_cmp( pcout, pcmax )<0 );
	//pcin< pcout < pcmax
	return true;
}

tRevNtk * Top_TtbToRev_PseudoCare( Top_Ttb_t * pTtb ){
	Top_Ttb_t * pDup = pTtb->Duplicate();
	Top_Ttb_t::iterator itr, sub;
	std::vector<Top_Mpz_t> IG, OG;
	tRevNtk * pRev = new tRevNtk;
	tRevNtk & Rev = * pRev;
	mpz_t mask, result;

	mpz_init(mask);
	mpz_init(result);
	IG.resize( pDup->size() );
	OG.resize( pDup->size() );

	for( itr = pDup->begin(); itr != pDup->end(); itr ++ ){
		int index = itr - pDup->begin();
		IG[index].obj = &itr->first;
		OG[index].obj = &itr->second;
	}
	std::sort( OG.begin(), OG.end(), Top_Mpz_t::cmptor() );
	//for( itr = pDup->begin(); itr != pDup->end(); itr ++ ){\
		int index = itr - pDup->begin();\
		mpz_out_str( stdout, 2, *OG[index].obj );\
		printf(" ");\
		mpz_out_str( stdout, 2, *IG[index].obj );\
		printf("\n");\
	}
	unsigned long Gmin = 0;
	for( itr = pDup->begin(); itr != pDup->end(); itr ++, Gmin++ ){

		if( Top_Mpz_t::cmptor()( OG[Gmin], IG[Gmin] ) ){
		//printf(" Gmin=%d ", Gmin);\
		mpz_out_str( stdout, 2, *OG[Gmin].obj );\
		printf(" vs ");\
		mpz_out_str( stdout, 2, *IG[Gmin].obj );\
		printf("\n");
			//min(OG) < min(IG), insert pseudo_care
			//determine the output of pseudo_care
			//compute Q-gates
			//perform transformation
			//sort
			//repeat until min(OG) >= min(IG)
			mpz_t pcin, pcout; // pseudo care in/out
			mpz_t pcmax; // range of pcout: [pcmin:pcmax)
			mpz_init( pcin );
			mpz_init( pcout );
			mpz_init( pcmax );
			
			mpz_set_ui( pcmax, 0 );
			mpz_set_ui( pcout, 0 );
			mpz_setbit( pcmax, pDup->nLine );
			do {
				mpz_set( pcin, *OG[Gmin].obj ); // pseudo care term
				Determine_Pseudo_Care_Output( 
					OG.begin()+Gmin, OG.end(), 
					pcout, pcin, itr->first, pcmax, pDup->nLine );
				// Start of compute Q-gate and perform Transform
				/**
				int SetVal, Direction;
				SetVal = 1;
				Direction = 1;
				Top_Synthesis_PseudoCare( Rev, pDup, SetVal
				, Direction, itr, pcin, pcout );
				SetVal = 0;
				Top_Synthesis_PseudoCare( Rev, pDup, SetVal
				, Direction, itr, pcin, pcout );
				/**/
				for( int i=0; i<pDup->nLine; i++ ){
					if( mpz_tstbit( pcout, i ) 
						== mpz_tstbit( pcin, i ) )
						continue;
					if( mpz_tstbit(pcout, 1 )==1 )
						continue;
					mpz_combit( pcout, i );
					mpz_set( mask, pcout );
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
					//Note: (sub=itr) is correct in this function!!
					//Normal version is (sub=itr+1)
					for( sub = itr; sub != pDup->end(); sub++ ){
						mpz_and( result, mask, sub->second );
						if( mpz_cmp( result, mask ) == 0 )
							mpz_combit( sub->second, i );
					}
				}
				for( int i=0; i<pDup->nLine; i++ ){
					if( mpz_tstbit( pcout, i ) 
						== mpz_tstbit( pcin, i ) )
						continue;
					if( mpz_tstbit(pcout, 1 )==0 )
						continue;
					mpz_combit( pcout, i );
					mpz_set( mask, pcout );
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
					//Note: (sub=itr) is correct in this function!!
					//Normal version is (sub=itr+1)
					for( sub = itr; sub != pDup->end(); sub++ ){
						mpz_and( result, mask, sub->second );
						if( mpz_cmp( result, mask ) == 0 )
							mpz_combit( sub->second, i );
					}
				}/**/
				//Start of compute Q-gate and perform Transform
				//printf("Perform Transform ...\n");\
				pDup->print(std::cout);\
				printf("Done\n");
				std::sort( OG.begin(), OG.end(), Top_Mpz_t::cmptor() );
			} while( Top_Mpz_t::cmptor()( OG[Gmin], IG[Gmin] ) );
			mpz_clear( pcin );
			mpz_clear( pcout );
			mpz_clear( pcmax );

			//check again
			if( mpz_cmp( itr->first, itr->second ) == 0 )
				continue;
		} else {
			//printf(" Gmin=%d ", Gmin);\
			mpz_out_str( stdout, 2, *OG[Gmin].obj );\
			printf(" vs ");\
			mpz_out_str( stdout, 2, *IG[Gmin].obj );\
			printf("... else \n");
		}

		if( mpz_cmp( itr->first, itr->second ) == 0 )
			continue;
		/**/
		int SetVal, Direction;
		SetVal = 1;
		Direction= 1;
		Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
		SetVal= 0;
		Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
		/**
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
		/**/
		//sort after the last Transform
		std::sort( OG.begin(), OG.end(), Top_Mpz_t::cmptor() );
	}
	mpz_clear(mask);
	mpz_clear(result);
	//pRev->print(std::cout);
	delete pDup;

	return pRev;

}
int Top_Synthesis_PseudoCare( tRevNtk& Rev, Top_Ttb_t * pTtb
, int SetVal, int Direction, Top_Ttb_t::iterator itr
, mpz_t& pcin, mpz_t& pcout ){
	Top_Ttb_t::iterator sub;
	mpz_t mask, result;
	mpz_init( mask );
	mpz_init( result );
	for( int i=0; i<pTtb->nLine; i++ ){
		int InTrue = mpz_tstbit( pcin, i ); 
		int OutTrue= mpz_tstbit( pcout, i );
		if( (Direction? OutTrue: InTrue) == SetVal )
			continue;
		if( InTrue == OutTrue )
			continue;
		mpz_t& Target = (Direction)? pcout: pcin;
		mpz_combit( Target, i );
		mpz_set( mask, Target );
		mpz_clrbit( mask, i );
		//use mask to encode a gate
		if( Direction ){
			Rev.push_front( tRevNtk::value_type() );
			Rev.front().resize( pTtb->nLine, '-' );
			Rev.front()[i] = 'X';
		} else {
			Rev.push_back( tRevNtk::value_type() );
			Rev.back().resize( pTtb->nLine, '-' );
			Rev.back()[i] = 'X';
		}
		for( int j=0; j<pTtb->nLine; j++ ){
			if( j==i )
				continue;
			if( mpz_tstbit( mask, j ) ){
				if( Direction )
					Rev.front()[j] = '+';
				else
					Rev.back()[j] = '+';
			}
		}
		//flip bit
		for( sub = itr + 1; sub != pTtb->end(); sub++ ){
			mpz_t& SubTarget = (Direction)? sub->second: sub->first;
			mpz_and( result, mask, SubTarget );
			if( mpz_cmp( result, mask ) == 0 )
				mpz_combit( SubTarget, i );
		}
	}
	mpz_clear( mask );
	mpz_clear( result );
}
int Top_Synthesis( tRevNtk& Rev, Top_Ttb_t * pTtb, int SetVal, int Direction, Top_Ttb_t::iterator itr ){
	Top_Ttb_t::iterator sub;
	mpz_t mask, result;
	mpz_init( mask );
	mpz_init( result );
	for( int i=0; i<pTtb->nLine; i++ ){
		int InTrue = mpz_tstbit( itr->first, i ); 
		int OutTrue= mpz_tstbit( itr->second, i );
		if( (Direction? OutTrue: InTrue) == SetVal )
			continue;
		if( InTrue == OutTrue )
			continue;
		mpz_t& Target = (Direction)? itr->second: itr->first;

		//Correction
		/**/
		if( Direction && SetVal )
			mpz_set( mask, itr->second );
		else
		if( Direction && !SetVal )
			mpz_set( mask, itr->first );
		else
		if( !Direction && SetVal )
			mpz_set( mask, itr->first );
		else
			mpz_set( mask, itr->second );
		/**/

		mpz_combit( Target, i );
		//mpz_set( mask, Target );  //original
		//mpz_clrbit( mask, i );
		//use mask to encode a gate
		if( Direction ){
			Rev.push_front( tRevNtk::value_type() );
			Rev.front().resize( pTtb->nLine, '-' );
			Rev.front()[i] = 'X';
		} else {
			Rev.push_back( tRevNtk::value_type() );
			Rev.back().resize( pTtb->nLine, '-' );
			Rev.back()[i] = 'X';
		}
		for( int j=0; j<pTtb->nLine; j++ ){
			if( j==i )
				continue;
			if( mpz_tstbit( mask, j ) ){
				if( Direction )
					Rev.front()[j] = '+';
				else
					Rev.back()[j] = '+';
			}
		}
		//flip bit
		for( sub = itr + 1; sub != pTtb->end(); sub++ ){
			mpz_t& SubTarget = (Direction)? sub->second: sub->first;
			mpz_and( result, mask, SubTarget );
			if( mpz_cmp( result, mask ) == 0 )
				mpz_combit( SubTarget, i );
		}
	}
	mpz_clear( mask );
	mpz_clear( result );
}

tRevNtk * Top_TtbToRev( Top_Ttb_t * pTtb ){
	Top_Ttb_t * pDup = pTtb->Duplicate();
	Top_Ttb_t::iterator itr, sub;
	tRevNtk * pRev = new tRevNtk;
	tRevNtk & Rev = * pRev;
	mpz_t mask, result;
//std::cout<<":"<<std::endl; exit(0);
	mpz_init(mask);
	mpz_init(result);
	for( itr = pDup->begin(); itr != pDup->end(); itr ++ ){
		if( mpz_cmp( itr->first, itr->second ) == 0 )
			continue;
		;
		int SetVal, Direction;
		SetVal = 1;
		Direction = 1;
		Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
		SetVal = 0;
		Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
		/**
		for( int i=0; i<pDup->nLine; i++ ){
			int InTrue = mpz_tstbit( itr->first, i ); 
			int OutTrue= mpz_tstbit( itr->second, i );
			if( InTrue == 0 )
				continue;
			if( InTrue == OutTrue )
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
		for( int i=0; i<pDup->nLine; i++ ){
			int InTrue = mpz_tstbit( itr->first, i ); 
			int OutTrue= mpz_tstbit( itr->second, i );
			if( InTrue == 1 )
				continue;
			if( InTrue == OutTrue )
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
		}/**/
	}
	mpz_clear(mask);
	mpz_clear(result);
	delete pDup;

	return pRev;
}

tRevNtk * Top_TtbToRev_Bi( Top_Ttb_t * pTtb ){
	if( pTtb->size() != (1<<pTtb->nLine) ){
		printf("Error: this is a incomplete spec.\n");
		exit(0);
	}
	Top_Ttb_t * pDup = pTtb->Duplicate();
	tRevNtk * pRev = new tRevNtk;
	Top_Ttb_t::iterator itr, sub, itr2;
	tRevNtk & Rev = * pRev;
	tRevNtk RevInv;
	mpz_t mask, result;

	mpz_init(mask);
	mpz_init(result);
	for( itr = pDup->begin(); itr != pDup->end(); itr ++ ){
		if( mpz_cmp( itr->first, itr->second ) == 0 )
			continue;
		;
		itr2 = pDup->find_second( itr, pDup->end(), itr->first );
		
		mp_bitcnt_t hamdist[2];
		hamdist[0] = mpz_hamdist( itr ->first, itr ->second );
		hamdist[1] = mpz_hamdist( itr2->first, itr2->second );
		if( hamdist[0]<=hamdist[1] ){
			//std::cout<<">>1\n";\
			pDup->print(std::cout);
			int SetVal, Direction;
			SetVal = 1;
			Direction = 1;
			Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
			SetVal = 0;
			Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
			/**
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
			/**/
			//pDup->print(std::cout);\
			std::cout<<"<<1\n";
		} else {
			std::sort( itr, pDup->end(), Tte::cmptor_second() );
			//std::cout<<">>2\n";\
			pDup->print(std::cout);
			int SetVal, Direction;/**/
			SetVal = 1;
			Direction = 0;
			Top_Synthesis( RevInv, pDup, SetVal, Direction, itr );
			SetVal = 0;
			Top_Synthesis( RevInv, pDup, SetVal, Direction, itr );

			std::sort( itr, pDup->end(), Tte::cmptor_first() );
			/**
			for( int i=0; i<pDup->nLine; i++ ){
				if( mpz_tstbit( itr->first, i ) 
					== mpz_tstbit(itr->second, i ) )
					continue;
				mpz_combit( itr->first, i );
				mpz_set( mask, itr->first );
				mpz_clrbit( mask, i );
				//use mask to encode a gate
				RevInv.push_back( tRevNtk::value_type() );
				RevInv.back().resize( pDup->nLine, '-' );
				RevInv.back()[i] = 'X';
				for( int j=0; j<pDup->nLine; j++ ){
					if( j==i )
						continue;
					if( mpz_tstbit( mask, j ) )
						RevInv.back()[j] = '+';
				}
				//flip bit
				for( sub = itr + 1; sub != pDup->end(); sub++ ){
					mpz_and( result, mask, sub->first );
					if( mpz_cmp( result, mask ) == 0 )
						mpz_combit( sub->first, i );
				}
			}
			std::sort( itr, pDup->end(), Tte::cmptor_first() );
			/**/
			//pDup->print(std::cout);\
			std::cout<<"<<2\n";
		}
	}
	mpz_clear(mask);
	mpz_clear(result);
	pRev->splice( pRev->begin(), RevInv );

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
			std::cout<<"Nequi:"<< std::hex << itr->first<<" "<< itr->second\
			<< " "<< num<< std::endl;

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




