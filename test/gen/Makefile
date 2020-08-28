CPP = g++ -std=c++11
CC = gcc
LD = g++ -std=c++11
LDFLAGS  += 
CPPFLAGS := -g -O0 -fpermissive -Wnarrowing -Wno-write-strings

#-fpermissive -Wnarrowing

TARGET = gen_test
TOP_SRC = ../../
TOP_TEST = ../../test


INC := -I$(TOP_SRC)/inc
INC += -I$(TOP_TEST)/inc
INC += -I$(TOP_SRC)/src

SRC_CPP := gen_test.cpp
SRC_CPP += mock_arch.cpp
SRC_CPP += main.cpp
SRC := $(TOP_SRC)/src/gen.c
SRC += $(TOP_SRC)/src/log.c

CFLAGS += $(INC)

# generate dependence file
OBJDIR = .
OBJ += $(SRC:%.c=$(OBJDIR)/%.o)
OBJ_CPP += $(SRC_CPP:%.cpp=$(OBJDIR)/%.o)

# Add target to build library
all:TARGET

TARGET:$(TARGET)

$(TARGET):$(OBJ) $(OBJ_CPP)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(OBJ_CPP) $(LIBS)

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	-rm $(OBJ) $(OBJ_CPP)
	-rm $(TARGET)

.PHONY:clean all TARGET

-include $(shell mkdir $(OBJDIR) 2>/dev/null) $(wildcard $(OBJDIR)/*.d)
