#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

#include "lib/struct.h"
#include "src/inf149533_s.c"
#include "src/inf151789_s.c"

void CloseProgram(){
    printf("\nKoniec pracy. Do widzenia\n");
    msgctl(123456, IPC_RMID, NULL);
    exit(0);
}


int main(int argc, char* argv[]) {
    signal(SIGINT, CloseProgram);
    User users[users_n];
    int user_file, group_file, load = 0, n, client_queues[users_n], BanMatrix[users_n][users_n];
    char buf_names[MAX_strlen], buf_groups[MAX_strlen], buf[username_length], bufg[groupname_length], groups[groups_n][groupname_length];
    group_file = open("config/groups.txt", O_RDWR);
    if(user_file == -1)
        printf("Could not open the file\n");
    while((n=read(group_file, buf_groups, MAX_strlen))>0);
    close(group_file);
    for (int i = 0; i < groups_n; i++){
    	int j = 0;
    	while(buf_groups[load] != '\n')
            bufg[j++] = buf_groups[load++];
      	bufg[j++] = '\0';
    	load++;
      	strcpy(groups[i], bufg);
      	printf("%s\n", groups[i]);
    	}
    user_file = open("config/users.txt", O_RDWR);
    if(user_file == -1)
        printf("Could not open the file\n");
    while((n=read(user_file, buf_names, MAX_strlen))>0);
    close(user_file);
    load = 0;
    for(int i = 0; i < users_n; i++){
        client_queues[i] = 0;
        int j = 0;
        while(buf_names[load] != '\n')
            buf[j++] = buf_names[load++];
        buf[j++] = '\0';
        load++;
        strcpy(users[i].uname, buf); 
        printf("%s\n", users[i].uname);
        users[i].ulog = 0;
        users[i].login_count = 0;
        for(int k = 0; k < groups_n; k++)
            users[i].ugroups[k] = 0;
        for(int k = 0; k < users_n; k++){
            BanMatrix[i][k] = 0;
        }
    }
    
    int start_q = msgget(123456, 0644 | IPC_CREAT);
    while(1){
        LogInClient(start_q, users, client_queues);
        for(int i = 0; i < users_n; i++){
            if(client_queues[i] != 0){
                PrintLoggedUsers(client_queues[i], users);
                SendPublicMessage(client_queues[i], users, client_queues, BanMatrix);
                SendGroupMessage(client_queues[i], users, client_queues, groups, BanMatrix);
                SendPrivateMessageToClient(client_queues[i], users, client_queues, BanMatrix);
                JoinGroup(client_queues[i], users, groups);
                LeaveGroup(client_queues[i], users, groups);
                BlockGroup(client_queues[i], users, groups);
                UnblockGroup(client_queues[i], users, groups);
                Block(client_queues[i], users, BanMatrix);
                unlockUser(client_queues[i], users, BanMatrix);
                Logout(client_queues[i], users, client_queues);
            }
        }
        sleep(1);
    }
}
