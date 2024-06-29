#include "table.h"

#include "hash_map.h"

#include <stdio.h>
#include <string.h>

void table_init(Table *table, const char *data_filename, const char *fsm_filename) {
    init_buffer_pool(data_filename, &table->data_pool);
    hash_table_init(fsm_filename, &table->fsm_pool, PAGE_SIZE / HASH_MAP_DIR_BLOCK_SIZE);
}

void table_close(Table *table) {
    close_buffer_pool(&table->data_pool);
    hash_table_close(&table->fsm_pool);
}

off_t table_get_total_blocks(Table *table) {
    return table->data_pool.file.length / PAGE_SIZE;
}

short table_block_get_total_items(Table *table, off_t block_addr) {
    Block *block = (Block*)get_page(&table->data_pool, block_addr);
    short n_items = block->n_items;
    release(&table->data_pool, block_addr);
    return n_items;
}

void table_read(Table *table, RID rid, ItemPtr dest) {
    off_t block_addr = get_rid_block_addr(rid);
    short idx = get_rid_idx(rid);
    Page* block_page = get_page(&(table->data_pool), block_addr);
    Block* block = (Block*)block_page;
    ItemPtr res_item_ptr = get_item(block, idx);
    ItemID res_item_id = get_item_id(block, idx);
    short res_item_size = get_item_id_size(res_item_id);
    memmove(dest, res_item_ptr, res_item_size);
    release(&(table->data_pool), block_addr);
}

RID table_insert(Table *table, ItemPtr src, short size) {
    RID res_rid;
    off_t res_addr = hash_table_pop_lower_bound(&(table->fsm_pool), size);
    short res_idx;
    short free_size = -1;
    if(res_addr != -1){
        Page* block_page = get_page(&(table->data_pool), res_addr);
        Block* block = (Block*)block_page;
        res_idx = new_item(block, src, size);
        for(auto i = 0; i < block->n_items; i ++) {
            ItemID cur_item_id = get_item_id(block, i);
            if(get_item_id_availability(cur_item_id)) {
                free_size = block->tail_ptr - block->head_ptr;
                break;
            }
        }
        if(free_size == -1) {
            free_size = block->tail_ptr - block->head_ptr - sizeof(ItemID);
        }
        release(&(table->data_pool), res_addr);
    } else {
        res_addr = table->data_pool.file.length;
        Block block;
        init_block(&block);
        res_idx = new_item(&block, src, size);
        write_page((Page*)(&block), &(table->data_pool.file), res_addr);
        free_size = block.tail_ptr - block.head_ptr - sizeof(ItemID);
        if(free_size) {
            hash_table_insert(&(table->fsm_pool), free_size, res_addr);
        }
    }
    get_rid_block_addr(res_rid) = res_addr;
    get_rid_idx(res_rid) = res_idx;
    return res_rid;
}

void table_delete(Table *table, RID rid) {
    off_t block_addr = get_rid_block_addr(rid);
    short idx = get_rid_idx(rid);
    Page* block_page = get_page(&(table->data_pool), block_addr);
    Block* block = (Block*)block_page;
    short free_size = -1;
    for(auto i = 0; i < block->n_items; i ++) {
        ItemID cur_item_id = get_item_id(block, i);
        if(get_item_id_availability(cur_item_id)) {
            free_size = block->tail_ptr - block->head_ptr;
            break;
        }
    }
    if(free_size == -1) {
        free_size = block->tail_ptr - block->head_ptr - sizeof(ItemID);
    }
    hash_table_pop(&(table->fsm_pool), free_size, block_addr);
    delete_item(block, idx);
    free_size = -1;
    for(auto i = 0; i < block->n_items; i ++) {
        ItemID cur_item_id = get_item_id(block, i);
        if(get_item_id_availability(cur_item_id)) {
            free_size = block->tail_ptr - block->head_ptr;
            break;
        }
    }
    if(free_size == -1) {
        free_size = block->tail_ptr - block->head_ptr - sizeof(ItemID);
    }
    release(&(table->data_pool), block_addr);
    hash_table_insert(&(table->fsm_pool), free_size, block_addr);
}

/* void print_table(Table *table, printer_t printer) {
    printf("\n---------------TABLE---------------\n");
    off_t i, total = table_get_total_blocks(table);
    off_t block_addr;
    Block *block;
    for (i = 0; i < total; ++i) {
        block_addr = i * PAGE_SIZE;
        block = (Block*)get_page(&table->data_pool, block_addr);
        printf("[" FORMAT_OFF_T "]\n", block_addr);
        print_block(block, printer);
        release(&table->data_pool, block_addr);
    }
    printf("***********************************\n");
    print_hash_table(&table->fsm_pool);
    printf("-----------------------------------\n\n");
} */

void print_rid(RID rid) {
    printf("RID(" FORMAT_OFF_T ", %d)", get_rid_block_addr(rid), get_rid_idx(rid));
}

/* void analyze_table(Table *table) {
    block_stat_t stat, curr;
    off_t i, total = table_get_total_blocks(table);
    off_t block_addr;
    Block *block;
    stat.empty_item_ids = 0;
    stat.total_item_ids = 0;
    stat.available_space = 0;
    for (i = 0; i < total; ++i) {
        block_addr = i * PAGE_SIZE;
        block = (Block*)get_page(&table->data_pool, block_addr);
        analyze_block(block, &curr);
        release(&table->data_pool, block_addr);
        accumulate_stat_info(&stat, &curr);
    }
    printf("++++++++++ANALYSIS++++++++++\n");
    printf("total blocks: " FORMAT_OFF_T "\n", total);
    total *= PAGE_SIZE;
    printf("total size: " FORMAT_OFF_T "\n", total);
    printf("occupancy: %.4f\n", 1. - 1. * stat.available_space / total);
    printf("ItemID occupancy: %.4f\n", 1. - 1. * stat.empty_item_ids / stat.total_item_ids);
    printf("++++++++++++++++++++++++++++\n\n");
} */