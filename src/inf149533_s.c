#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

#include "../lib/struct.h"

void LogInClient(int start_q, User *users, int *client_queues){
    Message mes;
    int receive = msgrcv(start_q, &mes, (sizeof(mes) - sizeof(long)), 20, IPC_NOWAIT);
    int success = 0, logged = 0, exists = 0;
    if(receive > 0){
        for(int i = 0; i < users_n; i++){
            if(strcmp(users[i].uname, mes.mfrom) == 0 && mes.send_login_count < MAX_login){
                exists = 1;
                if(users[i].ulog == 0){
                    users[i].ulog = 1;
                    users[i].uid = mes.mid;
                    printf("Uzytkownik %s o nr id %ld zalogowal sie\n", users[i].uname, users[i].uid);
                    strcpy(mes.mtext, "Sukces");
                    success = 1;
                    logged = 1;
                    for(int j = 0; j < users_n; j++){
                        if(client_queues[j] == 0) {
                            client_queues[j] = msgget(mes.mid, 0644 | IPC_CREAT);
                            break;
                        }
                    }
                    break;
                }
            }
        }
        if(exists == 0) strcpy(mes.mtext, "Uzytkownik nie istnieje");
        else if(logged == 0 && exists == 1) strcpy(mes.mtext, "Uzytkownik jest juz zalogowany");
        if (success == 0){
            mes.send_login_count++;
            printf("Nieudane PROBY: %d\n", mes.send_login_count);
            if(mes.send_login_count >= MAX_login){
                printf("TU JESTEM\n");
                strcpy(mes.mtext, "Zostales zablokowany. Zbyt wiele nieudanych prob zalogowania");
            }
            printf("Blad logowania uzytkownika\n");
        }
        printf("%s\n", mes.mtext);
        mes.mtype = server_mes;
        msgsnd(start_q, &mes, sizeof(mes) - sizeof(long), 0);
    }
}

void PrintLoggedUsers(int queue, User *users){
    Message mes;
    int receive = msgrcv(queue, &mes, sizeof(mes) - sizeof(long), 1, IPC_NOWAIT);
    if(receive > 0){
        printf("Uzytkownik %s: wyswietl liste zalogowanych klientow\n", mes.mfrom);
        char list[MAX_strlen], tmplist[MAX_strlen];
        memset(list, 0, MAX_strlen);
        memset(tmplist, 0, MAX_strlen);
        int k = 1;
        for(int i = 0; i < users_n; i++){
            if(users[i].ulog == 1){
                sprintf(tmplist, "%d. %s\n", k++, users[i].uname);
                strcat(list, tmplist);
            }
        }
        mes.mtype = server_mes;
        strcpy(mes.mtext, list);
        msgsnd(queue, &mes, sizeof(mes) - sizeof(long), 0);
    }
}

void SendPrivateMessageToClient(int queue, User *users, int *userqueues, int (*BanMatrix)[users_n]){
    Message mes;
    int receive = msgrcv(queue, &mes, sizeof(mes) - sizeof(long), 2, IPC_NOWAIT), check = 0;
    if(receive > 0){
        int block = 0;
        int from = -1, to = -1;
        for(int i = 0; i < users_n; i++){
            if(strcmp(users[i].uname, mes.mto) == 0)
                to = i;
            if(strcmp(users[i].uname, mes.mfrom) == 0)
                from = i;
        }
        for(int i = 0; i < users_n; i++){
            if(strcmp(users[i].uname, mes.mto) == 0 && userqueues[i] != 0 && users[i].ulog == 1){
                if(to >= 0 && from >= 0 && BanMatrix[to][from] == 1){
                    block = 1;
                    break;
                }
                check = 1;
                mes.mtype = server_chat_mes;
                msgsnd(userqueues[i], &mes, sizeof(mes) - sizeof(long), 0);
                printf("Przeslano wiadomosc pomiedzy %s i %s\n", mes.mfrom, mes.mto);
                break;
            }
        }
        if(check == 0 && block == 0)
            strcpy(mes.mtext, "Adresat niedostepny");
        else if(block == 1){
            strcpy(mes.mtext, "Ten uzytkownik cie zablokowal");
        }
        else
            strcpy(mes.mtext, "Wyslano");
        mes.mtype = server_mes;
        msgsnd(queue, &mes, sizeof(mes) - sizeof(long), 0);
    }
}

void Logout(int queue, User *users, int *userqueues){
    Message mes;
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), 10, IPC_NOWAIT);
    if(receive > 0){
        for(int i = 0; i < users_n; i++){
            if(strcmp(users[i].uname, mes.mfrom) == 0 && userqueues[i] == queue && users[i].ulog == 1){
                users[i].ulog = 0;
                userqueues[i] = 0;
                strcpy(mes.mtext, "Wylogowano");
                mes.mtype = server_mes;
                msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
                msgctl(queue, IPC_RMID, NULL);
                printf("wylogowano uzytkownika %s\n", users[i].uname);
                break;
            }
        }
    }
}

void Block(int queue, User *users, int (*BanMatrix)[users_n]){
    Message mes;
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), 100, IPC_NOWAIT);
    if(receive > 0){
        int from = -1, to = -1;
        for(int i = 0; i < users_n; i++){
            if(strcmp(users[i].uname, mes.mto) == 0)
                to = i;
            if(strcmp(users[i].uname, mes.mfrom) == 0)
                from = i;
        }
        mes.mtype = server_mes;
        if(from == -1 || to == -1){
            strcpy(mes.mtext, "Nie mozna zablokowac");
        }
        else{
            BanMatrix[from][to] = 1;
            strcpy(mes.mtext, "Zablokowano uztkownika");
            printf("uztkownik %s zablokowal uzytkownika %s\n",mes.mfrom, mes.mto);
        }
        msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
        for(int i = 0; i < users_n; i++){
            for(int j = 0; j < users_n; j++){
                printf("%d ", BanMatrix[i][j]);
            }
            printf("\n");
        }
    }
}

void unlockUser(int queue, User *users, int (*BanMatrix)[users_n]){
    Message mes;
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), 101, IPC_NOWAIT);
    if(receive > 0){
        int from = -1, to = -1;
        for(int i = 0; i < users_n; i++){
            if(strcmp(users[i].uname, mes.mto) == 0)
                to = i;
            if(strcmp(users[i].uname, mes.mfrom) == 0)
                from = i;
        }
        mes.mtype = server_mes;
        if(from == -1 || to == -1)
            strcpy(mes.mtext, "Nie mozna odblokowac");
        else{
            BanMatrix[from][to] = 0;
            strcpy(mes.mtext, "Odblokowano uztkownika");
            printf("uztkownik %s odblokowal uzytkownika %s\n",mes.mfrom, mes.mto);
        }
        msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
        for(int i = 0; i < users_n; i++){
            for(int j = 0; j < users_n; j++){
                printf("%d ", BanMatrix[i][j]);
            }
            printf("\n");
        }
    }
}
