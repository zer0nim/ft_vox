NAME = ft_vox
PROJECT_NAME = $(shell echo $(NAME) | tr a-z A-Z)# name in MAJUSCULE

ARGS =

SRCS_DIR	= srcs
OBJS_DIR	= objs
INC_DIR		= includes

SRC =	main.cpp

HEAD =	commonInclude.hpp \
		ft_vox.hpp

CC = g++
DEBUG_FLAGS = -g3 -fsanitize=address
LIBS_FLAGS	= -L ~/.brew/lib -framework OpenGL -lglfw -lassimp
CFLAGS = -Wno-deprecated -Ofast -Wall -Wextra -std=c++0x -Werror

HEADS	= $(addprefix $(INC_DIR)/, $(HEAD))
OBJS	= $(addprefix $(OBJS_DIR)/, $(SRC:.cpp=.o))
INC		= -I $(INC_DIR) $(addprefix -I , $(addprefix $(INC_DIR)/, $(dir $(HEAD)))) -I ~/.brew/include

NORMAL = "\x1B[0m"
RED = "\x1B[31m"
GREEN = "\x1B[32m"
YELLOW = "\x1B[33m"
BLUE = "\x1B[34m"
MAGENTA = "\x1B[35m"
CYAN = "\x1B[36m"
WHITE = "\x1B[37m"
BOLD = "\e[1m"
LIGHT = "\e[2m"
ITALIC = "\e[3m"
ULINE = "\e[4m"

START = @printf $(GREEN)$(BOLD)"$(PROJECT_NAME)\n--------------------\n"$(NORMAL)
END = @printf $(GREEN)$(BOLD)"--------------------\n"$(NORMAL)

all:
	$(START)
	@make $(NAME)
	$(END)

$(NAME): $(OBJS_DIR) $(OBJS)
	@printf $(CYAN)"-> create program : $(NAME)\n"$(NORMAL)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBS_FLAGS)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(HEADS)
	@printf $(YELLOW)"-> $<\n"$(NORMAL)
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC)


$(OBJS_DIR):
	@mkdir -p $(dir $(OBJS))

clean:
	$(START)
	@printf $(RED)"-x remove .o files\n"$(NORMAL)
	@rm -rf $(OBJS_DIR)
	$(END)

fclean: clean
	$(START)
	@printf $(RED)"-x remove $(NAME)\n"$(NORMAL)
	@rm -f $(NAME)
	$(END)

re: fclean
	@make

exec:
	@make
	@printf $(MAGENTA)$(BOLD)"EXEC $(PROJECT_NAME)\n--------------------\n"$(NORMAL)
	@./$(NAME) $(ARGS)
	@printf $(MAGENTA)$(BOLD)"--------------------\n"$(NORMAL)

.PHONY: all clean fclean re exec
