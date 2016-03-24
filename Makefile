CC=g++
#CFLAGS=-O3 -fomit-frame-pointer -fno-rtti -fno-exceptions
CFLAGS=-O2
#CFLAGS=-g -pg
#CFLAGS=-Os -fno-rtti -fno-exceptions -fomit-frame-pointer

all: Revello MoveBenchmark IndividualTest GAOptimize

clean:
	rm -f *.o revello MoveBenchmark IndividualTest GAOptimize

Revello: BitBoard.o Board.o Revello.o Player.o RandomPlayer.o ABPlayer.o TranspositionTable.o OTPEngine.o
	$(CC) $(CFLAGS) BitBoard.o Board.o Revello.o Player.o RandomPlayer.o ABPlayer.o TranspositionTable.o OTPEngine.o -o revello

MoveBenchmark: BitBoard.o Board.o Player.o RandomPlayer.o MoveBenchmark.o
	$(CC) $(CFLAGS) BitBoard.o Board.o Player.o RandomPlayer.o MoveBenchmark.o -o MoveBenchmark

IndividualTest: BitBoard.o Board.o Player.o ABPlayer.o TranspositionTable.o Individual.o IndividualTest.o
	$(CC) $(CFLAGS) BitBoard.o Board.o Player.o ABPlayer.o TranspositionTable.o Individual.o IndividualTest.o -o IndividualTest

GAOptimize: BitBoard.o Board.o Player.o ABPlayer.o TranspositionTable.o Individual.o GAOptimize.o
	$(CC) $(CFLAGS) BitBoard.o Board.o Player.o ABPlayer.o TranspositionTable.o Individual.o GAOptimize.o -o GAOptimize

MoveBenchmark.o:
	$(CC) $(CFLAGS) -c MoveBenchmark.cpp

BitBoard.o:
	$(CC) $(CFLAGS) -c BitBoard.cpp

Board.o:
	$(CC) $(CFLAGS) -c Board.cpp

Revello.o:
	$(CC) $(CFLAGS) -c Revello.cpp

Player.o:
	$(CC) $(CFLAGS) -c Player.cpp

RandomPlayer.o:
	$(CC) $(CFLAGS) -c RandomPlayer.cpp

ABPlayer.o:
	$(CC) $(CFLAGS) -c ABPlayer.cpp

TranspositionTable.o:
	$(CC) $(CFLAGS) -c TranspositionTable.cpp

OTPEngine.o:
	$(CC) $(CFLAGS) -c OTPEngine.cpp

Individual.o:
	$(CC) $(CFLAGS) -c Individual.cpp

IndividualTest.o:
	$(CC) $(CFLAGS) -c IndividualTest.cpp

GAOptimize.o:
	$(CC) $(CFLAGS) -c GAOptimize.cpp

