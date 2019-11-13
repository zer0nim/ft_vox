NAME = ft_vox
PROJECT_NAME = $(shell echo $(NAME) | tr a-z A-Z)  # name in MAJUSCULE

ARGS =

SRCS_DIR	= srcs
OBJS_DIR	= objs
INC_DIR		= includes

SRC =	main.cpp \
		chunk/AChunk.cpp \
		chunk/ChunkManager.cpp \
		chunk/Chunk.cpp \

HEAD =	commonInclude.hpp \
		ft_vox.hpp \
		chunk/AChunk.hpp \
		chunk/ChunkManager.hpp \
		chunk/Chunk.hpp \


# download the cpp linter (https://github.com/isocpp/CppCoreGuidelines)
# set command to launch linter on LINTER
# add rules for linter in LINTER_RULES
LINTER = $(CPPLINT)
LINTER_RULES = --filter=-whitespace/tab,-legal/copyright,-build/c++11 --linelength=120

CC = g++
DEBUG_FLAGS = -g3 -fsanitize=address
LIBS_FLAGS	= -L ~/.brew/lib -framework OpenGL -lglfw
LIBS_INC	= ~/.brew/include
CFLAGS		= -Ofast -std=c++11 -Wall -Wextra -Werror

ifneq ($(DEBUG),)
	CFLAGS := $(CFLAGS) $(DEBUG_FLAGS)
endif

HEADS	= $(addprefix $(INC_DIR)/, $(HEAD))
OBJS	= $(addprefix $(OBJS_DIR)/, $(SRC:.cpp=.o))
INC		= -I $(INC_DIR) $(addprefix -I , $(addprefix $(INC_DIR)/, $(dir $(HEAD)))) $(addprefix -I , $(LIBS_INC))

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

init:
	$(START)
	@printf $(CYAN)"create pre-commit\n"$(NORMAL)
	@cp .pre-commit .git/hooks/pre-commit
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

exec-nolint:
	@make
	@printf $(MAGENTA)$(BOLD)"EXEC $(PROJECT_NAME)\n--------------------\n"$(NORMAL)
	@./$(NAME) $(ARGS)
	@printf $(MAGENTA)$(BOLD)"--------------------\n"$(NORMAL)

exec:
	@make lint ; true
	@make exec-nolint ; true

lint:
	@printf $(BLUE)$(BOLD)"LINTER ON $(PROJECT_NAME)\n--------------------\n"$(NORMAL)
	@if [ "$(LINTER)" = "" ]; then\
		printf $(RED)$(BOLD)"Error:"$(NORMAL)" env var CPPLINT is not set\n"; \
	else \
		$(LINTER) $(LINTER_RULES) $(HEADS) $(addprefix $(SRCS_DIR)/, $(SRC)); \
    fi
	@printf $(BLUE)$(BOLD)"--------------------\n"$(NORMAL)

check:
	@make fclean
	@make lint
	@make exec-nolint

help:
	@printf $(YELLOW)$(BOLD)"HELP\n--------------------\n"$(NORMAL)
	@printf $(NORMAL)"-> make "$(BOLD)"init"$(NORMAL)": init the project\n"
	@printf $(NORMAL)"-> make "$(BOLD)"all"$(NORMAL)": build the project and create $(NAME)\n"
	@printf $(NORMAL)"-> make "$(BOLD)"clean"$(NORMAL)": remove all .o files\n"
	@printf $(NORMAL)"-> make "$(BOLD)"fclean"$(NORMAL)": make clean and remove executable\n"
	@printf $(NORMAL)"-> make "$(BOLD)"re"$(NORMAL)": make fclean and make all\n"
	@printf $(NORMAL)"-> make "$(BOLD)"lint"$(NORMAL)": exec linter on project (use env var CPPLINT)\n"
	@printf $(NORMAL)"-> make "$(BOLD)"exec"$(NORMAL)": make lint, make all and exec the program with ARGS='$(ARGS)'\n"
	@printf $(NORMAL)"-> make "$(BOLD)"exec-nolint"$(NORMAL)": make all and exec the program with ARGS='$(ARGS)'\n"
	@printf $(NORMAL)"-> make "$(BOLD)"check"$(NORMAL)": make fclean, make lint, make exec-nolint -> stop if there is an error\n"
	@printf $(NORMAL)"-> make "$(BOLD)"help"$(NORMAL)": show the help\n"
	@printf $(NORMAL)"-> make "$(BOLD)"... DEBUG=1"$(NORMAL)": use debug mode\n"
	@printf $(YELLOW)$(BOLD)"--------------------\n"$(NORMAL)

.PHONY: init all clean fclean re exec-nolint exec lint check help
