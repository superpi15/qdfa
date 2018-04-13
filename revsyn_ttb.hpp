#ifndef REVSYN_TTB_HPP
#define REVSYN_TTB_HPP
#include <gmpxx.h>
#include "ttb.hpp"
#include "revsyn.hpp"
#include "qdfa.hpp"

Top_Ttb_t * Ttb_ReadSpec( char * FileName );
tRevNtk * Top_TtbToRev( Top_Ttb_t * pTtb );
tRevNtk * Top_TtbToRev_DC( Top_Ttb_t * pTtb );
tRevNtk * Top_TtbToRev_Top( Top_Ttb_t * pTtb );
tQdfa * RdfaToRevNtk_Ttb( tRdfa * pRdfa, bool UseDC );
int RevNtkVerify( tRevNtk * pRev, Top_Ttb_t * pTtb );
tRevNtk * Top_TtbToRev_Bi( Top_Ttb_t * pTtb ); 
tRevNtk * Top_TtbToRev_PseudoCare( Top_Ttb_t * pTtb );
tRevNtk * Top_TtbToRev_Bi_Core( Top_Ttb_t * pTtb );
tRevNtk * Top_GBDL( Top_Ttb_t * pTtb );
tRevNtk * TtbToRevNtk();
int Top_Synthesis( tRevNtk& Rev, Top_Ttb_t * pTtb, int SetVal, int Direction, Top_Ttb_t::iterator itr );
int Top_Synthesis_PseudoCare( tRevNtk& Rev, Top_Ttb_t * pTtb
, int SetVal, int Direction, Top_Ttb_t::iterator itr
, mpz_t& pcin, mpz_t& pcout );
void LogPrint( std::ostream& ostr );

struct ContainTest {
	bool operator()( const mpz_t& sub, const mpz_t& prime ) const {
		mpz_t res;
		int equ;
		mpz_init(res);
		mpz_and( res, sub, prime );
		equ = mpz_cmp( res, sub );
		mpz_clear(res);
		return equ == 0;
	}
};

class Pth { //Pattern Handler
public:
	int pos: 30;
	typedef enum {
		in =0,
		out=1
	} eio;
	eio io : 2;
	Pth( int npos, eio nio ): pos(npos),io(nio){}
};

typedef std::vector<Pth> vPth;	//
typedef std::map<int,vPth> mHW; // Hamming Weight map

inline 
void print_mHW( std::ostream& ostr, mHW& HW, Top_Ttb_t * pTtb ){
	int dim = pTtb->nLine;
	for( mHW::iterator itr = HW.begin(); itr != HW.end(); itr ++ ){
		ostr<<"group "<< itr->first;
		ostr<<": ";
		for( int i=0; i<itr->second.size(); i++ ){
			int pos = itr->second[i].pos;
			Pth::eio io  = itr->second[i].io;
			ostr<<(io==Pth::in?"(i)":"(o)");
			ostr.fill('0');
			ostr.width(dim);
			ostr<< mpz_class( 
				io==Pth::in
				? (*pTtb)[pos].first
				: (*pTtb)[pos].second ).get_str(2) <<" ";
		}
		ostr<<std::endl;
	}
}

#endif

