CPP := g++
CPPFLAGS := -std=c++14 -Wall -g

LGSL := $(shell gsl-config --libs)
IGSL := $(shell gsl-config --cflags)

LBLAS := -L/opt/homebrew/opt/openblas/lib
IBLAS := -I/opt/homebrew/opt/openblas/include

LIB := -L./
INC := -I./

LDLIBS := -lgsl -lcblas

SRCS := $(wildcard BlockSim/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

STRAT_SRCS := $(wildcard StratSim/*.cpp)
STRAT_OBJS := $(patsubst %.cpp,%.o,$(STRAT_SRCS))

all: strat selfish

strat: $(STRAT_SRCS) $(OBJS)
	$(CPP) $(CPPFLAGS) $(INC) $(IBLAS) $(IGSL) $(LDLIBS) $(LGSL) $(LBLAS)-o $@ $^

selfish: SelfishSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS) $(LDLIBS) $(LGSL) $(LBLAS) -o $@ $^

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(INC) $(IGSL) $(IBLAS) $(LDLIBS) $(LGSL) $(LBLAS)-o $@ -c $<

clean:
	rm -rf BlockSim/*.o *.o strat selfish

.PHONY: all clean

