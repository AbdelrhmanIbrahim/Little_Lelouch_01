NOT WORKING NOW (fucked up sth after the refactor, back to the repo later)

Little_Lelouch_01 is a simple UCI bitboard chesss engine written in C++.

the engine is a UCI engine can work with chess arena.

no en-passant move implemented in this version yet

some of the implemented algorithms:

a) alpha-beta search
b) iterative deepening search
c) moves ordering using principle variation ,most valuable victim-least valuable attacker,killer moves, history heuristic and positional score
d) null move search
e) aspiration window
f) quiescence search
g) zobrist hashing
h) transposition table
k) hyperbola quintessence
l) static evaluation


Project is a vs2015 one, future plan to get premake in.
If there's any crash with chess arena, you can play on the terminal with UCI protocol, till crash gets fixed.

UCI commands :
1) "ucinewgame" to start a new game
2) "position " with :
  a) "startpos moves" + "chessmovenotation" to make your move in the start of the game
  b) "moves" + "chessmovenotation" to make your move through the game
3) "go" to make lelouch make its move


//TODO
Fix refactoring bugs : king attacks calc is not right and there's sth wrong in cap moves generation that makes it crash.
