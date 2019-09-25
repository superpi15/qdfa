#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <assert.h>
#include <gmpxx.h>
#include <iomanip>

#include "revsyn_ttb.hpp"
#include "revsyn_qca.hpp"

int ForceMap( std::vector<bool>& CareMask, std::vector<int>& Output ){
	std::vector<int> OAssign;
	std::set<int> Unused;
	
	OAssign.resize( Output.size(), -1 );
	for( int i=0; i<Output.size(); i++ ){
		if( CareMask[i] )
			OAssign[i] = Output[i];
		else
			Unused.insert( Output[i] );
	}

	std::set<int>::iterator itr;
	for( int i=0; i<Output.size(); i++ ){
		if( CareMask[i] )
			continue;
		itr = Unused.find(i);
		if( itr == Unused.end() )
			continue;
		OAssign[i] = i;
		Unused.erase(i);
	}

	itr = Unused.begin();
	for( int i=0; i<Output.size(); i++ ){
		if( CareMask[i] )
			continue;
		if( OAssign[i] != -1 )
			continue;
		if( itr == Unused.end() )
			continue;
		OAssign[i] = *itr;
		itr++;
	}
	assert( itr == Unused.end() );

	for( int i=0; i<Output.size(); i++ ){
		assert( OAssign[i]>=0 );
		if( CareMask[i] ){
			assert( OAssign[i] == Output[i] );
		}
	}
	Output = OAssign;
}

void Write( int dim, std::ostream& ostr, std::vector<int>& Output ){
	ostr<< dim<<" 2\n";
	for( int i=0; i<Output.size(); i++ ){
		ostr.fill('0');
		ostr.width(dim);
		ostr<< mpz_class( i );
		ostr<<" ";
		ostr.fill('0');
		ostr.width(dim);
		ostr<< mpz_class( Output[i] );
		ostr<<std::endl;
	}
}

int TestSize2Index( tRevNtk * (*Func)(Top_Ttb_t * ), Top_Ttb_t * pTtb ){
	tRevNtk * pRev = Func(pTtb);
	int index = pRev->size();
	delete pRev;
	return index;
}

