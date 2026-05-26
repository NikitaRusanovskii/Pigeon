#pragma once
#include "curl/curl.h"

struct Http_server {
    CURL *curl;
};

struct MemoryResponse {
    char *memory;
    size_t size;
};

void init_http_server(struct Http_server* server);
void cleanup_http_server(struct Http_server* server);

size_t write_callback(void* contents, size_t size, size_t nmemb, void *userp);

void init_memory(struct MemoryResponse* m);


void free_memory(struct MemoryResponse* m); 
char* get_role_json(const char* role);

void free_role_json(char* json);

char* set_master_json(const char* addr_port);

void free_set_master_json(char* json);



CURLcode ping(struct Http_server* server);
CURLcode connect_to_signal_server(struct Http_server* server, const char* role);

CURLcode disconnect_from_signal_server(struct Http_server* server);
CURLcode heartbeat(struct Http_server *server);

CURLcode get_masters(struct Http_server* server);
CURLcode get_slaves(struct Http_server* server);