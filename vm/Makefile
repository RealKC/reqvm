# The Ark Makefile:tm:
NAME= vm

CC= g++
CFLAGS= -std=c++17 -Wall -Wextra -fexceptions
LDFLAGS= 

SRCDIR= src
OBJDIR= obj
# END CONFIG


rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard,$(SRCDIR),*.cpp)
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

#$(warning $(OBJ))
#$(warning $(SRC))

DEBUG = yes
ifeq ($(DEBUG), yes)
	CFLAGS += -Og -g
else
	CFLAGS += -O3
endif
# AUTO VARIABLE DEFINITION


build: $(NAME)

$(NAME): $(OBJ)
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
