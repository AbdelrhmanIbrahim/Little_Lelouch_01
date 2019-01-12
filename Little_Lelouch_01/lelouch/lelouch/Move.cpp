#include "Move.h"

Move::Move()
{
	From=0;
	To=0;
	CapturedPiece=0;
	PromotedPiece=0;
	castling=0;
	score=0;
}
Move::Move(int _From,int _To,int _CapturedPiece,int _PromotedPiece,bool _castling,int score)
{
	From=_From;
	To=_To;
	CapturedPiece=_CapturedPiece;
	PromotedPiece=_PromotedPiece;
	castling=_castling;
	score=0;
}
bool Move::operator ==(const Move& m)
{
	if(From==m.From && To==m.To && CapturedPiece==m.CapturedPiece && PromotedPiece==m.PromotedPiece && castling==m.castling && score==m.score)
		return true;

	return false;
}
Move & Move::operator = (const Move & other)
{
	From=other.From;
	To=other.To;
	CapturedPiece=other.CapturedPiece;
	PromotedPiece=other.PromotedPiece;
	castling=other.castling;
	score=other.score;
	return *this;

}
bool Move::operator < (const Move& other) const  
{
	return (score < other.score);
}
bool Move::operator > (const Move& other) const  
{
	return (score > other.score);
}