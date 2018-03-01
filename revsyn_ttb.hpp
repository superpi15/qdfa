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
tRevNtk * TtbToRevNtk();

#endif

