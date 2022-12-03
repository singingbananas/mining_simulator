CPP := g++
CPPFLAGS := -std=c++14 -Wall -g

LGSL := $(shell gsl-config --libs)
IGSL := $(shell gsl-config --cflags)

LBLAS := -L/home/linuxbrew/.linuxbrew/opt/openblas/lib
IBLAS := -I/home/linuxbrew/.linuxbrew/opt/openblas/include

LIB := -L./
INC := -I./

LDLIBS := -lgsl -lcblas

SRCS := $(wildcard BlockSim/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

STRAT_SRCS := $(wildcard StratSim/*.cpp)
STRAT_OBJS := $(patsubst %.cpp,%.o,$(STRAT_SRCS))

all: strat selfish

strat: $(STRAT_SRCS) $(OBJS)
	$(CPP) $(CPPFLAGS) $(INC) $(IBLAS) $(IGSL) $(LDLIBS) $(LGSL) $(LBLAS) -o $@ $^

selfish-bv: SelfishBlockValueSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-cm: SelfishCostMiningSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-nexp: SelfishNetworkExpSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-nlin: SelfishNetworkLinearSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-npoiss: SelfishNetworkPoissonSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-nuni: SelfishNetworkUniformSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-comp: SelfishClassicClever/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish: SelfishSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(INC) $(IGSL) $(IBLAS) $(LGSL) $(LBLAS) $(LDLIBS) -o $@ -c $<

clean:
	rm -rf BlockSim/*.o *.o strat selfish

.PHONY: all clean

