/////////////////////////////////////////////////////////////////////////////
//
// xsmtp.cpp
//
// XSMTP start-up code
//
/////////////////////////////////////////////////////////////////////////////

#include "xsmtp.h"

int quit(int arg);
void *mail_proc(void* param);
void respond(int client_sockfd, char* request);
void send_data(int sockfd, const char* data);
void mail_data(int sockfd);
int check_user();
void auth(int sockfd);
int check_name_pass(char* name, char* pass);
char *base64_decode(char *s);
void user_quit();

int main() {
	//signal(SIGINT, (void*)quit);  //go to MiniWebQuit when Ctrl+C key pressed.
	//signal(SIGTERM, (void*)quit); //terminal signal
	signal(SIGPIPE, SIG_IGN); //ignore pipe signal.For more see http://www.wlug.org.nz/SIGPIPE

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
	//fcntl(server_sockfd, F_SETFL, fcntl(server_sockfd, F_GETFL, 0)|O_NONBLOCK);

	//listening requests from clients
	if (listen(server_sockfd, MAX_CLIENTS - 1) == -1) {
		perror("S:listen error！\n");
		exit(1);
	}

	//accept requests from clients,loop and wait.
	while (1) {
		cout << "-XSMTP mail server by XBC started..." << endl;
		sin_size = sizeof(client_addr);
		if ((client_sockfd = accept(server_sockfd,
				(struct sockaddr *) &client_addr, &sin_size)) == -1) {
			perror("S:accept error!\n");
			sleep(10);
			continue;
		}
		cout << "S:received a connection from " << inet_ntoa(
				client_addr.sin_addr) << " at " << time(NULL) << endl;

		pthread_t id;
		pthread_create(&id, NULL, mail_proc, &client_sockfd);
		pthread_join(id, NULL);

		close(client_sockfd);
	}
	return 0;
}

int quit(int arg) {
	if (arg)
		printf("\nS:Caught signal (%d). Mail server shutting down...\n\n", arg);
	return 1;
}

void *mail_proc(void* param) {
	int client_sockfd, len;
	char buf[BUF_SIZE];

	memset(buf, 0, sizeof(buf));
	client_sockfd = *(int*) param;

	send_data(client_sockfd, reply_code[4]); //send 220
	mail_stat = 1;

	while (1) {
		memset(buf, 0, sizeof(buf));
		len = recv(client_sockfd, buf, sizeof(buf), 0);
		if (len > 0) {
			cout << "Request stream: " << buf;
			respond(client_sockfd, buf);
		} else {
			cout
					<< "S: no data received from client. The server exit permanently.\n";
			break;
		}
		cout << "S:[" << client_sockfd << "] socket closed by client." << endl;
		cout << "============================================================\n\n";
	}
	return NULL;
}

void respond(int client_sockfd, char* request) {
	char output[1024];
	memset(output, 0, sizeof(output));

	//smtp
	if (strncmp(request, "HELO", 4) == 0) {
		if (mail_stat == 1) {
			send_data(client_sockfd, reply_code[6]);
			rcpt_user_num = 0;
			memset(rcpt_user, 0, sizeof(rcpt_user));
			mail_stat = 2;
		} else {
			send_data(client_sockfd, reply_code[15]);
		}
	} else if (strncmp(request, "MAIL FROM", 9) == 0) {
		if (mail_stat == 2 || mail_stat == 13) {
			char *pa, *pb;
			pa = strchr(request, '<');
			pb = strchr(request, '>');
			strncpy(from_user, pa + 1, pb - pa - 1);
			if (check_user()) {
				send_data(client_sockfd, reply_code[6]);
				mail_stat = 3;
			} else {
				send_data(client_sockfd, reply_code[15]);
			}
		} else if (mail_stat == 12) {
			send_data(client_sockfd, reply_code[23]);
		} else {
			send_data(client_sockfd, "503 Error: send HELO/EHLO first\r\n");
		}
	} else if (strncmp(request, "RCPT TO", 7) == 0) {
		if ((mail_stat == 3 || mail_stat == 4) && rcpt_user_num < MAX_RCPT_USR) {
			char *pa, *pb;
			pa = strchr(request, '<');
			pb = strchr(request, '>');
			strncpy(rcpt_user[rcpt_user_num++], pa + 1, pb - pa - 1);
			send_data(client_sockfd, reply_code[6]);
			mail_stat = 4;
		} else {
			send_data(client_sockfd, reply_code[16]);
		}
	} else if (strncmp(request, "DATA", 4) == 0) {
		if (mail_stat == 4) {
			send_data(client_sockfd, reply_code[8]);
			mail_data(client_sockfd);
			mail_stat = 5;
		} else {
			send_data(client_sockfd, reply_code[16]);
		}
	} else if (strncmp(request, "RSET", 4) == 0) {
		mail_stat = 1;
		send_data(client_sockfd, reply_code[6]);
	} else if (strncmp(request, "NOOP", 4) == 0) {
		send_data(client_sockfd, reply_code[5]);
	} else if (strncmp(request, "QUIT", 4) == 0) {
		mail_stat = 0;
		user_quit();
		send_data(client_sockfd, reply_code[5]);
		pthread_exit((void*)1);
	}
	//esmpt
	else if (strncmp(request, "EHLO", 4) == 0) {
		if (mail_stat == 1) {
			send_data(client_sockfd, reply_code[24]);
			mail_stat = 12;
		} else {
			send_data(client_sockfd, reply_code[15]);
		}
	} else if (strncmp(request, "AUTH LOGIN", 10) == 0) {
		auth(client_sockfd);
	} else if (strncmp(request, "AUTH LOGIN PLAIN", 10) == 0) {
		auth(client_sockfd);
	} else if (strncmp(request, "AUTH=LOGIN PLAIN", 10) == 0) {
		auth(client_sockfd);
	} else {
		send_data(client_sockfd, reply_code[15]);
	}
}