int enumk(){
	size_t dim = 3;
	size_t size = 1<<dim;
	size_t comb = 1<<size;
	std::vector<int> domain(size);
	for( int i=0; i<size; i++ )
		domain[i] = i;
	char * fprefix = NULL;
//	if( argc>1 )\
		fprefix = argv[1];
	int count = 0;
	std::reverse( domain.begin(), domain.end() );

	typedef std::map<int,int> I2I_t;
	
	I2I_t   BidNum; 	// Top_TtbToRev_Bi
	I2I_t   GdbNum; 	// Top_TtbToRev_Bi_Core
	I2I_t  GdblNum; 	// Top_GBDL
	I2I_t  qGdbNum; 	// Top_TtbToRev_GBD_qca
	I2I_t qGdblNum; 	// Top_TtbToRev_GBDL_qca

	do {
		count++;
		std::cout<<"\r"<<"Permu= "<< count ;
		fflush(stdout);
		for( int i=0; i<comb; i++ ){
//			if( i == 0 ) continue;	// No DC, the same as 1 DC
//			if( i == comb-1 ) continue; // all DC
			std::vector<bool> CareSet( size );
			for( int j=0; j<size; j++ ){
				CareSet[j] = (i&(1<<j))? false: true;
			}
			std::vector<int> Output;
			Output = domain;
			if( i == comb-1 )
				ForceMap( CareSet, Output );
			//std::cout<< count<< "_"<< i<<std::endl;

			//std::cout << (fname +".cmp").c_str()<<std::endl;
			//if( fprefix ){
			if( true ){
//				std::string fname;
//				fname = fprefix;
//				fname+= "/";
//				fname+= mpz_class(count).get_str(10) + "_";
//				fname+= mpz_class(i).get_str(10);

//				std::fstream ostr1( (fname+".cmp").c_str(), std::ios::out );
				std::ostringstream ostr1;
				ostr1 << dim<<" 2"<<std::endl;
				for( int k=0; k<Output.size(); k++ ){
					ostr1.fill('0');
					ostr1.width(dim);
					ostr1 << mpz_class(k).get_str(2) <<" ";
					ostr1.fill('0');
					ostr1.width(dim);
					ostr1 << mpz_class(Output[k]).get_str(2)<<std::endl;
				}
//				ostr1.close();
				//std::cout<<"Permu="<< count <<", Comb="<<i<<std::endl;\
				std::cout<< ostr1.str() <<std::endl;


//				std::fstream ostr2( (fname+".incmp").c_str(), std::ios::out );
				std::ostringstream ostr2;
				ostr2 << dim<<" 2"<<std::endl;
				for( int k=0; k<Output.size(); k++ ){
					if( !CareSet[k] )
						continue;
					ostr2.fill('0');
					ostr2.width(dim);
					ostr2 << mpz_class(k).get_str(2) <<" ";
					ostr2.fill('0');
					ostr2.width(dim);
					ostr2 << mpz_class(Output[k]).get_str(2) <<std::endl;
				}
				//ostr2.close();
				//std::cout << ostr2.str() <<std::endl;
				//TestSize2Index();
				std::istringstream istr1( ostr1.str() );
				std::istringstream istr2( ostr2.str() );
				Top_Ttb_t * pTtb1 = Ttb_ReadSpecStream( istr1 );
				Top_Ttb_t * pTtb2 = Ttb_ReadSpecStream( istr2 );
	
				  BidNum[ TestSize2Index(Top_TtbToRev_Bi      , pTtb1) ]++; 	// Top_TtbToRev_Bi       
				  GdbNum[ TestSize2Index(Top_TtbToRev_Bi_Core , pTtb2) ]++; 	// Top_TtbToRev_Bi_Core  
				 GdblNum[ TestSize2Index(Top_GBDL             , pTtb2) ]++; 	// Top_GBDL              
				 qGdbNum[ TestSize2Index(Top_TtbToRev_GBD_qca , pTtb2) ]++; 	// Top_TtbToRev_GBD_qca  
				qGdblNum[ TestSize2Index(Top_TtbToRev_GBDL_qca, pTtb2) ]++; 	// Top_TtbToRev_GBDL_qca 
				delete pTtb1;
				delete pTtb2;
			}
		}
	} while( std::prev_permutation( domain.begin(), domain.end() ) );
	std::cout<<std::endl;

	#define OBS(TARGET)\
	std::cout<< "TARGET: "<< #TARGET <<std::endl;\
	for( I2I_t::iterator itr = TARGET.begin(); itr!=TARGET.end(); itr++ )\
		std::cout<<std::setw(10)<< itr->first <<" : "<<std::setw(10)<< itr->second<<std::endl;
OBS(  BidNum);
OBS(  GdbNum);
OBS( GdblNum);
OBS( qGdbNum);
OBS(qGdblNum);

	#undef TARGET
	//std::cout << "count="<<count<<std::endl;
}


