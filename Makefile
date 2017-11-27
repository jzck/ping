# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: wescande <wescande@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/08/29 21:32:58 by wescande          #+#    #+#              #
#    Updated: 2017/10/08 14:36:33 by jhalford         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL		:=	bash

NAME		=	ft_ping

CC			=	gcc
RM			=	rm -rf 2>&-
W_FLAGS		=	-Wall -Wextra -Werror
D_FLAGS		=
FLAGS		=	$(W_FLAGS) $(D_FLAGS)

LEN_NAME	=	`printf "%s" $(NAME) |wc -c`
DELTA		=	$$(echo "$$(tput cols)-31-$(LEN_NAME)"|bc)

SRC_DIR		=	./
INC_DIR		=	./
OBJ_DIR		=	objs/

SRC_BASE	=	\
ping.c\
create_client.c\
epoch.c\
cksum.c\
rs.c\

SRCS		=	$(addprefix $(SRC_DIR), $(SRC_BASE))
OBJS		=	$(addprefix $(OBJ_DIR), $(SRC_BASE:.c=.o))
NB			=	$(words $(SRC_BASE))
INDEX		=	0

all :
	@make -j $(NAME)

$(NAME):	$(OBJ_DIR) $(OBJS) $(CLIENT_OBJ)
	$(CC) $(OBJS) -o $@ \
		-I $(INC_DIR) \
		$(CLIENT_OBJ) $(FLAGS) \
		-lm
	# sudo setcap cap_net_raw+ep $@ 2>&-

$(OBJ_DIR) :
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o :	$(SRC_DIR)%.c | $(OBJ_DIR)
	$(CC) $(FLAGS) $(OBJ_FLAG) -MMD -c $< -o $@ -I $(INC_DIR)

clean:
	$(RM) $(OBJ_DIR)

re:				fclean all

fclean:			clean
	$(RM) $(NAME)

.PHONY :		fclean clean re

-include $(OBJS:.o=.d)
