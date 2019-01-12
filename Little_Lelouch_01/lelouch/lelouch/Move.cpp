#include "Move.h"
#include "knowledge.h"

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
bool Move::IsEqual(Move m)
{
	if(From==m.From && To==m.To && CapturedPiece==m.CapturedPiece && PromotedPiece==m.PromotedPiece && castling==m.castling && score==m.score)
		return true;

	return false;
}
void Move::SetScore(Move& PV, int piece,int side,int & depth,vector< vector<Move> > & _KillerMoves)
{
	//Capture score
	score=0;
	if(CapturedPiece!=Empty)
		score+=PiecesValues[CapturedPiece]-(PiecesValues[piece]/100) +1000;

	//promoted piece
	if(PromotedPiece!=Empty)
		score+=PiecesValues[PromotedPiece]+900;

	//positional score
	if(PromotedPiece==Empty && CapturedPiece==Empty && depth>=0)
	{
		//killers come first
		if(_KillerMoves[0][depth].From!=_KillerMoves[0][depth].To && IsEqual(_KillerMoves[0][depth]))
			score+=400;
		else if(_KillerMoves[1][depth].From!=_KillerMoves[1][depth].To && IsEqual(_KillerMoves[1][depth]))
			score+=200;
		else
		{
			//history
			if(HistoryMoves[piece][Board120[To]]!=0)
				score+=HistoryMoves[piece][Board120[To]]+100;
			//positional 
			else
				score+=AllPiecesTable[piece][Board120[To]]-AllPiecesTable[piece][Board120[From]];
		}
		//most of the time, castling  is a good	move	
		if(castling) 
			score+=10;
	}	
	//PV move is the best move found from the previos depth..this is gonna be the first move to search in the next depth, because most probably it's going to be the same best move so we give it a really high score
	//( we search he best move first -> the more pruning -> I got my estimated best score -> the opponent moves are ordered too so he searches his best move first and if alpha>=beta (his and my score)
	//we cutoff we don't need to search the rest of moves because the first one was the best searched
	if(PV.From!=PV.To && IsEqual(PV))
		score+=100000;

	//sorting descending for black and ascending for white
	if(side==Black)
		score=-score;
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