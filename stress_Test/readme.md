#Turbo zadanie z linuxa:
##Co jest zmienione względem treści:
1. Pole sun_family jest typu unsigned short. Nie mogę przypisywać tam -1 (**Błąd w treści zadania!**). Daje tam maksymalny zakres shorta.
Zmieńcie to jakoś bo jest to bardzo charakterystyczne.

2. Używam _CLOCK_REALTIME_ zamiast _Wall_Clock_. Tego drugiego nie potrafię użyć.
W dodatku Wall Clock zwraca czas lokalny dla pracy każdego procesu - porównanie ich międy procesami jest niemożliwe.(**Błąd w treści zadania!**)

3. Zamykanie pierwszego gniazda... nie wiem, czy jest tak jak w zadaniu, bo tego kurwa nie rozumiem.

4. Wprowadziłem też dodatkową linię konunikacji klient serwer:

    #####TCP
    Jeżeli klient zamierza wysłać jeszcze jeden pakiet z adresem wysyła integera ustawionego na 1
    
    Jeżeli kończy to wysyła 0;
    
    #####LOCAL/UNIX
    Kiedy klient kończy testy na każde gnizado wysyła pojedyńczy zerowy bit. Serwer przerywa czytanie.
    Jest to wywołane problemem z nieskończonego czytania z zamkniętego deskryptora. **Bo kurwa unix jest zjebany!**

##Co nie działa:
Kończenie, wychodzenie. Serwer na konec lubi się po ostatniej wiadomości zaciąć i nie ruszać.
Z niewiadomych dla mnie przyczyn gdy klient zamyka połączenie nie przychodzi na drugą stronę informacja o tym.
Read nie zwraca błędu errno nie zostaje ustawione. Super sprawa. Można czytać w nieskończonośc z zamkniętego deskryptora.

##Co robią funkcje:
    void options(int argc, char** argv, int* conections_num, int* port, float* intervals, float* full_time);

_getopt_

    void generate_pathname(char* path);
   
Do tworzenia nazw plików w których będą zapisywane logi.
  
    void func_inet(int sockfd, int unix_desc, struct sockaddr_un server, int* socket_unix,int* active_connections);
    
Główna funkcja klienta inet - łączy sie z serwerem i rejestruje gniazda _AF_UNIX_ (program klient rola klient)
    
    void print_time(int desc, struct timespec time);
    
Formatuje ładnie (według zał zadania) i wypisuje na podany deskryptor strukturę _timespec_. Dla niezorientowanych - terminal to deskryptor 0.
    
    void funct_server(const int* socket_unix, int active_clients,struct sockaddr_un server_unix);
    
Program klient rola serwer.
    
    void time_to_text(char* text, struct timespec time);
    
Domyśl się. tekst wyjściowy ma 18 znaków i nie jest zakończony _'\0'_
    
    long check_time(struct timespec real_time,float intervals);
    void sighandle();
    
W uchwycie zwiększamy tylko numer pliku w który wpisany zostanie kolejny czas. To czy sygnał przyszedł ma efekt dopiero na początku sprawdzania kolejnego gniazda.
    
    int open_log();
    
Otworzenie pliku do zapisu.
    
    void setsig();
    
Ustawienie obsługi sygnału do zmiany pliku.
    
    void server_function(int inet_desc, int* client_num, int** table_desc,struct sockaddr_un** table_addr);
    
Główna funkcja programu serwera gdy jest serwerem (podpunkt 1)...
    
    void client_function(int client_num, int* desc_array , struct sockaddr_un* array_addr, int* desc);
    
Główna funkcja programu serwera gdy jest klientem (podpunkt 2)...
    
    struct timespec time_difference(struct timespec first_time, struct timespec second_time);

Różnica między dwoma strukturami _timespec_.
    
    int authorisation(struct sockaddr_un x, struct sockaddr_un y);
    
Sprawdzanie czy adres zawarty w wiadomości domeny _AF_UNIX_ został wcześniej zarejestrowany.
    
    void sighandle_out();
    
Obsługa ręcznego zatrzymania serwera za pomocą sygnału _SIGUSR2_ Możecie ją wywalić.
    