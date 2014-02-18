#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <dirent.h>

using namespace std;

//Mail Server configurations
#define MAX_RCPT_USR 50
#define PORT 25
#define MAX_CLIENTS 32
#define BUF_SIZE 1024

int mail_stat = 0;
const char data_dir[] = "/home/user/data/";
const char userinfo[] = "userinfo";
const char userstat[] = "userstat";
char from_user[30] = "";
int rcpt_user_num = 0;
char rcpt_user[MAX_RCPT_USR][30] = {""};

static char B64[64] =
{
	'A','B','C','D','E','F','G','H',
	'I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X',
	'Y','Z',
	'a','b','c','d','e','f','g','h',
	'i','j','k','l','m','n','o','p',
	'q','r','s','t','u','v','w','x',
	'y','z',
	'0','1','2','3','4','5','6','7',
	'8','9','+','/'
};

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
		{"334 dXNlcm5hbWU6\r\n"},  //25
		{"334 UGFzc3dvcmQ6\r\n"},  //26
		{"235 Authentication successful\r\n"} //27
};
