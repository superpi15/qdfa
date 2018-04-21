#ifndef REVSYN_QCA_HPP
#define REVSYN_QCA_HPP

#include "revsyn_ttb.hpp"
#include "qdfa.hpp"

bool SelectControl( Top_Ttb_t * pDup
, mpz_t& check
, mpz_t& selected
, mpz_t& cand
, std::vector<std::pair<int*,int> >& vRankFreq
, std::vector<int>& Gate
, bool ForceDominate = false
);
void ComputeGate( int Direction
, Top_Ttb_t * pDup
, Top_Ttb_t::iterator last
, std::vector<std::pair<int*,int> >& vRankFreq
, std::vector<mpz_class>& vControl
, std::vector<int>& vFixPath
, std::vector< std::vector<int> >& vGate );

void MoveState( int Direction
, Top_Ttb_t * pDup
, Top_Ttb_t::iterator begin
, std::vector<std::vector<int> >& vGate
, std::vector<int>& vFixPath );

void AddGate( int Direction
, Top_Ttb_t * pDup
, tRevNtk * pNtk
, std::vector<std::vector<int> >& vGate
, std::vector<int>& vFixPath );

tRevNtk * Top_TtbToRev_GBD_qca( Top_Ttb_t * pTtb );
tRevNtk * Top_TtbToRev_GBDL_qca( Top_Ttb_t * pTtb );

struct FreqCmp {
	bool operator()( const std::pair<int*,int>& A,
		const std::pair<int*,int>& B ) const {
		if( *A.first == *B.first )
			return A.second>B.second;
		return *A.first < *B.first;
	}
};


#endif
