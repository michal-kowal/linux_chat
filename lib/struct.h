#ifndef STRUCT
#define STRUCT

#include <stdio.h>
#define groups_n 3
#define users_n 9
#define username_length 25
#define message_length 1000
#define server_mes 50
#define server_chat_mes 51
#define MAX_strlen 1000
#define MAX_login 3

typedef struct Message Message;
struct Message{
    long mtype;
    char mtext[message_length];
    char mfrom[username_length]; //sender
    long mid;
    char mto[username_length];
    int send_login_count;
};

typedef struct User User;
struct User{
    long uid; //user id - pid
    char uname[username_length];
    int ulog; //1-user logged id 0-not
    int ugroups[groups_n]; //1-user belongs, 0-doesnt
    int login_count;
};

#endif
