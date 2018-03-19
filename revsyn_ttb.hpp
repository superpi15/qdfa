#ifndef REVSYN_TTB_HPP
#define REVSYN_TTB_HPP

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
tRevNtk * TtbToRevNtk();
int Top_Synthesis( tRevNtk& Rev, Top_Ttb_t * pTtb, int SetVal, int Direction, Top_Ttb_t::iterator itr );
int Top_Synthesis_PseudoCare( tRevNtk& Rev, Top_Ttb_t * pTtb
, int SetVal, int Direction, Top_Ttb_t::iterator itr
, mpz_t& pcin, mpz_t& pcout );
void LogPrint( std::ostream& ostr );
#endif

