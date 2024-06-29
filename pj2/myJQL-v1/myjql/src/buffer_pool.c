#include "buffer_pool.h"
#include "file_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void init_buffer_pool(const char *filename, BufferPool *pool) {
    // printf("init_buffer_pool\n");
    FileInfo file;
    FileIOResult result = open_file(&file, filename);
    if(result != FILE_IO_SUCCESS) {
        // printf("Failed to open file %s\n", filename);
        // printf("Error code: %d\n", result);
        return;
    }
    pool->file = file;
    memset(pool->addrs, -1, sizeof(pool->addrs));
    memset(pool->cnt, 0, sizeof(pool->cnt));
    memset(pool->ref, 0, sizeof(pool->ref));
}

void close_buffer_pool(BufferPool *pool) {
    // printf("close_buffer_pool\n");
    for(auto i = 0; i < CACHE_PAGE; i++) {
        if((pool->addrs)[i] == -1) continue;
        FileIOResult result = write_page(&((pool->pages)[i]), &(pool->file), (pool->addrs)[i]);
        if(result != FILE_IO_SUCCESS) {
            // printf("Failed to write page %d\n", i);
            // printf("Error code: %d\n", result);
            return;
        }
    }
    close_file(&(pool->file));
}

Page *get_page(BufferPool *pool, off_t addr) {
    //LRU
    // printf("get_page\n");
    for(auto i = 0; i < CACHE_PAGE; i++) {
        (pool->cnt)[i]++;
        if((pool->addrs)[i] == addr) {
            // page hit
            (pool->cnt)[i] = 0;
            (pool->ref)[i]++;
            return &((pool->pages)[i]);
        }
    }
    int pre[CACHE_PAGE]; // pages that have not been referenced
    int idx = 0; // pre's index
    int pos = -1; // the position to replace
    for(auto i = 0; i < CACHE_PAGE; i++) {
        if((pool->ref)[i] == 0) {
            pre[idx] = i;
            idx++;
        }
    }
    if(idx == 0) return NULL;
    else pos = pre[0];
    // find the page that has the maximum cnt
    for(auto i = 1; i < idx; i++) {
        if((pool->cnt)[pos] < (pool->cnt)[pre[i]]) {
            pos = pre[i];
        }
    }
    // printf("pos: %d\n", pos);
    FileIOResult result;
    if((pool->addrs)[pos] != -1) {
        result = write_page(&((pool->pages)[pos]), &(pool->file), (pool->addrs)[pos]);
        if(result != FILE_IO_SUCCESS) {
            // printf("Failed to write page %d in get_page\n", pos);
            // printf("Error code: %d\n", result);
            return NULL;
        }
    }
    result = read_page(&((pool->pages)[pos]), &(pool->file), addr);
    (pool->addrs)[pos] = addr;
    (pool->cnt)[pos] = 0;
    (pool->ref)[pos] = 1;
    return &((pool->pages)[pos]);

}

void release(BufferPool *pool, off_t addr) {
    // printf("release\n");
    for(auto i = 0; i < CACHE_PAGE; i++) {
        if((pool->addrs)[i] == addr) {
            pool->ref[i]--;
            return;
        }
    }
    // printf("Page not found in buffer pool\n");
}

/* void print_buffer_pool(BufferPool *pool) {
} */

/* void validate_buffer_pool(BufferPool *pool) {
} */
