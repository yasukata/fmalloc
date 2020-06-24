PROGS = fmalloc_list

CLEANFILES = $(PROGS) *.o

SRCDIR ?= ./

FMALLOCDIR ?= ../../

CC=g++

NO_MAN=
CPPFLAGS = -O3 -pipe -g
CPPFLAGS += -Werror -Wall -Wunused-function
CPPFLAGS += -Wextra
CPPFLAGS += -I$(SRCDIR) -I$(FMALLOCDIR)
CPPFLAGS += -std=c++11
CPPFLAGS += -L$(FMALLOCDIR)

LDLIBS += -lfmalloc -lpthread
LDFLAGS += -L$(FMALLOCDIR)

SRCS = list.cpp
OBJS += $(SRCS:.cpp=.o)

.PHONY: all
all: $(PROGS)

$(PROGS): $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	-@rm -rf $(CLEANFILES)