int enumk2(){
	size_t dim = 3;
	size_t size = 1<<dim;
	size_t comb = 1<<size;
	int count = 0;
	typedef std::map<int,int> I2I_t;
	
	I2I_t   BidNum; 	// Top_TtbToRev_Bi
	I2I_t   GdbNum; 	// Top_TtbToRev_Bi_Core
	I2I_t  GdblNum; 	// Top_GBDL
	I2I_t  qGdbNum; 	// Top_TtbToRev_GBD_qca
	I2I_t qGdblNum; 	// Top_TtbToRev_GBDL_qca

	for(int i=1; i<=size; i++){ // number of care-term
		count++;
		std::cout<<"\r"<<"Permu= "<< count ;
		fflush(stdout);
		for( int c1=0; c1<comb; c1++ ){
			std::vector<bool> CareSet( size );
			int nCare = 0;
			for( int j=0; j<size && nCare <= i; j++ ){
				if( (c1&(1<<j))? true: false ){
					CareSet[j] = true;
					nCare ++ ;
				}
			}
			if( nCare != i )
				continue;


			for( int c2=0; c2<comb; c2++ ){
				std::vector<bool> CareSetOut( size );
				int nCareOut = 0;
				for( int j=0; j<size && nCareOut <= i; j++ ){
					if( (c2&(1<<j))? true: false ){
						CareSetOut[j] = true;
						nCareOut ++ ;
					}
				}
				if( nCareOut != i )
					continue;

				assert( CareSetOut.size()==CareSet.size() );

				std::vector<int> domain, dcset;
				for(int j=0; j<CareSetOut.size(); j++)
					if( CareSetOut[j] )
						domain.push_back(j);
					else
						dcset .push_back(j);

				int count = 0;
				std::reverse( domain.begin(), domain.end() );
				do {
					int nOutTop = 0;
					std::vector<int> vMapping(size,-1);
					for(int j=0; j<size; j++){
						if( !CareSet[j] )
							continue;
						assert(nOutTop<size);
						vMapping[j] = domain[nOutTop++];
					}
					int nAssginDC = 0;
					for(int j=0; j<size; j++){
						if( -1<vMapping[j] )
							continue;
						vMapping[j] = dcset[nAssginDC++];
					}
					ForceMap(CareSet, vMapping);

//					std::cout<<"domain size= "<< domain.size()<<std::endl;\
					for(int j=0; j<size; j++)\
						std::cout<<(CareSet[j]?"CARE":"  DC")<<" - "<<(CareSetOut[j]?"CARE":"  DC")<<": "<< vMapping[j]<<std::endl;
//					std::cout<<std::endl;
//					for(int j=0; j<dcset.size(); j++)
//						std::cout<<"dc "<< dcset[j]<<std::endl;
//					std::cout<<std::endl;
//					for(int j=0; j<domain.size(); j++)
//						std::cout<<"domain "<< domain[j]<<std::endl;
//					std::cout<<std::endl;

//					std::string fname;
//					fname = fprefix;
//					fname+= "/";
//					fname+= mpz_class(count).get_str(10) + "_";
//					fname+= mpz_class(i).get_str(10);
//					std::fstream ostr1( (fname+".cmp").c_str(), std::ios::out );
					std::ostringstream ostr1;
					ostr1 << dim<<" 2"<<std::endl;
					for( int k=0; k<vMapping.size(); k++ ){
						ostr1.fill('0');
						ostr1.width(dim);
						ostr1 << mpz_class(k).get_str(2) <<" ";
						ostr1.fill('0');
						ostr1.width(dim);
						ostr1 << mpz_class(vMapping[k]).get_str(2)<<std::endl;
					}
//					ostr1.close();
//					std::cout<<"Permu="<< count <<", Comb="<<i<<std::endl;\
					std::cout<< ostr1.str() <<std::endl;


//					std::fstream ostr2( (fname+".incmp").c_str(), std::ios::out );
					std::ostringstream ostr2;
					ostr2 << dim<<" 2"<<std::endl;
					for( int k=0; k<vMapping.size(); k++ ){
						if( !CareSet[k] )
							continue;
						ostr2.fill('0');
						ostr2.width(dim);
						ostr2 << mpz_class(k).get_str(2) <<" ";
						ostr2.fill('0');
						ostr2.width(dim);
						ostr2 << mpz_class(vMapping[k]).get_str(2) <<std::endl;
					}
					//ostr2.close();
//					std::cout << ostr2.str() <<std::endl;
					//TestSize2Index();
					std::istringstream istr1( ostr1.str() );
					std::istringstream istr2( ostr2.str() );
					Top_Ttb_t * pTtb1 = Ttb_ReadSpecStream( istr1 );
					Top_Ttb_t * pTtb2 = Ttb_ReadSpecStream( istr2 );
		
					  BidNum[ TestSize2Index(Top_TtbToRev_Bi      , pTtb1) ]++; 	// Top_TtbToRev_Bi       
					  GdbNum[ TestSize2Index(Top_TtbToRev_Bi_Core , pTtb2) ]++; 	// Top_TtbToRev_Bi_Core  
					 GdblNum[ TestSize2Index(Top_GBDL             , pTtb2) ]++; 	// Top_GBDL              
					 qGdbNum[ TestSize2Index(Top_TtbToRev_GBD_qca , pTtb2) ]++; 	// Top_TtbToRev_GBD_qca  
					qGdblNum[ TestSize2Index(Top_TtbToRev_GBDL_qca, pTtb2) ]++; 	// Top_TtbToRev_GBDL_qca 
					delete pTtb1;
					delete pTtb2;




				} while( std::prev_permutation( domain.begin(), domain.end() ) );
			}

			
		}
	}

	std::cout<<std::endl;

	#define OBS(TARGET)\
	std::cout<< "TARGET: "<< #TARGET <<std::endl;\
	for( I2I_t::iterator itr = TARGET.begin(); itr!=TARGET.end(); itr++ )\
		std::cout<<std::setw(10)<< itr->first <<" : "<<std::setw(10)<< itr->second<<std::endl;
OBS(  BidNum);
OBS(  GdbNum);
OBS( GdblNum);
OBS( qGdbNum);
OBS(qGdblNum);

	#undef TARGET
}


