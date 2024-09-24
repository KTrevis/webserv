#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>

int	main(int ac, char **av) {
	if (ac != 2) return dprintf(2, "usage: %s [port]\n", av[0]);
	struct sockaddr_in servAddr;
	struct sockaddr_in clientAddr;
	int	serverSocket;
	int	clientSocket;
	unsigned int	clientLen = sizeof(struct sockaddr_in);

	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(atoi(av[1]));
	servAddr.sin_family = AF_INET;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(serverSocket, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
		return dprintf(2, "ERROR: binding failed");
	listen(serverSocket, 5);
	clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
	printf("WESH Y'A UN MEC");
	close(clientSocket);
	close(serverSocket);
}
