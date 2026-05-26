#include <unistd.h>
#include <stdlib.h>
#include <connector.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"

int main() {

    struct Http_server* server = (struct Http_server*)malloc(sizeof(struct Http_server));

    init_http_server(server);
    CURLcode res;

    printf(BLUE "\t\t\tACTION: connecting\n" RESET);
    res = connect_to_signal_server(server, "slave");
    if (res != CURLE_OK) printf(RED "\t\t\tRESULT: connection error\n" RESET);
    else printf(GREEN "\t\t\tRESULT: successful\n" RESET);
    sleep(1);

    printf(BLUE "\t\t\tACTION: ping\n" RESET);
    res = ping(server);
    if (res != CURLE_OK) printf(RED "\t\t\tRESULT: ping error\n" RESET);
    else printf(GREEN "\t\t\tRESULT: successful\n" RESET);
    sleep(1);

    printf(BLUE "\t\t\tACTION: heartbeat\n" RESET);
    res = heartbeat(server);
    if (res != CURLE_OK) printf(RED "\t\t\tRESULT: heartbeat error\n" RESET);
    else printf(GREEN "\t\t\tRESULT: successful\n" RESET);
    sleep(1);
    
    printf(BLUE "\t\t\tACTION: get_masters\n" RESET);
    res = get_masters(server);
    if (res != CURLE_OK) printf(RED "\t\t\tRESULT: get_masters error\n" RESET);
    else printf(GREEN "\t\t\tRESULT: successful\n" RESET);
    sleep(1);

    printf(BLUE "\t\t\tACTION: get_slaves\n" RESET);
    res = get_slaves(server);
    if (res != CURLE_OK) printf(RED "\t\t\tRESULT: get_slaves error\n" RESET);
    else printf(GREEN "\t\t\tRESULT: successful\n" RESET);
    sleep(1);
    
    printf(BLUE "\t\t\tACTION: disconnecting\n" RESET);
    res = disconnect_from_signal_server(server);
    if (res != CURLE_OK) printf(RED "\t\t\tRESULT: disconnecting error\n" RESET);
    else printf(GREEN "\t\t\tRESULT: successful\n" RESET);

    cleanup_http_server(server);

    return 0;
}