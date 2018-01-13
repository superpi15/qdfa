#include <iostream> 
#include <vector>
#include <fstream>
#include <string>
class tSpec: public  std::vector<std::vector<bool> > {
public:
	int nBit;
	tSpec():nBit(0){}
};
int ReadSpec( const char * FileName, tSpec& Spec ){
	std::ifstream in( argv[1], std::ios::in );
	
	in.close();
	return 0;
}

int main( int argc, char * argv[] ){
	if( argc!=2 )
		return 0;
	
}
