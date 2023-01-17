#ifndef STRUCT
#define STRUCT

#include <stdio.h>
#define groups_n 3
#define users_n 9

typedef struct Message Message;
struct Message{
    long mtype;
    char mtext[1000];
    char mfrom[100]; //sender
    long mid;   //sender id
    char mto[100];
};

typedef struct User User;
struct User{
    long uid; //user id - pid
    char uname[25];
    int ulog; //1-user logged id 0-not
    int ugroups[groups_n]; //1-user belongs, 0-doesnt
};

#endif
