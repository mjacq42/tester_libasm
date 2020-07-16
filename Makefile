# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/07/06 18:24:18 by mjacq             #+#    #+#              #
#    Updated: 2020/07/16 20:06:46 by mjacq            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

DIR="../libasm"

CC=clang
LDFLAGS=-L$(DIR) -lasm

all: compile_lib
	$(CC) main.c $(LDFLAGS)
	mkdir tests
	./a.out

bonus: compile_bonuses
	$(CC) -D BONUS main.c $(LDFLAGS)
	./a.out

compile_lib:
	$(MAKE) -C $(DIR)

compile_bonuses:
	$(MAKE) -C $(DIR) bonus

clean:
	$(MAKE) -C $(DIR) clean

fclean: clean
	$(MAKE) -C $(DIR) fclean
	rm -f a.out
	rm -rf tests

re: fclean all
	
.PHONY: all clean fclean re bonus
