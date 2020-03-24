# The Ark Makefile:tm:
NAME= vm

CC= g++
CFLAGS= -std=c++11 -Wall -Wextra -fexceptions
LDFLAGS= 

SRCDIR= src
OBJDIR= obj
# END CONFIG

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard,$(SRCDIR),*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

DEBUG = no
ifeq ($(DEBUG), yes)
  CFLAGS += -O0 -g
else
  CFLAGS += -O3
endif
# AUTO VARIABLE DEFINITION


build: $(NAME)

$(NAME): $(OBJ)
    $(CC) -o $@ $^ $(CFLAGS)

-include $(OBJ:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
    @mkdir -p $(@D)
    $(CC) -o $@ $< $(CFLAGS) -c -MMD
    @mv -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
    @sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
    @sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | \
      sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
    @rm -f $(OBJDIR)/$*.d.tmp

.PHONY: clean
clean:
    rm -f $(NAME)
    rm -rf $(OBJDIR)/*