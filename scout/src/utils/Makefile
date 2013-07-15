LEX    = flex
YACC   = bison
YFLAGS = -d

OBJS = \
	parser.o \
	scanner.o \
	Pattern.o


all: gen_pattern SCOUT.pattern Generic.pattern MPI.pattern OMP.pattern \
	Message.pattern Counts.pattern MPI_File.pattern RMA.pattern \
	Imbalance.pattern Statistics.pattern
	./gen_pattern SCOUT.pattern
	cp Patterns_gen.h Patterns_gen.cpp ../
	cp Patterns_gen.html ../../doc/patterns/patterns.html

gen_pattern: $(OBJS)
	$(CXX) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) gen_pattern scanner.cpp parser.cpp parser.h \
              Patterns_gen.h Patterns_gen.cpp Patterns_gen.html


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

parser.cpp: parser.y
	$(YACC) $(YFLAGS) parser.y
	mv parser.tab.h parser.h
	mv parser.tab.c parser.cpp

scanner.cpp: scanner.l parser.y
	$(LEX) $(LFLAGS) scanner.l
	mv lex.yy.c scanner.cpp
