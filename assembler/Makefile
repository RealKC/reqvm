# The Ark Makefile:tm:
NAME= assembler

CC= g++
CFLAGS= -std=c++17 -Wall -Wextra -fexceptions
LDFLAGS= 

SRCDIR= src
OBJDIR= obj
# END CONFIG

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard,$(SRCDIR),*.cpp)
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

DEBUG = yes
ifeq ($(DEBUG), yes)
	CFLAGS += -Og -g
else
	CFLAGS += -O3
endif
AGGRESIVE_LOGGING = yes
ifeq ($(AGGRESIVE_LOGGING), yes)
	CFLAGS += -DAGGRESIVE_LOGGING -Ithirdparty/magic_enum/include
endif
# AUTO VARIABLE DEFINITION


build: $(NAME)

thirdparty/magic_enum/include/magic_enum.hpp:
	git submodule init # Is this the correct git submodule workflow? if not and
	git submodule update # you're a git expert, plz help

$(NAME): thirdparty/magic_enum/include/magic_enum.hpp $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS)

-include $(OBJ:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
		@mkdir -p $(@D)
		$(CC) -o $@ $< $(CFLAGS) -c -MMD
		@mv -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
		@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
		@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | \
			sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
		@sed -i '/\\\:/d' $(OBJDIR)/$*.d
		@rm -f $(OBJDIR)/$*.d.tmp

.PHONY: clean
clean:
		rm -f $(NAME)
		rm -rf $(OBJDIR)/*
