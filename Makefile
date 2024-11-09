# add this line to bashrc to have multithreaded makefile
# export GNUMAKEFLAGS=-j
CPP = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g3

NAME = webserv

SRCS =	main.cpp Server.cpp Address.cpp NetworkUtils.cpp Socket.cpp \
		Log.cpp EventHandler.cpp ConfigParser.cpp StringUtils.cpp \
		ConfigTokenizer.cpp LocationConfig.cpp ServerConfig.cpp Request.cpp \
		Response.cpp HeaderFields.cpp Utils.cpp CGI.cpp

OBJS = $(addprefix objs/,$(SRCS:.cpp=.o))
DEPS = $(OBJS:.o=.d)
LOG_LEVEL = TRACE

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(FLAGS) $(OBJS) -o $(NAME)

objs/%.o: srcs/%.cpp
	@mkdir -p objs/
	$(CPP) $(FLAGS) -MMD -c $< -o $@ -I includes/ -D LOG_LEVEL=$(LOG_LEVEL)

clean:
	rm -rf objs/

fclean: clean
	rm -f $(NAME)

re:
	make fclean
	make all

-include $(DEPS)
