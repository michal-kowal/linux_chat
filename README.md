# Linux chat
IPC Chat Service - projekt zaliczeniowy

### Kompilacja
Aby skompilować, w terminalu nalezy wpisac komendy:
```
  gcc server.c -o server
  gcc client.c -o client
```

### Uruchamianie
Serwer mozna uruchomic tylko w jednym terminalu wpisujac ```./server```
Program klienta mozna uruchomic na wielu terminalach wpisujac ```./client```

### Zawartosc plikow
 - server.c zawiera glowna petle programu servera, wywoluje wszystkie funkcje
 - /src/inf149533_s.c zawiera funkcje serwera dotyczace logowania uzytkownika i wymiany wiadomosci prywatnych
 - /src/inf_151789.c zawiera funkcje serwera dotyczace oblugi grup i komunikacji miedzy nimi
 - client.c zawiera pelny program klienta

### Projekt wykonali
Michal Kowalewski - 149533
Marcin Lukomski - 151789
