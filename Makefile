# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/01 16:32:31 by shaintha          #+#    #+#              #
#    Updated: 2025/07/01 16:32:36 by shaintha         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := ircserv

COMPILE := c++
CFLAGS := -Wall -Wextra -Werror -std=c++98

SRC_DIR := sources
OBJ_DIR := objects

SRCS := $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/Client.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/Client.cpp \
	$(SRC_DIR)/Channel.cpp \
	$(SRC_DIR)/Parser.cpp

OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(COMPILE) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@$(COMPILE) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re