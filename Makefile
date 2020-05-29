CXX = g++
CXXFLAGS = -std=c++11 -Wall -w

SOURCE = AIG.cpp aigraph.cpp AND.cpp input.cpp latch.cpp nodeAig.cpp output.cpp
ERROR = ErrorEstimator.cpp main.cpp

OBJECTS = AIG.o aigraph.o AND.o input.o latch.o nodeAig.o output.o main.o ErrorEstimator.o

HEADERS = AIG.h ErrorEstimator.h

main: $(SOURCE) $(ERROR)
	$(CXX) $(CXXFLAGS) -c $^
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(HEADERS) -o $@

$(SOURCE) : AIG.h 

$(ERROR) : AIG.h ErrorEstimator.h

clean:
	rm *.o main