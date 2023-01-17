#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include "lib/struct.h"

int main(int argc, char* argv[]) {
    User users[users_n];
    int user_file, load = 0, n;
    char buf_names[512], buf[25];
    user_file = open("config/users.txt", O_RDWR);
    if(user_file == -1)
        printf("Could not open the file\n");
    while((n=read(user_file, buf_names, 512))>0);
    close(user_file);
    for(int i = 0; i < users_n; i++){
        int j = 0;
        while(buf_names[load] != '\n')
            buf[j++] = buf_names[load++];
        load++;
        strcpy(users[i].uname, buf); 
        users[i].ulog = 0;
        for(int k = 0; k < groups_n; k++)
            users[i].ugroups[k] = 0;
    }
}