void send_data(int sockfd, const char* data) {
	if (data != NULL) {
		send(sockfd, data, strlen(data), 0);
		cout << "Reply stream: " << data;
	}
}

void mail_data(int sockfd) {
	sleep(1);
	char buf[BUF_SIZE];
	memset(buf, 0, sizeof(buf));
	recv(sockfd, buf, sizeof(buf), 0);
	cout << "Mail Contents: \n" << buf << endl;

	//mail content and format check

	//mail content store
	int tm = time(NULL), i;
	char file[80], tp[20];

	for (i = 0; i < rcpt_user_num; i++) {
		strcpy(file, data_dir);
		strcat(file, rcpt_user[i]);
		if (access(file,0) == -1) {
			mkdir(file,0777);
		}
		sprintf(tp, "/%d", tm);
		strcat(file, tp);

		FILE* fp = fopen(file, "w+");
		if (fp != NULL) {
			fwrite(buf, 1, strlen(buf), fp);
			fclose(fp);
		} else {
			cout << "File open error!" << endl;
		}
	}
	send_data(sockfd, reply_code[6]);
}

int check_user() {
	FILE* fp;
	char file[80] = "";
	char data[60];

	strcpy(file, data_dir);
	strcat(file, userinfo);

	fp = fopen(file, "r");
	while (fgets(data, sizeof(data), fp) > 0) {
		if (strncmp(from_user, data, strlen(from_user)) == 0)
			return 1;
	}
	return 0;
}

void auth(int sockfd) {
	char ename[50], epass[50];
	char *name, *pass;
	int len;

	send_data(sockfd, reply_code[25]);
	sleep(3);
	len = recv(sockfd, ename, sizeof(ename), 0);
	if (len > 0) {
		cout << "Request stream: " << ename << endl;
		name = base64_decode(ename);
		cout << "Decoded username: " << name << endl;
		send_data(sockfd, reply_code[26]);
		sleep(3);
		len = recv(sockfd, epass, sizeof(epass), 0);
		if (len > 0) {
			cout << "Request stream: " << epass << endl;
			pass = base64_decode(epass);
			cout << "Decoded password: " << pass << endl;
			if (check_name_pass(name, pass)) {
				mail_stat = 13;
				send_data(sockfd, reply_code[27]);
			} else {
				send_data(sockfd, reply_code[16]);
			}
		} else {
			send_data(sockfd, reply_code[16]);
		}
	} else {
		send_data(sockfd, reply_code[16]);
	}
}

int check_name_pass(char* name, char* pass) {
	FILE* fp;
	char file[80], data[60];

	strcpy(file, data_dir);
	strcat(file, userinfo);
	fp = fopen(file, "r");
	while (fgets(data, sizeof(data), fp) > 0) {
		if (strncmp(data, name, strlen(name)) == 0) {
			char *p;
			p = strchr(data, ' ');
			if (strncmp(p + 1, pass, strlen(pass)) == 0) {
				fclose(fp);
				strcpy(file, data_dir);
				strcat(file, userstat);
				fp = fopen(file, "w+");
				strcat(name, " on");
				fwrite(name, 1, strlen(name), fp);
				fclose(fp);
				return 1;
			} else {
				return 0;
			}
		}
	}
	return 0;
}

void user_quit() {
	FILE* fp;
	char file[80], data[60];
	int flag = 0, len;

	strcpy(file, data_dir);
	strcat(file, userstat);
	fp = fopen(file, "w+");
	while (fgets(data, sizeof(data), fp) > 0) {
		if (strncmp(data, from_user, strlen(from_user)) == 0) {
			flag = 1;
		}
		if (flag) {
			len = strlen(data);
			if (fgets(data, sizeof(data), fp) > 0) {
				len = -len;
				fseek(fp, len, SEEK_CUR);
				fputs(data, fp);
				len = strlen(data);
				fseek(fp, len, SEEK_CUR);
			}
		}
	}
	fclose(fp);
}

char *base64_decode(char *s) {
	char *p = s, *e, *r, *_ret;
	int len = strlen(s);
	unsigned char i, unit[4];

	e = s + len;
	len = len / 4 * 3 + 1;
	r = _ret = (char *) malloc(len);

	while (p < e) {
		memcpy(unit, p, 4);
		if (unit[3] == '=')
			unit[3] = 0;
		if (unit[2] == '=')
			unit[2] = 0;
		p += 4;

		for (i = 0; unit[0] != B64[i] && i < 64; i++)
			;
		unit[0] = i == 64 ? 0 : i;
		for (i = 0; unit[1] != B64[i] && i < 64; i++)
			;
		unit[1] = i == 64 ? 0 : i;
		for (i = 0; unit[2] != B64[i] && i < 64; i++)
			;
		unit[2] = i == 64 ? 0 : i;
		for (i = 0; unit[3] != B64[i] && i < 64; i++)
			;
		unit[3] = i == 64 ? 0 : i;
		*r++ = (unit[0] << 2) | (unit[1] >> 4);
		*r++ = (unit[1] << 4) | (unit[2] >> 2);
		*r++ = (unit[2] << 6) | unit[3];
	}
	*r = 0;

	return _ret;
}
