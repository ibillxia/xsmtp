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
  * This is a header file for configurations in the XSMTP project.
  * 
  */

#ifndef __XSMTP_CONF_H
#define __XSMTP_CONF_H

// Mail Server configurations
#define PORT 25  // use port 25, need root privilege
#define MAX_CLIENTS 32
#define MAX_RCPT_USR 50
#define BUF_SIZE 1024

// User information saving configurations
const char data_dir[] = "/home/bill/data/";
const char userinfo[] = "userinfo";
const char userstat[] = "userstat";

extern int mail_stat;
extern int rcpt_user_num;
extern char from_user[30];
extern char rcpt_user[MAX_RCPT_USR][30];

#endif
