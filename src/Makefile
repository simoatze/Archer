include ../Makefile.config

BIN_DIR 	= ../bin
LIB_DIR 	= ../lib
PROG		= libArcher.so
SRC_FILES	= ArcherDDA.cpp LoadsStoresList.cpp InstructionContext.cpp

O_FILES		= $(SRC_FILES:%.cpp=%.o)

all: $(LIB_DIR) $(PROG) archer-cmp

$(LIB_DIR): 
	(mkdir -p $(LIB_DIR))

$(PROG): $(O_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(LIB_DIR)/$(PROG) $(SHARED_LIB_OPT) $(O_FILES)

archer-cmp:
	$(CXX) $(CPPFLAGS) -o $(BIN_DIR)/archer-cmp archer-cmp.cc

clean:
	$(RM) -rf *.o $(LIB_DIR) $(BIN_DIR)/archer-cmp
