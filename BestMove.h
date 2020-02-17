#pragma once
#include "Move.h"

//represents the data of the position (how far is searched this position :depth, its score and best move)
class BestMoveDataObject
{
public :
	short depth = -1;
	int Score = 0;
	Move BestMove = Move();
};
