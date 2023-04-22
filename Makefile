# Makeconfig
MAKEFLAGS = --no-print-directory

# Compiler
CC		:=	cc
CFLAGS	:= -g

# Targets
TARGET	:= wolfenstein

# Dependencies
LIB_DEPS := libbutton.a libft.a libmlx42.a libglfw.a
LIB_DIRS := libft MLX42/build Mlx_Module_Button

# Includes
INC_DIRS := $(wildcard */) $(wildcard */*/) $(wildcard */*/*/) $(wildcard */*/*/*/)

# Directories
SRC_DIR := src
BLD_DIR	:= build

# =====DO NOT EDIT BELOW THIS LINE=====

# Sources, Objects and Includes
SRC := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
OBJ := $(addprefix $(BLD_DIR)/, $(SRC:.c=.o))

E_INC_DIRS	:= $(foreach dir, $(INC_DIRS), $(addprefix -I./, $(dir)))
E_LIB_DIRS	:= $(foreach dir, $(LIB_DIRS), $(addprefix -L./, $(dir)))
E_LIB_DEPS	:= $(foreach lib, $(LIB_DEPS), $(addprefix -l, $(subst .a, , $(subst lib, , $(lib)))))

# =======BUILD=======
all:
	@$(MAKE) build_mlx
	@$(MAKE) compile

compile: $(TARGET)

# Linker
$(TARGET): $(OBJ)
	$(MAKE) -C ./libft
	@printf "Creating libft: $(GREEN)OK!\n$(DEF_COLOR)"
	@$(CC) $(CFLAGS) $^ $(E_LIB_DIRS) $(E_LIB_DEPS) -o $@
	@printf "Linking: $(GREEN)OK!\n$(DEF_COLOR)"

# Compiler
$(BLD_DIR)/%.o: %.c
	@mkdir -p $(BLD_DIR)
	@mkdir -p $(@D)
	@$(CC) -c $(CFLAGS) $< $(E_INC_DIRS) -o $@
	@printf "Compiling $@: $(GREEN)OK!\n$(DEF_COLOR)"

# Commands
fclean:
	@$(MAKE) fclean -C ./libft
	@if [ -d "./Mlx_Module_Button" ]; then $(MAKE) fclean -C ./Mlx_Module_Button; fi
	@rm -rf $(BLD_DIR) $(TARGET) $(BONUST)
	@printf "$(BLUE)F_Cleaned!\n$(DEF_COLOR)"

clean:
	@$(MAKE) clean -C ./libft
	@if [ -d "./Mlx_Module_Button" ]; then $(MAKE) clean -C ./Mlx_Module_Button; fi
	@rm -rf $(BLD_DIR)
	@printf "$(BLUE)Cleaned!\n$(DEF_COLOR)"

re: fclean all

delete_mlx:
	rm -rf MLX42
	rm -rf Mlx_Module_Button

run: $(TARGET)
	make && ./$(TARGET)

print-%:
	@echo $* = $($*)

build_mlx:
	@if ! [ -d "./MLX42" ]; then make install_mlx; fi
	@if ! [ -d "./Mlx_Module_Button" ]; then make install_mlx_button; fi

install_mlx:
	git clone https://github.com/codam-coding-college/MLX42.git
	cd MLX42; cmake -B build; cmake --build build -j4

install_mlx_button:
	git clone https://github.com/MrM0lten/Mlx_Module_Button.git
	cd Mlx_Module_Button; $(MAKE)

# Colors
DEF_COLOR	=	\033[0;39m
RED			=	\033[1;31m
GREEN		=	\033[1;32m
YELLOW		=	\033[1;33m
BLUE		=	\033[1;34m
MAGENTA		=	\033[1;35m
CYAN		=	\033[1;36m
WHITE		=	\033[1;37m

