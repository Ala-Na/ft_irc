# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: anadege <anadege@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/26 11:14:39 by opacaud           #+#    #+#              #
#    Updated: 2022/04/21 20:48:04 by anadege          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME =		ircserv

OBJ_DIR =	obj

CXX =		c++

CFLAGS =	-Wall -Wextra -Werror -std=c++98 -g3 #-fsanitize=address

DEPFLAGS =	-MMD -MP

SRC =		Channel.cpp Command.cpp main.cpp Numerics.cpp Server.cpp User.cpp Utilitary.cpp

OBJ =		${addprefix ${OBJ_DIR}/,${SRC:.cpp=.o}}

DEP =		${OBJ:.o=.d}

all:		${NAME}

${NAME}:	${OBJ}
			${CXX} ${CFLAGS} ${DEPFLAGS} ${OBJ} -o ${NAME}

${OBJ_DIR}/%.o:%.cpp
			@mkdir -p ${@D}
			${CXX} ${CFLAGS} ${DEPFLAGS} -c $< -o $@

clean:
			rm -rf ${OBJ_DIR}

fclean:		clean
			rm -f ${NAME}

re:			fclean all

.PHONY :	all clean fclean re

-include	${DEP}
