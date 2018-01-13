#include "revsyn.hpp"

int ReversibleBasic( tSpec& Spec, tRevNtk& RevNtk ){
	tSpec * pSpecCur , * pSpecPrev;
	pSpecCur = new tSpec;
	pSpecPrev = new tSpec;
	*pSpecCur = Spec;
	*pSpecPrev = Spec;
	//= Spec;
	std::list<std::vector<char> >& RevTmp = RevNtk;
	int LineNum, BitNum;
	LineNum = Spec.size();
	BitNum 	= Spec.front().size();
	// let LocalSpec become ordered binary number
	for( int nLine = 0; nLine<LineNum; nLine++ ){
		for( int tarBit = 0; tarBit<BitNum; tarBit++ ){
			if( (nLine & (1<<tarBit)) ^ ! ((*pSpecCur)[nLine][tarBit] == 1 ) )
				continue;
			// encode
			RevTmp.push_front( tRevNtk::value_type() );
			RevTmp.front().resize( BitNum, '-' );
			RevTmp.front()[tarBit] = 'X';
			std::vector<bool> vDetect( BitNum, 0  );
			for( int cond = 0; cond<BitNum; cond++ ){
				if( cond == tarBit )
					continue;
				if( (*pSpecCur)[nLine][cond] == 1 ){
					RevTmp.front()[cond] = '+';
					vDetect[cond] = 1;
				}
			}
			// flip
			for( int jLine = nLine; jLine<LineNum; jLine++ ){
				bool Reject = 0;
				for( int kBit = 0; kBit<BitNum; kBit++ ){
					if( kBit == tarBit )
						continue;
					if( !vDetect[kBit] )
						continue;
					if( (*pSpecCur)[jLine][kBit] == 0 ){
						Reject = 1;
						break;
					}
				}
				if( !Reject )
					(*pSpecCur)[jLine][tarBit] ^= 1;
			}
			* pSpecPrev = * pSpecCur;
			//pSpecCur->print(std::cout); printf("\n");
		}
	}
	delete pSpecCur;
	delete pSpecPrev;
}

int ReadSpec( const char * FileName, tSpec& Spec ){
	std::ifstream in( FileName, std::ios::in );
	std::string line;
	int nLine;
	for( nLine = 0; std::getline(in,line); nLine++ ){
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
			Spec[nLine][i] = (line[i] == '0') ? 0: 1;
			//printf("%d",(int)Spec[nLine][i]);
		}
		//printf("\n");
	}
	in.close();
	return 1;
}

int main( int argc, char * argv[] ){
	if( argc!=2 )
		return 0;
	tSpec Spec;
	ReadSpec( argv[1], Spec );
	tRevNtk RevNtk;
	ReversibleBasic( Spec, RevNtk );
	RevNtk.print(std::cout);
	/*
	for( tRevNtk::iterator itr = RevNtk.begin(); itr!=RevNtk.end(); itr++ ){
		for( int i = 0; i<itr->size(); i++ ){
			printf("%c",(*itr)[i]);
		}
		printf("\n");
	}*/
}
