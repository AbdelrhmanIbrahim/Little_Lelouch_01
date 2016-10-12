#include<iostream> 
#include<algorithm>
#include<time.h>
#include"Little_Lelouch_01.h"

using namespace std;

//generating 64 bit number used for zobrist hashing
unsigned long long  New64BitNUmber()
{
 unsigned long long RandomKey=((unsigned long long)rand()) | ((unsigned long long)rand()<<15)
	 | ((unsigned long long)rand()<<30) | ((unsigned long long)rand()<<45) | ((unsigned long long)rand()<<60);
 return RandomKey;
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
void  GenerateHashKey(ChessBoard & MyCB) 
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
//mapping file,ranks to BoardArray square
int FileRankToSquare120(int rank,int file)
{

	int square = (10*rank) + (21+file);
	return square;
}
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
	


BestMoveDataObject::BestMoveDataObject()
{
	Score=0;
	depth=-1;
	BestMove=Move();
		
}
	
ChessBoard::ChessBoard ()
{
		
	//IntializeEveryThing (Globals,PawnBits and both boards for converting etc..)
	for(int x=0;x<Board120ArraySize;x++)
		Board120[x]=65;
	
	for(int x=0;x<Board64ArraySize;x++)
	{
		Board64[x]=120;
			
		if(x<NumberOfPossiblePieces)
		{
			for(int y=0;y<Board64ArraySize;y++)
				HistoryMoves[x][y]=0;
		}
			
	}
	
	//killers
	for(int x=0;x<2;x++)
	{
		vector<Move> LEVELS(MAX_DEPTH);		
		KillerMoves.push_back(LEVELS);
	}

	//PiecesList
	vector<int>v;
	for(int x=0;x<NumberOfPossiblePieces;x++)
		PiecesList.push_back(v);
	

	//the mapping arrays
	int sq64=0;
	for(int x=Rank1;x<=Rank8;x++)
	{
		int sq120;
		for(int y=FileA;y<=FileH;y++)
		{
				
			sq120=FileRankToSquare120(x,y);
			Board64[sq64]=sq120;
			Board120[sq120]=sq64;
			sq64++;

		}
	
	}
		
	//the ranks,files arrays	
	sq64=0;
	for(int x=Rank1;x<=Rank8;x++)
	{
		for(int y=FileA;y<=FileH;y++)
		{
			SquaresAndTheirRanks[sq64]=x;
			SquaresAndTheirFiles[sq64]=y;
			sq64++;
		}
	}

		 
	//the main board
	for(int x=0;x<Board64ArraySize;x++)
	{
		BoardArray[Board64[x]]=Empty;
		if(x<13)
			BitBoardsArray[x]=0;		

	}

	//the borders of the board
	for(int x=0;x<Board120ArraySize;x++)
	{
		if(BoardArray[x]!=Empty)
			BoardArray[x]=OffBoard;
	}

	
	//material
	for(int x=0;x<2;x++)
		BoardMaterialValue[x]=Empty;
	

	CastlingPermission=15;
	PositionHashKey=0;
	LastMove="";
	SideTotalBitBoardArray[White]=0;
	SideTotalBitBoardArray[Black]=0;	
	KnightStart=0xa1100110a;

}	
	
//setting the boardarray, bitboards and the castling permission
void ChessBoard::ParseFENAndSetTheBoard(string& FEN)
{
	string FEN1,FEN2;
	for(int x=0;x<FEN.size();x++)
	{
		if(FEN[x]==' ')
		{
			FEN1=FEN.substr(0,x);
			FEN2=FEN.substr(x,FEN.size()-x);
			break;
		}	
	}
		
		

	int R=Rank8;
	int F=FileA;
	int EmptySquaresCount=0;
	int piece=Empty;
		
	for(int x=0;x<FEN1.size();x++)
	{
			switch(FEN[x])
			{
				case 'r' : piece =BlackRook;  BitBoardsArray[BlackRook]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]);break;
				case 'n' : piece =BlackKnight;  BitBoardsArray[BlackKnight]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]); break;
				case 'b' : piece =BlackBishop;  BitBoardsArray[BlackBishop]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]); break;
				case 'q' : piece =BlackQueen;   BitBoardsArray[BlackQueen]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]);break;
				case 'k' : piece =BlackKing;   BitBoardsArray[BlackKing]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]);break;
				case 'p' : piece =BlackPawn;  BitBoardsArray[BlackPawn]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]);break;

				case 'R' : piece =WhiteRook; BitBoardsArray[WhiteRook]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]); break;
				case 'N' : piece =WhiteKnight; BitBoardsArray[WhiteKnight]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]); break;
				case 'B' : piece =WhiteBishop; BitBoardsArray[WhiteBishop]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]);  break;
				case 'Q' : piece =WhiteQueen; BitBoardsArray[WhiteQueen]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]); break;
				case 'K' : piece =WhiteKing; BitBoardsArray[WhiteKing]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]); break;
				case 'P' : piece =WhitePawn; BitBoardsArray[WhitePawn]|=1LL<<(63-Board120[FileRankToSquare120(R,F)]); break;
				
				case '1' :  
				case '2' :  
				case '3' :  
				case '4' :  
				case '5' :  
				case '6' :  
				case '7' : 
				case '8' : 
					piece=Empty;
					EmptySquaresCount=FEN[x]-'0';
					break;
				

				case '/' :
					R--;
					F=FileA;
					break;
			}
				
			if(FEN[x]!='/' && FEN[x]!=' ')
			{
				if(piece==Empty)
				{
					for(int x=0;x<EmptySquaresCount;x++)
					{
						BoardArray[FileRankToSquare120(R,F)]=piece;
						F++;
					}
				}

				else
				{
					BoardArray[FileRankToSquare120(R,F)]=piece;
					F++;
				}
			}
				
	}
		
	for(int x=WhitePawn;x<=WhiteKing;x++)
		SideTotalBitBoardArray[White] |=BitBoardsArray[x];

	for(int x=BlackPawn;x<=BlackKing;x++)
		SideTotalBitBoardArray[Black] |=BitBoardsArray[x];

	BitBoardsArray[Empty]=~(SideTotalBitBoardArray[White]|SideTotalBitBoardArray[Black]);

	for(int x=0;x<FEN2.size();x++)

	{
		if(FEN2[x]=='k')
			CastlingPermission|=BlackKingSideCasstling;
		else if(FEN2[x]=='q')
			CastlingPermission|=BlackQueenSideCasstling;
		else if(FEN2[x]=='K')
			CastlingPermission|=WhiteKingSideCasstling;
		else if(FEN2[x]=='Q')
			CastlingPermission|=WhiteQueenSideCasstling;

	}
		
}

void ChessBoard::IntializePiecesList()
{
		
	//Intializing
	for(int x=0;x<Board64ArraySize;x++)
	{
		int sq=Board64[x];
		int piece=BoardArray[sq];
		if(piece!=Empty)
		{
			PiecesList[piece].push_back(sq);
			BoardMaterialValue[PiecesColor[piece]]+=PiecesValues[piece];
		}

	}
		
		
}

//hyperbola quintessence algorithm to find horizontal,vertical,diagonal and anti diagonal bitboards (H,V,D)
long long ChessBoard::HAndVMoves(int s)
{
	s=63-Board120[s];
	long long binaryS=1LL<<s;
	long long possibilitiesHorizontal = ((~BitBoardsArray[Empty]) - 2 * binaryS) ^ Reverse(Reverse((~BitBoardsArray[Empty])) - 2 * Reverse(binaryS));
     long long possibilitiesVertical = (((~BitBoardsArray[Empty]) & FilesBitBoard[s % 8]) - (2 * binaryS)) ^ Reverse(Reverse((~BitBoardsArray[Empty]) & FilesBitBoard[s % 8]) - (2 * Reverse(binaryS)));
     return (possibilitiesHorizontal & RanksBitBoard[s / 8]) | (possibilitiesVertical & FilesBitBoard[s % 8]);
}
long long ChessBoard::DMoves(int s)
{
	s=63-Board120[s];
	long long binaryS=1LL<<s;
	long long possibilitiesDiagonal = (((~BitBoardsArray[Empty]) & DiagonalBitBoard[(s / 8) + (s % 8)]) - (2 * binaryS)) ^ Reverse(Reverse((~BitBoardsArray[Empty])&DiagonalBitBoard[(s / 8) + (s % 8)]) - (2 * Reverse(binaryS)));
     long long possibilitiesAntiDiagonal = (((~BitBoardsArray[Empty]) &AntiDiagonalBitBoard[(s / 8) + 7 - (s % 8)]) - (2 * binaryS)) ^ Reverse(Reverse((~BitBoardsArray[Empty]) &AntiDiagonalBitBoard[(s / 8) + 7 - (s % 8)]) - (2 * Reverse(binaryS)));
     return (possibilitiesDiagonal & DiagonalBitBoard[(s / 8) + (s % 8)]) | (possibilitiesAntiDiagonal & AntiDiagonalBitBoard[(s / 8) + 7 - (s % 8)]);
}
	

