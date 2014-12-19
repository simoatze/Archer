include ../Makefile.config

LIB_DIR 	= ../lib
PROG		= libArcher.so
SRC_FILES	= ArcherDDA.cpp

O_FILES		= $(SRC_FILES:%.cpp=%.o)

all: $(LIB_DIR) $(PROG)

$(LIB_DIR): 
	(mkdir -p $(LIB_DIR))

$(PROG): $(O_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(LIB_DIR)/$(PROG) $(SHARED_LIB_OPT) $(O_FILES)

clean:
	$(RM) -rf *.o $(LIB_DIR)
