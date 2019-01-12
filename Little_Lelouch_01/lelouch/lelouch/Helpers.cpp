#include "Helpers.h"
#include "Knowledge.h"
#include <iostream>

namespace helpers
{
	//generating 64 bit number used for zobrist hashing
	unsigned long long  New64BitNUmber()
	{
		unsigned long long RandomKey=((unsigned long long)rand()) | ((unsigned long long)rand()<<15)
			| ((unsigned long long)rand()<<30) | ((unsigned long long)rand()<<45) | ((unsigned long long)rand()<<60);
		return RandomKey;
	}

	//mapping file,ranks to BoardArray square
	int FileRankToSquare120(int rank,int file)
	{

		int square = (10*rank) + (21+file);
		return square;
	}
	void  GenerateHashKey(Board & MyCB) 
	{	
		if(!FirstTime)
		{
			unsigned long long FinalKey=0;
			for(int x=Rank1;x<=Rank8;x++)
			{
				for(int y=FileA;y<=FileH;y++)
				{
					int Square=FileRankToSquare120(x,y);
					int piece=MyCB.BoardArray[Square];
					if(piece!=Empty)
						FinalKey^=PiecesHashKeysRandomNumbers[piece][Square];

				}
			}

			FinalKey^=CastleKeys[MyCB.CastlingPermission];

			MyCB.PositionHashKey=FinalKey;
			FirstTime=true;
		}
	}
	//filling the arrays used in the hashing
	void FillRandomNumbersArray() 
	{
		for(int x=0;x<NumberOfPossiblePieces;x++)
		{
			for(int y=0;y<Board120ArraySize;y++)
				PiecesHashKeysRandomNumbers[x][y]=New64BitNUmber();
		}
		for(int x=0;x<16;x++)
			CastleKeys[x]=New64BitNUmber();
	}
	//generating the hashkey

	//reversing bytes using the lookup table
	long long  Reverse(long long v)
	{

		long long  c =
			(  (long long)BitReverseTable256[(v >> 56) & 0xff]) |
			( ((long long)BitReverseTable256[(v >> 48) & 0xff]) << 8) |
			( ((long long)BitReverseTable256[(v >> 40) & 0xff]) << 16) | 
			( ((long long)BitReverseTable256[(v >> 32) & 0xff]) << 24) | 
			( ((long long)BitReverseTable256[(v >> 24) & 0xff]) <<32) |
			( ((long long)BitReverseTable256[(v >> 16) & 0xff]) << 40) |
			( ((long long)BitReverseTable256[(v >> 8) & 0xff]) << 48) | 
			( ((long long)BitReverseTable256[v & 0xff]) << 56) ;

		return c;
	}


	//Parsing the position string and getting the input from the GUI
	void ParsePosition(Board & MyBoard,string fen)
	{

		string START="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";

		if(NewGame)
		{
			if(fen.substr(0,8)=="startpos")
				MyBoard.ParseFENAndSetTheBoard(START);

			else
			{	
				string FenStr=fen.substr(4,fen.size()-4);
				MyBoard.ParseFENAndSetTheBoard(FenStr);
			}
		}

		if(NewGame)
		{
			MyBoard.IntializePiecesList();
			GenerateHashKey(MyBoard);
		}
		string move="";
		int y=fen.find("moves");
		if(fen.find("moves")!=-1)
		{

			for(int x=fen.size()-1;x>=0;x--)
			{

				if(fen[x]==' ')
					break;	
				move+=fen[x];
			}


			for(int x=0;x<move.size()/2;x++)
			{
				char letter=move[move.size()-1-x];
				move[move.size()-1-x]=move[x];
				move[x]=letter;
			}
			bool allowed=true;
			//RECHECK
			if(!NewGame)
			{
				int file=move[0]-'a';
				int rank=move[1]-'0'-1;

				int sq2=FileRankToSquare120(rank,file);
				if(PiecesColor[MyBoard.BoardArray[sq2]]!=(MyBoard.AITurn^1))
				{
					allowed=false;
					Right=false;
				}
				else
					Right=true;

			}
			if(allowed)
			{
				if(move==last)
				{
					cout<<" not my turn"<<endl;
					Right=false;
				}
				else 
					Right=true;

				MyBoard.GetMoveFromUser(move);

				if(NewGame)
					MyBoard.AITurn=Black;
			}
		}
		else
		{
			if(NewGame)
				MyBoard.AITurn=White;
		}

		MyBoard.PrintBoardData();
		NewGame=false;

	}
}