//finding pieces attacks using bitboards and checking if square attacked or not
long long ChessBoard::KnightsMoves(int s)
{
		 	
		int save=s;
		s=63-Board120[s];
		
		long long KnightAttacks=0;

		//knigtStart represents eight attacks of knight on its  all-attacks-available first square on the board (C6) then shifting to the given squares (or it can be F3 square but swap the shiftings)
		if(s>=18)
		KnightAttacks=KnightStart<<(s-18);
		else
		KnightAttacks=KnightStart>>(18-s);

		//eliminating the attacks on the file A and B if you are on the G,H  and vice versa (because it's shifting a number)
		if(SquaresAndTheirFiles[Board120[save]]==FileA || SquaresAndTheirFiles[Board120[save]]==FileB)
		KnightAttacks&= (~(FilesBitBoard[FileA] | FilesBitBoard[FileB]));

		else if(SquaresAndTheirFiles[Board120[save]]==FileG || SquaresAndTheirFiles[Board120[save]]==FileH)
			KnightAttacks&= (~(FilesBitBoard[FileG] | FilesBitBoard[FileH]));

		return KnightAttacks;

}
long long ChessBoard::KingMoves(int s)
{
	int save =s;
	s=63-Board120[s];
	//here we only shift right and left with 7,8,9 values (the boardarray is 1D) (king can move only one move in any direction (8) )
	long long KingAttacks=(1LL<<(s-7)) | (1LL<<(s-8)) | (1LL<<(s-9)) | (1LL<<(s-1))| (1LL<<(s+1)) | (1LL<<(s-1)) | (1LL<<(s+7)) | (1LL<<(s+8)) | (1LL<<(s+9));
		
	//same as we did with the knight
	if(SquaresAndTheirFiles[Board120[save]]==FileA)
		KingAttacks &= ~(FilesBitBoard[FileA]);

	else if(SquaresAndTheirFiles[Board120[save]]==FileH)
		KingAttacks &= ~(FilesBitBoard[FileH]);

	return KingAttacks;
}
bool ChessBoard::IsSquareAttacked(int AttackedSquare,int AttackingSide)
{
	if(BoardArray[AttackedSquare]!=OffBoard)
	{
		long long PawnLeftAttacks=0,PawnRightAttacks=0,Attacks=0,TheSquare=1LL<<(63-Board120[AttackedSquare]);
		int King,Queen,Rook,Bishop,Knight;
		if(AttackingSide==White)
		{

			King=WhiteKing;
			Queen=WhiteQueen;
			Rook=WhiteRook;
			Bishop=WhiteBishop;
			Knight=WhiteKnight;

			//pawns
			PawnLeftAttacks=(BitBoardsArray[WhitePawn]>>7) & (~RanksBitBoard[Rank8]) & (~ FilesBitBoard[FileA]);
			PawnRightAttacks=(BitBoardsArray[WhitePawn]>>9) & (~RanksBitBoard[Rank8]) & (~FilesBitBoard[FileH]); 
		}
		else
		{
			
			King=BlackKing;
			Queen=BlackQueen;
			Rook=BlackRook;
			Bishop=BlackBishop;
			Knight=BlackKnight;

			//pawns
			PawnRightAttacks=(BitBoardsArray[BlackPawn]<<7) & (~RanksBitBoard[Rank1]) & (~FilesBitBoard[FileH]);
			PawnLeftAttacks=(BitBoardsArray[BlackPawn]<<9) & (~RanksBitBoard[Rank1]) & (~FilesBitBoard[FileA]); 
		}
				
		//pawns
		Attacks=(PawnLeftAttacks|PawnRightAttacks);
		if( TheSquare & Attacks)
			return true;

		//rooks
		for(int x=0;x<PiecesList[Rook].size();x++)
		{
			Attacks|= (HAndVMoves(PiecesList[Rook][x]));
			if(TheSquare & Attacks)
				return true;
		}

		//bishops
		for(int x=0;x<PiecesList[Bishop].size();x++)
		{
			Attacks|= (DMoves(PiecesList[Bishop][x]));
			if(TheSquare & Attacks)
				return true;
		}

		//Queens
		for(int x=0;x<PiecesList[Queen].size();x++)
		{
			Attacks|= (DMoves(PiecesList[Queen][x]));
			Attacks|= (HAndVMoves(PiecesList[Queen][x]));

			if(TheSquare & Attacks)
				return true;
		}

		//knights
		for(int x=0;x<PiecesList[Knight].size();x++)
		{
			Attacks|= (KnightsMoves(PiecesList[Knight][x]));
			if(TheSquare & Attacks)
				return true;
		}

		//king
		if(PiecesList[King].size()==1)
		{
			Attacks|=KingMoves(PiecesList[King] [0]);
			if(TheSquare & Attacks)
				return true;
		}

	}


	return false;
}

//printing board
void ChessBoard::PrintBoardData()
{
	for(int x=0;x<Board64ArraySize;x++)
	{
		if(x%8==0 && x!=0)
			cout<<endl;
		if(BoardArray[Board64[x]]==Empty)
			cout<<"- ";
		else if(BoardArray[Board64[x]]==WhitePawn)
			cout<<"P ";
		else if(BoardArray[Board64[x]]==WhiteRook)
			cout<<"R ";
		else if(BoardArray[Board64[x]]==WhiteKnight)
			cout<<"N ";
		else if(BoardArray[Board64[x]]==WhiteBishop)
			cout<<"B ";
		else if(BoardArray[Board64[x]]==WhiteQueen)
			cout<<"Q ";
		else if(BoardArray[Board64[x]]==WhiteKing)
			cout<<"K ";
		else if(BoardArray[Board64[x]]==BlackPawn)
			cout<<"p ";
		else if(BoardArray[Board64[x]]==BlackRook)
			cout<<"r ";
		else if(BoardArray[Board64[x]]==BlackKnight)
			cout<<"n ";
		else if(BoardArray[Board64[x]]==BlackBishop)
			cout<<"b ";
		else if(BoardArray[Board64[x]]==BlackQueen)
			cout<<"q ";
		else if(BoardArray[Board64[x]]==BlackKing)
			cout<<"k ";
	}

	cout<<endl;
		

}

//editing castle permission if king or a rook moved
void ChessBoard::KingRookMovement(int piece,int from)
{
	//king moved means he can't castle anymore
	if(piece==WhiteKing && from==E1)
		CastlingPermission&=(~(WhiteKingSideCasstling|WhiteQueenSideCasstling));
	
	else if(piece==BlackKing && from==E8)
		CastlingPermission&=(~(BlackKingSideCasstling|BlackQueenSideCasstling));
	
	//same for the rook but only one side
	else if(piece==WhiteRook)
	{
		if(from==A1)
			CastlingPermission&=(~WhiteQueenSideCasstling);
		
		else if(from==H1)
			CastlingPermission&=(~WhiteKingSideCasstling);
		
	}
	else if(piece==BlackRook)
	{
		if(from==A8)
			CastlingPermission&=(~BlackQueenSideCasstling);
		
		else if(from==H8)
			CastlingPermission&=(~BlackKingSideCasstling);
		
		
	}
	
}

