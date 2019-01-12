#pragma once

#include "Move.h"
#include "BestMove.h"

#include <unordered_map>
#include <vector>

using namespace std;

//represents the chessboard with its all data,variables,generating moves,search,evaluation functions etc.. 
class Board
{
public : 
	//board array represents the current board with all the pieces on it,AITurn sets Lelouch turn,
	//BitBoardsArray are  bitboards (64 bits numbers) that represent each piece on the board (much easier when evaluating the position and faster),
	//same for SideTotalBitBoardArray that represents all the occupied squares for each side, KNightStart is a bitboard used to easily find the attacked square by a knight (just shifting the number gets you the squares)
	bool firstTime;
	short BoardArray[120];
	short AITurn;
	long long BitBoardsArray[13];
	long long SideTotalBitBoardArray[2];
	long long KnightStart;
	long NODES = 0;
	bool NewGame = true;
	bool Right = true;

	//HashKey for every positiion used in zobrist hashing
	unsigned long long PositionHashKey;

	//array represents how much the material for each side,PiecesList is an updated list contains pieces squares for each side
	//KillerMoves are the moves that cause a "cuttoff" or a "pruning the tree" in alpha-beta search ,memorizing them in a vector 
	//and used them for moves ordering speeds up the search,a cutoff move is a move that gets you a really good score and really bad for the opponent (worse than his current one)
	int BoardMaterialValue[2];
	short HistoryMoves[13][64];
	unsigned long long CastleKeys[16];
	unsigned long long PiecesHashKeysRandomNumbers[13][120];
	int PiecesValues[13];
	short AllPiecesTable[13][64];

	vector< vector<int> >PiecesList; 
	vector< vector<Move> > KillerMoves;
	unordered_map<unsigned long long,BestMoveDataObject> BestMoveData;
	unordered_map<unsigned long long,short> CastlingData;
	
	string LastMove;

	//used in endgame to replace the midgame arrays with the endgame ones
	short MidGameWhiteKingTable[64];
	short MidGameBlackKingTable[64];

	//Castling
	int CastlingPermission; //4 bits number to permit to both sides castling permissions	
	bool NoMoreCastling;

	Board ();

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

};
