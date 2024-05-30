# 定义编译器
CC=g++

# 定义编译选项
CFLAGS=-Wall -Wextra -pthread

# 定义项目源文件目录和目标二进制文件目录
SRC_DIR=src
BIN_DIR=bin

# 定义所有源文件和目标文件
SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SOURCES))
TARGET=$(BIN_DIR)/test
# 运行目标文件
run:$(TARGET)
	./$(TARGET)

# 默认目标
all: $(TARGET)

# 目标依赖于对象文件
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# 编译对象文件
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# 清理编译生成的文件
clean:
	rm -f $(BIN_DIR)/*.o $(TARGET)

# 伪目标，避免make多次执行
.PHONY: all clean run

# 包含自动生成的依赖文件
-include $(OBJECTS:.o=.d)