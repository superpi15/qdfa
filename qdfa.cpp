#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <cmath>

class tDfa{
public:
	typedef std::map<int,int> tTrans;
	typedef std::map<int,tTrans> tAdj;
	bool delta( int state, int input, int * pDes );
	tAdj adj;

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
	;
};

class tDfaToRdfa{
public:
	typedef std::map<int,std::set<int> > tFrom;
	typedef std::map<int,tFrom> tSrcLog;
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
		tRdfa * pRdfa;
		pRdfa = new tRdfa;
		typedef std::set<std::pair<int,int> > tOpSet;
		typedef std::map<int,tOpSet> tOpMap;
		tOpMap OpMap;
		for( tSrcLog::iterator itr = SrcLog.begin(); itr != SrcLog.end(); itr++  ){
			int des = itr->first;
			tFrom& From = itr->second;
			for( tFrom::iterator fitr = From.begin();
				fitr != From.end(); fitr++ ){
				if( fitr->second.size()<2 )
					continue;
				int input = fitr->first;
				int counter = 0;
				std::set<int>::iterator srcItr;
				for( srcItr = fitr->second.begin(); 
					srcItr != fitr->second.end(); 
					srcItr++, counter++ ){
					int ExtInput;
					ExtInput = input + (counter<<AlphaBetBitNum);
					pRdfa->adj[*srcItr];
					pRdfa->adj[*srcItr][ExtInput] = des;
					OpMap[ExtInput]; 
					OpMap[ExtInput].insert( 
						std::pair<int,int>(*srcItr,des) );
				}
			}
		}
		// reversible transition
		for( tOpMap::iterator itr = OpMap.begin(); itr!=OpMap.end(); itr++ ){
			;

		}
		return pRdfa;
	}
};

int ReadDfa( const char * FileName, tDfa& Dfa ){
	std::ifstream in( FileName, std::ios::in );
	std::string line;
	int nLine;
	for( nLine = 0; std::getline(in,line); nLine++ ){
		int src, input, des;
		std::istringstream istr(line);
		//printf("%s\n",line.c_str());
		if( !(istr>>src>>input>>des) ){
			printf("Error: read dfa failed on line %d.\n",nLine);
			return 0;
		}
		Dfa.adj[src];
		Dfa.adj[src][input] = des;
		//printf("%d %d %d\n",src,input,des);
	}
	return 1;
}

int main( int argc, char * argv[] ){
	if( argc!=2 )
		return 0;
	tDfa Dfa;
	ReadDfa( argv[1], Dfa );
	Dfa.print( std::cout );
	tDfaToRdfa Man;
	Man.scan( Dfa );
	tRdfa * pRdfa = Man.convert();
	pRdfa->print( std::cout );
	delete pRdfa;
	printf("AlphaBet# %d\n", Man.AlphaBet.size() );
	printf("maxSrcNum %d\n", Man.maxSrcNum );
	//tRevNtk RevNtk;
	//ReversibleBasic( Spec, RevNtk );
	//RevNtk.print(std::cout);
	/*
	for( tRevNtk::iterator itr = RevNtk.begin(); itr!=RevNtk.end(); itr++ ){
		for( int i = 0; i<itr->size(); i++ ){
			printf("%c",(*itr)[i]);
		}
		printf("\n");
	}*/
}


