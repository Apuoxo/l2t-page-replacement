// vmsim.c - Page replacement simulator: LRU, LFU, L²/t
// Compile: gcc -O3 -o vmsim vmsim.c -lm
// Run: ./vmsim <cache_size> <trace_file>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_PAGES 500000
#define MAX_CANDIDATES 16

typedef struct {
    int id;
    int last_access;
    int freq;
    int insert_time;
    int valid;
} Page;

typedef struct {
    Page *pages;
    int size;
    int count;
    int time;
} Cache;

Cache *cache_create(int size) {
    Cache *c = malloc(sizeof(Cache));
    c->pages = calloc(size, sizeof(Page));
    c->size = size;
    c->count = 0;
    c->time = 0;
    return c;
}

void cache_destroy(Cache *c) {
    free(c->pages);
    free(c);
}

int find_page(Cache *c, int id) {
    for (int i = 0; i < c->size; i++)
        if (c->pages[i].valid && c->pages[i].id == id)
            return i;
    return -1;
}

int evict_lru(Cache *c) {
    int idx = -1;
    int oldest = INT_MAX;
    for (int i = 0; i < c->size; i++) {
        if (c->pages[i].valid && c->pages[i].last_access < oldest) {
            oldest = c->pages[i].last_access;
            idx = i;
        }
    }
    return idx;
}

int evict_lfu(Cache *c) {
    int idx = -1;
    int min_freq = INT_MAX;
    for (int i = 0; i < c->size; i++) {
        if (c->pages[i].valid && c->pages[i].freq < min_freq) {
            min_freq = c->pages[i].freq;
            idx = i;
        }
    }
    return idx;
}

int evict_l2t(Cache *c) {
    int idx = -1;
    double min_score = 1e18;
    for (int i = 0; i < MAX_CANDIDATES; i++) {
        int pos = rand() % c->size;
        if (!c->pages[pos].valid) continue;
        int age = c->time - c->pages[pos].insert_time;
        double score = (double)c->pages[pos].freq * c->pages[pos].freq / (age + 1);
        if (score < min_score) {
            min_score = score;
            idx = pos;
        }
    }
    return idx;
}

int access_page(Cache *c, int page_id, int policy) {
    c->time++;
    int pos = find_page(c, page_id);
    if (pos != -1) {
        c->pages[pos].last_access = c->time;
        c->pages[pos].freq++;
        return 1;
    }
    if (c->count < c->size) {
        pos = c->count++;
    } else {
        switch (policy) {
            case 0: pos = evict_lru(c); break;
            case 1: pos = evict_lfu(c); break;
            case 2: pos = evict_l2t(c); break;
            default: pos = evict_lru(c);
        }
    }
    c->pages[pos].id = page_id;
    c->pages[pos].last_access = c->time;
    c->pages[pos].freq = 1;
    c->pages[pos].insert_time = c->time;
    c->pages[pos].valid = 1;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <cache_size> <trace_file>\n", argv[0]);
        return 1;
    }
    int cache_size = atoi(argv[1]);
    FILE *fp = fopen(argv[2], "r");
    if (!fp) {
        perror("Error opening trace file");
        return 1;
    }
    const char *names[] = {"LRU", "LFU", "L2t"};
    srand(42);
    for (int policy = 0; policy < 3; policy++) {
        Cache *c = cache_create(cache_size);
        int hits = 0, total = 0, page;
        rewind(fp);
        while (fscanf(fp, "%d", &page) == 1) {
            total++;
            if (access_page(c, page, policy)) hits++;
        }
        printf("%s: hit ratio = %.4lf (%d/%d)\n", names[policy], (double)hits/total, hits, total);
        cache_destroy(c);
    }
    fclose(fp);
    return 0;
}