
#pragma once
#include<unordered_map>
#include<vector>
#include<string>

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

const string NAME="Little_Lelouch_01";

const int  MATE=1000000;
const int  NegINF=-2000000;
const int  PosINF=2000000;
const int  Board120ArraySize=120;
const int  Board64ArraySize=64;
const int  NumberOfPossiblePieces=13;
const int  MaxPossiblemNumberForOnePiece=10;
const int  MAX_DEPTH=20;

int Board120[Board120ArraySize];
int Board64[Board64ArraySize];	
unsigned long long CastleKeys[16];
unsigned long long Center=0x0000001818000000,ExtendedCenter=0x00003C3C3C3C0000;
int OffBoard=-1,NodesNumber=0,PushingPawnBonusEndGame=35;
long NODES=0;
float ALLTIME=0;
bool Out=false,FirstTime=false,NewGame=true,Right=true,once=false;
string FENstring="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",last="";


enum  {Empty,WhitePawn,WhiteKnight,WhiteBishop,WhiteRook,WhiteQueen,WhiteKing,BlackPawn,BlackKnight,BlackBishop,BlackRook,BlackQueen,BlackKing};
enum  {FileA,FileB,FileC,FileD,FileE,FileF,FileG,FileH,FileNone};
enum  {Rank1,Rank2,Rank3,Rank4,Rank5,Rank6,Rank7,Rank8,RankNone};
enum  {White,Black,Both};
enum  
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
enum  {WhiteKingSideCasstling=1,WhiteQueenSideCasstling=2,BlackKingSideCasstling=4,BlackQueenSideCasstling=8};
	
 unsigned long long PiecesHashKeysRandomNumbers[NumberOfPossiblePieces][Board120ArraySize];
 int	PiecesValues[13]={0,100,390,390,550,1100,5000,100,390,390,550,1100,5000};
 int PiecesColor[13]={Both,White,White,White,White,White,White,Black,Black,Black,Black,Black,Black};
 int IsRookQueen[13]={0,0,0,0,1,1,0,0,0,0,1,1,0};
 int IsKnight[13]={0,0,1,0,0,0,0,0,1,0,0,0,0};
 int IsBishopQueen[13]={0,0,0,1,0,1,0,0,0,1,0,1,0};
 int IsKing[13]={0,0,0,0,0,0,1,0,0,0,0,0,1};
 int PiecesSLides[13]={0,0,0,1,1,1,0,0,0,1,1,1,0};
 int SquaresAndTheirRanks[Board64ArraySize];
 int SquaresAndTheirFiles[Board64ArraySize];
 
 //Moves
 int AllDirsSizes[13]={0,0,8,4,4,8,8,0,8,4,4,8,8};
 int AllDirs[13][8]=
 {
 {0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0},
 {-8,-19,-21,-12,8,19,21,12},
 {-9,-11,9,11,0,0,0,0},
 {-1,-10,1,10,0,0,0,0},
 {-1,-10,1,10,-9,-11,9,11},
 {-1,-10,1,10,-9,-11,9,11},
 {0,0,0,0,0,0,0,0},
 {-8,-19,-21,-12,8,19,21,12},
 {-9,-11,9,11,0,0,0,0},
 {-1,-10,1,10,0,0,0,0},
 {-1,-10,1,10,-9,-11,9,11},
 {-1,-10,1,10,-9,-11,9,11},
 };

 short HistoryMoves[13][64];


