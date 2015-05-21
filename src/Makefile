include ../Makefile.config

BIN_DIR 	= ../bin
LIB_DIR 	= ../lib
PROG		= libArcher.so
SRC_FILES	= Util.cpp ArcherDDA.cpp LoadsStoresList.cpp InstructionContext.cpp

O_FILES		= $(SRC_FILES:%.cpp=%.o)

all: $(LIB_DIR) $(PROG) archer-bl
# archer-cmp

$(LIB_DIR): 
	(mkdir -p $(LIB_DIR))

$(PROG): $(O_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(LIB_DIR)/$(PROG) $(SHARED_LIB_OPT) $(O_FILES)

archer-bl:
	$(CXX) $(CPPFLAGS) -o $(BIN_DIR)/archer-bl archer-bl.cpp Util.o

clean:
	$(RM) -rf *.o $(LIB_DIR) $(BIN_DIR)/archer-cmp $(BIN_DIR)/archer-bl
