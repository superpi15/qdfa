#ifndef QDFA_HPP
#define QDFA_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <assert.h>

class tDfa{
public:
	typedef std::map<int,int> tTrans;
	typedef std::map<int,tTrans> tAdj;
	bool delta( int state, int input, int * pDes );
	tAdj adj;
	int nState;

	tDfa(){
		nState = 0;
	}

	void print( std::ostream& ostr){
		for( tAdj::iterator iItr = adj.begin(); iItr != adj.end(); iItr++ ){
			tTrans& Paths = iItr->second;
			for( tTrans::iterator jItr = Paths.begin(); 
				jItr != Paths.end(); jItr++ ){
				printf("%d %d %d\n",iItr->first, jItr->first, jItr->second);
			}
		}
	}
};

class tRdfa: public tDfa{
public:
	typedef std::map<int,std::vector<int> > tSynonym;
	typedef std::set<std::pair<int,int> > tVirtualPath;
	typedef std::set<std::pair<int,int> > tRealPath;
	tSynonym synonym;
	tVirtualPath virtual_path;
	tRealPath real_path;
	void print( std::ostream& ostr ){
		//((tDfa*)this)->print( ostr );
		for( tAdj::iterator iItr = adj.begin(); iItr != adj.end(); iItr++ ){
			tTrans& Paths = iItr->second;
			for( tTrans::iterator jItr = Paths.begin(); 
				jItr != Paths.end(); jItr++ ){
				int src, input, des;
				src = iItr->first;
				input = jItr->first;
				des = jItr->second;
				printf("%d %d %d %c"
					,src, input, des, 
					real_path.find(std::pair<int,int>(src,input))
					!=real_path.end()? '+':' ');
				printf("\n");
			}
		}

		if( !synonym.empty() ){
			printf("Synonym:\n");
			for( tSynonym::iterator itr = synonym.begin(); 
				itr!= synonym.end(); itr++ ){
				printf("%d: ", itr->first);
				for( int i = 0; i < itr->second.size(); i++ )
					printf("%d ", itr->second[i] );
				printf("\n");
			}
		}
	}
};

class tDfaToRdfa{
public:
	typedef std::map<int,std::set<int> > tFrom;
	typedef std::map<int,tFrom> tSrcLog;
	// Add virtual path
	typedef std::set<std::pair<int,int> > tOpSet;
	typedef std::map<int,tOpSet> tOpMap;
	// Data structure 
	std::set<int> AlphaBet;
	int maxSrcNum;
	int ExtraBitNum;
	int AlphaBetBitNum;
	tSrcLog SrcLog; 	// to detect multi-source
	tDfa * pDfa;
	int scan( tDfa& Dfa ){
		pDfa = &Dfa;
		AlphaBet.clear();
		maxSrcNum = 0;
		ExtraBitNum = 0;
		AlphaBetBitNum = 0;
		SrcLog.clear();
		tDfa::tAdj::iterator iItr;
		tDfa::tTrans::iterator jItr;
		for( iItr = pDfa->adj.begin(); iItr != pDfa->adj.end(); iItr++ ){
			int src;
			src = iItr->first;
			tDfa::tTrans& Paths =  iItr->second;
			for( jItr = Paths.begin(); jItr != Paths.end(); jItr++ ){
				int input, des;
				input 	= jItr->first;
				des 	= jItr->second;
				AlphaBet.insert(input);
				SrcLog[des];
				SrcLog[des][input].insert(src);
			}
		}
		for( tSrcLog::iterator itr = SrcLog.begin(); itr != SrcLog.end(); itr++  ){
			tFrom& From = itr->second;
			for( tFrom::iterator fitr = From.begin();
				fitr != From.end(); fitr++ ){
				maxSrcNum =  fitr->second.size()>maxSrcNum?
					fitr->second.size(): maxSrcNum;
			}
		}
		AlphaBetBitNum = log(AlphaBet.size())/log(2);
		ExtraBitNum = log(maxSrcNum)/log(2);
		return 1;
	}

	tRdfa * convert(){
		;
		tRdfa * pRdfa;
		pRdfa = new tRdfa;
		pRdfa->nState = pDfa->nState;
		tOpMap OpMap;
		//return pRdfa;
		for( tSrcLog::iterator itr = SrcLog.begin(); itr != SrcLog.end(); itr++  ){
			int des = itr->first;
			tFrom& From = itr->second;
			for( tFrom::iterator fitr = From.begin();
				fitr != From.end(); fitr++ ){
				int input = fitr->first;
				int counter = 0;
				std::set<int>::iterator srcItr;
				for( srcItr = fitr->second.begin(); 
					srcItr != fitr->second.end(); 
					srcItr++, counter++ ){
					int ExtInput;
					ExtInput = input + (counter<<AlphaBetBitNum);
					//printf("(%d %d %d %d \n" \
						,*srcItr,input,ExtInput,des);
					//pRdfa->adj[*srcItr];
					pRdfa->adj[*srcItr][ExtInput] = des;
					//if( fitr->second.size()<2 )
					//	continue;
					if( counter != 0)
						pRdfa->synonym[input].push_back(ExtInput);
					//OpMap[ExtInput]; 
					pRdfa->real_path.insert(
						std::pair<int,int>(*srcItr,ExtInput));
					OpMap[ExtInput].insert( 
						std::pair<int,int>(*srcItr,des) );
				}
			}
		}
		// add virtual path
		for( tOpMap::iterator itr = OpMap.begin(); itr != OpMap.end(); itr++ ){
			
			AddVirtualPath( itr->first, itr->second, pRdfa);
		}
		return pRdfa;
	}

	int AddVirtualPath( int input, tOpSet& OpSet, tRdfa * pRdfa ){
		std::vector<int> HasIn, HasOut, InUse;
		HasIn .resize( pRdfa->nState, 0 );
		HasOut.resize( pRdfa->nState, 0 );
		InUse .resize( pRdfa->nState, 0 );
		for( tOpSet::iterator itr = OpSet.begin(); itr != OpSet.end(); itr++ ){
			//printf("OpSet %d: %d %d\n",input,itr->first,itr->second);
			InUse[itr->first ] = 1;
			InUse[itr->second] = 1;
			HasOut[itr->first ] = 1;
			HasIn [itr->second] = 1;
		}
		std::set<int> OnlyIn, OnlyOut;
		for( int i=0; i<pRdfa->nState; i++  ){
			//printf("On input %d, state %d/%d: %d %d\n", input, i, pDfa->nState, HasIn[i], HasOut[i] );
			if( HasIn[i] == 0 && HasOut[i] == 0 ){
				pRdfa->adj[i][input] = i;
			}
			if( HasIn[i] == 0 && HasOut[i] != 0 )
				OnlyOut.insert(i);
			if( HasIn[i] != 0 && HasOut[i] == 0 )
				OnlyIn .insert(i);
		}
		assert( OnlyIn.size() == OnlyOut.size() );
		std::set<int>::iterator inItr, outItr;
		inItr  = OnlyIn .begin();
		outItr = OnlyOut.begin();
		
		for( ; inItr!=OnlyIn.end() || outItr!=OnlyOut.end(); inItr++, outItr++ ){
			pRdfa->adj[*inItr][input] = *outItr;
		}
	}
};


#endif

