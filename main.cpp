#include "Board.h"
#include "Helpers.h"
#include "Knowledge.h"
#include "Move.h"
#include <iostream>

using namespace helpers;
using namespace std;

void UciLoop()
{
	Board MyBoard;
	FillRandomNumbersArray(MyBoard);
	bool once = false;

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
			if(MyBoard.r && (MyBoard.PiecesList[BlackKing].size()!=0 && MyBoard.PiecesList[WhiteKing].size()!=0 ))
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
						if(MyBoard.PiecesValues[WhitePawn]==100)
						{
							MyBoard.PiecesValues[WhitePawn]=150;
							MyBoard.PiecesValues[BlackPawn]=150;
						}


						for(int x=8;x<56;x++)
						{
							if(x>=8 && x<48)
								MyBoard.AllPiecesTable[BlackPawn][x]+=PushingPawnBonusEndGame;
							if(x>=16 && x<56)
								MyBoard.AllPiecesTable[WhitePawn][x]+=PushingPawnBonusEndGame;
						}

						for(int x=0;x<64;x++)
						{					
							MyBoard.MidGameWhiteKingTable[x]=MyBoard.AllPiecesTable[WhiteKing][x];
							MyBoard.AllPiecesTable[WhiteKing][x]=WhiteKingTableEndGame[x];
							MyBoard.MidGameBlackKingTable[x]=MyBoard.AllPiecesTable[BlackKing][x];
							MyBoard.AllPiecesTable[BlackKing][x]=BlackKingTableEndGame[x];
						}
					}
					once=true;

					if(sum<=6)
						depth=7;

					//cout<<PiecesValues[WhitePawn]<<"<- Pawn material , Pawn table first value (20)-> "<<AllPiecesTable[WhitePawn][16]<<endl;
					//cout<<"Endgame white king (-50) ->"<<AllPiecesTable[WhiteKing][0]<<endl;

				}

				if(MyBoard.CastlingPermission==0)
					MyBoard.NoMoreCastling=true;

				MyBoard.IterativeDeepening(depth,NegINF,PosINF,MyBoard.AITurn,depth);
				Move _BestMove=MyBoard.BestMoveData[MyBoard.PositionHashKey].BestMove;

				cout<<"bestmove ";
				MyBoard.PrintMove(_BestMove);
				last=MyBoard.LastMove;
				MyBoard.MakingMove(_BestMove);
				MyBoard.PrintBoardData();
				if(!MyBoard.NoMoreCastling)
					MyBoard.CastlingData.clear();


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
				MyBoard.PiecesValues[WhitePawn]=100;
				MyBoard.PiecesValues[BlackPawn]=100;

				for(int x=8;x<56;x++)
				{
					if(x>=8 && x<48)
						MyBoard.AllPiecesTable[BlackPawn][x]-=PushingPawnBonusEndGame;	
					if(x>=16 && x<56)
						MyBoard.AllPiecesTable[WhitePawn][x]-=PushingPawnBonusEndGame;
				}


				for(int x=0;x<64;x++)
				{	
					MyBoard.AllPiecesTable[WhiteKing][x]=MyBoard.MidGameWhiteKingTable[x];
					MyBoard.AllPiecesTable[BlackKing][x]=MyBoard.MidGameBlackKingTable[x];
				}
				//cout<<PiecesValues[WhitePawn]<<"<- Pawn material , Pawn table first value (5)-> "<<AllPiecesTable[WhitePawn][16]<<endl;
				//cout<<" white king table first(20) ->"<<AllPiecesTable[WhiteKing][0]<<endl;
			}

			MyBoard.NewGame=true;
			once=false;
			MyBoard.r=true;
			MyBoard.BestMoveData.clear();
			MyBoard=Board();
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
			Move _BestMove=MyBoard.BestMoveData[MyBoard.PositionHashKey].BestMove;
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