//Pieces Evaluation Table
short AllPiecesTable[13][64]=
{
	//empty
{
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
}
,
	//wp
{
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10,-25,-25, 10, 10,  5,
    5, 10,-10,  0,  0,-10, -5,  5,
    0,  0,  0, 25, 25,  0,  0,  0,
    5,  5, 10, 27, 27, 10,  5,  5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0,  0,  0,  0,  0,  0,  0,  0,
    
}
,
	//wn
{
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-20,-30,-30,-20,-40,-50,
     
}
,
	//wb
{
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-40,-10,-10,-40,-10,-20

}
,
	//wr
{
	0,  0,  5,  15,  15,  5,  0,  0,
	0,  0,  5,  10,  10,  5,	 0,	0,
	0,  0,  5,  10,  10,  5,	 0,	0,
	0,  0,  5,  10,  10,  5,	 0,	0,
	0,  0,  5,  10,  10,  5,  0,	0,
	0,  0,  5,  10,  10,  5,  0,	0,
	25,25, 25,  25,  25, 25, 25, 25,
	0,  0,  5,  10,  10,  5,  0,  0		
}
,
	//wq 
{
    -10,-10,-10, 0, 0, -10,-10,-10,
	0,  0,  0,  10, 10, 0,  0,  0,
	0,  0,  10, 15, 15, 10, 0,  0,
	0,  10, 15, 20, 20,	15, 10, 0,
	0,  10, 15, 20, 20,	15, 10, 0,
	0,  0,  10, 15, 15, 10, 0,  0,
	0,  0,  0,  10, 10, 0,  0,  0,
	0,  0,  0,  0,	 0,  0,  0,  0	
}
,
	//wk
{
   20,  30,  10,   0,   0,  10,  30,  20,
   20,  20,   0,   0,   0,   0,  20,  20,
  -10, -20, -20, -20, -20, -20, -20, -10, 
  -20, -30, -30, -40, -40, -30, -30, -20,
  -30, -40, -40, -50, -50, -40, -40, -30,
  -30, -40, -40, -50, -50, -40, -40, -30,
  -30, -40, -40, -50, -50, -40, -40, -30,
  -30, -40, -40, -50, -50, -40, -40, -30
}
,
	//bp
{
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 27, 27, 10,  5,  5,
    0,  0,  0, 25, 25,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-25,-25, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
    
}
,
	//bn
{
    
    -50,-40,-20,-30,-30,-20,-40,-50
    -40,-20,  0,  5,  5,  0,-20,-40,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
}
,
	//bb
{
	-20,-10,-40,-10,-10,-40,-10,-20
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,  
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20	
}
,
	//br
{
	0,  0,  5,  10,  10,  5,  0,  0,
	25,  25,  25,  25,  25,  25,  25,  25,
	0,  0,  5,  10,  10,  5,  0,	0,
	0,  0,  5,  10,  10,  5,  0,	0,
	0,  0,  5,  10,  10,  5,  0,	0,
	0,  0,  5,  10,  10,  5,  0,	0,
	0,  0,  5,  10,  10,  5,  0,	0,
	0,  0,  5,  15,  15,  5,  0,	0
}
,
	//bq
{	
	0,  0,  0,  0,	 0,  0,  0,  0,
	0,  0,  0,  10, 10, 0,  0,  0,
	0,  0,  10, 15, 15, 10, 0,  0,
	0,  10, 15, 20, 20,	15, 10, 0,
	0,  10, 15, 20, 20,	15, 10, 0,
	0,  0,  10, 15, 15, 10, 0,  0,
	0,  0,  0,  10, 10, 0,  0,  0,
    -10,-10,-10, 0, 0, -10,-10,-10
	
}
,
	//bk
{
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-20, -30, -30, -40, -40, -30, -30, -20,
	-10, -20, -20, -20, -20, -20, -20, -10, 
	 20,  20,   0,   0,   0,   0,  20,  20,
	 20,  30,  10,   0,   0,  10,  30,  20
}
};

short WhiteKingTableEndGame[64] =
{
	-50,-40,-30,-20,-20,-30,-40,-50,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-30,  0,  0,  0,  0,-30,-30,
	-50,-30,-30,-30,-30,-30,-30,-50
};

short BlackKingTableEndGame[64] =
{
	-50,-30,-30,-30,-30,-30,-30,-50,
	-30,-30,  0,  0,  0,  0,-30,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-50,-40,-30,-20,-20,-30,-40,-50
};


short BitReverseTable256[] = 
{
  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
}; 

