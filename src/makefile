appname  = chip_8
CXX      = g++
CXXFLAGS = -std=c++17 -lSDL2
RM       = rm -f
srcfiles = $(shell find . -name "*.cc")
objects  = $(patsubst %.cc, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(objects) $(LDLIBS)

depend: .depend

.depend: $(srcfiles)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	$(RM) *~ *.o .depend

dist-clean: clean
	$(RM) *~ *.o .depend

include .depend
