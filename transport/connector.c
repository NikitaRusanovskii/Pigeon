#define BASE_ROUTE_URL "http://localhost:8080"
#define ROUTE_URL(route) BASE_ROUTE_URL route

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"

#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <unistd.h>

struct Http_server {
    CURL *curl;
};

void init_http_server(struct Http_server* server) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    server->curl = curl_easy_init();
}

void cleanup_http_server(struct Http_server* server) {
    curl_easy_cleanup(server->curl);
    curl_global_cleanup();
}

struct MemoryResponse {
    char *memory;
    size_t size;
};

static size_t write_callback(void* contents, size_t size, size_t nmemb, void *userp) {
    size_t r = size * nmemb;
    struct MemoryResponse* mem = (struct MemoryResponse*)userp;

    char* ptr = (char*)realloc(mem->memory, mem->size + r + 1);
    if (!ptr) return 0;

    mem->memory = ptr;

    memcpy(&mem->memory[mem->size], (char*)contents, r); // хотим дописывать данные в конец
    mem->size += r;
    mem->memory[mem->size] = 0;
    return r;
}

void init_memory(struct MemoryResponse* m) {
    m->memory = (char*)malloc(1);
    m->size = 0;
    if (m->memory[0]) m->memory[0] = '\0';
}


void free_memory(struct MemoryResponse* m) {
    if (m->memory) free(m->memory);
    m->memory = NULL;
    m->size = 0;
}

char* get_role_json(const char* role) {
    size_t len = snprintf(NULL, 0, "{\"role\": \"%s\"}", role) + 1;
    char* json = (char*)malloc(len);
    if (!json) {
        return NULL;
    }
    snprintf(json, len, "{\"role\": \"%s\"}", role);
    return json;
}

void free_role_json(char* json) {
    if(json) free(json);
}


char* set_master_json(const char* addr_port) {
    size_t len = snprintf(NULL, 0, "{\"addr_port\": \"%s\"}", addr_port) + 1;
    char* json = (char*)malloc(len);
    if (!json) {
        return NULL;
    }
    snprintf(json, len, "{\"role\": \"%s\"}", addr_port);
    return json;
}

void free_set_master_json(char* json) {
    if(json) free(json);
}


CURLcode ping(struct Http_server* server) {
    CURLcode res;

    struct MemoryResponse m;

    init_memory(&m);

    curl_easy_setopt(server->curl, CURLOPT_URL, ROUTE_URL("/ping"));
    curl_easy_setopt(server->curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(server->curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(server->curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(server->curl);

    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_reset(server->curl);
    return res;
}

CURLcode connect_to_signal_server(struct Http_server* server, const char* role) {
    CURLcode res;
    struct curl_slist *headers = NULL;

    struct MemoryResponse m;
    init_memory(&m);

    char* request_json = get_role_json(role);
    headers = curl_slist_append(headers, "Content-type: application/json");

    curl_easy_setopt(server->curl, CURLOPT_URL, ROUTE_URL("/connect"));
    curl_easy_setopt(server->curl, CURLOPT_POST, 1L);

    curl_easy_setopt(server->curl, CURLOPT_POSTFIELDS, request_json);
    curl_easy_setopt(server->curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(server->curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(server->curl, CURLOPT_WRITEDATA, (void *)&m);

    res = curl_easy_perform(server->curl);
    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    free_role_json(request_json);
    curl_easy_reset(server->curl);

    return res;
}

CURLcode disconnect_from_signal_server(struct Http_server* server) {
    CURLcode res;
    
    struct MemoryResponse m;
    init_memory(&m);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl_easy_setopt(server->curl, CURLOPT_URL, ROUTE_URL("/disconnect"));
    curl_easy_setopt(server->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    
    curl_easy_setopt(server->curl, CURLOPT_POSTFIELDS, NULL);
    curl_easy_setopt(server->curl, CURLOPT_HTTPHEADER, NULL);

    curl_easy_setopt(server->curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(server->curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(server->curl);
    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_reset(server->curl);
    return res;
}

CURLcode heartbeat(struct Http_server *server) {
    CURLcode res;
    
    struct MemoryResponse m;
    init_memory(&m);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl_easy_setopt(server->curl, CURLOPT_URL, ROUTE_URL("/heartbeat"));
    curl_easy_setopt(server->curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    
    curl_easy_setopt(server->curl, CURLOPT_POSTFIELDS, NULL);
    curl_easy_setopt(server->curl, CURLOPT_HTTPHEADER, NULL);

    curl_easy_setopt(server->curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(server->curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(server->curl);
    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_reset(server->curl);

    return res;
}

CURLcode get_masters(struct Http_server* server) {
    CURLcode res;

    struct MemoryResponse m;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    init_memory(&m);

    curl_easy_setopt(server->curl, CURLOPT_URL, ROUTE_URL("/get_masters"));
    curl_easy_setopt(server->curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(server->curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(server->curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(server->curl);

    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_reset(server->curl);

    return res;
}


CURLcode get_slaves(struct Http_server* server) {
    CURLcode res;

    struct MemoryResponse m;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    init_memory(&m);

    curl_easy_setopt(server->curl, CURLOPT_URL, ROUTE_URL("/get_slaves"));
    curl_easy_setopt(server->curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(server->curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(server->curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(server->curl);

    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_reset(server->curl);

    return res;
}

int main() {

    struct Http_server* server;

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