//making the move
void ChessBoard::MakingMove(Move& MyMove)
{
	
	int sq1=MyMove.From;
	int sq2=MyMove.To;
	int piece=BoardArray[sq1];

	if(piece!=Empty)
	{
			//saving the castle permission 
			CastlingData[PositionHashKey]=CastlingPermission;
			//from and to positions bitboards
			long long FromPosition= 1LL<<(63-Board120[MyMove.From]),ToPosition= 1LL<<(63-Board120[MyMove.To]);		

			//Promotion
			if(MyMove.PromotedPiece!=Empty)
			{
				//zobrist hashing
				PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
				PositionHashKey^=PiecesHashKeysRandomNumbers[MyMove.PromotedPiece][sq2];

				for(int x=0;x<PiecesList[piece].size();x++)
				{
					if(PiecesList[piece][x]==sq1)
					{
						//update the bitboards
						BitBoardsArray[piece] ^=	FromPosition;
						BitBoardsArray[MyMove.PromotedPiece] |= ToPosition;
							
						//update the pieceslist
						PiecesList[piece][x]=PiecesList[piece][PiecesList[piece].size()-1];
						PiecesList[piece].pop_back();
						PiecesList[MyMove.PromotedPiece].push_back(sq2);

						
						//update the material
						BoardMaterialValue[PiecesColor[piece]]-=PiecesValues[piece];
						BoardMaterialValue[PiecesColor[MyMove.PromotedPiece]]+=PiecesValues[MyMove.PromotedPiece];

						//update the board array
						BoardArray[sq1]=Empty;
						BoardArray[sq2]=MyMove.PromotedPiece;

						break;


					}
					}
			
				}

			//Capture
			if(MyMove.CapturedPiece!=0)
			{
				//checking for the castling permission
				KingRookMovement(piece,sq1);

				//hashing
				PositionHashKey^=PiecesHashKeysRandomNumbers[MyMove.CapturedPiece][sq2];

				//hashing and editing bitboards
				if(!MyMove.PromotedPiece)
				{
					//zobrisht hashing
					PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
					PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];

					//bitboards
					BitBoardsArray[piece] ^=	FromPosition;
					BitBoardsArray[piece] |= ToPosition;
					BitBoardsArray[MyMove.CapturedPiece] ^=ToPosition;

					//pieceslist
					for(int x=0;x<PiecesList[piece].size();x++)
					{
						if(PiecesList[piece][x]==sq1)
						{
							PiecesList[piece][x]=sq2;
							break;
						}
					}
				}
				else
					BitBoardsArray[MyMove.CapturedPiece] ^=ToPosition;

				//updating pieceslist,material and boardarray for the captured move
				for(int x=0;x<PiecesList[MyMove.CapturedPiece].size();x++)
				{
					if(PiecesList[MyMove.CapturedPiece][x]==sq2)
					{
						PiecesList[MyMove.CapturedPiece][x]=PiecesList[MyMove.CapturedPiece][PiecesList[MyMove.CapturedPiece].size()-1];
						PiecesList[MyMove.CapturedPiece].pop_back();
							
						BoardMaterialValue[PiecesColor[MyMove.CapturedPiece]]-=(PiecesValues[MyMove.CapturedPiece]);

						if(!MyMove.PromotedPiece)
						{
							BoardArray[sq1]=Empty;
							BoardArray[sq2]=piece;
						}
						break;
						
					}
				}

					
			}

			//Castling
			if(MyMove.castling)
			{
				if(piece==BlackKing && PiecesList[piece].size()==1)
				{
					if(sq2==G8)
					{
							
						PiecesList[piece][0]=sq2;
						BoardArray[sq1]=Empty;
						BoardArray[sq2]=piece;

						for(int x=0;x<PiecesList[BlackRook].size();x++)
						{
							if(PiecesList[BlackRook][x]==H8)
							{
								///bitboard
								BitBoardsArray[BlackKing]^=FromPosition;
								BitBoardsArray[BlackKing]|=ToPosition;
								BitBoardsArray[BlackRook]^=1LL<<(63-Board120[H8]);
								BitBoardsArray[BlackRook]|=1LL<<(63-Board120[F8]);

								//pieceslist
								PiecesList[BlackRook][x]=F8;

								//boardarray
								BoardArray[H8]=Empty;
								BoardArray[F8]=BlackRook;

								//hashing	
								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
								PositionHashKey^=PiecesHashKeysRandomNumbers[BlackRook][H8];
								PositionHashKey^=PiecesHashKeysRandomNumbers[BlackRook][F8];
								break;

							}
						}
								
					}

					else if(sq2==C8)
					{
							
						PiecesList[piece][0]=sq2;
						BoardArray[sq1]=Empty;
						BoardArray[sq2]=piece;

						for(int x=0;x<PiecesList[BlackRook].size();x++)
						{
							if(PiecesList[BlackRook][x]==A8)
							{
								///edit bitboard
								BitBoardsArray[BlackKing]^=FromPosition;
								BitBoardsArray[BlackKing]|=ToPosition;
								BitBoardsArray[BlackRook]^=1LL<<(63-Board120[A8]);
								BitBoardsArray[BlackRook]|=1LL<<(63-Board120[D8]);

								PiecesList[BlackRook][x]=D8;
								BoardArray[A8]=Empty;
								BoardArray[D8]=BlackRook;

								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
								PositionHashKey^=PiecesHashKeysRandomNumbers[BlackRook][A8];
								PositionHashKey^=PiecesHashKeysRandomNumbers[BlackRook][D8];
								break;

							}
						}
								
					}
					//king moved, can't castle anymore
					CastlingPermission&=(~(BlackKingSideCasstling|BlackQueenSideCasstling));

				}

				else if(piece==WhiteKing && PiecesList[piece].size()==1)
				{
					if(sq2==G1)
					{
						PiecesList[piece][0]=sq2;
						BoardArray[sq1]=Empty;
						BoardArray[sq2]=piece;

						for(int x=0;x<PiecesList[WhiteRook].size();x++)
						{
							if(PiecesList[WhiteRook][x]==H1)
							{
								///edit bitboard
								BitBoardsArray[WhiteKing]^=FromPosition;
								BitBoardsArray[WhiteKing]|=ToPosition;
								BitBoardsArray[WhiteRook]^=1LL<<(63-Board120[H1]);
								BitBoardsArray[WhiteRook]|=1LL<<(63-Board120[F1]);

								PiecesList[WhiteRook][x]=F1;
								BoardArray[H1]=Empty;
								BoardArray[F1]=WhiteRook;

								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
								PositionHashKey^=PiecesHashKeysRandomNumbers[WhiteRook][H1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[WhiteRook][F1];
								break;

							}
						}
								
					}

					else if(sq2==C1)
					{
						
						PiecesList[piece][0]=sq2;
						BoardArray[sq1]=Empty;
						BoardArray[sq2]=piece;

						for(int x=0;x<PiecesList[WhiteRook].size();x++)
						{
							if(PiecesList[WhiteRook][x]==A1)
							{
								///edit bitboard
								BitBoardsArray[WhiteKing]^=FromPosition;
								BitBoardsArray[WhiteKing]|=ToPosition;
								BitBoardsArray[WhiteRook]^=1LL<<(63-Board120[A1]);
								BitBoardsArray[WhiteRook]|=1LL<<(63-Board120[D1]);

								PiecesList[WhiteRook][x]=D1;
								BoardArray[A1]=Empty;
								BoardArray[D1]=WhiteRook;

								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
								PositionHashKey^=PiecesHashKeysRandomNumbers[WhiteRook][A1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[WhiteRook][D1];
								break;

							}
						}
								
					}
					CastlingPermission&=(~(WhiteKingSideCasstling|WhiteQueenSideCasstling));
					
				}
			}

			//Normal move
			if(MyMove.CapturedPiece==0 && MyMove.PromotedPiece==0 && MyMove.castling==0 )
			{
				KingRookMovement(piece,sq1);
				
				PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
				PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];

				//edit bitboards
				BitBoardsArray[piece] ^=	FromPosition;
				BitBoardsArray[piece] |= ToPosition;
					
				for(int x=0;x<PiecesList[piece].size();x++)
				{
					if(PiecesList[piece][x]==sq1)
					{
						PiecesList[piece][x]=sq2;
						BoardArray[sq1]=Empty;
						BoardArray[sq2]=piece;		
						break;

					}
				}

					

			
			}
			
			//the occupied squares bitboards for each side
			SideTotalBitBoardArray[White]=(BitBoardsArray[WhitePawn] | BitBoardsArray[WhiteKnight] |BitBoardsArray[WhiteRook] | BitBoardsArray[WhiteBishop] 
			| BitBoardsArray[WhiteQueen] | BitBoardsArray[WhiteKing] );
			SideTotalBitBoardArray[Black]=(BitBoardsArray[BlackPawn] | BitBoardsArray[BlackKnight] |BitBoardsArray[BlackRook] | BitBoardsArray[BlackBishop] 
			| BitBoardsArray[BlackQueen] | BitBoardsArray[BlackKing] );
			BitBoardsArray[Empty]=~(SideTotalBitBoardArray[White] | SideTotalBitBoardArray[Black]);
	}


}

//we do excatly the same in TakingMove
void ChessBoard::TakingMove(Move & MyMove,int side )
{
	int sq1=MyMove.From;
	int sq2=MyMove.To;
	int piece=BoardArray[sq2];
	if(piece!=Empty)
	{
			long long FromPosition= 1LL<<(63-Board120[MyMove.From]),ToPosition= 1LL<<(63-Board120[MyMove.To]);		
			//Promotion
			if(MyMove.PromotedPiece!=Empty)
			{
				int Pawn=WhitePawn;
				if(side==Black)
					Pawn=BlackPawn;

				//Hashing key
				PositionHashKey^=PiecesHashKeysRandomNumbers[MyMove.PromotedPiece][sq2];
				PositionHashKey^=PiecesHashKeysRandomNumbers[Pawn][sq1];
					


				for(int x=0;x<PiecesList[MyMove.PromotedPiece].size();x++)
				{
					if(PiecesList[MyMove.PromotedPiece][x]==sq2)
					{

						//EditBitBoard
						BitBoardsArray[MyMove.PromotedPiece] ^=	ToPosition;
						BitBoardsArray[Pawn] |= FromPosition;
							

						//EditPieceslist
						PiecesList[MyMove.PromotedPiece][x]=PiecesList[MyMove.PromotedPiece][PiecesList[MyMove.PromotedPiece].size()-1];
						PiecesList[MyMove.PromotedPiece].pop_back();
						PiecesList[Pawn].push_back(sq1);

							
						BoardMaterialValue[PiecesColor[Pawn]]+=PiecesValues[Pawn];
						BoardMaterialValue[PiecesColor[MyMove.PromotedPiece]]-=PiecesValues[MyMove.PromotedPiece];

						//boardArray
						BoardArray[sq1]=Pawn;
						BoardArray[sq2]=Empty;

						break;


					}
					}
			
				}

			//Capture
			if(MyMove.CapturedPiece!=Empty)
			{	
				//hashing and editing bitboards
				if(!MyMove.PromotedPiece)
				{
					//hashing
					PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
					PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
						
					//EditBitBoards
					BitBoardsArray[piece] ^=	ToPosition;
					BitBoardsArray[piece] |= FromPosition;
					BitBoardsArray[MyMove.CapturedPiece] |=ToPosition;

					//Pieceslist
					for(int x=0;x<PiecesList[piece].size();x++)
					{
						if(PiecesList[piece][x]==sq2)
						{
							PiecesList[piece][x]=sq1;
							PiecesList[MyMove.CapturedPiece].push_back(sq2);
							break;

						}
					}
						
					//Boardarrays
					BoardArray[sq1]=piece;
				}

				else
				{

					PiecesList[MyMove.CapturedPiece].push_back(sq2);
					BitBoardsArray[MyMove.CapturedPiece] |=ToPosition;
					
				}

				BoardArray[sq2]=MyMove.CapturedPiece;
				PositionHashKey^=PiecesHashKeysRandomNumbers[MyMove.CapturedPiece][sq2];
						
				BoardMaterialValue[PiecesColor[MyMove.CapturedPiece]]+=(PiecesValues[MyMove.CapturedPiece]);

			}

			//Castling
			if(MyMove.castling)
			{
					
				if(piece==BlackKing && PiecesList[piece].size()==1)
				{
					if(sq2==G8)
					{
						PiecesList[piece][0]=sq1;
						BoardArray[sq2]=Empty;
						BoardArray[sq1]=piece;

						for(int x=0;x<PiecesList[BlackRook].size();x++)
						{
							if(PiecesList[BlackRook][x]==F8)
							{
								///edit bitboard
								BitBoardsArray[BlackKing]|=FromPosition;
								BitBoardsArray[BlackKing]^=ToPosition;
								BitBoardsArray[BlackRook]|=1LL<<(63-Board120[H8]);
								BitBoardsArray[BlackRook]^=1LL<<(63-Board120[F8]);

								PiecesList[BlackRook][x]=H8;
								BoardArray[F8]=Empty;
								BoardArray[H8]=BlackRook;

								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[BlackRook][F8];
								PositionHashKey^=PiecesHashKeysRandomNumbers[BlackRook][H8];
								break;

							}
						}
								
					}

					else if(sq2==C8)
					{
						PiecesList[piece][0]=sq1;
						BoardArray[sq2]=Empty;
						BoardArray[sq1]=piece;

						for(int x=0;x<PiecesList[BlackRook].size();x++)
						{
							if(PiecesList[BlackRook][x]==D8)
							{
								///edit bitboard
								BitBoardsArray[BlackKing]|=FromPosition;
								BitBoardsArray[BlackKing]^=ToPosition;
								BitBoardsArray[BlackRook]|=1LL<<(63-Board120[A8]);
								BitBoardsArray[BlackRook]^=1LL<<(63-Board120[D8]);

								PiecesList[BlackRook][x]=A8;
								BoardArray[D8]=Empty;
								BoardArray[A8]=BlackRook;

								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[BlackRook][D8];
								PositionHashKey^=PiecesHashKeysRandomNumbers[BlackRook][A8];
								break;

							}
						}
								
					}
				}

				else if(piece==WhiteKing && PiecesList[piece].size()==1)
				{
					if(sq2==G1)
					{
						PiecesList[piece][0]=sq1;
						BoardArray[sq2]=Empty;
						BoardArray[sq1]=piece;

						for(int x=0;x<PiecesList[WhiteRook].size();x++)
						{
							if(PiecesList[WhiteRook][x]==F1)
							{
								///edit bitboard
								BitBoardsArray[WhiteKing]|=FromPosition;
								BitBoardsArray[WhiteKing]^=ToPosition;
								BitBoardsArray[WhiteRook]|=1LL<<(63-Board120[H1]);
								BitBoardsArray[WhiteRook]^=1LL<<(63-Board120[F1]);

								PiecesList[WhiteRook][x]=H1;
								BoardArray[F1]=Empty;
								BoardArray[H1]=WhiteRook;

								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[WhiteRook][F1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[WhiteRook][H1];
								break;

							}
						}
								
					}

					else if(sq2==C1)
					{
						PiecesList[piece][0]=sq1;
						BoardArray[sq2]=Empty;
						BoardArray[sq1]=piece;

						for(int x=0;x<PiecesList[WhiteRook].size();x++)
						{
							if(PiecesList[WhiteRook][x]==D1)
							{
								///edit bitboard
								BitBoardsArray[WhiteKing]|=FromPosition;
								BitBoardsArray[WhiteKing]^=ToPosition;
								BitBoardsArray[WhiteRook]|=1LL<<(63-Board120[A1]);
								BitBoardsArray[WhiteRook]^=1LL<<(63-Board120[D1]);

								PiecesList[WhiteRook][x]=A1;
								BoardArray[D1]=Empty;
								BoardArray[A1]=WhiteRook;

								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
								PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[WhiteRook][D1];
								PositionHashKey^=PiecesHashKeysRandomNumbers[WhiteRook][A1];
								break;

							}
						}
								
					}
		
				}
			}

			//Normal
			if(MyMove.CapturedPiece==0 && MyMove.PromotedPiece==0 && MyMove.castling==0 )
			{
				
				//hashing
				PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq2];
				PositionHashKey^=PiecesHashKeysRandomNumbers[piece][sq1];

				
				//edit bitboards
				BitBoardsArray[piece] ^=	ToPosition;
				BitBoardsArray[piece] |= FromPosition;
					
				for(int x=0;x<PiecesList[piece].size();x++)
				{
					if(PiecesList[piece][x]==sq2)
					{
						PiecesList[piece][x]=sq1;
						BoardArray[sq2]=Empty;
						BoardArray[sq1]=piece;		
						break;

					}
				}
			
			}
				
			SideTotalBitBoardArray[White]=(BitBoardsArray[WhitePawn] | BitBoardsArray[WhiteKnight] |BitBoardsArray[WhiteRook] | BitBoardsArray[WhiteBishop] 
			| BitBoardsArray[WhiteQueen] | BitBoardsArray[WhiteKing] );
			SideTotalBitBoardArray[Black]=(BitBoardsArray[BlackPawn] | BitBoardsArray[BlackKnight] |BitBoardsArray[BlackRook] | BitBoardsArray[BlackBishop] 
			| BitBoardsArray[BlackQueen] | BitBoardsArray[BlackKing] );
			BitBoardsArray[Empty]=~(SideTotalBitBoardArray[White] | SideTotalBitBoardArray[Black]);
			CastlingPermission=CastlingData[PositionHashKey];
				
	}

	
}