//Ranks 
long long  RanksBitBoard[] =
    {
        0xFFLL, 0xFF00LL, 0xFF0000LL, 0xFF000000LL, 0xFF00000000LL, 0xFF0000000000LL, 0xFF000000000000LL, 0xFF00000000000000LL
    };

//Files 
long long  FilesBitBoard[] =
    {
        0x101010101010101LL, 0x202020202020202LL, 0x404040404040404LL, 0x808080808080808LL,
        0x1010101010101010LL, 0x2020202020202020LL, 0x4040404040404040LL, 0x8080808080808080LL
    };



//Diagonals
 long long DiagonalBitBoard[] =
    {
	0x1L, 0x102L, 0x10204L, 0x1020408L, 0x102040810L, 0x10204081020L, 0x1020408102040L,
	0x102040810204080L, 0x204081020408000L, 0x408102040800000L, 0x810204080000000L,
	0x1020408000000000L, 0x2040800000000000L, 0x4080000000000000L, 0x8000000000000000L
    };

 //AntiDiagonals
 long  long AntiDiagonalBitBoard[] =
    {
	0x80L, 0x8040L, 0x804020L, 0x80402010L, 0x8040201008L, 0x804020100804L, 0x80402010080402L,
	0x8040201008040201L, 0x4020100804020100L, 0x2010080402010000L, 0x1008040201000000L,
	0x804020100000000L, 0x402010000000000L, 0x201000000000000L, 0x100000000000000L
    };


 //Move class represents the move on the board from square to another one and if the move is normal,capture,promotion or castling and the score for the move used in the moves ordering while doing the search
class Move
{
public :
	short From;
	short To;
	short CapturedPiece;
	short PromotedPiece;
	bool castling;
	int score;

	Move();
	Move(int _From,int _To,int _CapturedPiece,int _PromotedPiece,bool _castling,int score);
	bool IsEqual(Move m);

	//set score function for the move..PV move is PrincipleVariation move which is the best move for the given position in the previous search (depth-1),
	//killerMoves is used for speeding up the search and helps in the moves ordering  
	void SetScore(Move& PV, int piece,int side,int & depth,vector< vector<Move> > & _KillerMoves);
	Move & operator = (const Move & other);
	bool operator < (const Move& other) const ;
	bool operator > (const Move& other) const  ;
};	


//represents the data of the position (how far is searched this position :depth, its score and best move)
class BestMoveDataObject
{
public :
	short depth;
	int Score;
	Move BestMove;

	BestMoveDataObject();
	
};	

//represents the chessboard with its all data,variables,generating moves,search,evaluation functions etc.. 
class ChessBoard
{
public : 
	//board array represents the current board with all the pieces on it,AITurn sets Lelouch turn,
	//BitBoardsArray are  bitboards (64 bits numbers) that represent each piece on the board (much easier when evaluating the position and faster),
	//same for SideTotalBitBoardArray that represents all the occupied squares for each side, KNightStart is a bitboard used to easily find the attacked square by a knight (just shifting the number gets you the squares)
	short BoardArray[120];
	short AITurn;
	long long BitBoardsArray[13];
	long long SideTotalBitBoardArray[2];
	long long KnightStart;

	//HashKey for every positiion used in zobrist hashing
	unsigned long long PositionHashKey;
	
	//array represents how much the material for each side,PiecesList is an updated list contains pieces squares for each side
	//KillerMoves are the moves that cause a "cuttoff" or a "pruning the tree" in alpha-beta search ,memorizing them in a vector 
	//and used them for moves ordering speeds up the search,a cutoff move is a move that gets you a really good score and really bad for the opponent (worse than his current one)
	int BoardMaterialValue[2];
	vector< vector<int> >PiecesList; 
	vector< vector<Move> > KillerMoves;
	string LastMove;

	//used in endgame to replace the midgame arrays with the endgame ones
	short MidGameWhiteKingTable[64];
	short MidGameBlackKingTable[64];

