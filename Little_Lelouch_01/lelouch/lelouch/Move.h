#pragma once

#include <vector>

using namespace std;

//Move class represents the move on the board from square to another one and if the move is normal,capture,promotion or castling and the score for the move used in the moves ordering while doing the search
class Move
{
public:
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
	void SetScore(Move& PV, int piece,int side,int & depth, vector< vector<Move> > & _KillerMoves);
	Move & operator = (const Move & other);
	bool operator < (const Move& other) const;
	bool operator > (const Move& other) const;
};