//generating legal moves
void ChessBoard::GenerateMoves(vector<Move>& MovesList,int WhichSide,int  _depth,bool OnlyCaptures)
{
	//All Pieces
	int END=100,Start=BlackPawn,Ending=BlackKing;
	int PawnSelected=BlackPawn,PawnColor=Black,Forward=-10,Right=-9,Left=-11,TheRank=Rank2,Q=BlackQueen,B=BlackBishop,R=BlackRook,N=BlackKnight,King=BlackKing,RankBase=Rank7;
	bool KingAttacked;

	if((PiecesList[BlackKing].size()!=0 && PiecesList[WhiteKing].size()!=0))
	{
		if(WhichSide==Black)
			KingAttacked=IsSquareAttacked(PiecesList[BlackKing][0],White);
		else
			KingAttacked=IsSquareAttacked(PiecesList[WhiteKing][0],Black);

		if(WhichSide==White)
		{
			Start=WhitePawn;
			Ending=WhiteKing;
			PawnSelected=WhitePawn;
			PawnColor=White;
			Forward=10;
			Right=9;
			Left=11;
			TheRank=Rank7;
			Q=WhiteQueen;
			B=WhiteBishop;
			R=WhiteRook;
			N=WhiteKnight;
			King=WhiteKing;
			RankBase=Rank2;
		}
			


		Move PVMove(0,0,0,0,false,0);
		if(BestMoveData.find(PositionHashKey)!=BestMoveData.end())
			PVMove=BestMoveData[PositionHashKey].BestMove;
		
		for(int a=Start;a<=Ending;a++)
		{
			bool PawnUnSafe=false;
			for(int b=0;b<PiecesList[a].size();b++)
			{
				//Pawns
				int sq=PiecesList[a][b];
				if(BoardArray[sq]==PawnSelected)
				{
					if(WhichSide==PawnColor)
					{
							//pawn moves
							//promote
							if(SquaresAndTheirRanks[Board120[sq]]==TheRank)
							{
								//capture move right + promotion
								if(BoardArray[sq+Right]!=OffBoard && (PiecesColor[BoardArray[sq+Right]]==(WhichSide^1) ))
								{

									
									Move mtake=Move (sq,sq+Right,BoardArray[sq+Right],Q,0,0);
									MakingMove(mtake);

									if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
									{
										TakingMove(mtake,WhichSide);
											

											
										Move m1=Move (sq,sq+Right,BoardArray[sq+Right],Q,0,0);
										Move m2=Move (sq,sq+Right,BoardArray[sq+Right],B,0,0);
										Move m3=Move (sq,sq+Right,BoardArray[sq+Right],N,0,0);
										Move m4=Move (sq,sq+Right,BoardArray[sq+Right],R,0,0);

										m1.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
										m2.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
										m3.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
										m4.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);

							 			MovesList.push_back(m1);
										MovesList.push_back(m2);
										MovesList.push_back(m3);
										MovesList.push_back(m4);
										
									}
									else
										TakingMove(mtake,WhichSide);
									
										
								}
										
								//capture move left + promotion
								if(BoardArray[sq+Left]!=OffBoard && (PiecesColor[BoardArray[sq+Left]]==(WhichSide^1) ))
								{
										
									Move mtake=Move (sq,sq+Left,BoardArray[sq+Left],Q,0,0);
									MakingMove(mtake);

										
									if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
									{
										TakingMove(mtake,WhichSide);
											

										Move m1=Move (sq,sq+Left,BoardArray[sq+Left],Q,0,0);
										Move m2=Move (sq,sq+Left,BoardArray[sq+Left],B,0,0);
										Move m3=Move (sq,sq+Left,BoardArray[sq+Left],N,0,0);
										Move m4=Move (sq,sq+Left,BoardArray[sq+Left],R,0,0);

										m1.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
										m2.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
										m3.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
										m4.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);

							 			MovesList.push_back(m1);
										MovesList.push_back(m2);
										MovesList.push_back(m3);
										MovesList.push_back(m4);
										
									}
									else
										TakingMove(mtake,WhichSide);
									
										
									
								}

								// move forward + promotion
								if(!OnlyCaptures && BoardArray[sq+Forward]==Empty && BoardArray[sq+Forward]!=OffBoard)
								{
										
									Move mtake=Move (sq,sq+Forward,BoardArray[sq+Forward],Q,0,0);
									MakingMove(mtake);

									
									if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
									{
										TakingMove(mtake,WhichSide);
											

										Move m1=Move (sq,sq+Forward,BoardArray[sq+Forward],Q,0,0);
										Move m2=Move (sq,sq+Forward,BoardArray[sq+Forward],B,0,0);
										Move m3=Move (sq,sq+Forward,BoardArray[sq+Forward],N,0,0);
										Move m4=Move (sq,sq+Forward,BoardArray[sq+Forward],R,0,0);
											
										m1.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
										m2.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
										m3.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
										m4.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);

							 			MovesList.push_back(m1);
										MovesList.push_back(m2);
										MovesList.push_back(m3);
										MovesList.push_back(m4);

										
									}
									else
										TakingMove(mtake,WhichSide);
									
								}
	
							}
								
							//normal move, not promotion
							else
							{

								//normal forward
								if(!OnlyCaptures && BoardArray[sq+Forward]!=OffBoard && BoardArray[sq+Forward]==Empty)
								{
									if(!KingAttacked)
									{
										Move m1=Move (sq,sq+Forward,BoardArray[sq+Forward],0,0,0);
										MakingMove(m1);
										if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
										{
											TakingMove(m1,WhichSide);

											m1.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
											
											MovesList.push_back(m1);
										}
										else
										{
											TakingMove(m1,WhichSide);		
											PawnUnSafe=true;
											
										}
									}
									else
									{
										Move m1=Move (sq,sq+Forward,BoardArray[sq+Forward],0,0,0);
										MakingMove(m1);
										if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
										{
											TakingMove(m1,WhichSide);

											m1.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
												
											MovesList.push_back(m1);
										}
										else
											TakingMove(m1,WhichSide);
												
										
									}
								}

								//capture left
								if(BoardArray[sq+Left]!=OffBoard && (PiecesColor[BoardArray[sq+Left]]==(WhichSide^1) ))
								{
									Move m1=Move(sq,sq+Left,BoardArray[sq+Left],0,0,0);
									MakingMove(m1);
									if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
									{
										TakingMove(m1,WhichSide);
											
	
										m1.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
											
										MovesList.push_back(m1);
									} 
									else
										TakingMove(m1,WhichSide);
								}

								//capture right
								if(BoardArray[sq+Right]!=OffBoard && (PiecesColor[BoardArray[sq+Right]]==(WhichSide^1) ))
								{
									Move m1=Move (sq,sq+Right,BoardArray[sq+Right],0,0,0);
									MakingMove(m1);
										
									if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
									{
										TakingMove(m1,WhichSide);
											
										m1.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
											
										MovesList.push_back(m1);
						
									}
									else
										TakingMove(m1,WhichSide);
											
										
								}

								//foward 2 steps
								if(!OnlyCaptures && SquaresAndTheirRanks[Board120[sq]]==RankBase && 
								BoardArray[sq+(2*Forward)]==Empty && BoardArray[sq+(2*Forward)]!=OffBoard && BoardArray[sq+Forward]==Empty)
								{
									Move m1=Move (sq,sq+(2*Forward),BoardArray[sq+(2*Forward)],0,0,0);
											
									if(KingAttacked)
									{
										MakingMove(m1);
										if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
										{
											TakingMove(m1,WhichSide);
											m1.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
											
											MovesList.push_back(m1);
										}
										else
											TakingMove(m1,WhichSide);
									}
									else
									{
										if(!PawnUnSafe)
										{
											m1.SetScore(PVMove,BoardArray[sq],WhichSide,_depth,KillerMoves);
											MovesList.push_back(m1);
										}
							 
										
									}
									

							
							}
								
						}
		
				}
				}
				//Other pieces but pawns
				else
				{
					int ChosenSq=PiecesList[a][b];
					int ChosenPiece=BoardArray[ChosenSq];
					int MoveSq;
					bool WKflag=false,BKflag=false;
					//cout<<CastlingPermission<<endl;
					if(ChosenPiece!=Empty && PiecesColor[ChosenPiece]==WhichSide)
					{
						for(int x=0;x<AllDirsSizes[ChosenPiece];x++)
						{
							MoveSq=ChosenSq+AllDirs[ChosenPiece][x];
							bool Safe=false;
							while(BoardArray[MoveSq]!=OffBoard)
							{
								//normal move
								if(BoardArray[MoveSq]==Empty)
								{
									//Non slider
									if( ChosenPiece==WhiteKing)
									{
										if(!OnlyCaptures)
										{
											Move m1=Move (ChosenSq,MoveSq,0,0,0,0);
											if(!IsSquareAttacked(MoveSq,WhichSide^1))
											{
												
												m1.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
											
												MovesList.push_back(m1);
											}
										


											if(!WKflag)
											{
												if((CastlingPermission & WhiteKingSideCasstling)  && BoardArray[E1]==WhiteKing
													&& BoardArray[F1]==Empty && BoardArray[G1]==Empty && BoardArray[H1]==WhiteRook
													&&  !IsSquareAttacked(E1,WhichSide^1) && !IsSquareAttacked(F1,WhichSide^1)
													 &&  !IsSquareAttacked(G1,WhichSide^1) 
													)
													{
														Move Cas=Move (E1,G1,0,0,1,0);
														Cas.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
														MovesList.push_back(Cas);
														WKflag=true;
													}

												if((CastlingPermission & WhiteQueenSideCasstling)  && BoardArray[E1]==WhiteKing
													&& BoardArray[D1]==Empty && BoardArray[C1]==Empty && BoardArray[B1]==Empty  
													&& BoardArray[A1]==WhiteRook
													&&  !IsSquareAttacked(E1,WhichSide^1) && !IsSquareAttacked(D1,WhichSide^1)
													  &&  !IsSquareAttacked(C1,WhichSide^1) 
													)
													{
														Move Cas=Move (E1,C1,0,0,1,0);
														Cas.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
														MovesList.push_back(Cas);
														WKflag=true;
													}
											}
		
										}
											break;
									}

									else if(ChosenPiece==BlackKing)
									{
										if(!OnlyCaptures)
										{
											Move m1=Move (ChosenSq,MoveSq,0,0,0,0);
											if(!IsSquareAttacked(MoveSq,WhichSide^1))
											{
												
												m1.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
											
												MovesList.push_back(m1);
											}

											if(!BKflag)
											{
												if((CastlingPermission & BlackKingSideCasstling)  && BoardArray[E8]==BlackKing
													&& BoardArray[F8]==Empty && BoardArray[G8]==Empty && BoardArray[H8]==BlackRook
													&&  !IsSquareAttacked(E8,WhichSide^1) && !IsSquareAttacked(F8,WhichSide^1)
													&&  !IsSquareAttacked(G8,WhichSide^1) 
													)
													{
														Move Cas=Move (E8,G8,0,0,1,0);
														Cas.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
														MovesList.push_back(Cas);
														BKflag=true;
													}

												if((CastlingPermission & BlackQueenSideCasstling)  && BoardArray[E8]==BlackKing
													&& BoardArray[D8]==Empty && BoardArray[C8]==Empty && BoardArray[B8]==Empty && BoardArray[A8]==BlackRook
													&&  !IsSquareAttacked(E8,WhichSide^1) && !IsSquareAttacked(D8,WhichSide^1)
													&&  !IsSquareAttacked(C8,WhichSide^1) 
													)
													{
														Move Cas=Move (E8,C8,0,0,1,0);
														Cas.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
														MovesList.push_back(Cas);
														BKflag=true;
													}
											}

											
										}
											break;
									}
									// slider
									else
									{
										if(!OnlyCaptures)
										{
											if(KingAttacked)
											{
												Move m1=Move (ChosenSq,MoveSq,0,0,0,0);
												MakingMove(m1);
												if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
												{
													TakingMove(m1,WhichSide);
													m1.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
											
													MovesList.push_back(m1);
												}
												else
													TakingMove(m1,WhichSide);
											}
											else
											{
												Move m1=Move (ChosenSq,MoveSq,0,0,0,0);
												if(!Safe)
												{
													MakingMove(m1);
													if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
													{
														TakingMove(m1,WhichSide);
														m1.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
											
														MovesList.push_back(m1);
														Safe=true;
													}
													else
													{
														TakingMove(m1,WhichSide);
														break;
													}
												}
												else
												{
													m1.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
											
													MovesList.push_back(m1);
												}
											
											}
										}
									}
								}

								//capture move
								else
								{
									
									if(PiecesColor[BoardArray[MoveSq]]==(WhichSide^1))
									{
										Move m1=Move (ChosenSq,MoveSq,BoardArray[MoveSq],0,0,0);
										if(!Safe)
										{
											MakingMove(m1);
											if(!IsSquareAttacked(PiecesList[King][0],WhichSide^1))
												{
													TakingMove(m1,WhichSide);
													m1.SetScore(PVMove,ChosenPiece,WhichSide,_depth,KillerMoves);
													MovesList.push_back(m1);
												}
												else
												TakingMove(m1,WhichSide);
										}
										else
										{
											MovesList.push_back(m1); 
										}
										
										break;
									}
									else
										break;
								}
								if(ChosenPiece==WhiteKnight || ChosenPiece==BlackKnight)
									break;
								else
									MoveSq+=AllDirs[ChosenPiece][x];


							}

						}
		
					}
																																																																																																																																																																																																																													//Rook
				
				}


			}

		}
	}
	
		
}

