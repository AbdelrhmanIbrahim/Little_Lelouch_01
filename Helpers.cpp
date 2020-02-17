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

	void  GenerateHashKey(Board & board) 
	{	
		if(!board.firstTime)
		{
			unsigned long long FinalKey=0;
			for(int x=Rank1;x<=Rank8;x++)
			{
				for(int y=FileA;y<=FileH;y++)
				{
					int Square=FileRankToSquare120(x,y);
					int piece=board.BoardArray[Square];
					if(piece!=Empty)
						FinalKey^=board.PiecesHashKeysRandomNumbers[piece][Square];

				}
			}

			FinalKey^=board.CastleKeys[board.CastlingPermission];

			board.PositionHashKey=FinalKey;
			board.firstTime=true;
		}
	}
	//filling the arrays used in the hashing
	void FillRandomNumbersArray(Board& board) 
	{
		for(int x=0;x<NumberOfPossiblePieces;x++)
		{
			for(int y=0;y<Board120ArraySize;y++)
				board.PiecesHashKeysRandomNumbers[x][y]=New64BitNUmber();
		}
		for(int x=0;x<16;x++)
			board.CastleKeys[x]=New64BitNUmber();
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

		if(MyBoard.NewGame)
		{
			if(fen.substr(0,8)=="startpos")
				MyBoard.ParseFENAndSetTheBoard(START);

			else
			{	
				string FenStr=fen.substr(4,fen.size()-4);
				MyBoard.ParseFENAndSetTheBoard(FenStr);
			}
		}

		if(MyBoard.NewGame)
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
			if(!MyBoard.NewGame)
			{
				int file=move[0]-'a';
				int rank=move[1]-'0'-1;

				int sq2=FileRankToSquare120(rank,file);
				if(PiecesColor[MyBoard.BoardArray[sq2]]!=(MyBoard.AITurn^1))
				{
					allowed=false;
					MyBoard.r=false;
				}
				else
					MyBoard.r=true;

			}
			if(allowed)
			{
				if(move==last)
				{
					cout<<" not my turn"<<endl;
					MyBoard.r=false;
				}
				else 
					MyBoard.r=true;

				MyBoard.GetMoveFromUser(move);

				if(MyBoard.NewGame)
					MyBoard.AITurn=Black;
			}
		}
		else
		{
			if(MyBoard.NewGame)
				MyBoard.AITurn=White;
		}

		MyBoard.PrintBoardData();
		MyBoard.NewGame=false;

	}

	void SetScore(Board& board, Move& move, const Move PV, int piece,int side,int & depth,vector< vector<Move> > & _KillerMoves)
	{
		//Capture score
		move.score=0;
		if(move.CapturedPiece!=Empty)
			move.score+=board.PiecesValues[move.CapturedPiece]-(board.PiecesValues[piece]/100) +1000;

		//promoted piece
		if(move.PromotedPiece!=Empty)
			move.score+=board.PiecesValues[move.PromotedPiece]+900;

		//positional score
		if(move.PromotedPiece==Empty && move.CapturedPiece==Empty && depth>=0)
		{
			//killers come first
			if(_KillerMoves[0][depth].From!=_KillerMoves[0][depth].To && move==(_KillerMoves[0][depth]))
				move.score+=400;
			else if(_KillerMoves[1][depth].From!=_KillerMoves[1][depth].To && move==(_KillerMoves[1][depth]))
				move.score+=200;
			else
			{
				//history
				short history = board.HistoryMoves[piece][Board120[move.To]];
				if(history!=0)
					move.score+=history+100;
				//positional 
				else
					move.score+=board.AllPiecesTable[piece][Board120[move.To]]-board.AllPiecesTable[piece][Board120[move.From]];
			}
			//most of the time, castling  is a good	move	
			if(move.castling) 
				move.score+=10;
		}	
		//PV move is the best move found from the previos depth..this is gonna be the first move to search in the next depth, because most probably it's going to be the same best move so we give it a really high score
		//( we search he best move first -> the more pruning -> I got my estimated best score -> the opponent moves are ordered too so he searches his best move first and if alpha>=beta (his and my score)
		//we cutoff we don't need to search the rest of moves because the first one was the best searched
		if(PV.From!=PV.To && move==(PV))
			move.score+=100000;

		//sorting descending for black and ascending for white
		if(side==Black)
			move.score=-move.score;
	}
}