# Makefile


CFLAGS = -c -g -O3 -Wall -std=c++17 -flto -MMD -MP
FINAL_CFLAGS = -g -O3 -Wall -std=c++17 -flto
LDLIBS =

TARGET = happy_arrange
SIMPLE_TARGET = simple
GREEDY_TARGET = greedy

SRC_DIR = ./src
INC_DIR = ./include
OBJ_DIR = ./obj

SRCS = $(shell ls $(SRC_DIR)/*.cpp)
OBJS = $(subst $(SRC_DIR), $(OBJ_DIR), $(SRCS:.cpp=.o))
DEPS = $(OBJS:.o=.d)

SHARED_OBJS = $(OBJ_DIR)/common.o $(OBJ_DIR)/arg_parser.o
SIMPLE_OBJS = $(OBJ_DIR)/simple.o $(SHARED_OBJS)
GREEDY_OBJS = $(OBJ_DIR)/greedy.o $(SHARED_OBJS)

INCLUDE = -I $(INC_DIR)

# for test
TEST_CFLAGS = -c -g -O3 -Wall -std=c++17 -flto -MMD -MP
TEST_FINAL_CFLAGS = -g -O3 -Wall -std=c++17 -flto
TEST_LDLIBS = -lpthread -pthread -lgtest_main -lgtest

TEST_TARGET = unittest

TEST_SRC_DIR = ./test_src
TEST_INC_DIR = ./test_include
TEST_OBJ_DIR = ./test_obj

TEST_SRCS = $(shell ls $(TEST_SRC_DIR)/*.cpp)
TEST_OBJS = $(subst $(TEST_SRC_DIR), $(TEST_OBJ_DIR), $(TEST_SRCS:.cpp=.o))
TEST_DEPS = $(TEST_OBJS:.o=.d)

# テスト対象のオブジェクトファイルからmain関数を含むものを除く
MAIN_SRC = main.cpp
SRCS_WITHOUT_MAIN = $(shell find $(SRC_DIR) -name $(MAIN_SRC) -prune -o -name *.cpp -print)
OBJS_WITHOUT_MAIN = $(subst $(SRC_DIR), $(OBJ_DIR), $(SRCS_WITHOUT_MAIN:.cpp=.o))

TEST_INCLUDE = -I $(TEST_INC_DIR)

CC = g++

all: $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CC) $(INCLUDE) -o $@ $^ $(FINAL_CFLAGS) $(LDLIBS)

$(OBJ_DIR):
	@if [ ! -e $(OBJ_DIR) ] ; then mkdir $(OBJ_DIR) ; fi

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR)
	$(CC) $(INCLUDE) -o $@ $< $(CFLAGS) $(LDLIBS)

$(OBJ_DIR)/simple.o: $(SRC_DIR)/simple/simple.cpp
	$(CC) $(INCLUDE) -o $@ $< $(CFLAGS) $(LDLIBS)

$(OBJ_DIR)/greedy.o: $(SRC_DIR)/greedy/greedy.cpp
	$(CC) $(INCLUDE) -o $@ $< $(CFLAGS) $(LDLIBS)

$(SIMPLE_TARGET): $(SIMPLE_OBJS)
	$(CC) -o $@ $^ $(FINAL_CFLAGS)

$(GREEDY_TARGET): $(GREEDY_OBJS)
	$(CC) -o $@ $^ $(FINAL_CFLAGS)


# for test
test: $(TEST_TARGET)

-include $(TEST_DEPS)

echo:
	# echo $(TEST_SRCS)
	# echo $(TEST_OBJS)
	echo $(OBJS_WITHOUT_MAIN)

$(TEST_OBJ_DIR):
	@if [ ! -e $(TEST_OBJ_DIR) ] ; then mkdir $(TEST_OBJ_DIR) ; fi

$(TEST_TARGET): $(TEST_OBJS) $(OBJS_WITHOUT_MAIN)
	$(CC) $(TEST_INCLUDE) $(INCLUDE) -o $@ $^ $(TEST_FINAL_CFLAGS) $(TEST_LDLIBS)


$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp $(TEST_OBJ_DIR)
	$(CC) $(TEST_INCLUDE) $(INCLUDE) -o $@ $< $(TEST_CFLAGS) $(TEST_LDLIBS)

clean:
	rm -f $(TARGET) $(OBJ_DIR)/* $(SRC_DIR)/*~ $(INC_DIR)/*~ $(TEST_TARGET) $(TEST_OBJ_DIR)/* $(TEST_DEPS) $(TEST_SRC_DIR)/*~ $(TEST_INC_DIR)/*~ ./*~

.PHONY: all clean
