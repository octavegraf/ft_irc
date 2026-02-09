CC					=	c++
CFLAGS				=	-Wall -Wextra -Werror -Wshadow -std=c++98 -MMD
NAME				=	ircserv
FILES				=	main parsing Server User Channel commands
SRC					=	$(addprefix src/, $(addsuffix .cpp, $(FILES)))
INCLUDES			=	-Iinclude
OBJ_FOLDER			=	obj/
OBJ					=	$(addprefix $(OBJ_FOLDER), $(SRC:.cpp=.o))
DPD					=	$(addprefix $(OBJ_FOLDER), $(SRC:.cpp=.d))

all					:	$(NAME)

$(NAME)				:	$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIBS)

$(OBJ_FOLDER)%.o	:	%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

-include $(DPD)

clean				:
	rm -rf $(OBJ_FOLDER)

fclean				:	clean
	rm -rf $(NAME)

re					:	fclean all

debug				:	CFLAGS += -g3 -DDEBUG
debug				:	re

fsanitize			:	CFLAGS += -fsanitize=address
fsanitize			:	debug

.PHONY				:	all clean fclean re debug fsanitize
.DELETE_ON_ERROR	:
#.SILENT				: