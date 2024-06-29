#include "hash_map.h"

#include <stdio.h>
#include <string.h>

void hash_table_init(const char *filename, BufferPool *pool, off_t n_directory_blocks) {
    init_buffer_pool(filename, pool);
    /* TODO: add code here */
    if(pool->file.length) return;
    Page* ctl_page = get_page(pool, 0);
    HashMapControlBlock* ctl_block = (HashMapControlBlock*)ctl_page;
    // hashmap control block
    ctl_block->n_directory_blocks = n_directory_blocks;
    ctl_block->max_size = 8; // 8 is just a random number
    ctl_block->free_block_head = PAGE_SIZE * (n_directory_blocks + 1);
    write_page(ctl_page, &(pool->file), 0);
    release(pool, 0);
    // hashmap directory block
    for(auto i = 0; i < n_directory_blocks; i ++) {
        HashMapDirectoryBlock dir_block_entity;
        HashMapDirectoryBlock* dir_block = &dir_block_entity;
        memset(dir_block->directory, (off_t)-1, sizeof(dir_block->directory));
        write_page((Page*)dir_block, &(pool->file), PAGE_SIZE * (i + 1));
    }
    // hashmap block
    for(auto i = 0; i < ctl_block->max_size; i ++) {
        HashMapBlock map_block_entity;
        HashMapBlock* map_block = &map_block_entity;
        map_block->n_items = 0;
        if(i != ctl_block->max_size - 1) {
            map_block->next = ctl_block->free_block_head + PAGE_SIZE * (i + 1);
        } else {
            map_block->next = -1;
        }
        write_page((Page*)map_block, &(pool->file), ctl_block->free_block_head + PAGE_SIZE * i);
    }
}

void hash_table_close(BufferPool *pool) {
    close_buffer_pool(pool);
}

void hash_table_insert(BufferPool *pool, short size, off_t addr) {
    if(size < 0 || size >= PAGE_SIZE) {
        // printf("size is out of range in hash_table_insert\n");
        return;
    }
    HashMapControlBlock* ctl_block = (HashMapControlBlock*)get_page(pool, 0);
    off_t cur_addr = PAGE_SIZE * ((size / HASH_MAP_DIR_BLOCK_SIZE) + 1);
    Page* cur_page = get_page(pool, cur_addr);
    off_t* cur_ptr = &(((HashMapDirectoryBlock*)cur_page)->directory[size % HASH_MAP_DIR_BLOCK_SIZE]);
    off_t new_addr = PAGE_SIZE * (ctl_block->n_directory_blocks + ctl_block->max_size + 1);
    // printf("cur_ptr_entity: %ld\n", *cur_ptr);
    if(*cur_ptr != -1) {
        off_t tmp_ptr_entity = *cur_ptr;
        while(tmp_ptr_entity != -1) {
            HashMapBlock* tmp_block = (HashMapBlock*)get_page(pool, tmp_ptr_entity);
            if(tmp_block->n_items < HASH_MAP_BLOCK_SIZE) {
                // printf("tmp_ptr_entity hit: %ld\n", tmp_ptr_entity);
                tmp_block->table[tmp_block->n_items] = addr;
                tmp_block->n_items++;
                release(pool, tmp_ptr_entity);
                release(pool, 0);
                release(pool, cur_addr);
                return;
            } else {
                // printf("tmp_ptr_entity not hit: %ld\n", tmp_ptr_entity);
                off_t pre_ptr_entity = tmp_ptr_entity;
                tmp_ptr_entity = tmp_block->next;
                release(pool, pre_ptr_entity);
            }
        }
        HashMapBlock new_map_block;
        new_map_block.n_items = 1;
        new_map_block.table[0] = addr;
        new_map_block.next = *cur_ptr;
        
        write_page((Page*)&new_map_block, &(pool->file), new_addr);
        *cur_ptr = new_addr;
        ctl_block->max_size++;
        release(pool, 0);
        release(pool, cur_addr);
        return;
    } else {
        off_t free_block_addr = ctl_block->free_block_head;
        // printf("free_block_addr: %ld\n", free_block_addr);
        if(free_block_addr != -1) {
            HashMapBlock* free_map_block = (HashMapBlock*)get_page(pool, free_block_addr);
            ctl_block->free_block_head = free_map_block->next;
            free_map_block->n_items = 1;
            free_map_block->table[0] = addr;
            free_map_block->next = -1;
            *cur_ptr = free_block_addr;
            release(pool, free_block_addr);
            release(pool, 0);
            release(pool, cur_addr);
            return;
        } else {
            HashMapBlock new_map_block;
            new_map_block.n_items = 1;
            new_map_block.table[0] = addr;
            new_map_block.next = -1;
            // off_t new_addr = PAGE_SIZE * (ctl_block->n_directory_blocks + ctl_block->max_size + 1);
            write_page((Page*)&new_map_block, &(pool->file), new_addr);
            *cur_ptr = new_addr;
            ctl_block->max_size++;
            release(pool, 0);
            release(pool, cur_addr);
            return;
        }
    }
}

