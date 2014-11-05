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
  * This is a header file, include the main messages respond to the client.
  * 
  */

#ifndef __XSMTP_MESSAGES_H
#define __XSMTP_MESSAGES_H

const char reply_code[][100]={
	{" \r\n"},  //0
	{"200 Mail OK.\r\n"},   //1
	{"211 System status, or system help reply.\r\n"},  //2
	{"214 Help message.\r\n"},   //3
	{"220 Ready\r\n"},  //4
	{"221 Bye\r\n"},  //5
	{"250 OK\r\n"},  //6
	{"251 User not local; will forward to %s<forward-path>.\r\n"},  //7

	{"354 Send from Rising mail proxy\r\n"},  //8
	{"421 service not available, closing transmission channel\r\n"},  //9
	{"450 Requested mail action not taken: mailbox unavailable\r\n"},  //10
	{"451 Requested action aborted: local error in processing\r\n"},   //11
	{"452 Requested action not taken: insufficient system storage\r\n"}, //12
	{"500 Syntax error, command unrecognised\r\n"},  //13
	{"501 Syntax error in parameters or arguments\r\n"},  //14
	{"502 Error: command not implemented\r\n"},  //15
	{"503 Error: bad sequence of commands\r\n"}, //16
	{"504 Error: command parameter not implemented\r\n"},  //17
	{"521 <domain>%s does not accept mail (see rfc1846)\r\n"},  //18
	{"530 Access denied (???a Sendmailism)\r\n"},  //19
	{"550 Requested action not taken: mailbox unavailable\r\n"},  //20
	{"551 User not local; please try <forward-path>%s\r\n"},  //21
	{"552 Requested mail action aborted: exceeded storage allocation\r\n"},  //22
	{"553 authentication is required\r\n"},  //23

	{"250-mail \n250-PIPELINING \n250-AUTH LOGIN PLAIN\n250-AUTH=LOGIN PLAIN\n250 8BITMIME\r\n"},  //24
	{"334 dXNlcm5hbWU6\r\n"},  //25, "dXNlcm5hbWU6" is "username:"'s base64 code
	{"334 UGFzc3dvcmQ6\r\n"},  //26, "UGFzc3dvcmQ6" is "Password:"'s base64 code
	{"235 Authentication successful\r\n"} //27
};

#endif