//parsing the input string to move object
void ChessBoard::GetMoveFromUser(string move)
{
	int rank,file,captured=Empty,promoted=Empty,cas=false;
	file=move[0]-'a';
	rank=move[1]-'0'-1;
	int sq1=FileRankToSquare120(rank,file);

	file=move[2]-'a';
	rank=move[3]-'0'-1;
	int sq2=FileRankToSquare120(rank,file);
	captured=BoardArray[sq2];

	if(move.size()>4)
	{
		int side=PiecesColor[BoardArray[sq1]];
		if(side==White)
		{
			if(move[4]=='q') promoted=WhiteQueen;
			else if(move[4]=='r') promoted= WhiteRook;
			else if(move[4]=='n') promoted= WhiteKnight;
			else if(move[4]=='b') promoted= WhiteBishop;
		}
		else 
		{
			if(move[4]=='q') promoted= BlackQueen;
			else if(move[4]=='r') promoted= BlackRook;
			else if(move[4]=='n') promoted= BlackKnight;
			else if(move[4]=='b') promoted= BlackBishop;
		}

	}
	//castling 
	if(move=="e1g1" || move=="e1c1" || move=="e8g8" || move=="e8c8" )
		cas=true;

	Move ToMake(sq1,sq2,captured,promoted,cas,0);
	MakingMove(ToMake);
		
		
}

//printing the move in board notation,all the moves and pieceslist 
void ChessBoard::PrintMove(Move m)	
{	
	string move="";
	LastMove="";
	int sq64=Board120[m.From];
	int rank=SquaresAndTheirRanks[sq64];
	int file=SquaresAndTheirFiles[sq64];
	char r='1'+rank;
	char f='a'+file;
	LastMove+=f;
	LastMove+=r;
	cout<<f<<r;
	sq64=Board120[m.To];
	rank=SquaresAndTheirRanks[sq64];
	file=SquaresAndTheirFiles[sq64];
	r='1'+rank;
	f='a'+file;
	LastMove+=f;
	LastMove+=r;
	cout<<f<<r;
	if(m.PromotedPiece==Empty)
		cout<<endl;
	else
	{
		if(m.PromotedPiece==WhiteQueen || m.PromotedPiece==BlackQueen )
		{
			LastMove+='q';
			cout<<"q"<<endl;
		}
		else if(m.PromotedPiece==WhiteRook || m.PromotedPiece==BlackRook )
		{
			LastMove+='r';
			cout<<"r"<<endl;
		}
		else if(m.PromotedPiece==WhiteBishop || m.PromotedPiece==BlackBishop )
		{
			LastMove+='b';
			cout<<"b"<<endl;
		}
		else if(m.PromotedPiece==WhiteKnight || m.PromotedPiece==BlackKnight )
		{
			LastMove+='n';
			cout<<"n"<<endl;
		}

	}

}
void ChessBoard::PrintAllMoves(vector<Move>& AllGeneratedMoves)
{
	for(int x=0;x<AllGeneratedMoves.size();x++)
		PrintMove(AllGeneratedMoves[x]);
	
}
void ChessBoard::PrintPiecesList()
{
	for(int a=0;a<PiecesList.size();a++)
		{
			for(int b=0;b<PiecesList[a].size();b++)
			{
				cout<<dec<<PiecesList[a][b]<<" ";
			}
			cout<<endl;
		}
		
}

