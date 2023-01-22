#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

#include "lib/struct.h"

Message mes;
User user;

void PressKey(){
    printf("Wcisnij ENTER aby kontynuuowac\n");
    getchar();
    char enter = 0;
    while(enter != '\r' && enter != '\n') enter = getchar();
}

void DisplayMenu(){
    printf("\nMenu:\n");
    printf("0. Odswiez\n");
    printf("1. Wyswietl liste zalogowanych uzytkownikow\n");
    printf("2. Wyslij prywatna wiadomosc do uzytkownika\n");
    printf("3. Wyslij wiadomosc do grupy\n");
    printf("4. Wyslij wiadomosc publiczna\n");
    printf("5. Zablokuj uzytkownika\n");
    printf("6. Odblokuj uzytkownika\n");
    printf("7. Dolacz do grupy\n");
    printf("8. Wyjdz z grupy\n");
    printf("9. Zablokuj grupe\n");
    printf("10. Odblokuj grupe\n");
    printf("20. Wyloguj\n");
}

int ChooseOption(){
    int option;
    while (scanf("%d", &option) != 1) scanf("%*s");
    return option;
}

int Register(){
    int start_q = msgget(123456, 0644 | IPC_CREAT);
    char name[username_length];
    printf("Wprowadz nazwe uzytkownika: ");
    scanf("%s", name);
    strcpy(user.uname, name);
    user.uid = getpid();
    mes.mtype = 20;
    strcpy(mes.mfrom, user.uname);
    mes.mid = user.uid;
    mes.send_login_count = user.login_count;
    msgsnd(start_q, &mes, (sizeof(mes) - sizeof(long)), 0);
    if(msgrcv(start_q, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0) > 0){
        if(strcmp(mes.mtext, "Uzytkownik nie istnieje") == 0 || strcmp(mes.mtext, "Uzytkownik jest juz zalogowany") == 0 || strcmp(mes.mtext, "Zostales zablokowany. Zbyt wiele nieudanych prob zalogowania") == 0){
            user.login_count = mes.send_login_count;
            printf("UWAGA! %s\nNieudane proby logowania: %d\n", mes.mtext, user.login_count);
            PressKey();
            return 0;
        }
        user.ulog = 1;
        int queue = msgget(user.uid, 0644 | IPC_CREAT); //Kolejka dla klienta
        printf("Zalogowano uzytkownika: %s id: %ld\n", user.uname, user.uid);
        return queue;
    }
    else{
        printf("Cholibka. Wystapil blad\n");
        return 0;
    }
}

void PrintList(int queue){
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    if(msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0) > 0){
        printf("%s\n", mes.mtext);
    }
    else printf("Blad przy wyswietlaniu listy zalogowanych uzytkownikow\n");
}

void SendMessageToClient(int queue){
    char sendTo[username_length];
    printf("Podaj adresata: ");
    scanf("%s", sendTo);
    printf("Tresc wiadomosci: ");
    getchar();
    scanf("%[^\n]s", mes.mtext);
    mes.mtype = 2;
    strcpy(mes.mfrom, user.uname);
    strcpy(mes.mto, sendTo);
    mes.mid = 0;
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
    if(receive > 0)
        printf("%s\n", mes.mtext);
    else
        printf("Blad podczas wysylania wiadomosci\n");
}

void SendMessageToGroup(int queue){
    char sendTo[groupname_length];
    printf("Podaj docelowa grupe: ");
    scanf("%s", sendTo);
    printf("Tresc wiadomosci: ");
    getchar();
    scanf("%[^\n]s", mes.mtext);
    mes.mtype = 3;
    strcpy(mes.mfrom, user.uname);
    strcpy(mes.mtogroup, sendTo);
    mes.mid = 0;
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
    if(receive > 0)
        printf("%s\n", mes.mtext);
    else
        printf("Blad podczas wysylania wiadomosci\n");
}

void SendPublicMessage(int queue){
    printf("Tresc wiadomosci: ");
    getchar();
    scanf("%[^\n]s", mes.mtext);
    mes.mtype = 4;
    strcpy(mes.mfrom, user.uname);
    mes.mid = 0;
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
    if(receive > 0)
        printf("%s\n", mes.mtext);
    else
        printf("Blad podczas wysylania wiadomosci\n");
}

void JoinGroup(int queue){
    printf("Podaj nazwe grupy: ");
    getchar();
    scanf("%[^\n]s", mes.mtext);
    mes.mtype = 30;
    strcpy(mes.mfrom, user.uname);
    mes.mid = 0;
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
    if(receive > 0)
        printf("%s\n", mes.mtext);
    else
        printf("Blad \n");
}

void LeaveGroup(int queue){
    printf("Podaj nazwe grupy: ");
    getchar();
    scanf("%[^\n]s", mes.mtext);
    mes.mtype = 31;
    strcpy(mes.mfrom, user.uname);
    mes.mid = 0;
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
    if(receive > 0)
        printf("%s\n", mes.mtext);
    else
        printf("Blad \n");
}

void BlockGroup(int queue){
    printf("Podaj nazwe grupy: ");
    getchar();
    scanf("%[^\n]s", mes.mtext);
    mes.mtype = 32;
    strcpy(mes.mfrom, user.uname);
    mes.mid = 0;
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
    if(receive > 0)
        printf("%s\n", mes.mtext);
    else
        printf("Blad \n");
}

void UnblockGroup(int queue){
    printf("Podaj nazwe grupy: ");
    getchar();
    scanf("%[^\n]s", mes.mtext);
    mes.mtype = 33;
    strcpy(mes.mfrom, user.uname);
    mes.mid = 0;
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
    if(receive > 0)
        printf("%s\n", mes.mtext);
    else
        printf("Blad \n");
}

void getMessage(int queue){
    while(msgrcv(queue, &mes, sizeof(mes) - sizeof(long), server_chat_mes, IPC_NOWAIT) > 0){
        if(mes.mid == 0)
            printf("%s: %s\n", mes.mfrom, mes.mtext);
        else if (mes.mid == 1)
            printf("[%s]%s: %s\n", mes.mtogroup, mes.mfrom, mes.mtext);
        else if (mes.mid == 2)
            printf("[public]%s: %s\n", mes.mfrom, mes.mtext);
    }
}

void blockUser(int queue){
    strcpy(mes.mfrom, user.uname);
    char sendTo[username_length];
    printf("Podaj nazwe uzytkownika do zablokowania: ");
    scanf("%s", sendTo);
    strcpy(mes.mto, sendTo);
    mes.mtype = 100;
    mes.mid = 0;
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
    if(receive > 0){
        printf("%s\n", mes.mtext);
    }
    else{
        printf("Blad podczas blokowania uzytkownika\n");
    }
}

void unlockUser(int queue){
    strcpy(mes.mfrom, user.uname);
    char sendTo[username_length];
    printf("Podaj nazwe uzytkownika do odblokowania: ");
    scanf("%s", sendTo);
    strcpy(mes.mto, sendTo);
    mes.mtype = 101;
    mes.mid = 0;
    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
    if(receive > 0){
        printf("%s\n", mes.mtext);
    }
    else{
        printf("Blad podczas odblokowania uzytkownika\n");
    }
}

int main(int argc, char* argv[]) {
    printf("Czat przygotowali: Michal Kowalewski i Marcin Lukomski\n");
    user.ulog = 0;
    user.login_count = 0;
    int queue;
    while(1){
        if(user.ulog == 0){
            queue = Register(); // Kolejka do komunikacji tego klienta
            while(queue == 0) queue = Register();
            //PressKey();
        }
        else{
            getMessage(queue);
            DisplayMenu();
            switch(ChooseOption()){
            	case 0:{
            	    break;
            	 }
                case 1:{ //lista zalogowanych
                    strcpy(mes.mfrom, user.uname);
                    mes.mtype = 1;
                    PrintList(queue);
                    PressKey();
                    break;
                }
                case 2:{
                    SendMessageToClient(queue);
                    PressKey();
                    break;
                }
                case 3:{
                    SendMessageToGroup(queue);
                    PressKey();
                    break;
                    }
                case 4:{
                    SendPublicMessage(queue);
                    PressKey();
                    break;
                    }
                case 5:{
                    blockUser(queue);
                    PressKey();
                    break;
                }
                case 6:{
                    unlockUser(queue);
                    PressKey();
                    break;
                }
                case 7:{
                    JoinGroup(queue);
                    PressKey();
                    break;
                    }
                case 8:{
                    LeaveGroup(queue);
                    PressKey();
                    break;
                    }
                case 9:{
                    BlockGroup(queue);
                    PressKey();
                    break;
                    }
                case 10:{
                    UnblockGroup(queue);
                    PressKey();
                    break;
                    }
                case 20:{
                    getMessage(queue);
                    strcpy(mes.mfrom, user.uname);
                    mes.mid = user.uid;
                    mes.mtype = 10;
                    msgsnd(queue, &mes, (sizeof(mes) - sizeof(long)), 0);
                    int receive = msgrcv(queue, &mes, (sizeof(mes) - sizeof(long)), server_mes, 0);
                    if(receive > 0){
                        printf("%s\n", mes.mtext);
                        kill(user.uid, SIGTERM);
                        return 0;
                    }
                    else{
                        printf("Blad podczas wylogowania\n");
                        return 0;
                    }
                    break;
                }
                default:{
                    printf("Prosze wprowadzic opcje z menu!\n");
                }
            }
        }
    }
}
