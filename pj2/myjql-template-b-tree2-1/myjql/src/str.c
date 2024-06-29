#include "str.h"

#include "table.h"
#include <string.h>

void read_string(Table *table, RID rid, StringRecord *record) {
    table_read(table, rid, (ItemPtr)(&(record->chunk)));
    record->idx = 0;
}

int has_next_char(StringRecord *record) {
    StringChunk *chunk = &(record->chunk);
    if(record->idx >= get_str_chunk_size(chunk)) {
        if(get_rid_block_addr(get_str_chunk_rid(chunk)) == (off_t)-1) {
            return 0;
        }
    }
    return 1;
}

char next_char(Table *table, StringRecord *record) {
    StringChunk* chunk = &(record->chunk);
    char res;
    if(record->idx < get_str_chunk_size(chunk)) {
        res = get_str_chunk_data_ptr(chunk)[record->idx];
        record->idx++;
    } else {
        read_string(table, get_str_chunk_rid(chunk), record);
        res = get_str_chunk_data_ptr(chunk)[record->idx];
        record->idx++;
    }
    return res;
}

int compare_string_record(Table *table, const StringRecord *a, const StringRecord *b) {
        StringRecord sra = *a;
    StringRecord srb = *b;
    while(1){
        int has_nextchar_a = has_next_char(&sra);
        int has_nextchar_b = has_next_char(&srb);
        if(has_nextchar_a && has_nextchar_b){
            char a_c = next_char(table,&sra);
            char b_c = next_char(table,&srb);
            if(a_c > b_c) return 1;
            else if(a_c < b_c) return -1;
            else continue;
        }
        else if(has_nextchar_a && !has_nextchar_b){
            return 1;
        }
        else if(!has_nextchar_a && has_nextchar_b){
            return -1;
        }
        else return 0;
    }
}

RID write_string(Table *table, const char *data, off_t size) {
    RID first_rid, next_rid;
    get_rid_block_addr(first_rid) = (off_t)(-1);
    get_rid_idx(first_rid) = (short)(0);

    if (size == 0) {
        StringChunk newchunk;
        get_str_chunk_rid(&newchunk) = first_rid;
        get_str_chunk_size(&newchunk) = 0;
        first_rid = table_insert(table, (ItemPtr)&newchunk, calc_str_chunk_size(0));
        return first_rid;
    }

    int rest = size % STR_CHUNK_MAX_LEN;
    off_t num_chunks = size / STR_CHUNK_MAX_LEN;

    if (rest != 0) {
        StringChunk newchunk;
        get_str_chunk_rid(&newchunk) = first_rid;
        get_str_chunk_size(&newchunk) = rest;
        memmove((get_str_chunk_data_ptr(&newchunk)), data + (size - rest), rest);
        next_rid = table_insert(table, (ItemPtr)&newchunk, (short)calc_str_chunk_size(rest));
        size -= rest;
        first_rid = next_rid;
    }

    for (int i = 0; i < num_chunks; i++) {
        StringChunk newchunk;
        get_str_chunk_rid(&newchunk) = first_rid;
        get_str_chunk_size(&newchunk) = STR_CHUNK_MAX_LEN;
        memmove((get_str_chunk_data_ptr(&newchunk)), data + (size - STR_CHUNK_MAX_LEN), STR_CHUNK_MAX_LEN);
        next_rid = table_insert(table, (ItemPtr)&newchunk, (short)calc_str_chunk_size(STR_CHUNK_MAX_LEN));
        size -= STR_CHUNK_MAX_LEN;
        first_rid = next_rid; 
    }

    return first_rid;
}

void delete_string(Table *table, RID rid) {
    RID nxt_rid = rid;
    while (get_rid_block_addr(nxt_rid) != (off_t)-1) {
        StringChunk chunk;
        table_read(table, nxt_rid, (ItemPtr)&chunk);
        table_delete(table, nxt_rid);
        nxt_rid = get_str_chunk_rid(&chunk);
    }
}

/* void print_string(Table *table, const StringRecord *record) {
    StringRecord rec = *record;
    printf("\"");
    while (has_next_char(&rec)) {
        printf("%c", next_char(table, &rec));
    }
    printf("\"");
} */

size_t load_string(Table *table, const StringRecord *record, char *dest, size_t max_size) {
    size_t cnt = 0;
    StringRecord cpy = *record;
    while(has_next_char(&cpy) && cnt < max_size) {
        dest[cnt++] = next_char(table, &cpy);
    }
    return cnt;
}

/* void chunk_printer(ItemPtr item, short item_size) {
    if (item == NULL) {
        printf("NULL");
        return;
    }
    StringChunk *chunk = (StringChunk*)item;
    short size = get_str_chunk_size(chunk), i;
    printf("StringChunk(");
    print_rid(get_str_chunk_rid(chunk));
    printf(", %d, \"", size);
    for (i = 0; i < size; i++) {
        printf("%c", get_str_chunk_data_ptr(chunk)[i]);
    }
    printf("\")");
} */

int compare_string_string_record(Table *table, char *a, size_t size, const StringRecord *b) {
    int i = 0;
    StringRecord srb = *b;
    while(1){
        int has_nextchar_a = (i < size) ? 1 : 0;
        int has_nextchar_b = has_next_char(&srb);
        if(has_nextchar_a && has_nextchar_b){
            char a_c = a[i++];
            char b_c = next_char(table,&srb);
            if(a_c > b_c) return 1;
            else if(a_c < b_c) return -1;
            else continue;
        }
        else if(has_nextchar_a && !has_nextchar_b){
            return 1;
        }
        else if(!has_nextchar_a && has_nextchar_b){
            return -1;
        }
        else return 0;
    }
}