//the Alpha-Beta search
long long ChessBoard::AlphaBetaSearch(int depth,int alpha,int beta,int Player,int &StatrtingDepth,bool NullMove)
{	
	/*
	alpha beta search is an optimization for MINIMAX search on the tree by adding lower and upper bounds called alpha-beta , technically these are your score and the opponent score,
	alpha seeks to be max value as possible,beta seeks to be the min value as possible, cutting off or alpha-beta pruning happens whenever alpha>=beta, in this case your opponent or you know 
	 this branch will get him/you nowhere but a losing score (worse than his/your current score) < if alpha, > if beta...so it cutoffs the whole subtree because no need to search it 
	(it's going to be bad but do we really want to know how bad it is? no,here where the pruning comes and how alpha-beta works..this is excatly how human think while playing  any strategy board games
	*/

	//if you are a leaf node return how strong is this position
	if(depth==0)
	{
		long long sc=Quiescence(alpha,beta,Player);
		//cout<<sc<<endl;
		return sc;
	}

	int MoveScore=0;
	vector<Move>NewDepthMoves;
		
	//null move pruning, passing two moves to the opponent and if he still can't beat my score then no need to generate my moves..starting from depth 4 null move algorithm is doing well (by experiment)
	//null move can't be done if you are in chech because this will result in capturing the king which is illegal
	int k=WhiteKing;
	if(Player==Black)
		k=BlackKing;
	if(NullMove && depth>=4 && PiecesList[BlackKing].size()==1 && PiecesList[WhiteKing].size()==1 && PiecesList[k].size()==1 &&!IsSquareAttacked(PiecesList[k][0],Player^1))
	{
			
		Player^=1;
		//searching here is done with zero window search (alpha and beta difference is 1) just to check if the given value is better or not (you can beat my score or not)
		MoveScore=-AlphaBetaSearch(depth-4,-beta,-beta+1,Player,StatrtingDepth,false);
		Player^=1;
			
		if(MoveScore>=beta)
			return beta;
	}
	
	//generate the moves
	GenerateMoves(NewDepthMoves,Player,depth,false);		
		
	//if no moves and your king attacked then this is a mate
	if(NewDepthMoves.size()==0)
	{
		if(PiecesList[WhiteKing].size()==1)
		{
			if(IsSquareAttacked(PiecesList[WhiteKing][0],Black))
			{
				//cout<<"king attacked"<<endl;				
				if(Player==White)
					return -MATE;
				else
					return MATE; 
			}
		}
		if(PiecesList[BlackKing].size()==1)
		{
			if(IsSquareAttacked(PiecesList[BlackKing][0],White))
			{
				//cout<<"king attacked"<<endl;
				if(Player==White)
					return MATE;
				else
					return -MATE;
			}
		}
			
			
	}

	//order the moves before searching (the most important part in alpha-beta search is ordering the moves from best to worst,
	//if we searced the best moves first we most probably will cutoff alot of subtrees which will speeds up the search),that's an estimating order because you never know what the best move is
	//if there's a pawn captures a queen move we give this move a high score to put in the first moves to search etc..
	MovesOrdering(NewDepthMoves,Player);
		
	bool zugzwang=true;
	int BestScore=NegINF,BestScore2;
	int BestDepth=0,BestDepth2;
	Move BestMove(0,0,0,0,0,0);
	Move BestMove2(0,0,0,0,0,0);
		
	//alpha-beta search
	for(int x=0;x<NewDepthMoves.size();x++)
	{
		
		NODES++;
		MakingMove(NewDepthMoves[x]);	
		//alpha will become your -beta because it's the opponent turn and vice versa and this is called recursively (DFS)
		MoveScore=-AlphaBetaSearch(depth-1,-beta,-alpha,Player^1,StatrtingDepth,true);
		TakingMove(NewDepthMoves[x],Player);
			
	
		// in case of zugzwang (every move is a bad move)
		if(MoveScore>BestScore)
		{
				BestScore=MoveScore;
				BestDepth=depth;
				BestMove=NewDepthMoves[x];
				//is the output score of this move better than my current best score? yes..then assign it to alpha and keep searching for the best
				if(MoveScore>alpha)
				{
					//no there are good moves then set zugzwang to false
					zugzwang=false;

					//cutoff (alpha>=beta)
					if(MoveScore>=beta )
					{
						//quiet killers and history moves (killers and history used for ordering normal moves)
						if(NewDepthMoves[x].CapturedPiece==Empty && NewDepthMoves[x].PromotedPiece==Empty)
						{
							//killers got two slots for the same depth..the first gets a high score than the second..cutoffs moves gets a really high score for normal moves. (killers because they "Kill"  some of opponent positions by cutting off subtrees)
							KillerMoves[1][depth]=KillerMoves[0][depth];
							KillerMoves[0][depth]=NewDepthMoves[x];

							//updating the score of the HistoryMoves array..along the iterative deepening search, is this piece on this square caused a cutoff? yes then add the current depth to its score and so on
							//so it called history it depends on the history of each piece and the squares where it caused a cutoff
							HistoryMoves[BoardArray[NewDepthMoves[x].From]][Board120[NewDepthMoves[x].To]]+=depth;
						}

						//table where we save the best move,score and the depth for the current position with the hash key that represents the position
						TranspositionTable(beta,depth,NewDepthMoves[x]);
						//we cutoff by returning beta
						return beta;
					}
				
					alpha=MoveScore;
					const clock_t begin_time = clock();
						
					BestScore2=MoveScore;
					BestDepth2=depth;
					BestMove2=NewDepthMoves[x];
					ALLTIME+=float( clock () - begin_time ) /  CLOCKS_PER_SEC;
				}
		}
				
	}

	if(zugzwang)
		TranspositionTable(BestScore,BestDepth,BestMove);
	else
		TranspositionTable(BestScore2,BestDepth2,BestMove2);
			
	//draw
	if(NewDepthMoves.size()==0)
		return EvaluateThisPosition(Player); 

	//else we return the best score we got (alpha)
	else
		return alpha;
}

//results table
void ChessBoard::TranspositionTable(int _score,int _depth,Move & _BestMove)
{
	if(BestMoveData.find(PositionHashKey)!=BestMoveData.end())
	{
		//depth is higher? then assign
		if(_depth>=BestMoveData[PositionHashKey].depth)
		{
				
			BestMoveData[PositionHashKey].BestMove=_BestMove;
			BestMoveData[PositionHashKey].Score=_score;
			BestMoveData[PositionHashKey].depth=_depth;
				
		}
			
			
	}
	else
	{
		BestMoveData[PositionHashKey].BestMove=_BestMove;
		BestMoveData[PositionHashKey].Score=_score;
		BestMoveData[PositionHashKey].depth=_depth;
	}
						
}

void ChessBoard::IterativeDeepening(int depth,int alpha,int beta,int Player,int  TheDepth)
{
	//clear for search
	for(int x=0;x<NumberOfPossiblePieces;x++)
	{
		for(int y=0;y<Board64ArraySize;y++)
			HistoryMoves[x][y]=0;
			
	}
	for(int x=0;x<2;x++)
	{
		for(int y=0;y<MAX_DEPTH;y++)
			KillerMoves[x][y]=Move();
	}
		
	long long A=NegINF,B=PosINF;

	//aspiratoin window is a technique used to speed up the search where you can estimate the next result of the search given the current one
	//so you set the new alpha beta bounds around the final evaluatin score of the current position with a window size you determine by expermient and according to evaluatin etc..
	//if the bounds (alpha-beta) close to each other, the number of cutoffs goes high because most likely there a value of alpha will be >= beta and will cause a cutoff faster than
	//-oo and oo alpha-beta bounds.
	int ASPIRATION_WINDOW_SIZE=50;

	//the iterative deepening search..searching level 1->taking results(PV move (best move found) ,killers,history etc..)->use these results to speed up the search in level 2 and so on..
	for(int x=1;x<=TheDepth;x++)
	{
		float Leveltime=0;
		Out=false;
		
		Move BestMove(0,0,0,0,false,0);
		
		cout<<"depth :"<<x<<endl;
		const clock_t begin_time = clock();
		NODES=0;
		//PrintBoardData();

		AlphaBetaSearch(x,A,B,Player,x,false);

		//if the final score is out the window you will need to research with open window
		if(BestMoveData[PositionHashKey].Score<=A || BestMoveData[PositionHashKey].Score>=B)
		{
			cout<<"research depth "<<x<<endl; 
			AlphaBetaSearch(x,NegINF,PosINF,Player,x,false);
			//cout<<BestMoveData[PositionHashKey].Score<<endl;
		}

		Leveltime=float( clock () - begin_time ) ;
		//cout << "search time : " <<Leveltime/ CLOCKS_PER_SEC<<endl;
			
		cout<<"info score cp " <<BestMoveData[PositionHashKey].Score<<" depth "<<x<<" nodes "<<NODES<<" time "<<Leveltime<<" pv ";
		PrintMove(BestMoveData[PositionHashKey].BestMove);
			
		if(BestMoveData[PositionHashKey].Score==MATE)
		{
			cout<<"MATE"<<endl;
			break;
		}


		//the new boundries	
		A=BestMoveData[PositionHashKey].Score-ASPIRATION_WINDOW_SIZE;
		B=BestMoveData[PositionHashKey].Score+ASPIRATION_WINDOW_SIZE;
			
	}

}

