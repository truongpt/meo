CC = gcc
LD = gcc
LDFLAGS  += 
CMNFLAGS := -g -O0

TARGET = meo
TOP_SRC = src
INC := -Iinc

SRC := $(TOP_SRC)/meo.c
SRC += $(TOP_SRC)/lex.c
SRC += $(TOP_SRC)/parse.c
SRC += $(TOP_SRC)/gen.c

CFLAGS += $(INC)

# generate dependence file
OBJDIR = .
OBJ += $(SRC:%.c=$(OBJDIR)/%.o)

# Add target to build library
all:TARGET

TARGET:$(TARGET)

$(TARGET):$(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

$(OBJDIR)/%.o : %.c
	$(CC) $(CMNFLAGS) $(CFLAGS) -c $< -o $@

clean:
	-rm $(OBJ)
	-rm $(TARGET)

.PHONY:clean all TARGET

-include $(shell mkdir $(OBJDIR) 2>/dev/null) $(wildcard $(OBJDIR)/*.d)
