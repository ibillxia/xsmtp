# xsmtp

A simple SMTP mail server, a tiny but complete realization of smtp protocol.


## Features

- OS: only for Linux/Unix plantform  
- Multithread: create a thread for each client's request  
- Authentication: store username and passwd to a file
- Base64 encode and decode  

## Usage  
1) Get the source    
```
git clone https://github.com/ibillxia/xsmtp.git
cd xsmtp
```

2) Edit Config
Set the user data storage directory:
```
vim conf.h
```
Set the variable `data_dir` to `/home/YourName/data`. 

3) Add files and user account
```
cd /home/YourName/
mkdir data
vim userinfo
#add two users in this file
#bill@localhost.com
#alice@localhost.com
#then write and quit
touch userstat
```

4) Send Mail Example
Run the executable file in one terminal:  
```
sudo xsmtp
```
This started the mail server. And now you can send e-mail to the server in another terminal like this: 
```
telnet localhost 25
S: 220 Ready
C: HELO
S: 250 OK
C: MAIL FROM:<bill@localhost.com>
S: 250 OK
C: RCPT TO:<alice@localhost.com>
S: 250 OK
C: DATA
S: 354 Send from Rising mail proxy
C: Hello Alice. This is a test message.<CR><LF>.<CR><LF>
S: 250 OK
C: QUIT
S: 221 Bye
```
Well done!

## About SMTP protocal  

This program is a simple mail dispatcher via smtp protocol. It runs only on Linux/Unix plantforms.
For more about SMTP, please refer to wiki and it's RFC documents:   
[wiki: Simple_Mail_Transfer_Protocol](http://en.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol)  
[RFC 5321 – The Simple Mail Transfer Protocol](http://tools.ietf.org/html/rfc5321)  


## Lisense

The MIT License (MIT)
Copyright (C) 2011-2014 Bill Xia (ibillxia@gmail.com) 
All rights reserved.
