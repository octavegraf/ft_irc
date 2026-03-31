CC					=	c++
CFLAGS				=	-Wall -Wextra -Werror -Wshadow -std=c++98 -MMD
NAME				=	ircserv
NAME_DEBUG			=	debug_$(NAME)
FILES				=	main parsing Server User Channel commands utils
SRC					=	$(addprefix src/, $(addsuffix .cpp, $(FILES)))
INCLUDES			=	-Iinclude
OBJ_FOLDER			=	obj/
OBJ					=	$(addprefix $(OBJ_FOLDER), $(SRC:.cpp=.o))
OBJ_DEBUG			=	$(addprefix $(OBJ_FOLDER), $(SRC:.cpp=_debug.o))
DPD					=	$(addprefix $(OBJ_FOLDER), $(SRC:.cpp=.d))
DPD_DEBUG			=	$(addprefix $(OBJ_FOLDER), $(SRC:.cpp=_debug.d))

all					:	$(NAME)

$(NAME)				:	$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIBS)

$(NAME_DEBUG)		:	$(OBJ_DEBUG)
	$(CC) $(CFLAGS) $(OBJ_DEBUG) -o $(NAME_DEBUG) $(LIBS)

$(OBJ_FOLDER)%_debug.o	:	%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_FOLDER)%.o	:	%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

-include $(DPD) $(DPD_DEBUG)

clean				:
	rm -rf $(OBJ_FOLDER)

fclean				:	clean
	rm -rf $(NAME) $(NAME_DEBUG)

re					:	fclean all

d					:	debug
debug				:	CFLAGS += -g3 -DDEBUG
debug				:	$(NAME_DEBUG)

fsanitize			:	CFLAGS += -fsanitize=address
fsanitize			:	debug

.PHONY				:	all clean fclean re debug fsanitize
.DELETE_ON_ERROR	:
#.SILENT				:
