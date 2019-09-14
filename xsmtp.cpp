/*
 * Copyright (C) 2011
 * Bill Xia
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 */

/*
 * The main function entrance for the whole project. 
 *
 */

#include "xsmtp.h"

int mail_stat = 0;
int rcpt_user_num = 0;
char from_user[64] = "";
char rcpt_user[MAX_RCPT_USR][30] = {""};

int quit(int arg);

int main(int argc,char* argv[]) {
	//signal(SIGINT, (void*)quit);  //go to MiniWebQuit when Ctrl+C key pressed.
	//signal(SIGTERM, (void*)quit); //terminal signal
	signal(SIGPIPE, SIG_IGN);     //ignore pipe signal.For more see http://www.wlug.org.nz/SIGPIPE

	int server_sockfd, client_sockfd;
	socklen_t sin_size;
	struct sockaddr_in server_addr, client_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	//create socket
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("S:socket create error！\n");
		exit(1);
	}

	//set the socket's attributes
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(server_addr.sin_zero), 8);

	//create a link
	if (bind(server_sockfd, (struct sockaddr *) &server_addr,
			sizeof(struct sockaddr)) == -1) {
		perror("S:bind error！\n");
		exit(1);
	}
	// set to non-blocking to avoid lockout issue
	fcntl(server_sockfd, F_SETFL, fcntl(server_sockfd, F_GETFL, 0)|O_NONBLOCK);

	//listening requests from clients
	if (listen(server_sockfd, MAX_CLIENTS - 1) == -1) {
		perror("S:listen error！\n");
		exit(1);
	}

	//accept requests from clients,loop and wait.
	cout << "================================================================\n";
	cout << "-XSMTP mail server by [Bill Xia](ibillxia@gmail.com) started..." << endl;
	sin_size = sizeof(client_addr);
	while (1) {
		if ((client_sockfd = accept(server_sockfd,
				(struct sockaddr *) &client_addr, &sin_size)) == -1) {
			//perror("S:accept error!\n");
			sleep(1);
			continue;
		}
		cout << "S:received a connection from " 
			 << inet_ntoa(client_addr.sin_addr) << " at " 
			 << time(NULL) << endl;

		pthread_t id;
		pthread_create(&id, NULL, mail_proc, &client_sockfd);
		pthread_join(id, NULL);
	}
	close(client_sockfd);
	return 0;
}

int quit(int arg) {
	if (arg)
		printf("\nS:Caught signal (%d). Mail server shutting down...\n\n", arg);
	return 1;
}

