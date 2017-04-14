CCC = clang++
LEX = flex
YACC = bison
CFLAGS = -Wall -std=c++11
LEXFLAGS = -Wno-unused

#LEXDEBUG = -d
LEXDEBUG =
YACCDEBUG = -vtd

OBJS = main.o symTable.o tableManager.o ast.o parse.tab.o lex.yy.o

run: $(OBJS)
	$(CCC) $(CFLAGS) -o run $(OBJS)

main.o: main.cpp parse.y scan.l parse.tab.cpp lex.yy.cpp \
	symTable.o tableManager.o
	$(CCC) $(CFLAGS) -c main.cpp

tableManager.o: tableManager.cpp tableManager.h
	$(CCC) $(CFLAGS) -c tableManager.cpp


symbolTable.o: symTable.cpp symTable.h
	$(CCC) $(CFLAGS) -c symTable.cpp

ast.o: ast.cpp ast.h symTable.h tableManager.h
	$(CCC) $(CFLAGS) -c ast.cpp

parse.tab.cpp: parse.y ast.h symTable.h tableManager.h
	$(YACC) -o parse.tab.cpp $(YACCDEBUG) parse.y

parse.tab.o: parse.tab.cpp
	$(CCC) $(CFLAGS) -c parse.tab.cpp

lex.yy.cpp: scan.l parse.tab.o
	$(LEX) -o lex.yy.cpp $(LEXDEBUG) scan.l

lex.yy.o: lex.yy.cpp
	$(CCC) $(CFLAGS) $(LEXFLAGS) -c lex.yy.cpp
	
clean:
	rm -f run *.o *.o parse.tab.cpp lex.yy.cpp
	rm -f parse.tab.hpp
	rm -f parse.output
