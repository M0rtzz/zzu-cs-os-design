CC = clang
CFLAGS = -std=gnu11 -Wall -g -O2 -pthread # 避免使用-std=c11，否则将无法使用一些必要的函数和类型
SRC_FILES := $(wildcard *.c)
OUT_DIR := out
OUT_FILES := $(patsubst %.c,$(OUT_DIR)/%.out,$(SRC_FILES))

all: $(OUT_FILES)

$(OUT_DIR)/%.out: %.c | $(OUT_DIR)
	$(CC) $< -o $@ $(CFLAGS)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

.PHONY: clean

clean:
	rm -rf $(OUT_DIR)
