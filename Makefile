#suppress directory messages
MAKEFLAGS	+=	--no-print-directory

CC			=	cc
CFLAGS		=	-Werror -Wextra -Wall -Ilibft -Ift_printf
NAME		=	pipex

SRC_PATH	=	src/
OBJ_PATH	=	src/obj/

LIBFT_PATH	=	./libs/libft
LIBFT		=	$(LIBFT_PATH)/libft.a

FTPRINTF_PATH		= ./libs/ft_printf
FTPRINTF	=	$(FTPRINTF_PATH)/libftprintf.a

INC			=	-I ./includes/ -I $(LIBFT_PATH)/ -I $(FTPRINTF_PATH)/


SRC			=	data_init.c \
				file_handler.c \
				parse_envp.c \
				pipex_utils.c \
				pipex.c

# Formatting
BOLD		=	\033[1m
NO_FORMAT	=	\033[0m

SRCS		=	$(addprefix $(SRC_PATH), $(SRC))
OBJ			=	$(SRC:.c=.o)
OBJS		=	$(addprefix $(OBJ_PATH), $(OBJ))

$(OBJ_PATH)%.o:$(SRC_PATH)%.c
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC)
	@echo "$(BOLD)✔️ Compiling $<$(NO_FORMAT)"

all:
	@echo ""
	@$(MAKE) $(NAME)

$(NAME):$(OBJS) $(LIBFT) $(FTPRINTF)
	@$(CC) $(CFLAGS) $(OBJS) -o $@ -L$(FTPRINTF_PATH) -lftprintf -L$(LIBFT_PATH) -lft
	@echo "\n✅	$(BOLD)Pipex compiled$(NO_FORMAT)"

$(LIBFT):
	@make -C $(LIBFT_PATH) all
	@echo "\n✔️	$(BOLD)Libft compiled$(NO_FORMAT)"

$(FTPRINTF):
	@make -C $(FTPRINTF_PATH) all
	@echo	"\n✔️	$(BOLD)ft_printf compiled$(NO_FORMAT)"

bonus: all

clean:
	@echo ""
	@make -C $(LIBFT_PATH) clean
	@make -C $(FTPRINTF_PATH) clean
	@rm -rf $(OBJ_PATH)
	@echo		"🗑️	$(BOLD)Object files removed$(NO_FORMAT)"

fclean: clean
	@echo ""
	@make -C $(LIBFT_PATH) fclean
	@make -C $(FTPRINTF_PATH) fclean
	@rm -f $(NAME)
	@echo		"🗑️	$(BOLD)Pipex removed$(NO_FORMAT)"

re: fclean all

test:
	cd test/ && ./test.sh

.PHONY: all re clean fclean bonus test