	//Castling
	int CastlingPermission; //4 bits number to permit to both sides castling permissions	

	ChessBoard ();	
	
	//setting the board with the given FEN
	void ParseFENAndSetTheBoard(string& FEN);

	//intitalize the positions for each piece
	void IntializePiecesList();

	//hyperbola quintessence to find horizontal,vertical,diagonal and anti diagonal moves (H,V,D)
     long long HAndVMoves(int s);
	long long DMoves(int s);
	
	//Finding pieces attacks using bitboards and checking if some square attacked or not (mainly used in checking the king square safety and some evaluation functions)	
	long long KnightsMoves(int s);
	long long KingMoves(int s);
	bool IsSquareAttacked(int AttackedSquare,int AttackingSide);
	
	//printing the board
	void PrintBoardData();

	//
	void KingRookMovement(int piece,int from);

	//making the move on the board with updating position,hashkey and the castling permission etc..
	void MakingMove(Move& MyMove);
	
	//taking the move back (Make and take are used in the search and generating moves (it's only one position and we edit it by making and taking (by refrence))
	void TakingMove(Move & MyMove,int side );
	
	//generating the legal moves for the given side, depth here is used for the killer moves (there are killer moves for each depth you search),OnlyCaptures is a flag used 
	//to determine if we need the capture moves only or not (used in quiescence search)
	void GenerateMoves(vector<Move>& MovesList,int WhichSide,int  _depth,bool OnlyCaptures);

	//getting the input from the GUI
	void GetMoveFromUser(string move);

	//printing the move according to the chess board notaion, pieceslist
	void PrintMove(Move m);
	void PrintAllMoves(vector<Move>& AllGeneratedMoves);
	void PrintPiecesList();

	//the main search function
	long long AlphaBetaSearch(int depth,int alpha,int beta,int Player,int &StatrtingDepth,bool NullMove);
	
	//table used to save the searched positions each with its own data to use in the coming search to speed the search
	void TranspositionTable(int _score,int _depth,Move & _BestMove);

	//iterative deepening is a must algorithm in every chess engine (this is where we can benefit from the previous search results)
	void IterativeDeepening(int depth,int alpha,int beta,int Player,int  TheDepth);

	//used in the leaf nodes to beat the horizon effect
	long long Quiescence(int alpha ,int beta,int side);

	//evaluating how strong the given position
	int EvaluateThisPosition(int side);
	
	//center control (controlling the center is so important in the game) (D4,E4,D5,E5 squares)
	int CenterControl(int side);

	//pawns structure (double or isolated pawns)
	int PawnsStructure(int side);

	//algorithm used to count the ones in the bitboard
	int PopBit(unsigned long long MyBitBoard);

	//rooks (connected rooks are much stronger than isolated ones,rooks that hold open files is better, rooks that attack the files near the opponent's king)
	int Rooks(int side);
	bool IsConnected(int & sq1,int & sq2);
	bool CheckConnection(int & sq1,int & sq2,bool rf);

	//a bad bishop is the bishop that its moves are blocked by its own pawns
	int BadGoodBishop(int side);

	//the kingsafety
	int KingSafety(int side);
	int KingPawns(int side);
	int  SlidingAttackPiecesOnTheKingFiles(int & file,unsigned long long &FILE,unsigned long long &OpenFile,int & side,int & Q,int & R,int & KingSafetyPen,int & AttackedFilePen);
	
	//sorting the moves according to their scores
	void MovesOrdering(vector<Move>& NonOrderdMoves,int Player);
	
} ; 


unordered_map<unsigned long long,BestMoveDataObject> BestMoveData;
unordered_map<unsigned long long,short> CastlingData;
unsigned long long  New64BitNUmber();
int FileRankToSquare120(int rank,int file);
long long  Reverse(long long v);
void FillRandomNumbersArray();
void  GenerateHashKey(ChessBoard & MyCB) ;
void ParsePosition(ChessBoard & MyBoard,string fen);
void  Uci_Loop();