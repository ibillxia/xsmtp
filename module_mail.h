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
 * This is a header file, mainly about mailing events processing.
 * 
 */

#ifndef __XSMTP_MODULE_MAIL_H
#define __XSMTP_MODULE_MAIL_H

#include "common.h"

static char B64[64] = {
	'A','B','C','D','E','F','G',
	'H','I','J','K','L','M','N',
	'O','P','Q','R','S','T',
	'U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g',
	'h','i','j','k','l','m','n',
	'o','p','q','r','s','t',
	'u','v','w','x','y','z',
	'0','1','2','3','4','5','6',
	'7','8','9','+','/'
};

void *mail_proc(void* param);
void respond(int client_sockfd, char* request);
void send_data(int sockfd, const char* data);
void mail_data(int sockfd);
char *base64_decode(char *s);

//extern int check_user();
//extern void auth(int sockfd);
//extern void user_quit();

#endif /* __XSMTP_MODULE_MAIL_H */

// Local Variables:
// mode: C++
// End:
