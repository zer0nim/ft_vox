# Makefile
# for each project, you can configure the Makefile
# /!\ each variables with -> need to be changed /!\

# project global config
#	-> NAME
#	ARGS
#	CC
#	CFLAGS
#	DEBUG_FLAGS
#	NODEBUG_FLAGS
#	DEPFLAGS
#	DEFINE

# source files
#	SRCS_DIR
#	OBJS_DIR
#	INC_DIR
#	LIBS_DIR
#	DEP_DIR
#	DEBUG_DIR
#	-> SRC
#	-> HEAD

# libs configuration
#	-> LIBS_SRC_C
#	-> LIBS_SRC_CPP
#	-> LIBS_HEAD
#	-> LIBS_FLAGS
#	-> LIBS_FLAGS_OSX
#	-> LIBS_FLAGS_LINUX
#	-> LIBS_INC

# linter config
#	LINTER
#	LINTER_RULES

# pre-commit & pre-push files
#	PRE_COMMIT_FILE
#	PRE_COMMIT
#	PRE_PUSH_FILE
#	PRE_PUSH

# after this, to install the project:
#	make install  # this command will exectute ./configure.sh so you need to create this file
# to compile & run
#	make exec -j8 ARGS="--usage"
# to get help:
#	make help

# to compile with debug mode:
#	make DEBUG=1 -j8

################################################################################
# project global config

# executable name
NAME = ft_vox
# args (./NAME ARGS) (make exec ARGS="-v" -> ./name -v)
ARGS =
# compiler (g++ or clang++)
CC = g++
# flags for compilation
CFLAGS = -Ofast -std=c++11 -Wall -Wextra -Wno-deprecated
# flags only for debug mode (make DEBUG=1)
DEBUG_FLAGS = -g3 -DDEBUG=true
# classic flags
NODEBUG_FLAGS = -Werror
# flags to create the .d files
DEPFLAGS = -MT $@ -MD -MP -MF $(DEP_DIR)/$*.Td
# to send define to program (make DEFINE="WIDTH=120")
DEFINE =


################################################################################
# source files

# directory names
SRCS_DIR	= srcs
OBJS_DIR	= objs
INC_DIR		= includes
LIBS_DIR	= libs
DEP_DIR		= .dep
# this will create a DEBUG file if we are in debug mode (make DEBUG=1)
DEBUG_DIR	= $(DEP_DIR)

# SRCS_DIR/SRC
SRC =	main.cpp \
		windowEvents.cpp \
		TextureManager.cpp \
		ft_vox.cpp \
\
		chunk/AChunk.cpp \
		chunk/ChunkManager.cpp \
		chunk/Chunk.cpp \
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
		utils/ImageRender.cpp \
		utils/Material.cpp \
		utils/Stats.cpp \
		utils/Logging.cpp \

# INC_DIR/HEAD
HEAD =	commonInclude.hpp \
		ft_vox.hpp \
		TextureManager.hpp \
\
		chunk/AChunk.hpp \
		chunk/ChunkManager.hpp \
		chunk/Chunk.hpp \
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
		utils/ImageRender.hpp \
		utils/Material.hpp \
		utils/Stats.hpp \
		utils/Logging.hpp \


################################################################################
# libs configuration

# for c libs
LIBS_SRC_C		= glad/glad.c \

# for cpp libs
LIBS_SRC_CPP	=

# headers for c & cpp libs
LIBS_HEAD		= glad/glad.h \
				  KHR/khrplatform.h \
				  json.hpp \
				  stb_image.h \
				  PerlinNoise.hpp \

# all flags for libs
LIBS_FLAGS			= -L ~/.brew/lib -lglfw -L ~/.brew/opt/freetype/lib -lfreetype \
					  -lboost_filesystem
# flags for libs on OSX only
LIBS_FLAGS_OSX		= -framework OpenGL
# flags for libs on LINUX only
LIBS_FLAGS_LINUX	= -ldl -pthread -lboost_system
# includes dir for external libs
LIBS_INC			= ~/.brew/include \
					  $(INC_DIR)/lib \
					  /usr/local/opt/freetype/include/freetype2 \
					  ~/.brew/opt/freetype/include/freetype2 \
					  /usr/include/freetype2

################################################################################
# linter config

# download the cpp linter (https://github.com/isocpp/CppCoreGuidelines)
# set command to launch linter on LINTER
# add rules for linter in LINTER_RULES
LINTER = $(CPPLINT)
LINTER_RULES =	--filter=-whitespace/tab,-legal/copyright,-build/c++11,-whitespace/newline,-readability/braces,-whitespace/indent,-build/include_what_you_use,-build/header_guard,-runtime/references \
				--linelength=120 --quiet

################################################################################
# pre-commit & pre-push files

# pre-commit rules
PRE_COMMIT_FILE = .git/hooks/pre-commit
define PRE_COMMIT
#!/bin/zsh

$(MAKE) $(MAKE_OPT) lint
res=$$?

exit $${res}
endef
export PRE_COMMIT

# pre-push rules
PRE_PUSH_FILE = .git/hooks/pre-push
define PRE_PUSH
#!/bin/zsh

$(MAKE) $(MAKE_OPT) check -j8
res=$$?

exit $${res}
endef
export PRE_PUSH

################################################################################
# automatic defined variables

# name in majuscule
PROJECT_NAME = $(shell echo $(NAME) | tr a-z A-Z)  # name in MAJUSCULE

MAKE_OPT = --no-print-directory

# os specific flags for libs
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
	LIBS_FLAGS += $(LIBS_FLAGS_OSX)
else
	LIBS_FLAGS += $(LIBS_FLAGS_LINUX)
endif

# setup correct shell
ifneq (,$(wildcard /bin/zsh))
	SHELL := /bin/zsh
else ifneq (,$(wildcard /bin/bash))
	SHELL := /bin/bash
else
	SHELL := /bin/sh
endif

# setup debug mode or normal mode
ifneq ($(DEBUG),)
	CFLAGS := $(CFLAGS) $(DEBUG_FLAGS)
else
	CFLAGS := $(CFLAGS) $(NODEBUG_FLAGS)
endif

# send define to program
CFLAGS := $(CFLAGS) $(addprefix -D, $(DEFINE))


HEADS		= $(addprefix $(INC_DIR)/, $(HEAD)) $(addprefix $(LIBS_DIR)/, $(LIBS_HEAD))
SRCS		= $(addprefix $(SRCS_DIR)/, $(SRC)) \
			  $(addprefix $(LIBS_DIR)/, $(LIBS_SRC_C)) \
			  $(addprefix $(LIBS_DIR)/, $(LIBS_SRC_CPP))
OBJS		= $(addprefix $(OBJS_DIR)/, $(SRC:.cpp=.o)) \
			  $(addprefix $(OBJS_DIR)/, $(LIBS_SRC_C:.c=.o)) \
			  $(addprefix $(OBJS_DIR)/, $(LIBS_SRC_CPP:.cpp=.o))
DEPFILES	= $(addprefix $(DEP_DIR)/, $(SRC:.cpp=.d)) \
			  $(addprefix $(DEP_DIR)/, $(LIBS_SRC_C:.c=.d)) \
			  $(addprefix $(DEP_DIR)/, $(LIBS_SRC_CPP:.cpp=.d))
INC			= $(addprefix -I , $(sort $(dir $(HEADS)))) $(addprefix -I , $(LIBS_INC)) -I .

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

################################################################################
# make rules

all:
ifneq ($(DEBUG),)
	@if [ -d $(DEBUG_DIR) ] && [ ! -f $(DEBUG_DIR)/DEBUG ]; then \
		$(MAKE) $(MAKE_OPT) fclean; \
	fi;
else
	@if [ -d $(DEBUG_DIR) ] && [ -f $(DEBUG_DIR)/DEBUG ]; then \
		$(MAKE) $(MAKE_OPT) fclean; \
	fi;
endif
	$(START)
	@$(MAKE) $(MAKE_OPT) $(NAME)
	$(END)
ifneq ($(DEBUG),)
	@touch $(DEBUG_DIR)/DEBUG
else
	@rm -f $(DEBUG_DIR)/DEBUG
endif

install:
	@printf $(YELLOW)$(BOLD)"INSTALL $(PROJECT_NAME)\n--------------------\n"$(NORMAL)
	@./configure
	@printf $(YELLOW)$(BOLD)"--------------------\n"$(NORMAL)
	@make init

init:
	@printf $(YELLOW)$(BOLD)"INIT $(PROJECT_NAME)\n--------------------\n"$(NORMAL)
	@printf $(CYAN)"create pre-commit\n"$(NORMAL)
	@echo "$$PRE_COMMIT" > $(PRE_COMMIT_FILE)
	@chmod 755 $(PRE_COMMIT_FILE)
	@printf $(CYAN)"create pre-push\n"$(NORMAL)
	@echo "$$PRE_PUSH" > $(PRE_PUSH_FILE)
	@chmod 755 $(PRE_PUSH_FILE)
	@printf $(YELLOW)$(BOLD)"--------------------\n"$(NORMAL)

$(NAME): $(OBJS_DIR) $(OBJS)
	@printf $(CYAN)"-> create program : $(NAME)\n"$(NORMAL)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBS_FLAGS)

$(OBJS_DIR)/%.o: $(LIBS_DIR)/%.c
$(OBJS_DIR)/%.o: $(LIBS_DIR)/%.c $(DEP_DIR)/%.d
	@printf $(YELLOW)"-> $<\n"$(NORMAL)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@ $(INC)
	@mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d


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
	@$(MAKE) $(MAKE_OPT)

exec-nolint:
	@$(MAKE) $(MAKE_OPT)
	@printf $(MAGENTA)$(BOLD)"EXEC $(PROJECT_NAME)\n--------------------\n"$(NORMAL)
	@./$(NAME) $(ARGS)
	@printf $(MAGENTA)$(BOLD)"--------------------\n"$(NORMAL)

exec:
	@$(MAKE) $(MAKE_OPT) lint ; true
	@$(MAKE) $(MAKE_OPT) exec-nolint ; true

lint:
	@printf $(BLUE)$(BOLD)"LINTER ON $(PROJECT_NAME)\n--------------------\n"$(NORMAL)
	@if [ "$(LINTER)" = "" ]; then\
		printf $(RED)$(BOLD)"Error:"$(NORMAL)" env var CPPLINT is not set\n"; \
	else \
		$(LINTER) $(LINTER_RULES) $(addprefix $(INC_DIR)/, $(HEAD)) $(addprefix $(SRCS_DIR)/, $(SRC)); \
    fi
	@printf $(BLUE)$(BOLD)"--------------------\n"$(NORMAL)

check:
	@$(MAKE) $(MAKE_OPT) fclean
	@$(MAKE) $(MAKE_OPT) lint
	@$(MAKE) $(MAKE_OPT)

help:
	@printf $(YELLOW)$(BOLD)"HELP\n--------------------\n"$(NORMAL)
	@printf $(NORMAL)"-> make "$(BOLD)"install"$(NORMAL)": install all depencies & run make init\n"
	@printf $(NORMAL)"-> make "$(BOLD)"init"$(NORMAL)": init the project (add pre-commit & pre-push files)\n"
	@printf $(NORMAL)"-> make "$(BOLD)"all"$(NORMAL)": build the project and create $(NAME)\n"
	@printf $(NORMAL)"-> make "$(BOLD)"clean"$(NORMAL)": remove all .o files\n"
	@printf $(NORMAL)"-> make "$(BOLD)"fclean"$(NORMAL)": make clean and remove executable\n"
	@printf $(NORMAL)"-> make "$(BOLD)"re"$(NORMAL)": make fclean and make all\n"
	@printf $(NORMAL)"-> make "$(BOLD)"lint"$(NORMAL)": exec linter on project (use env var CPPLINT)\n"
	@printf $(NORMAL)"-> make "$(BOLD)"exec"$(NORMAL)": make lint, make all and exec the program: ./$(NAME) ARGS('$(ARGS)')\n"
	@printf $(NORMAL)"-> make "$(BOLD)"exec-nolint"$(NORMAL)": make all and exec the program: ./$(NAME) ARGS('$(ARGS)')\n"
	@printf $(NORMAL)"-> make "$(BOLD)"check"$(NORMAL)": make fclean, make lint, make exec-nolint -> stop if there is an error\n"
	@printf $(NORMAL)"-> make "$(BOLD)"help | usage"$(NORMAL)": show the help\n"
	@printf $(NORMAL)"-> make "$(BOLD)"... DEBUG=1"$(NORMAL)": use debug mode\n"
	@printf $(NORMAL)"-> make "$(BOLD)"... ARGS='...'"$(NORMAL)": add arguments to exec: ./$(NAME) ARGS (for make exec & exec-nolint)\n"
	@printf $(NORMAL)"-> make "$(BOLD)"... DEFINE=\"DEFINE=value\""$(NORMAL)": send `#define DEFINE value` to all files\n"
	@printf $(YELLOW)$(BOLD)"--------------------\n"$(NORMAL)

usage: help

.PHONY: init all clean fclean re exec-nolint exec lint check help usage
