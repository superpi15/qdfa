#include "revsyn_ttb.hpp"
#include "revsyn_qca.hpp"
#include "qdfa.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> //Bidirection

int CountAddVal( int Val, int Direction, Top_Ttb_t * pDup
, Top_Ttb_t::iterator itr
, std::vector<mpz_class>& vControl
, std::vector<int>& vFixPath ){
	int dim = pDup->nLine;

	Pth in(itr-pDup->begin(),Pth::in),out(itr-pDup->begin(),Pth::out);
	std::cout<<"<"<< in(pDup) <<","<<out(pDup)<<">"<<std::endl;
	std::cout<<"Goal=";
	std::cout<< (Direction? out(pDup): in(pDup))<<"->";
	std::cout<< (Direction? in(pDup): out(pDup))<<std::endl;/**/
	//original pattern
	mpz_t& org = Direction? itr->second: itr->first;	
	//fixing target (org-[fix]->tar)
	mpz_t& tar = Direction? itr->first : itr->second; 	
	mpz_t op;
	mpz_init(op);
	mpz_set( op, org );
	vFixPath.clear();
	vControl.clear();
	int set_count = 0;
	for( int i=0; i<dim; ++i ){
		int index = Val? dim-1-i: i;

		if( mpz_tstbit( tar, index )==Val
		&& mpz_tstbit( org, index )==!Val ){
			if( Val )
				vControl.push_back( mpz_class(op) );
			if( Val ) // case: add one
				mpz_setbit(op,index);
			else
				mpz_clrbit(op,index);

			if( !Val )
				vControl.push_back( mpz_class(op) );
			vFixPath.push_back( index );
			set_count ++;
		}
	}
	/**/
	for( int i=0; i<vFixPath.size(); i++ ){
		std::cout<<i<<":";
		std::cout.fill('0');
		std::cout.width(dim);
		std::cout<< vControl[i].get_str(2)<<std::endl;
	} /**/
	mpz_clear(op);
	return set_count;
}

void ComputeGate( int Direction
, Top_Ttb_t * pDup
, Top_Ttb_t::iterator last
, std::vector<std::pair<int*,int> >& vRankFreq
, std::vector<mpz_class>& vControl
, std::vector<int>& vFixPath
, std::vector< std::vector<int> >& vGate ){
	int nLine = pDup->nLine;
	int pin;
	std::vector<mpz_class> vSelected( vFixPath.size(), 0 );
	vGate.clear();
	vGate.resize( vFixPath.size() );
	mpz_t selected, cand;
	mpz_init(selected);
	mpz_init(cand);
	for( Top_Ttb_t::iterator first = pDup->begin();
		first != last; first ++ ){
		for( int i=0; i<vFixPath.size(); i++ ){
			mpz_t& check = Direction? first->second: first->first;
			mpz_set(selected, vSelected[i].get_mpz_t() );
			mpz_set(cand, vControl[i].get_mpz_t() );
			bool res = SelectControl( 
				pDup, check, selected, cand, vRankFreq, vGate[i] );
			vSelected[i] = mpz_class(selected);
			mpz_out_str(stdout,2,selected); std::cout<<std::endl;
			assert(res);
		}
	}
	for( int i=0; i<vGate.size(); i++ ){
		std::cout<<"Gate "<<i<<":";
		for( int j=0; j<vGate[i].size(); j++ )
			std::cout<< vGate[i][j]<<" ";
		std::cout<<std::endl;
	}
	mpz_clear(selected);
	mpz_clear(cand);
}


bool SelectControl( Top_Ttb_t * pDup
, mpz_t& check
, mpz_t& selected
, mpz_t& cand
, std::vector<std::pair<int*,int> >& vRankFreq
, std::vector<int>& Gate 
){
	int Loop = 0;
	int Top = 0;
	if( !ContainTest()(selected,check) )
		return true;
	int index = -1;
	for(; Top<vRankFreq.size(); Top++ ){
		index = vRankFreq[Top].second;
		if( mpz_tstbit(selected,index) )
			continue;
		if( ! mpz_tstbit(cand,index) )
			continue;
		if( ! mpz_tstbit(check,index) ){
			Gate.push_back(index);
			mpz_setbit( selected, index );
			break;
		}
	}
		//assert();
	return !ContainTest()( selected, check );
}

tRevNtk * Top_TtbToRev_GBD_qca( Top_Ttb_t * pTtb ){
//	if( pTtb->size() != (1<<pTtb->nLine) ){
//		printf("Error: this is a incomplete spec.\n");
//		exit(0);
//	}
	Top_Ttb_t * pDup = pTtb->Duplicate();

	tRevNtk * pRev = new tRevNtk;
	Top_Ttb_t::iterator itr, sub, itr2, itr2min;
	tRevNtk & Rev = * pRev;
	tRevNtk RevInv;
	mpz_t mask, result;
	
	mpz_init(mask);
	mpz_init(result);
	unsigned long Gmin = 0;
	std::vector<int> vFreq( pDup->nLine, 0 );
	std::vector<std::pair<int*,int> > vRankFreq( pDup->nLine );
	for( int i=0; i<vRankFreq.size(); i++ ){
		vRankFreq[i].first = &vFreq[i];
		vRankFreq[i].second= i;
	}
	for( itr = pDup->begin(); itr != pDup->end(); itr ++ ){
//		if( mpz_cmp( itr->first, itr->second ) == 0 )
//			continue;
//		;
		for( itr2min = itr, itr2 = itr; itr2 != pDup->end(); itr2++ ){
			if( mpz_cmp( itr2->second, itr2min->second )<0 )
				itr2min = itr2;
		}

		int res = mpz_cmp( itr->first, itr2min->second );
		bool IsForward = true;
		if( res == 0 ){
			itr2 = itr2min;
			mp_bitcnt_t hamdist[2];
			hamdist[0] = mpz_hamdist( itr ->first, itr ->second );
			hamdist[1] = mpz_hamdist( itr2->first, itr2->second );
			IsForward = hamdist[0]<=hamdist[1]? true: false;
		} else {
			IsForward = res < 0? true: false;
		}
		//std::cout<< res <<std::endl;
		if( !IsForward )
			std::swap( *itr2min, *itr );
		if( IsForward ){
			//std::cout<<">>1\n";\
			pDup->print(std::cout);

			int SetVal, Direction;
			SetVal = 1;
			Direction= 1;
			std::vector<mpz_class> vControl;
			std::vector<int> vFixPath;
			std::vector< std::vector<int> > vGate;
			CountAddVal( 
				SetVal, Direction, pDup, itr, vControl, vFixPath );
			ComputeGate(
				Direction, pDup, itr, vRankFreq, vControl, vFixPath, vGate );
			Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
			SetVal= 0;
			CountAddVal( 
				SetVal, Direction, pDup, itr, vControl, vFixPath );
			ComputeGate(
				Direction, pDup, itr, vRankFreq, vControl, vFixPath, vGate );

			Top_Synthesis( Rev, pDup, SetVal, Direction, itr );
			//pDup->print(std::cout);\
			std::cout<<"<<1\n";
		} else {
			//std::sort( itr, pDup->end(), Tte::cmptor_second() );
			//std::cout<<">>2\n";\
			pDup->print(std::cout);

			int SetVal, Direction;
			SetVal = 1;
			Direction= 0;
			std::vector<mpz_class> vControl;
			std::vector<int> vFixPath;
			std::vector< std::vector<int> > vGate;
			CountAddVal( 
				SetVal, Direction, pDup, itr, vControl, vFixPath );
			ComputeGate(
				Direction, pDup, itr, vRankFreq, vControl, vFixPath, vGate );

			Top_Synthesis( RevInv, pDup, SetVal, Direction, itr );
			SetVal= 0;
			CountAddVal( 
				SetVal, Direction, pDup, itr, vControl, vFixPath );
			ComputeGate(
				Direction, pDup, itr, vRankFreq, vControl, vFixPath, vGate );

			Top_Synthesis( RevInv, pDup, SetVal, Direction, itr );
			//std::sort( itr, pDup->end(), Tte::cmptor_first() );
			//std::cout<<">>\n";\
			pDup->print(std::cout);\
			std::cout<<"<<2\n";
		}
		for( int i=0; i<pDup->nLine; i++ ){
			if( mpz_tstbit( itr->first, i ) )
				vFreq[i] ++;
		}
		std::sort( vRankFreq.begin(), vRankFreq.end(), FreqCmp() );
	}
	mpz_clear(mask);
	mpz_clear(result);
	pRev->splice( pRev->begin(), RevInv );
	//pRev->print( std::cout );
	delete pDup;
	return pRev;
}


