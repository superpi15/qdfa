#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <set>

class tDfa{
public:
	typedef std::map<int,int> tTrans;
	typedef std::map<int,tTrans> tAdj;
	bool delta( int state, int input, int * pDes );
	tAdj adj;
};

class tRdfa{
public:
	;
};

class tDfaToRdfa{
public:
	typedef std::map<int,std::set<int> > tFrom;
	typedef std::map<int,tFrom> tSrcLog;
	std::set<int> AlphaBet;
	int maxSrcNum;
	tSrcLog SrcLog; 	// to detect multi-source
	tDfa * pDfa;
	int scan( tDfa& Dfa ){
		pDfa = &Dfa;
		AlphaBet.clear();
		maxSrcNum = 0;
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
	tDfaToRdfa Man;
	Man.scan( Dfa );
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


