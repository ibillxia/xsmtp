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
 * This is the use management module, includes user login, authentication and quit.
 * 
 */

#include "module_user.h"
#include "module_mail.h"

// find if user exists
int check_user() {
	FILE* fp;
	char file[80] = "";
	char data[60];

	strcpy(file, data_dir);
	strcat(file, userinfo);

	fp = fopen(file, "r");
	while (fgets(data, sizeof(data), fp) > 0) {
		if (strncmp(from_user, data, strlen(from_user)) == 0) // valid user
			return 1;
	}
	return 0;
}

// user authentication
void auth(int sockfd) {
	char ename[50], epass[50];
	char *name, *pass;
	int len;

	send_data(sockfd, reply_code[25]); // require username
	sleep(3);
	len = recv(sockfd, ename, sizeof(ename), 0);
	if (len > 0) {
		cout << "Request stream: " << ename << endl;
		name = base64_decode(ename);
		cout << "Decoded username: " << name << endl;
		send_data(sockfd, reply_code[26]); // require passwd
		sleep(3);
		len = recv(sockfd, epass, sizeof(epass), 0);
		if (len > 0) {
			cout << "Request stream: " << epass << endl;
			pass = base64_decode(epass);
			cout << "Decoded password: " << pass << endl;
			if (check_name_pass(name, pass)) { // check username and passwd
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

// check username and passwd from file
int check_name_pass(char* name, char* pass) {
	FILE* fp;
	char file[80], data[60];

	strcpy(file, data_dir);
	strcat(file, userinfo);
	fp = fopen(file, "r");
	while (fgets(data, sizeof(data), fp) > 0) {
		if (strncmp(data, name, strlen(name)) == 0) { // find username
			char *p;
			p = strchr(data, ' ');
			if (strncmp(p + 1, pass, strlen(pass)) == 0) { // valid passwd
				fclose(fp);
				strcpy(file, data_dir);
				strcat(file, userstat);
				fp = fopen(file, "w+");
				strcat(name, " on"); // change the status of the user to ON
				fwrite(name, 1, strlen(name), fp);
				fclose(fp);
				return 1; // success
			} else { // invalid passwd
				return 0;
			}
		}
	}
	return 0; // invalid username
}

// user logout
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
