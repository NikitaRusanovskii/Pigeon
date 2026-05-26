#define BASE_ROUTE_URL "http://localhost:8080"
#define ROUTE_URL(route) BASE_ROUTE_URL route

#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

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


CURLcode ping() {
    CURL *curl;
    CURLcode res;

    struct MemoryResponse m;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    init_memory(&m);

    curl_easy_setopt(curl, CURLOPT_URL, ROUTE_URL("/ping"));
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

CURLcode connect_to_signal_server(const char* role) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    struct MemoryResponse m;
    init_memory(&m);

    char* request_json = get_role_json(role);
    headers = curl_slist_append(headers, "Content-type: application/json");

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();


    curl_easy_setopt(curl, CURLOPT_URL, ROUTE_URL("/connect"));
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_json);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&m);

    res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    free_role_json(request_json);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

CURLcode disconnect_from_signal_server() {
    CURL *curl;
    CURLcode res;
    
    struct MemoryResponse m;
    init_memory(&m);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, ROUTE_URL("/disconnect"));
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

CURLcode heartbeat() {
    CURL *curl;
    CURLcode res;
    
    struct MemoryResponse m;
    init_memory(&m);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, ROUTE_URL("/heartbeat"));
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

CURLcode get_masters() {
    CURL *curl;
    CURLcode res;

    struct MemoryResponse m;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    init_memory(&m);

    curl_easy_setopt(curl, CURLOPT_URL, ROUTE_URL("/get_masters"));
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}


CURLcode get_masters() {
    CURL *curl;
    CURLcode res;

    struct MemoryResponse m;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    init_memory(&m);

    curl_easy_setopt(curl, CURLOPT_URL, ROUTE_URL("/get_slaves"));
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&m);

    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

CURLcode connect_to_signal_server(const char* addr_port) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    struct MemoryResponse m;
    init_memory(&m);

    char* request_json = set_master_json(addr_port);
    headers = curl_slist_append(headers, "Content-type: application/json");

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();


    curl_easy_setopt(curl, CURLOPT_URL, ROUTE_URL("/set_master"));
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_json);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&m);

    res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        printf("Response: %s\n", m.memory);
    }
    else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    free_memory(&m);
    free_set_master_json(request_json);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

int main() {
    connect_to_signal_server("master");
    heartbeat();

    return 0;
}