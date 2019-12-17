# Makefile
# for each projects, change value of:
#	-> NAME
#	-> SRC
#	-> HEAD
#	-> LIBS_FLAGS
#	-> LIBS_INC

# you can also configure your Makefile with these variables:
#	-> ARGS
#	-> LINTER_RULES
#	-> DEBUG_FLAGS
#	-> PRE_COMMIT
#	-> PRE_PUSH

# after this, init the project:
#	make init

# to get help:
#	make help

# to run:
#	make -j8
#	make DEBUG=1 -j8

NAME = ft_vox
PROJECT_NAME = $(shell echo $(NAME) | tr a-z A-Z)  # name in MAJUSCULE

ARGS =

SRCS_DIR	= srcs
OBJS_DIR	= objs
INC_DIR		= includes
DEP_DIR		= .dep
DEBUG_DIR	= $(DEP_DIR)

SRC =	main.cpp \
		windowEvents.cpp \
		TextureManager.cpp \
		ft_vox.cpp \
\
		chunk/AChunk.cpp \
		chunk/ChunkManager.cpp \
		chunk/Chunk.cpp \
		chunk/GreedyChunk.cpp \
		chunk/GreedyChunk2.cpp \
		chunk/GreedyChunk3.cpp \
		chunk/MapGenerator.cpp \
\
		utils/Texture.cpp \
		utils/Shader.cpp \
		utils/Skybox.cpp \
		utils/Camera.cpp \
		utils/CameraCreative.cpp \
		utils/CameraSurvival.cpp \
		utils/debug.cpp \
		utils/TextRender.cpp \
		utils/Material.cpp \
		utils/Stats.cpp \
		utils/Logging.cpp \

HEAD =	commonInclude.hpp \
		ft_vox.hpp \
		TextureManager.hpp \
\
		chunk/AChunk.hpp \
		chunk/ChunkManager.hpp \
		chunk/Chunk.hpp \
		chunk/GreedyChunk.hpp \
		chunk/GreedyChunk2.hpp \
		chunk/GreedyChunk3.hpp \
		chunk/MapGenerator.hpp \
\
		utils/Texture.hpp \
		utils/Shader.hpp \
		utils/Skybox.hpp \
		utils/Camera.hpp \
		utils/CameraCreative.hpp \
		utils/CameraSurvival.hpp \
		utils/debug.hpp \
		utils/TextRender.hpp \
		utils/Material.hpp \
		utils/Stats.hpp \
		utils/Logging.hpp \

# download the cpp linter (https://github.com/isocpp/CppCoreGuidelines)
# set command to launch linter on LINTER
# add rules for linter in LINTER_RULES
LINTER = $(CPPLINT)
LINTER_RULES =	--filter=-whitespace/tab,-legal/copyright,-build/c++11,-whitespace/newline,-readability/braces,-whitespace/indent,-build/include_what_you_use,-build/header_guard,-runtime/references \
				--linelength=120 --quiet

CC = g++
DEBUG_FLAGS = -g3 -fsanitize=address -DDEBUG=true
LIBS_FLAGS	=	-L ~/.brew/lib -framework OpenGL -lglfw -L ~/.brew/opt/freetype/lib -lfreetype \
				-lboost_filesystem
LIBS_INC	=	~/.brew/include \
				$(INC_DIR)/lib/stb_image.h \
				$(INC_DIR)/lib/json.hpp \
				$(INC_DIR)/lib/PerlinNoise.hpp \
				/usr/local/opt/freetype/include/freetype2 \
				~/.brew/opt/freetype/include/freetype2

DEPFLAGS	= -MT $@ -MD -MP -MF $(DEP_DIR)/$*.Td
CFLAGS		= -Ofast -std=c++11 -Wall -Wextra -Wno-deprecated
DEFINE		=

# setup debug mode or normal mode
ifneq ($(DEBUG),)
	CFLAGS := $(CFLAGS) $(DEBUG_FLAGS)
else
	CFLAGS := $(CFLAGS) -Werror
endif

# send define to program
CFLAGS := $(CFLAGS) $(addprefix -D, $(DEFINE))

# pre-commit rules
PRE_COMMIT_FILE = .git/hooks/pre-commit
define PRE_COMMIT
#!/bin/zsh

make lint
res=$$?

exit $${res}
endef
export PRE_COMMIT

# pre-push rules
PRE_PUSH_FILE = .git/hooks/pre-push
define PRE_PUSH
#!/bin/zsh

make check -j8
res=$$?

exit $${res}
endef
export PRE_PUSH

HEADS		= $(addprefix $(INC_DIR)/, $(HEAD))
OBJS		= $(addprefix $(OBJS_DIR)/, $(SRC:.cpp=.o))
DEPFILES	= $(addprefix $(DEP_DIR)/, $(SRC:.cpp=.d))
INC			= -I $(INC_DIR) $(addprefix -I , $(addprefix $(INC_DIR)/, $(dir $(HEAD)))) $(addprefix -I , $(LIBS_INC))

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
ifneq ($(DEBUG),)
	@if [[ -d $(DEBUG_DIR) && ! -f $(DEBUG_DIR)/DEBUG ]] ; then \
		make fclean; \
	fi;
else
	@if [[ -d $(DEBUG_DIR) && -f $(DEBUG_DIR)/DEBUG ]] ; then \
		make fclean; \
	fi;
endif
	$(START)
	@make $(NAME)
	$(END)
ifneq ($(DEBUG),)
	@touch $(DEBUG_DIR)/DEBUG
else
	@rm -f $(DEBUG_DIR)/DEBUG
endif

init:
	$(START)
	@printf $(CYAN)"create pre-commit\n"$(NORMAL)
	@echo "$$PRE_COMMIT" > $(PRE_COMMIT_FILE)
	@chmod 755 $(PRE_COMMIT_FILE)
	@printf $(CYAN)"create pre-push\n"$(NORMAL)
	@echo "$$PRE_PUSH" > $(PRE_PUSH_FILE)
	@chmod 755 $(PRE_PUSH_FILE)
	$(END)

$(NAME): $(OBJS_DIR) $(OBJS)
	@printf $(CYAN)"-> create program : $(NAME)\n"$(NORMAL)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBS_FLAGS)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(DEP_DIR)/%.d
	@printf $(YELLOW)"-> $<\n"$(NORMAL)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@ $(INC)
	@mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d

$(OBJS_DIR):
	@mkdir -p $(dir $(OBJS))

$(DEP_DIR):
	@mkdir -p $(dir $(DEPFILES))

$(DEP_DIR)/%.d: $(DEP_DIR) ;

-include $(DEPFILES)

clean:
	$(START)
	@printf $(RED)"-x remove .o & .d files\n"$(NORMAL)
	@rm -rf $(OBJS_DIR)
	@rm -rf $(DEP_DIR)
	@rm -rf $(DEBUG_DIR)
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
	@make

help:
	@printf $(YELLOW)$(BOLD)"HELP\n--------------------\n"$(NORMAL)
	@printf $(NORMAL)"-> make "$(BOLD)"init"$(NORMAL)": init the project (add pre-commit & pre-push files)\n"
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
	@printf $(NORMAL)"-> make "$(BOLD)"... DEFINE=\"DEFINE=value\""$(NORMAL)": use send `#define DEFINE value` to all files\n"
	@printf $(YELLOW)$(BOLD)"--------------------\n"$(NORMAL)

.PHONY: init all clean fclean re exec-nolint exec lint check help
