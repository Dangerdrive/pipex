


# NAME			=	libftprintf.a

# CC				=	gcc
# CFLAGS			=	-Wall -Wextra -Werror -Ilibft
# AR				=	ar
# ARFLAGS 		=	rcs
# RM				=	rm -rf

# SRC				=	ft_print_ptr ft_print_char ft_print_str ft_print_hex ft_print_unsig ft_printf ft_print_nbr
# SRCS 			=	$(addsuffix .c, $(SRC))

# OBJ_DIR			=	obj
# OBJS			=	$(SRCS:%.c=$(OBJ_DIR)/%.o)

# LIBFT_PATH		=	../libft
# LIBFT			=	$(LIBFT_PATH)/libft.a

# $(OBJ_DIR)/%.o:		%.c
# 					$(CC) $(CFLAGS) -c $< -o $@

# all:				$(NAME)

# bonus:				all

# $(NAME):			$(LIBFT) $(OBJ_DIR) $(OBJS)
# 				cp	$(LIBFT) $(NAME)
# 					$(AR) $(ARFLAGS) $(NAME) $(OBJS)

# $(LIBFT):
# 					make -C $(LIBFT_PATH) all

# $(OBJ_DIR):
# 					mkdir -p $(OBJ_DIR)

# clean:
# 					make -C $(LIBFT_PATH) clean
# 					$(RM) $(OBJ_DIR)

# fclean:				clean
# 					make -C $(LIBFT_PATH) fclean
# 					$(RM) $(NAME)

# re:					fclean all

# .PHONY:				all bonus clean fclean re libft


#suppress directory messages
MAKEFLAGS += --no-print-directory

# Name of the library
NAME		= libftprintf.a

# Compiler and flags
CC		= gcc
CFLAGS		= -Wall -Wextra -Werror -I../libft
AR		= ar
ARFLAGS 	= rcs
RM		= rm -rf

# Source and object directories
SRC_DIR		= .
OBJ_DIR		= obj

# Source and object files
SRC		= ft_print_ptr ft_print_char ft_print_str ft_print_hex \
                  ft_print_unsig ft_printf ft_print_nbr
SRCS		= $(addprefix $(SRC_DIR)/, $(addsuffix .c, $(SRC)))
OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Libft path and archive
LIBFT_PATH	= ../libft
LIBFT		= $(LIBFT_PATH)/libft.a

# Formatting
BOLD		= \033[1m

# Rules
.PHONY: all clean fclean re bonus

# Compile the library
all: $(NAME)

# Linking the library
$(NAME): $(OBJS) $(LIBFT)
	@$(AR) $(ARFLAGS) $(NAME) $(OBJS)
#	@echo "$(BOLD)✅ libftprintf created"

# Compiling source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@ -I$(LIBFT_PATH)
#	@echo "$(BOLD)✔️ Compiling $<"

# Compiling libft
$(LIBFT):
	@make -C $(LIBFT_PATH) all

# Cleaning up object files
clean:
	@$(RM) $(OBJ_DIR)
	@make -C $(LIBFT_PATH) clean
#	@echo "$(BOLD)🗑️  Object files removed"

# Full clean-up
fclean: clean
	@$(RM) $(NAME)
	@make -C $(LIBFT_PATH) fclean
#	@echo "$(BOLD)🗑️  libftprintf removed"

# Re-compiling
re: fclean all