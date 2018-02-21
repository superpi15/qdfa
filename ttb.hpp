#ifndef TTB_HPP
#define TTB_HPP

#include <vector>
#include <utility>
#include <stdio.h>
#include <gmp.h>
#include <iostream>

using std::vector;

// Tte: Truth Table Entry
class Tte: public std::pair<mpz_t,mpz_t> {
public:
	Tte(){}
	void memory_free(){
		mpz_clear(this->first);
		mpz_clear(this->second);
	}
};

// Ttb: Truth Table
class Top_Ttb_t: public vector<Tte> {
public:
	int nLine;
	Top_Ttb_t( int nnLine = 0 ){
		nLine = nnLine;
	}
	~Top_Ttb_t(){
		for( iterator itr = begin(); itr != end(); itr++ )
			itr->memory_free();
	}
	void memory_clear(){}
	void print( std::ostream& ostr ){
		for( iterator itr = begin(); itr != end(); itr++ ){
			ostr<< std::hex << itr->first<<
			" "<<std::hex<< itr->second<< std::endl;
		}
	}
	void insert_entry( mpz_t input, mpz_t output ){
		resize( size()+ 1 );
		mpz_init_set( back().first , input );
		mpz_init_set( back().second, output );
	}
	void insert_entry( unsigned long int input
					, unsigned long int output ){
		resize( size()+ 1 );
		mpz_init( back().first  );
		mpz_init( back().second );
		mpz_set_ui( back().first , input );
		mpz_set_ui( back().second, output );
	}
	Top_Ttb_t * Duplicate(){
		Top_Ttb_t * pRet = new Top_Ttb_t;
		pRet->nLine = nLine;
		pRet->resize( size() );
		iterator prime, dup;
		prime = begin();
		dup  = pRet->begin();
		for( ; prime != end(); prime ++, dup ++ ){
			mpz_init( dup->first );
			mpz_init( dup->second );
			mpz_set( dup->first , prime->first  );
			mpz_set( dup->second, prime->second );
		}
		return pRet;
	}
	
};

#endif
