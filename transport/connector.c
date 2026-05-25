#define URL "http://localhost:8080/ping"

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

CURLcode ping() {
    CURL* curl;
    CURLcode res;

    struct MemoryResponse m;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    init_memory(&m);

    curl_easy_setopt(curl, CURLOPT_URL, URL);
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

int main() {
    ping();

    return 0;
}