CC		:= cc
CFLAGS	:= -Wall -Werror -Wextra
SRC_DIR	:= src
BLD_DIR	:= build
TARGET	:= libft.a

SRC		:= $(wildcard $(SRC_DIR)/*.c)
OBJ		:= $(addprefix $(BLD_DIR)/, $(SRC:.c=.o))

all: $(TARGET)

$(TARGET): $(OBJ)
	@ar -rcs $(TARGET) $(OBJ)

$(BLD_DIR)/%.o: %.c
	@mkdir -p $(BLD_DIR)
	@mkdir -p $(@D)
	@$(CC) -c $(CFLAGS) $^ -o $@ 

clean:
	@rm -rf $(BLD_DIR)

fclean:
	@rm -rf $(BLD_DIR) $(TARGET)

re: fclean all

print-%:
	@echo $* = $($*)
