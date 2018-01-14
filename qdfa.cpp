#include "qdfa.hpp"


int ReadDfa( const char * FileName, tDfa& Dfa ){
	std::ifstream in( FileName, std::ios::in );
	std::string line;
	int nLine;
	int MaxState = 0;
	Dfa.nState = 0;
	Dfa.adj.clear();
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
		MaxState = (src>MaxState)? src: MaxState;
		MaxState = (des>MaxState)? des: MaxState;
		//printf("%d %d %d\n",src,input,des);
	}
	Dfa.nState = MaxState+1;
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
	printf("Rdfq\n");
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


