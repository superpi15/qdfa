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
	std::ifstream in( FileName, std::ios::in );
	std::string line;
	int nLine;
	for( nLine = 0; in>>line; nLine++ ){
		if( nLine == 0 ){
			Spec.nBit = line.size();
			Spec.clear();
			Spec.resize( 1<<Spec.nBit );
		}
		if( line.size() != Spec.nBit ){
			printf("Error: inconsistent width at line %d.\n", Spec.nBit );
			return 0;
		}
		Spec[nLine].resize( Spec.nBit );
		for( int i = 0; i<line.size(); i++ ){
			if( line[i] != '0' && line[i] != '1' ){
				printf("Error: illegal bit at line %d.\n", nLine );
				return 0;
			}
			Spec[nLine][i] = (line[i] == '0') ? '0': '1';
		}
	}
	in.close();
	return 1;
}

int main( int argc, char * argv[] ){
	if( argc!=2 )
		return 0;
	tSpec Spec;
	ReadSpec( argv[1], Spec );
}
