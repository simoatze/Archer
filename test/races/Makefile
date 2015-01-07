SUF=''
ifeq ($(COMPILER), archer)
        SUF=.archer
	CC=archer
        # LDFLAGS := -lm -L$(HOME)/usr/lib/intelomprt -liomp5_tsan
else # ifeq ($(COMPILER), clang)
        SUF=.tsan
        CC=clang -fopenmp -Wall -O0 -g -fsanitize=thread
	LDFLAGS := -lm -L$(HOME)/usr/lib/intelomprt -liomp5_tsan
endif

PRG_SUFFIX_FLAG := 1

CFLAGS_INC :=
ifdef DEBUG
	CFLAGS := -g -Wall $(CFLAGS_INC) -DVERBOSE
else
	CFLAGS := -g -Wall $(CFLAGS_INC)
endif

ifdef TIMING
	CFLAGS := -g -Wall $(FLAGS_INC) -DTIME
else
	CFLAGS := -g -Wall $(CFLAGS_INC)
endif

SRCS := $(wildcard *.c)
PRGS := $(patsubst %.c,%,$(SRCS))
PRG_SUFFIX=$(SUF)
BINS := $(patsubst %,%$(PRG_SUFFIX),$(PRGS))
OBJS := $(patsubst %,%.o,$(PRGS))

all : $(BINS)

ifeq ($(COMPILER), archer)
.SECONDEXPANSION:
OBJ = $(patsubst %$(PRG_SUFFIX),%.c,$@)
ifeq ($(PRG_SUFFIX_FLAG),0)
	BIN = $(patsubst %$(PRG_SUFFIX),%,$@)
else
	BIN = $@
endif
else
.SECONDEXPANSION:
OBJ = $(patsubst %$(PRG_SUFFIX),%.o,$@)
ifeq ($(PRG_SUFFIX_FLAG),0)
	BIN = $(patsubst %$(PRG_SUFFIX),%,$@)
else
	BIN = $@
endif
endif

ifeq ($(COMPILER), archer)
%$(PRG_SUFFIX) : $(SRCS)
	$(CC) $(OBJ)
else
%$(PRG_SUFFIX) : $(OBJS)
	$(CC) $(OBJ)  $(LDFLAGS) -o $(BIN)
endif

clean:
ifeq ($(PRG_SUFFIX_FLAG),0)
	$(RM) $(PRGS) *.ll *.bc
else
	$(RM) $(BINS) *.ll *.bc
endif

rebuild: veryclean all
