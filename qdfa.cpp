#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>

class tDfa{
public:
	bool delta( int state, int input, int * pDes );
	typedef std::map<int,int> transition;
	std::map<int,transition> adj;
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