//here we do the alpha-beta search but only with the capture moves to beat the horizon effect..done only at the leaf nodes
long long ChessBoard::Quiescence(int alpha ,int beta,int side)
{
	
	int score=EvaluateThisPosition(side);
		
	if(score==MATE || score==-MATE)
		return score;
		

	//it's already that bad? no need for checking horizon effect
	if(score>=beta)
		return beta;

	//better? assign alpha
	if(score>alpha)
		alpha=score;

	int d=-1;
	vector<Move>Captures;
	GenerateMoves(Captures,side,d,true);
	//long long sc=0
	MovesOrdering(Captures,side);
		
	for(int x=0;x<Captures.size();x++)
	{
		
		NODES++;
		MakingMove(Captures[x]);
		score=-Quiescence(-beta,-alpha,side^1);
		TakingMove(Captures[x],side);
				
		if(score>alpha)
		{
			//cutoff
			if(score>=beta)
				return beta;
				
			alpha=score;
		}
	}
		
	return alpha;
		
}

//evaluating how strong is the position
int ChessBoard::EvaluateThisPosition(int side)
{
	long long score=0;
	int Sign=1;
	if(PiecesList[WhiteKing].size()==0 && PiecesList[BlackKing].size()!=0)
		score=-MATE;
	else if(PiecesList[WhiteKing].size()!=0 && PiecesList[BlackKing].size()==0)
		score=MATE;
	else
	{
		//material
		score=BoardMaterialValue[White]-BoardMaterialValue[Black];
		
		//pair bishops (having a pair bishops is preferable because of controlling alot of light and dark squares
		if(PiecesList[WhiteBishop].size()>=2)
			score+=PiecesValues[WhitePawn]/3;
		if(PiecesList[BlackBishop].size()>=2)
			score-=PiecesValues[BlackPawn]/3;

		//positional score for each piece
		int piece=Empty;
		for(piece=WhitePawn;piece<=BlackKing;piece++)
		{
			int WhatColor=0;	
			for(int x=0;x<PiecesList[piece].size();x++)
			{
				if(PiecesColor[piece]==White)
					Sign=1;
					
				else if(PiecesColor[piece]==Black)
					Sign=-1;

				WhatColor=Board120[PiecesList[piece][x]];
				score+=(Sign)*AllPiecesTable[piece][WhatColor];
		
			}
		}

		//center
		score+=CenterControl(White)-CenterControl(Black);

		//structure
		score-=PawnsStructure(White)-PawnsStructure(Black);
		
		//bishops
		score+=BadGoodBishop(White)-BadGoodBishop(Black);

		//rooks
		score+=Rooks(White)-Rooks(Black);

		//King 
		score-=KingSafety(White)-KingSafety(Black);
		score+=KingPawns(White)-KingPawns(Black);
		
	}

	if(side==White)
		return score;
	else
		return -score;


}

//controlling the center and the extended center
int ChessBoard::CenterControl(int side)
{
	
	//pawns
	int CenterCon=0;
	int Attacks=0,ExtendedAttacks=0,P=WhitePawn,start=WhiteKnight,end=WhiteRook,ExtendedAttackSide=C1,k=WhiteKing,B=B1,C=C1,D=D1;

	if(side==Black)
	{
		P=BlackPawn;
		start=BlackKnight;
		end=BlackRook;
		k=BlackKing;
		B=B8;
		C=C8;
		D=D8;
	}

	if(PiecesList[k].size()==1)
	{
		if(PiecesList[k][0]==B || PiecesList[k][0]==C || PiecesList[k][0]==D)
			ExtendedAttackSide=F1;
	}

	//Attacks for the pawns holding the center (getting how many  pawns in the center)
	Attacks=PopBit(BitBoardsArray[P] & Center);
	//ExtendedAttacks are good too (C and F files) for the pawns
	ExtendedAttacks= PopBit(BitBoardsArray[P] & ExtendedCenter &  FilesBitBoard[7-SquaresAndTheirFiles[Board120[ExtendedAttackSide]]]);
		
	//pawns center
	CenterCon+=10*Attacks;
	CenterCon+=5*ExtendedAttacks;

	
	//other pieces center control
	for(int x=start;x<=end;x++)
	{
		for(int y=0;y<PiecesList[x].size();y++)
		{
			if(x==WhiteKnight | x==BlackKnight)
			{
				Attacks= PopBit( (KnightsMoves(PiecesList[x][y])) & Center) ;
				CenterCon+=5*Attacks;
				//cout<<"knight holdign center "<<endl;
			}
			else if(x==WhiteBishop | x==BlackBishop)
			{
				Attacks= PopBit( (DMoves(PiecesList[x][y])) & Center) ;
				CenterCon+=5*Attacks;
				//cout<<"Bishop holdign center "<<endl;
			}
			else if(x==WhiteRook | x==BlackRook)
			{
				Attacks= PopBit( (HAndVMoves(PiecesList[x][y])) & Center) ;
				CenterCon+=5*Attacks;
				//cout<<"Rook holdign center "<<endl;
			}
		}
	}
		
	return CenterCon;

}

//pawns structure penalty	
int ChessBoard::PawnsStructure(int side)
{
	int pen=0;
	int Pawn=WhitePawn,WeakPawns=0,isolatedPawns=0;
	if(side==Black)
		Pawn=BlackPawn;

	//weak pawns (double / triple etc..)/ isolated pawns
	//loop over each file and see if the bit boards & with this file is giving more than '1' then this mean there are more than one pawn on the same file which is a weak structure called double pawns
	//isolated pawn is a pawn that got no backup pawns on each of its neighbour files..weak pawn (not always becuase it can be supported by sliding pieces defense to push to promotion so we give it a less penalty)
	for(int x=FileA;x<=FileH;x++)
	{
		WeakPawns=PopBit (FilesBitBoard[x] & BitBoardsArray[Pawn]);

		isolatedPawns=PopBit (FilesBitBoard[x] & BitBoardsArray[Pawn]);

		if(isolatedPawns>0)
		{
			if(x==FileA)
			{
				if(PopBit (FilesBitBoard[x+1] & BitBoardsArray[Pawn])==0)
					pen+=(isolatedPawns*5);
				
			}
			else if(x==FileG)
			{
				if(PopBit (FilesBitBoard[x-1] & BitBoardsArray[Pawn])==0)
					pen+=(isolatedPawns*5);
			}
			else
			{
				if(PopBit (FilesBitBoard[x+1] & BitBoardsArray[Pawn])==0 && PopBit (FilesBitBoard[x-1] & BitBoardsArray[Pawn])==0)
					pen+=(isolatedPawns*5);
			}
		}


		if(WeakPawns>=2)
			pen+=(WeakPawns*20);
	}

			
	return pen;


		
}

//returns the number of 1s in a bitboards
int ChessBoard::PopBit(unsigned long long MyBitBoard)
{
	int count=0;
	while(MyBitBoard!=0)
	{
		MyBitBoard&=(MyBitBoard-1);
		count++;
	}

	return count;
}

//Rooks
int ChessBoard::Rooks(int side)
{
	int Bonus=0,r=WhiteRook;
	int OpenFileBonus=12,ConnectedRookBonusRank=15,ConnectedRookBonusFile=17;

	if(side==Black)
		r=BlackRook;
	

	//open files
	for(int x=0;x<PiecesList[r].size();x++)
	{
		int sq=PiecesList[r][x];
		//we get the file of the rook
		unsigned long long FILE= FilesBitBoard[7-SquaresAndTheirFiles[Board120[sq]]];
		//we add any piece on the same file then we add the rook position
		unsigned long long  OpenFile= (FILE)  ^ (~BitBoardsArray[Empty] &FILE) ^ (1LL<<(63-Board120[sq]));
			
		//if they are the same this means that there's no piece but the rook,then it's openfile for the rook
		if(OpenFile==FILE)
			Bonus+=OpenFileBonus;

		//connected rooks
		for(int y=x+1;y<PiecesList[r].size();y++)
		{
			//checking if they are connected
			if(IsConnected(PiecesList[r][x],PiecesList[r][y])) 
			{
				//same file (controlling a files is usually stronger than ranks for rooks)
				if(FilesBitBoard[7-SquaresAndTheirFiles[Board120[PiecesList[r][x]]]] == FilesBitBoard[7-SquaresAndTheirFiles[Board120[PiecesList[r][y]]]])
					Bonus+=ConnectedRookBonusFile;
				//same rank
				else
					Bonus+=ConnectedRookBonusRank;
			}
		}
	}


	return Bonus;
}

//check if 2 squares connected
bool ChessBoard::IsConnected(int & sq1,int & sq2)
{
	unsigned long long RANKFILE1=FilesBitBoard[7-SquaresAndTheirFiles[Board120[sq1]]];
	unsigned long long RANKFILE2=FilesBitBoard[7-SquaresAndTheirFiles[Board120[sq2]]];
		
	//not same file
	if(RANKFILE1!=RANKFILE2)
	{
		RANKFILE1=RanksBitBoard[7-SquaresAndTheirRanks[Board120[sq1]]];
		RANKFILE2=RanksBitBoard[7-SquaresAndTheirRanks[Board120[sq2]]];

		//same rank
		if(RANKFILE1==RANKFILE2)
			return CheckConnection(sq1,sq2,false);
		else
			return false;
				
	}
			
	//same file
	else
		return CheckConnection(sq1,sq2,true);
}

//checking connection (all squares between them are empty)
bool ChessBoard::CheckConnection(int & sq1,int & sq2,bool rf)
{

	int sq_1=sq1,sq_2=sq2,shifting=1,_120Shift=1;

	//file
	if(rf)
	{
		shifting=8;
		_120Shift=10;
	}
		
	//start/end
	if(sq1>sq2)	
	{
		sq_1=sq2;
		sq_2=sq1;
	}
		
		
	//looping over the squares between both,if all are empty then connected
	unsigned long long Shifted=(1LL<<(63-Board120[sq_1]));
	for(int x=sq_1+_120Shift;x<sq_2;x+=_120Shift)
	{
		Shifted=Shifted>>(shifting);
		if(Shifted & (~BitBoardsArray[Empty]))
			return false;
		
	}
	return true;
	
}

