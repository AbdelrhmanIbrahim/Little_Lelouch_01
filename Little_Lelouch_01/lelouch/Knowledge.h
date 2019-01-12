#pragma once
#include <string>

using namespace std;

/*
Lelouch knowledge :

Board120 is the array representing board, in chess engines using 1D array to represent boards preferable when using bitboards (64 bit numbers representing all the data on the board)
Board120 contains offboard squares where moves there are illegeal (knight moves back when it's on the first rank) and used for mapping to Board64 using the assigned values in it

Board64 used for mapping between both arrays to use the 64 elements arrays in the program

We hash each position using zobrist hashing (ULL -> PositionData)

Center and ExtendedCenter are bitboards used in evaluation the position 

Enums here used to represent the pieces includeing the empty square,files,ranks and squares and the castlingpermission (castling permission is a 4 bits number each 2 bits represent the permission for castlling for each king (king and queen side))

PiecesHashKeysRandomNumbers array represent 120 random numbers for each piece, used in zobrist hashing after generating random numbers in it

Some arrays to represent the pieces values,color,specifying each piece..you edit your values while coding and testing your engine and the same for your evaluation functions

AllDirsSizes and AllDirs are used in generating moves for each piece

HistoryMoves array used in alpha-beta search for optimizing (array saves the best squares for each piece in the board while searching)

AllPiecesTable is the positionsal score array for each piece..for example : pawns (wp/bp) are encourged to push forward and specially holding the center and we take that into account while writing evaluaiton functions,
kings got middlegame and endgame arrays (to be safe in the corner while the board is full), to start move while the game is about to end

BitReverseTable256 is a lookup table for reversing bytes (this is used in the hyperbola quintessence (logical operations used for finding all the attacked squares bitboard for a sliding piece(queen,rook,bishop) )
*/

extern enum pieces{Empty,WhitePawn,WhiteKnight,WhiteBishop,WhiteRook,WhiteQueen,WhiteKing,BlackPawn,BlackKnight,BlackBishop,BlackRook,BlackQueen,BlackKing};
extern enum files{FileA,FileB,FileC,FileD,FileE,FileF,FileG,FileH,FileNone};
extern enum ranks{Rank1,Rank2,Rank3,Rank4,Rank5,Rank6,Rank7,Rank8,RankNone};
extern enum sides{White,Black,Both};
extern enum squares   
{
	A1=21,B1,C1,D1,E1,F1,G1,H1,
	A2=31,B2,C2,D2,E2,F2,G2,H2,
	A3=41,B3,C3,D3,E3,F3,G3,H3,
	A4=51,B4,C4,D4,E4,F4,G4,H4,
	A5=61,B5,C5,D5,E5,F5,G5,H5,
	A6=71,B6,C6,D6,E6,F6,G6,H6,
	A7=81,B7,C7,D7,E7,F7,G7,H7,
	A8=91,B8,C8,D8,E8,F8,G8,H8,NoneSquare
};
extern enum castle{WhiteKingSideCasstling=1,WhiteQueenSideCasstling=2,BlackKingSideCasstling=4,BlackQueenSideCasstling=8};

extern const string NAME;
extern const int MATE;
extern const int NegINF;
extern const int PosINF;
extern const int Board120ArraySize;
extern const int Board64ArraySize;
extern const int NumberOfPossiblePieces;
extern const int MaxPossiblemNumberForOnePiece;
extern const int MAX_DEPTH;
extern const int OffBoard;
extern const int PushingPawnBonusEndGame;
extern const unsigned long long Center,ExtendedCenter;

extern string last;

extern const int Board120[120];
extern const int Board64[64];
extern const int SquaresAndTheirRanks[64];
extern const int SquaresAndTheirFiles[64];
extern const int cPiecesValues[13];
extern const int PiecesColor[13];
extern const int IsRookQueen[13];
extern const int IsKnight[13];
extern const int IsBishopQueen[13];
extern const int IsKing[13];
extern const int PiecesSLides[13];

//Moves
extern const int AllDirsSizes[13];
extern const int AllDirs[13][8];

//Pieces Evaluation Table
extern const short cAllPiecesTable[13][64];
extern const short WhiteKingTableEndGame[64];
extern const short BlackKingTableEndGame[64];

extern const short BitReverseTable256[];

//Ranks 
extern const long long  RanksBitBoard[];
//Files 
extern const long long  FilesBitBoard[];

//Diagonals
extern const long long DiagonalBitBoard[];
//AntiDiagonals
extern const long  long AntiDiagonalBitBoard[];
