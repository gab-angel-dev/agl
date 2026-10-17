CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -MMD -MP
SRC_DIR = src
OBJ_DIR = obj
BIN = agl
PREFIX ?= /usr/local

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Templates: templates/*.agl viram strings dentro do binário
TEMPLATES = $(sort $(wildcard templates/*.agl))
TPL_C = $(OBJ_DIR)/templates_data.c
TPL_O = $(OBJ_DIR)/templates_data.o
OBJS += $(TPL_O)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(BIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TPL_C): $(TEMPLATES) scripts/gen_templates.sh
	mkdir -p $(OBJ_DIR)
	sh scripts/gen_templates.sh $@ $(TEMPLATES)

$(TPL_O): $(TPL_C)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

# make debug: binário com AddressSanitizer (acha erros de memória)
debug: CFLAGS += -g -fsanitize=address,undefined
debug: clean
	$(MAKE) CFLAGS="$(CFLAGS)" LDFLAGS="-fsanitize=address,undefined" $(BIN)

install: $(BIN)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BIN) $(DESTDIR)$(PREFIX)/bin/$(BIN)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: clean debug install uninstall