# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -g -I./inc
LDFLAGS = -lpthread -lsqlite3
# -lncurses sudo apt install libncursesw5-dev

# 目标文件
TARGET = ./bin/cloud_chat

# 源文件目录和对象文件目录
SRC_DIR = ./src
OBJ_DIR = ./obj

# 源文件和对应的对象文件
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# 目标: 生成可执行文件
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)  # 确保目标目录存在
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# 生成对象文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # 确保对象文件目录存在
	$(CC) $(CFLAGS) -c $< -o $@

# 清理目标
.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
