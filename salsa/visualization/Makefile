CXX      = g++
CXXFLAGS = -Wall

salsa_rebuild: salsa_rebuild.o
	$(CXX) $(CXXFLAGS) -o salsa_rebuild salsa_rebuild.o
	
salsa_rebuild.o: salsa_rebuild.cpp Comm_Matrix.hpp
	$(CXX) $(CXXFLAGS) -o salsa_rebuild.o -c salsa_rebuild.cpp

clean:
	rm *.o