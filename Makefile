CPP = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g3

NAME = webserv

SRCS = main.cpp Server.cpp Address.cpp NetworkUtils.cpp Socket.cpp Epoll.cpp Log.cpp EventHandler.cpp
OBJS = $(addprefix objs/,$(SRCS:.cpp=.o))
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(FLAGS) $(OBJS) -o $(NAME)

objs/%.o: srcs/%.cpp
	@mkdir -p objs/
	$(CPP) $(FLAGS) -MMD -c $< -o $@ -I includes/

clean:
	rm -rf objs/

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)