//check if a bishop is a bad or a good bishop by adding the mobility of the bishop through its own pawns as a bonus (bonus = 0 if the bishop is surronded by its own pawns from all directions)
int  ChessBoard::BadGoodBishop(int side)
{
	int B=WhiteBishop,P=WhitePawn,BishopsScore=0;
	if(side==Black)
	{
		B=BlackBishop;
		P=BlackPawn;
	}

	for(int x=0;x<PiecesList[B].size();x++)
	{
		unsigned long long BishopAttacks=DMoves(PiecesList[B][x]);
		BishopAttacks&=~(BitBoardsArray[P]);
		BishopsScore+=PopBit(BishopAttacks);
	}
	return BishopsScore;


}

//if king is on open file or the near files are open then this is dangerous..more danger in case if there's a sliding piece attack (Q,R) on this file 
int  ChessBoard::KingSafety(int side)
{
	int K=WhiteKing,sc=0,KingSafetyPen=10,Q=BlackQueen,R=BlackRook,AttackedFilePen=15;
	if(side==Black)
	{
		K=BlackKing;
		Q=WhiteQueen;
		R=WhiteRook;
	}

	if(PiecesList[K].size()==1)
	{
		int KingFile= 7-SquaresAndTheirFiles[Board120[PiecesList[K][0]]];
		int RightFile=-1,LeftFile=-1;
		if(KingFile==FileA)
			RightFile=KingFile+1;
				
		else if(KingFile==FileH)
			LeftFile=KingFile-1;
		else
		{
			RightFile=KingFile+1;
			LeftFile=KingFile-1;
		}

		unsigned long long FILE= FilesBitBoard[KingFile];	
		unsigned long long  OpenFile= (FILE)  ^ (~BitBoardsArray[Empty] &FILE) ^ (1LL<<(63-Board120[PiecesList[K][0]]));		
		if(OpenFile==FILE)
			sc+=KingSafetyPen;
		else
		{
			unsigned long long Attack=(FILE & BitBoardsArray[Q]) | (FILE & BitBoardsArray[R]);
			unsigned long long Defense=( FILE^ (1LL<<(63-Board120[PiecesList[K][0]])) ) & SideTotalBitBoardArray[side];
			if((Attack^Defense)==Attack)
				sc+=AttackedFilePen;
		}
		sc+=SlidingAttackPiecesOnTheKingFiles(RightFile,FILE,OpenFile, side, Q, R,KingSafetyPen,AttackedFilePen);
		sc+=SlidingAttackPiecesOnTheKingFiles(LeftFile,FILE,OpenFile, side, Q, R,KingSafetyPen,AttackedFilePen);
			
	}
	return sc;

}

//checking if there a attackign queen or a rook on the file 
int  ChessBoard::SlidingAttackPiecesOnTheKingFiles(int & file,unsigned long long &FILE,unsigned long long &OpenFile,int & side,int & Q,int & R,int & KingSafetyPen,int & AttackedFilePen)
{
	int sc=0,p=BlackPawn,q=BlackQueen,r=BlackRook;

	if(side==Black)
	{
		p=WhitePawn;
		q=WhiteQueen;
		r=WhiteQueen;
	}

	if(file!=-1)
	{
		FILE= FilesBitBoard[file];	
		OpenFile= (FILE)  ^ (~BitBoardsArray[Empty] &FILE) ;
		if(OpenFile==FILE)
			sc+=KingSafetyPen;

		else
		{
			
			// sliding enemy on the file?
			for(int x=p;x<=q;x++)
			{
				unsigned long long Attack=FILE & BitBoardsArray[x];
				unsigned long long Defense=FILE & SideTotalBitBoardArray[side];
				if(Defense==0 && Attack!=0)
				{
					if(x==r || x==q)
						sc+=AttackedFilePen;
					else
						sc+=(AttackedFilePen-5);
				}

			}
		}
	}
	return sc;

}

//king pawns,this determines where it should castle to the safe side (where the king will be under protection of the pawns not open king) a bonus for protection
int  ChessBoard::KingPawns(int side)
{
	int k=WhiteKing,G=G1,f2=F2,g2=G2,h2=H2,C=C1,d2=D2,c2=C2,b2=B2,p=WhitePawn;
	if(side==Black)
	{
		k=BlackKing;
		G=G8;
		f2=F7;
		g2=G7;
		h2=H7;
		C=C8;
		d2=D7;
		c2=C7;
		b2=B7;
		p=BlackPawn;
	}
	int sc=0;
	if(PiecesList[k].size()==1)
	{
		if(PiecesList[k][0]==G)
		{
			if(BoardArray[f2]==p) 
				sc+=10;
			if(BoardArray[g2]==p) 
				sc+=10;
			if(BoardArray[h2]==p) 
				sc+=10;	
		}
		else if(PiecesList[k][0]==C)
		{
			if(BoardArray[d2]==p) 
				sc+=10;
			if(BoardArray[c2]==p) 
				sc+=10;
			if(BoardArray[b2]==p) 
				sc+=10;	
		
		}
	}


	return sc;
}

//sorting moves according to the scores
void ChessBoard::MovesOrdering(vector<Move>& NonOrderdMoves,int Player)
{
	//sort according to score (low to high if black/high to low if White)
	if(Player==White)
		sort(NonOrderdMoves.begin(), NonOrderdMoves.end(),greater<Move>());
	else
		sort(NonOrderdMoves.begin(), NonOrderdMoves.end());
}

//Parsing the position string and getting the input from the GUI
void ParsePosition(ChessBoard & MyBoard,string fen)
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
void UciLoop()
{

	ChessBoard MyBoard;
	FillRandomNumbersArray();
	while(1)
	{
		string InputFromGUI="";
		fflush(stdout);
		getline (cin,InputFromGUI);

		if(InputFromGUI.substr(0,7)=="isready")
			cout<<"readyok"<<endl;
		else if(InputFromGUI=="uci")
		{
			cout<<"id name "<<NAME<<endl;
			cout<<"id author Abdelrhman"<<endl;
			cout<<"uciok"<<endl;
		}
		else if(InputFromGUI.substr(0,2)=="go")
		{
			//cout<<last<<endl;
			if(Right && (MyBoard.PiecesList[BlackKing].size()!=0 && MyBoard.PiecesList[WhiteKing].size()!=0 ))
			{
				//check first here if it's a draw or black or white win?
				int depth=5,sum=0;
				for(int x=0;x<MyBoard.PiecesList.size();x++)
					sum+=MyBoard.PiecesList[x].size();
				//EndGame
				if(sum<=12)
				{
					depth=6;
					if(!once)
					{
						if(PiecesValues[WhitePawn]==100)
						{
							PiecesValues[WhitePawn]=150;		
							PiecesValues[BlackPawn]=150;		
						}
		
		
						for(int x=8;x<56;x++)
						{
							if(x>=8 && x<48)
								AllPiecesTable[BlackPawn][x]+=PushingPawnBonusEndGame;	
							if(x>=16 && x<56)
								AllPiecesTable[WhitePawn][x]+=PushingPawnBonusEndGame;												
						}
					
						for(int x=0;x<64;x++)
						{					
							MyBoard.MidGameWhiteKingTable[x]=AllPiecesTable[WhiteKing][x];
							AllPiecesTable[WhiteKing][x]=WhiteKingTableEndGame[x];
							MyBoard.MidGameBlackKingTable[x]=AllPiecesTable[BlackKing][x];
							AllPiecesTable[BlackKing][x]=BlackKingTableEndGame[x];
						}
					}
					once=true;
	
					if(sum<=6)
						depth=7;

					//cout<<PiecesValues[WhitePawn]<<"<- Pawn material , Pawn table first value (20)-> "<<AllPiecesTable[WhitePawn][16]<<endl;
					//cout<<"Endgame white king (-50) ->"<<AllPiecesTable[WhiteKing][0]<<endl;

				}
				else
					depth=5;

				MyBoard.IterativeDeepening(depth,NegINF,PosINF,MyBoard.AITurn,depth);
				Move _BestMove=BestMoveData[MyBoard.PositionHashKey].BestMove;
				
				cout<<"bestmove ";
				MyBoard.PrintMove(_BestMove);	
				last=MyBoard.LastMove;
				MyBoard.MakingMove(_BestMove);
				MyBoard.PrintBoardData();
				CastlingData.clear();
				
				
			}	
		}
		else if(InputFromGUI.substr(0,8)=="position")
		{
			ParsePosition(MyBoard,InputFromGUI.substr(9,InputFromGUI.size()-9));
		}
		else if(InputFromGUI.substr(0,10)=="ucinewgame" )
		{
			//alot of initializations here
			
			if(once)
			{
				PiecesValues[WhitePawn]=100;		
				PiecesValues[BlackPawn]=100;	
				
				for(int x=8;x<56;x++)
				{
					if(x>=8 && x<48)
						AllPiecesTable[BlackPawn][x]-=PushingPawnBonusEndGame;	
					if(x>=16 && x<56)
						AllPiecesTable[WhitePawn][x]-=PushingPawnBonusEndGame;												
				}
			

				for(int x=0;x<64;x++)
				{	
					AllPiecesTable[WhiteKing][x]=MyBoard.MidGameWhiteKingTable[x];
					AllPiecesTable[BlackKing][x]=MyBoard.MidGameBlackKingTable[x];
				}
				//cout<<PiecesValues[WhitePawn]<<"<- Pawn material , Pawn table first value (5)-> "<<AllPiecesTable[WhitePawn][16]<<endl;
				//cout<<" white king table first(20) ->"<<AllPiecesTable[WhiteKing][0]<<endl;
			}

			NewGame=true;
			once=false;
			Right=true;
			BestMoveData.clear();
			MyBoard=ChessBoard();
			//ParsePosition(MyBoard,"startpos");
		
		}
		else if(InputFromGUI.substr(0,4)=="quit")
		{
			//break if searchig
			break;
		}
		else if(InputFromGUI.substr(0,4)=="stop")
		{
			cout<<"bestmove ";
			Move _BestMove=BestMoveData[MyBoard.PositionHashKey].BestMove;
			MyBoard.PrintMove(_BestMove);		
			last=MyBoard.LastMove;
			MyBoard.MakingMove(_BestMove);
			MyBoard.PrintBoardData();
				
		}

	}

}


int main()
{	
	cout<<"Lelouch welcomes you "<<endl;
	UciLoop();
	
	return 0;
}
