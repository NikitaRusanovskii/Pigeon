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

static size_t write_callback(void* contents, size_t size, size_t nmemb, void *userp);
CURLcode disconnect_from_signal_server(struct Http_server* server);
CURLcode heartbeat(struct Http_server *server);

CURLcode get_masters(struct Http_server* server);
CURLcode get_slaves(struct Http_server* server);