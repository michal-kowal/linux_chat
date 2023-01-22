#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

#include "../lib/struct.h"


void JoinGroup(int queue, User *users, char (*groups)[groupname_length]){
   Message mes;
   int receive = msgrcv(queue, &mes, sizeof(mes) - sizeof(long), 30, IPC_NOWAIT), check = 0;
   int group_no=-1, from=-1, member=0;
   if(receive > 0){
   	for(int i=0; i<groups_n; i++){
   	    if (strcmp(groups[i], mes.mtext)==0){
   		group_no = i;
   		break;
   	    }
   	}
   	for (int i=0; i<users_n; i++){
   	    if (strcmp(users[i].uname, mes.mfrom) == 0){
   	        from = i;
   	        break;
   	    }
   	}
   	if(from>=0 && group_no>=0 && users[from].ugroups[group_no]==0){
   	    users[from].ugroups[group_no]=1;
   	    strcpy(mes.mtext, "Zapisano do grupy");
   	}
   	else if(group_no<0){
   	    strcpy(mes.mtext, "Grupa nie istnieje");
   	}
   	else if(from>=0 && users[from].ugroups[group_no]!=0){
   	    strcpy(mes.mtext, "Juz jestes w tej grupie");
   	}
   	mes.mtype = server_mes;
        msgsnd(queue, &mes, sizeof(mes) - sizeof(long), 0);
   }
}

void LeaveGroup(int queue, User *users, char (*groups)[groupname_length]){
   Message mes;
   int receive = msgrcv(queue, &mes, sizeof(mes) - sizeof(long), 31, IPC_NOWAIT), check = 0;
   int group_no=-1, from=-1, member=0;
   if(receive > 0){
   	for(int i=0; i<groups_n; i++){
   	    if (strcmp(groups[i], mes.mtext)==0){
   		group_no = i;
   		break;
   	    }
   	}
   	for (int i=0; i<users_n; i++){
   	    if (strcmp(users[i].uname, mes.mfrom) == 0){
   	        from = i;
   	        break;
   	    }
   	}
   	if(from>=0 && group_no>=0 && users[from].ugroups[group_no]!=0){
   	    users[from].ugroups[group_no]=0;
   	    strcpy(mes.mtext, "Opuszczono grupe");
   	}
   	else if(group_no<0){
   	    strcpy(mes.mtext, "Grupa nie istnieje");
   	}
   	else if(from>=0 && users[from].ugroups[group_no]==0){
   	    strcpy(mes.mtext, "Nie jestes w tej grupie");
   	}
   	mes.mtype = server_mes;
        msgsnd(queue, &mes, sizeof(mes) - sizeof(long), 0);
   }
}

void BlockGroup(int queue, User *users, char (*groups)[groupname_length]){
   Message mes;
   int receive = msgrcv(queue, &mes, sizeof(mes) - sizeof(long), 32, IPC_NOWAIT), check = 0;
   int group_no=-1, from=-1, member=0;
   if(receive > 0){
   	for(int i=0; i<groups_n; i++){
   	    if (strcmp(groups[i], mes.mtext)==0){
   		group_no = i;
   		break;
   	    }
   	}
   	for (int i=0; i<users_n; i++){
   	    if (strcmp(users[i].uname, mes.mfrom) == 0){
   	        from = i;
   	        break;
   	    }
   	}
   	if(from>=0 && group_no>=0 && users[from].ugroups[group_no]==1){
   	    users[from].ugroups[group_no]=-1;
   	    strcpy(mes.mtext, "Zablokowano grupe");
   	}
   	else if(group_no<0){
   	    strcpy(mes.mtext, "Grupa nie istnieje");
   	}
   	else if(from>=0 && users[from].ugroups[group_no]==0){
   	    strcpy(mes.mtext, "Nie jestes w tej grupie");
   	}
   	else if(from>=0 && users[from].ugroups[group_no]==-1){
   	    strcpy(mes.mtext, "Juz blokujesz grupe");
   	}
   	mes.mtype = server_mes;
        msgsnd(queue, &mes, sizeof(mes) - sizeof(long), 0);
   }
}

void UnblockGroup(int queue, User *users, char (*groups)[groupname_length]){
   Message mes;
   int receive = msgrcv(queue, &mes, sizeof(mes) - sizeof(long), 33, IPC_NOWAIT), check = 0;
   int group_no=-1, from=-1, member=0;
   if(receive > 0){
   	for(int i=0; i<groups_n; i++){
   	    if (strcmp(groups[i], mes.mtext)==0){
   		group_no = i;
   		break;
   	    }
   	}
   	for (int i=0; i<users_n; i++){
   	    if (strcmp(users[i].uname, mes.mfrom) == 0){
   	        from = i;
   	        break;
   	    }
   	}
   	if(from>=0 && group_no>=0 && users[from].ugroups[group_no]==-1){
   	    users[from].ugroups[group_no]=1;
   	    strcpy(mes.mtext, "Odblokowano grupe");
   	}
   	else if(group_no<0){
   	    strcpy(mes.mtext, "Grupa nie istnieje");
   	}
   	else if(from>=0 && users[from].ugroups[group_no]==0){
   	    strcpy(mes.mtext, "Nie jestes w tej grupie");
   	}
   	else if(from>=0 && users[from].ugroups[group_no]==1){
   	    strcpy(mes.mtext, "Nie blokujesz tej grupy");
   	}
   	mes.mtype = server_mes;
        msgsnd(queue, &mes, sizeof(mes) - sizeof(long), 0);
   }
}

void SendGroupMessage(int queue, User *users, int *userqueues, char (*groups)[groupname_length], int (*BanMatrix)[users_n]){
    Message mes;
    int receive = msgrcv(queue, &mes, sizeof(mes) - sizeof(long), 3, IPC_NOWAIT), check = 0;
    if(receive > 0){
        int from = -1, to = -1;
        for(int i = 0; i < groups_n; i++){
            if(strcmp(groups[i], mes.mtogroup) == 0)
                to = i;
        }
        for(int i = 0; i < users_n; i++){
            if(strcmp(users[i].uname, mes.mfrom) == 0)
                from = i;
        }
        if (to >= 0 && from>=0 && users[from].ugroups[to]!=0){
      	    for(int i = 0; i < users_n; i++){
            	if(userqueues[i] != 0 && users[i].ulog == 1){
                    if(users[i].ugroups[to]==1 && BanMatrix[i][from] == 0){
			mes.mtype = server_chat_mes;
			mes.mid = 1;
                    	msgsnd(userqueues[i], &mes, sizeof(mes) - sizeof(long), 0);
                	} 
                 }
                 printf("Przeslano wiadomosc od %s do grupy %s\n", mes.mfrom, mes.mtogroup);
            }
            strcpy(mes.mtext, "Wyslano");
        }
        else if(to<0){
            strcpy(mes.mtext, "Ta grupa nie istnieje");
        }
        else
            strcpy(mes.mtext, "Nie jestes czlonkiem tej grupy");  
        mes.mtype = server_mes;
        msgsnd(queue, &mes, sizeof(mes) - sizeof(long), 0);
    }
}

void SendPublicMessage(int queue, User *users, int *userqueues, int (*BanMatrix)[users_n]){
    Message mes;
    int receive = msgrcv(queue, &mes, sizeof(mes) - sizeof(long), 4, IPC_NOWAIT), check = 0;
    if(receive > 0){
        int from = -1;
        for(int i = 0; i < users_n; i++){
            if(strcmp(users[i].uname, mes.mfrom) == 0)
                from = i;
        }
        if (from>=0){
      	    for(int i = 0; i < users_n; i++){
            	if(userqueues[i] != 0 && users[i].ulog == 1){
                    if(BanMatrix[i][from] == 0){
			mes.mtype = server_chat_mes;
			mes.mid = 2;
                    	msgsnd(userqueues[i], &mes, sizeof(mes) - sizeof(long), 0);
                	} 
                 }
                 printf("Przeslano wiadomosc publiczna od %s\n", mes.mfrom);
            }
            strcpy(mes.mtext, "Wyslano");
        mes.mtype = server_mes;
        msgsnd(queue, &mes, sizeof(mes) - sizeof(long), 0);
    	}
    }
}
