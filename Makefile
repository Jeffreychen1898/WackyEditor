APP_NAME := Wacky

CXX := gcc
CXXFLAGS := -Wall -std=c11 -O3

SRC_FOLDER := ./src
OBJ_FOLDER := ./obj

WEDIT_SRC := $(shell find $(SRC_FOLDER) -name "*.c")
WEDIT_OBJ = $(patsubst $(SRC_FOLDER)/%.c, $(OBJ_FOLDER)/%.o, $(WEDIT_SRC))

#build the program
$(WEDIT_OBJ) : $(OBJ_FOLDER)/%.o : $(SRC_FOLDER)/%.c
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(APP_NAME) : $(WEDIT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lncurses -lm

#helpful stuff
.PHONY: all
all: $(APP_NAME)

.PHONY: app
wedit: $(APP_NAME)

.PHONY: clean
clean:
	rm -rf $(APP_NAME) $(OBJ_FOLDER)
	find . -name '.DS_Store' -type f -delete
	find . -name '.DSYM' -type f -delete
	find . -name '.stackdump' -type f -delete

.PHONY: run
run: $(APP_NAME)
	#./$(APP_NAME)
	leaks -atExit -- ./$(APP_NAME)