off_t hash_table_pop_lower_bound(BufferPool *pool, short size) {
    if(size < 0 || size >= PAGE_SIZE) {
        // printf("size is out of range in hash_table_pop_lower_bound\n");
        return -1;
    }
    HashMapControlBlock* ctl_block = (HashMapControlBlock*)get_page(pool, 0);
    off_t start_dir = size / HASH_MAP_DIR_BLOCK_SIZE;
    short cur_size = size;
    for(auto i = start_dir; i < ctl_block->n_directory_blocks; i ++) {
        HashMapDirectoryBlock* dir_block = (HashMapDirectoryBlock*)get_page(pool, PAGE_SIZE * (i + 1));
        for(auto j = (i == start_dir ? size % HASH_MAP_DIR_BLOCK_SIZE : 0); j < HASH_MAP_DIR_BLOCK_SIZE; j ++) {
            off_t* cur_ptr = &dir_block->directory[j];
            if(*cur_ptr != -1) {
                HashMapBlock* tmp_block = (HashMapBlock*)get_page(pool, *cur_ptr);
                off_t ret = tmp_block->table[tmp_block->n_items - 1];
                release(pool, *cur_ptr);
                release(pool, 0);
                release(pool, PAGE_SIZE * (i + 1));
                hash_table_pop(pool, cur_size, ret);
                return ret;
            }
            cur_size ++;
            if(cur_size >= PAGE_SIZE) {
                release(pool, 0);
                release(pool, PAGE_SIZE * (i + 1));
                return -1;
            }
        }
        release(pool, PAGE_SIZE * (i + 1));
    }
    release(pool, 0);
    return -1;
}

void hash_table_pop(BufferPool *pool, short size, off_t addr) {
    if(size < 0 || size >= PAGE_SIZE) {
        // printf("size is out of range in hash_table_pop\n");
        return;
    }
    HashMapControlBlock* ctl_block = (HashMapControlBlock*)get_page(pool, 0);
    off_t cur_addr = PAGE_SIZE * ((size / HASH_MAP_DIR_BLOCK_SIZE) + 1);
    Page* cur_page = get_page(pool, cur_addr);
    off_t* cur_ptr = &(((HashMapDirectoryBlock*)cur_page)->directory[size % HASH_MAP_DIR_BLOCK_SIZE]);
    for(off_t* i = cur_ptr; *i != -1; ){
        HashMapBlock* tmp_block = (HashMapBlock*)get_page(pool, *i);
        for(auto j = 0; j < tmp_block->n_items; j ++) {
            if(tmp_block->table[j] == addr) {
                if(tmp_block->n_items == 1) {
                    
                    off_t tmp_next = tmp_block->next;
                    tmp_block->next = ctl_block->free_block_head;
                    tmp_block->n_items = 0;
                    ctl_block->free_block_head = *i;
                    release(pool, *i);
                    *i = tmp_next;

                    release(pool, 0);
                    release(pool, cur_addr);
                    return;
                } else {
                    memmove(tmp_block->table + j, tmp_block->table + j + 1, (tmp_block->n_items - j - 1) * sizeof(off_t));
                    tmp_block->n_items --;
                    release(pool, *i);
                    release(pool, 0);
                    release(pool, cur_addr);
                    return;
                }

            }
        }
        release(pool, *i);
        i = &tmp_block->next;
    }
    release(pool, 0);
    release(pool, cur_addr);
    return;
}

//  void print_hash_table(BufferPool *pool) {
//     HashMapControlBlock *ctrl = (HashMapControlBlock*)get_page(pool, 0);
//     HashMapDirectoryBlock *dir_block;
//     off_t block_addr, next_addr;
//     HashMapBlock *block;
//     int i, j;
//     printf("----------HASH TABLE----------\n");
//     for (i = 0; i < ctrl->n_directory_blocks; ++i) {
//         dir_block = (HashMapDirectoryBlock*)get_page(pool, (i + 1) * PAGE_SIZE);
//         for(int k=0;k<HASH_MAP_DIR_BLOCK_SIZE;++k){
//             if (dir_block->directory[k % HASH_MAP_DIR_BLOCK_SIZE] != -1) {
//                 printf("%d %d:", i,i*HASH_MAP_DIR_BLOCK_SIZE+k);
//                 block_addr = dir_block->directory[k % HASH_MAP_DIR_BLOCK_SIZE];
//                 while (block_addr != -1) {
//                     block = (HashMapBlock*)get_page(pool, block_addr);
//                     printf("  [" FORMAT_OFF_T "]", block_addr);
//                     printf("{");
//                     for (j = 0; j < block->n_items; ++j) {
//                         if (j != 0) {
//                             printf(", ");
//                         }
//                         printf(FORMAT_OFF_T, block->table[j]);
//                     }
//                     printf("}");
//                     next_addr = block->next;
//                     release(pool, block_addr);
//                     block_addr = next_addr;
//                 }
//                 printf("\n");
//             }
//         }
        
//         release(pool, (i + 1) * PAGE_SIZE);
//     }
//     release(pool, 0);
//     printf("------------------------------\n");
// } 