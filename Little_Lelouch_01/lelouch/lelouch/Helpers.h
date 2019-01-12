#pragma once

#include "Board.h"

namespace helpers
{
	//generating 64 bit number used for zobrist hashing
	unsigned long long  New64BitNUmber();

	//mapping file,ranks to BoardArray square
	int FileRankToSquare120(int rank, int file);
	void  GenerateHashKey(Board & MyCB);
	//filling the arrays used in the hashing
	void FillRandomNumbersArray();
	//generating the hashkey

	//reversing bytes using the lookup table
	long long  Reverse(long long v);


	//Parsing the position string and getting the input from the GUI
	void ParsePosition(Board & MyBoard, string fen);
}