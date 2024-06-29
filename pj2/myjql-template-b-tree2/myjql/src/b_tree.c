#include "b_tree.h"
#include "buffer_pool.h"

#include <stdio.h>
#include <string.h>

void b_tree_init(const char *filename, BufferPool *pool) {
    init_buffer_pool(filename, pool);
    /* TODO: add code here */
    if(pool->file.length) return;
    BCtrlBlock *ctl_block = (BCtrlBlock *)get_page(pool, 0);
    ctl_block->root_node = PAGE_SIZE;
    ctl_block->free_node_head = PAGE_SIZE * 2;
    ctl_block->max_size = 16;
    write_page((Page*)ctl_block, &(pool->file), 0);

    BNode *root_block = (BNode *)get_page(pool, ctl_block->root_node);
    root_block->n = 0;
    root_block->next = (off_t)-1;
    root_block->leaf = '1';
    write_page((Page*)root_block, &(pool->file), PAGE_SIZE);
    release(pool, PAGE_SIZE);

    for(auto i = 0; i < ctl_block->max_size; i++){
        off_t cur_addr = ctl_block->free_node_head + i * PAGE_SIZE;
        BNode *free_block = (BNode *)get_page(pool, cur_addr);
        free_block->leaf = 'f';
        free_block->n = 0;
        if(i == ctl_block->max_size - 1){
            free_block->next = (off_t)-1;
        } else {
            free_block->next = ctl_block->free_node_head + (i + 1) * PAGE_SIZE;
        }
        write_page((Page*)free_block, &(pool->file), cur_addr);
        release(pool, cur_addr);
    }
    release(pool, 0);
}

void b_tree_close(BufferPool *pool) {
    close_buffer_pool(pool);
}

off_t inside_search(BufferPool *pool, void *key, size_t size, off_t node_off, b_tree_ptr_row_cmp_t cmp) {
    while (1) {
        BNode *nodeptr = (BNode*)get_page(pool, node_off);
        if (nodeptr->leaf == '1') {
            release(pool, node_off);
            return node_off;
        }

        if (cmp(key, size, nodeptr->row_ptr[0]) < 0) {
            off_t temp = nodeptr->child[0];
            release(pool, node_off);
            node_off = temp;
        } else if (cmp(key, size, nodeptr->row_ptr[nodeptr->n - 1]) >= 0) {
            off_t temp = nodeptr->child[nodeptr->n];
            release(pool, node_off);
            node_off = temp;
        } else {
            for (int i = 0; i < nodeptr->n - 1; ++i) {
                if (cmp(key, size, nodeptr->row_ptr[i]) >= 0 && cmp(key, size, nodeptr->row_ptr[i + 1]) < 0) {
                    off_t temp = nodeptr->child[i + 1];
                    release(pool, node_off);
                    node_off = temp;
                    break;
                }
            }
        }
    }
}


RID b_tree_search(BufferPool *pool, void *key, size_t size, b_tree_ptr_row_cmp_t cmp) {
    BCtrlBlock *ctrlblock = (BCtrlBlock*)get_page(pool,0);
    off_t root_off = ctrlblock->root_node;
    release(pool,0);
    off_t find_node_off = inside_search(pool,key,size,root_off,cmp);
    BNode *find_node = (BNode*)get_page(pool,find_node_off);
    int i = 0;  
    for(;i<find_node->n;++i){
        if(cmp(key,size,find_node->row_ptr[i]) == 0) break;
    }
    if(i<find_node->n){
        RID temp = find_node->row_ptr[i];
        release(pool,find_node_off);
        return temp;
    }
    else{
        RID res;
        release(pool,find_node_off);
        get_rid_block_addr(res) = -1;
        get_rid_idx(res) = 0;
        return res;
    }
}

typedef struct {
    RID *newkey_rid;
    off_t *node_off;
} newchildentry_t;

void inside_insert(BufferPool *pool, off_t node_offset, RID rid, newchildentry_t new_child_entry, b_tree_row_row_cmp_t cmp, b_tree_insert_nonleaf_handler_t insert_handler) {
    BNode *current_node = (BNode*)get_page(pool, node_offset);

    if (current_node->leaf == '1') {
        if (current_node->n < 2 * DEGREE) {
            int i;
            for (i = 0; i < current_node->n; ++i) {
                if (cmp(rid, current_node->row_ptr[i]) < 0) {
                    break;
                }
            }
            if (i != current_node->n) {
                memmove(current_node->row_ptr + i + 1, current_node->row_ptr + i, (current_node->n - i) * sizeof(RID));
            }
            current_node->row_ptr[i] = rid;
            ++current_node->n;
            get_rid_block_addr(*(new_child_entry.newkey_rid)) = -1;
        } else {
            BCtrlBlock *control_block = (BCtrlBlock*)get_page(pool, 0);
            off_t free_block_offset = control_block->free_node_head;
            BNode *new_leaf;

            if (free_block_offset != -1) {
                new_leaf = (BNode*)get_page(pool, free_block_offset);
                control_block->free_node_head = new_leaf->next;
            } else {
                free_block_offset = PAGE_SIZE * (control_block->max_size + 2);
                new_leaf = (BNode*)get_page(pool, free_block_offset);
                write_page((const Page*)new_leaf, &pool->file, free_block_offset);
                ++control_block->max_size;
            }

            new_leaf->next = current_node->next;
            current_node->next = free_block_offset;
            new_leaf->leaf = '1';
            new_leaf->n = DEGREE + 1;

            int i;
            for (i = 0; i < current_node->n; ++i) {
                if (cmp(rid, current_node->row_ptr[i]) < 0) {
                    break;
                }
            }
            if (i < DEGREE) {
                memcpy(new_leaf->row_ptr, current_node->row_ptr + DEGREE - 1, (DEGREE + 1) * sizeof(RID));
                memmove(current_node->row_ptr + i + 1, current_node->row_ptr + i, (DEGREE - i - 1) * sizeof(RID));
                current_node->row_ptr[i] = rid;
            } else {
                memcpy(new_leaf->row_ptr, current_node->row_ptr + DEGREE, (i - DEGREE) * sizeof(RID));
                memcpy(new_leaf->row_ptr + i - DEGREE + 1, current_node->row_ptr + i, (2 * DEGREE - i) * sizeof(RID));
                new_leaf->row_ptr[i - DEGREE] = rid;
            }

            current_node->n = DEGREE;
            *(new_child_entry.newkey_rid) = new_leaf->row_ptr[0];
            *(new_child_entry.node_off) = current_node->next;
            release(pool, current_node->next);

            if (node_offset == control_block->root_node) {
                BNode *new_root;
                off_t free_block_offset1 = control_block->free_node_head;

                if (free_block_offset1 != -1) {
                    new_root = (BNode*)get_page(pool, free_block_offset1);
                    control_block->free_node_head = new_root->next;
                } else {
                    free_block_offset1 = PAGE_SIZE * (control_block->max_size + 2);
                    new_root = (BNode*)get_page(pool, free_block_offset1);
                    write_page((const Page*)new_root, &pool->file, free_block_offset1);
                    ++control_block->max_size;
                }

                control_block->root_node = free_block_offset1;
                new_root->n = 1;
                new_root->leaf = '0';
                new_root->next = -1;
                new_root->row_ptr[0] = insert_handler(*(new_child_entry.newkey_rid));
                new_root->child[0] = node_offset;
                new_root->child[1] = *(new_child_entry.node_off);

                release(pool, control_block->root_node);
            }
            release(pool, 0);
        }
        release(pool, node_offset);
        return;
    } else {
        if (cmp(rid, current_node->row_ptr[0]) < 0) {
            off_t temp = current_node->child[0];
            release(pool, node_offset);
            inside_insert(pool, temp, rid, new_child_entry, cmp, insert_handler);
        } else if (cmp(rid, current_node->row_ptr[current_node->n - 1]) >= 0) {
            off_t temp = current_node->child[current_node->n];
            release(pool, node_offset);
            inside_insert(pool, temp, rid, new_child_entry, cmp, insert_handler);
        } else {
            for (int i = 0; i < current_node->n - 1; ++i) {
                if (cmp(rid, current_node->row_ptr[i]) >= 0 && cmp(rid, current_node->row_ptr[i + 1]) < 0) {
                    off_t temp = current_node->child[i + 1];
                    release(pool, node_offset);
                    inside_insert(pool, temp, rid, new_child_entry, cmp, insert_handler);
                    break;
                }
            }
        }

        if (get_rid_block_addr(*(new_child_entry.newkey_rid)) == -1) {
            return;
        } else {
            current_node = (BNode*)get_page(pool, node_offset);

            if (current_node->n < 2 * DEGREE) {
                int i;
                for (i = 0; i < current_node->n; ++i) {
                    if (cmp(*(new_child_entry.newkey_rid), current_node->row_ptr[i]) < 0) {
                        break;
                    }
                }
                if (i != current_node->n) {
                    memmove(current_node->child + i + 2, current_node->child + i + 1, (current_node->n - i) * sizeof(off_t));
                    memmove(current_node->row_ptr + i + 1, current_node->row_ptr + i, (current_node->n - i) * sizeof(RID));
                }
                current_node->child[i + 1] = *(new_child_entry.node_off);
                current_node->row_ptr[i] = insert_handler(*(new_child_entry.newkey_rid));
                ++current_node->n;

                get_rid_block_addr(*(new_child_entry.newkey_rid)) = -1;
            } else { 
                BCtrlBlock *control_block = (BCtrlBlock*)get_page(pool, 0);
                off_t free_block_offset = control_block->free_node_head;
                BNode *new_non_leaf;

                if (free_block_offset != -1) {
                    new_non_leaf = (BNode*)get_page(pool, free_block_offset);
                    control_block->free_node_head = new_non_leaf->next;
                } else {
                    free_block_offset = PAGE_SIZE * (control_block->max_size + 2);
                    new_non_leaf = (BNode*)get_page(pool, free_block_offset);
                    write_page((const Page*)new_non_leaf, &pool->file, free_block_offset);
                    ++control_block->max_size;
                }

                new_non_leaf->next = current_node->next;
                current_node->next = free_block_offset;
                new_non_leaf->leaf = '0';
                new_non_leaf->n = DEGREE;

                int i;
                for (i = 0; i < current_node->n; ++i) {
                    if (cmp(*(new_child_entry.newkey_rid), current_node->row_ptr[i]) < 0) {
                        break;
                    }
                }
                if (i == DEGREE) {
                    memcpy(new_non_leaf->child + 1, current_node->child + DEGREE + 1, DEGREE * sizeof(off_t));
                    memcpy(new_non_leaf->row_ptr, current_node->row_ptr + DEGREE, DEGREE * sizeof(RID));
                    new_non_leaf->child[0] = *(new_child_entry.node_off);
                    current_node->n = DEGREE;
                    *(new_child_entry.node_off) = current_node->next;
                } else if (i < DEGREE) {
                    memcpy(new_non_leaf->child, current_node->child + DEGREE, (DEGREE + 1) * sizeof(off_t));
                    memcpy(new_non_leaf->row_ptr, current_node->row_ptr + DEGREE, DEGREE * sizeof(RID));
                    memmove(current_node->row_ptr + i + 1, current_node->row_ptr + i, (DEGREE - i) * sizeof(RID));
                    memmove(current_node->child + i + 2, current_node->child + i + 1, (DEGREE - i - 1) * sizeof(off_t));
                    current_node->child[i + 1] = *(new_child_entry.node_off);
                    current_node->row_ptr[i] = insert_handler(*(new_child_entry.newkey_rid));
                    *(new_child_entry.newkey_rid) = current_node->row_ptr[DEGREE];
                    *(new_child_entry.node_off) = current_node->next;
                    current_node->n = DEGREE;
                } else {
                    for (int j = 0; j < DEGREE; ++j) {
                        if (j + DEGREE + 1 < i) new_non_leaf->row_ptr[j] = current_node->row_ptr[j + DEGREE + 1];
                        else if (j + DEGREE + 1 == i) new_non_leaf->row_ptr[j] = insert_handler(*(new_child_entry.newkey_rid));
                        else new_non_leaf->row_ptr[j] = current_node->row_ptr[j + DEGREE];
                    }
                    for (int j = 0; j < DEGREE + 1; ++j) {
                        if (j + DEGREE < i) new_non_leaf->child[j] = current_node->child[j + DEGREE + 1];
                        else if (j + DEGREE == i) new_non_leaf->child[j] = *(new_child_entry.node_off);
                        else new_non_leaf->child[j] = current_node->child[j + DEGREE];
                    }
                    *(new_child_entry.newkey_rid) = current_node->row_ptr[DEGREE];
                    *(new_child_entry.node_off) = current_node->next;
                    current_node->n = DEGREE;
                }
                release(pool, current_node->next);

                if (node_offset == control_block->root_node) {
                    BNode *new_root;
                    off_t free_block_offset1 = control_block->free_node_head;

                    if (free_block_offset1 != -1) {
                        new_root = (BNode*)get_page(pool, free_block_offset1);
                        control_block->free_node_head = new_root->next;
                    } else {
                        free_block_offset1 = PAGE_SIZE * (control_block->max_size + 2);
                        new_root = (BNode*)get_page(pool, free_block_offset1);
                        write_page((const Page*)new_root, &pool->file, free_block_offset1);
                        ++control_block->max_size;
                    }

                    control_block->root_node = free_block_offset1;
                    new_root->n = 1;
                    new_root->leaf = '0';
                    new_root->next = -1;
                    new_root->row_ptr[0] = insert_handler(*(new_child_entry.newkey_rid));
                    new_root->child[0] = node_offset;
                    new_root->child[1] = *(new_child_entry.node_off);
                    release(pool, control_block->root_node);
                }
                release(pool, 0);
            }
            release(pool, node_offset);
            return;
        }
    }
}

RID b_tree_insert(BufferPool *pool, RID rid, b_tree_row_row_cmp_t cmp, b_tree_insert_nonleaf_handler_t insert_handler) {
    BCtrlBlock *ctrl_block = (BCtrlBlock*)get_page(pool, 0);
    off_t root_offset = ctrl_block->root_node;
    release(pool, 0);

    newchildentry_t new_child_entry;
    RID new_key_rid;
    get_rid_block_addr(new_key_rid) = -1;
    off_t new_node_offset = -1;
    new_child_entry.newkey_rid = &new_key_rid;
    new_child_entry.node_off = &new_node_offset;

    inside_insert(pool, root_offset, rid, new_child_entry, cmp, insert_handler);

    if (get_rid_block_addr(new_key_rid) != -1) {
        BNode *new_root;
        off_t free_block_offset;

        ctrl_block = (BCtrlBlock*)get_page(pool, 0);
        free_block_offset = ctrl_block->free_node_head;
        if (free_block_offset != -1) {
            new_root = (BNode*)get_page(pool, free_block_offset);
            ctrl_block->free_node_head = new_root->next;
        } else {
            free_block_offset = PAGE_SIZE * (ctrl_block->max_size + 2);
            new_root = (BNode*)get_page(pool, free_block_offset);
            write_page((const Page*)new_root, &pool->file, free_block_offset);
            ++ctrl_block->max_size;
        }

        new_root->n = 1;
        new_root->leaf = '0';
        new_root->next = -1;
        new_root->row_ptr[0] = insert_handler(new_key_rid);
        new_root->child[0] = root_offset;
        new_root->child[1] = new_node_offset;
        ctrl_block->root_node = free_block_offset;

        write_page((const Page*)ctrl_block, &pool->file, 0);
        release(pool, 0);
        release(pool, free_block_offset);
    }

    return rid;
}

void inside_delete(BufferPool *pool, off_t parent_node_off, off_t node_off, RID rid, RID *oldchildentry, b_tree_row_row_cmp_t cmp, b_tree_insert_nonleaf_handler_t insert_handler, b_tree_delete_nonleaf_handler_t delete_handler){
    BNode *nodeptr = (BNode*)get_page(pool,node_off);
    if(nodeptr->leaf == '1'){
        if(nodeptr->n > DEGREE){
            int i = 0;
            for(;i<nodeptr->n;++i){
                if(cmp(nodeptr->row_ptr[i],rid) == 0){
                    break;
                }
            }
            memmove(nodeptr->row_ptr + i, nodeptr->row_ptr + i + 1, (nodeptr->n-i-1)*sizeof(RID));
            --nodeptr->n;

            get_rid_block_addr(*oldchildentry) = -1;
        }
        else{
            int k = 0;
            for(;k<nodeptr->n;++k){
                if(cmp(nodeptr->row_ptr[k],rid) == 0){
                    break;
                }
            }
            memmove(nodeptr->row_ptr + k, nodeptr->row_ptr + k + 1, (nodeptr->n-k-1)*sizeof(RID));
            --nodeptr->n;
            BCtrlBlock *ctrlblock = (BCtrlBlock*)get_page(pool,0);
            if(node_off == ctrlblock->root_node){
                
            }
            else{
                BNode *parent_node = (BNode*)get_page(pool,parent_node_off);
                int i = 0;
                for(;i<=parent_node->n;++i){
                    if(parent_node->child[i] == node_off) break;
                }
                off_t sibling_off = (i == 0) ? parent_node->child[1] : parent_node->child[i-1];
                BNode *sibling_node = (BNode*)get_page(pool,sibling_off);

                if(sibling_node->n > DEGREE){
                    if(i == 0){
                        ++nodeptr->n;
                        nodeptr->row_ptr[nodeptr->n-1] = sibling_node->row_ptr[0];
                        delete_handler(parent_node->row_ptr[0]);
                        parent_node->row_ptr[0] = insert_handler(sibling_node->row_ptr[1]);
                        --sibling_node->n;
                        memmove(sibling_node->row_ptr, sibling_node->row_ptr + 1, (sibling_node->n)*sizeof(RID));
                    }
                    else{
                        memmove(nodeptr->row_ptr + 1, nodeptr->row_ptr, (nodeptr->n)*sizeof(RID));
                        ++nodeptr->n;
                        nodeptr->row_ptr[0] = sibling_node->row_ptr[sibling_node->n-1];
                        delete_handler(parent_node->row_ptr[i-1]);
                        parent_node->row_ptr[i-1] = insert_handler(sibling_node->row_ptr[sibling_node->n-1]);
                        --sibling_node->n;
                    }
                    get_rid_block_addr(*oldchildentry) = -1;
                }
                else{
                    if(i==0){
                        *oldchildentry = parent_node->row_ptr[0];
                        memcpy(nodeptr->row_ptr + nodeptr->n, sibling_node->row_ptr, (sibling_node->n)*sizeof(RID));
                        nodeptr->n += sibling_node->n;
                        nodeptr->next = sibling_node->next;
                        sibling_node->next = ctrlblock->free_node_head;
                        ctrlblock->free_node_head = sibling_off;
                    }
                    else{
                        *oldchildentry = parent_node->row_ptr[i-1];

                        memcpy(sibling_node->row_ptr + sibling_node->n, nodeptr->row_ptr, (nodeptr->n)*sizeof(RID));
                        sibling_node->n += nodeptr->n;
                        sibling_node->next = nodeptr->next;
                        nodeptr->next = ctrlblock->free_node_head;
                        ctrlblock->free_node_head = node_off;
                    }
                }
                release(pool,sibling_off);
                release(pool,parent_node_off);
            }
            release(pool,0);
        }
        release(pool,node_off);
        return;
    }
    else{
        if(cmp(rid, nodeptr->row_ptr[0]) < 0){
            off_t temp = nodeptr->child[0];
            release(pool,node_off);
            inside_delete(pool,node_off,temp,rid,oldchildentry,cmp,insert_handler,delete_handler);
        }
        else if(cmp(rid, nodeptr->row_ptr[nodeptr->n-1]) >= 0){
            off_t temp = nodeptr->child[nodeptr->n];
            release(pool,node_off);
            inside_delete(pool,node_off,temp,rid,oldchildentry,cmp,insert_handler,delete_handler);
        }
        else{
            for(int i=0;i<nodeptr->n-1;++i){
                if(cmp(rid, nodeptr->row_ptr[i]) >= 0 && cmp(rid, nodeptr->row_ptr[i+1]) < 0){
                    off_t temp = nodeptr->child[i+1];
                    release(pool,node_off);
                    inside_delete(pool,node_off,temp,rid,oldchildentry,cmp,insert_handler,delete_handler);
                    break;
                }
            }
        }
        if(get_rid_block_addr(*oldchildentry) == -1){
            return;
        }
        else{
            nodeptr = (BNode*)get_page(pool,node_off);
            int i = 0;
            for(;i<nodeptr->n;++i){
                if(cmp(nodeptr->row_ptr[i],*oldchildentry) == 0){
                    break;
                }
            }
            delete_handler(nodeptr->row_ptr[i]);
            memmove(nodeptr->row_ptr + i, nodeptr->row_ptr + i + 1, (nodeptr->n - i - 1)*sizeof(RID));
            memmove(nodeptr->child + i + 1, nodeptr->child + i + 2, (nodeptr->n - i - 1)*sizeof(off_t));
            --nodeptr->n;
            if(nodeptr->n >= DEGREE){
                get_rid_block_addr(*oldchildentry) = -1;
            }
            else{
                BCtrlBlock *ctrlblock = (BCtrlBlock*)get_page(pool,0);
                if(node_off == ctrlblock->root_node){
                    if(nodeptr->n == 0){
                        ctrlblock->root_node = nodeptr->child[0];
                        nodeptr->next = ctrlblock->free_node_head;
                        ctrlblock->free_node_head = node_off;
                    }
                }
                else{
                    BNode *parent_node = (BNode*)get_page(pool,parent_node_off);
                    int i = 0;
                    for(;i<=parent_node->n;++i){
                        if(parent_node->child[i] == node_off) break;
                    }
                    off_t sibling_off = (i == 0) ? parent_node->child[1] : parent_node->child[i-1];
                    BNode *sibling_node = (BNode*)get_page(pool,sibling_off);
                    if(sibling_node->n > DEGREE){
                        if(i == 0){
                            ++nodeptr->n;
                            nodeptr->row_ptr[nodeptr->n-1] = parent_node->row_ptr[0];
                            parent_node->row_ptr[0] = sibling_node->row_ptr[0];
                            nodeptr->child[nodeptr->n] = sibling_node->child[0];
                            --sibling_node->n;
                            memmove(sibling_node->row_ptr, sibling_node->row_ptr + 1, (sibling_node->n)*sizeof(RID));
                            memmove(sibling_node->child, sibling_node->child + 1, (sibling_node->n+1)*sizeof(off_t));
                        }
                        else{
                            memmove(nodeptr->row_ptr + 1, nodeptr->row_ptr, nodeptr->n*sizeof(RID));
                            memmove(nodeptr->child + 1, nodeptr->child, (nodeptr->n+1)*sizeof(off_t));
                            ++nodeptr->n;
                            nodeptr->row_ptr[0] = parent_node->row_ptr[i-1];
                            parent_node->row_ptr[i-1] = sibling_node->row_ptr[sibling_node->n-1];
                            nodeptr->child[0] = sibling_node->child[sibling_node->n];
                            --sibling_node->n;
                        }
                        get_rid_block_addr(*oldchildentry) = -1;
                    }
                    else{
                        if(i==0){
                            *oldchildentry = parent_node->row_ptr[0];
                            nodeptr->row_ptr[nodeptr->n] = insert_handler(parent_node->row_ptr[0]);
                            memcpy(nodeptr->row_ptr + nodeptr->n + 1, sibling_node->row_ptr, (sibling_node->n)*sizeof(RID));
                            memcpy(nodeptr->child + nodeptr->n + 1, sibling_node->child, (sibling_node->n+1)*sizeof(off_t));
                            nodeptr->n += (1 + sibling_node->n);
                            sibling_node->next = ctrlblock->free_node_head;
                            ctrlblock->free_node_head = sibling_off;
                        }
                        else{
                            *oldchildentry = parent_node->row_ptr[i-1];
                            sibling_node->row_ptr[sibling_node->n] = insert_handler(parent_node->row_ptr[i-1]);
                            memcpy(sibling_node->row_ptr + sibling_node->n + 1, nodeptr->row_ptr, (nodeptr->n)*sizeof(RID));
                            memcpy(sibling_node->child + sibling_node->n + 1, nodeptr->child, (nodeptr->n+1)*sizeof(off_t));
                            sibling_node->n += (1 + nodeptr->n);
                            nodeptr->next = ctrlblock->free_node_head;
                            ctrlblock->free_node_head = node_off;
                        }
                    }
                    release(pool,sibling_off);
                    release(pool,parent_node_off);
                }
                release(pool,0);
            }
            release(pool,node_off);
            return;
        }
    }
}

void b_tree_delete(BufferPool *pool, RID rid, b_tree_row_row_cmp_t cmp, b_tree_insert_nonleaf_handler_t insert_handler, b_tree_delete_nonleaf_handler_t delete_handler) {
    BCtrlBlock *ctrlblock = (BCtrlBlock*)get_page(pool, 0);
    off_t root_off = ctrlblock->root_node;
    release(pool, 0);

    RID oldchildentry;
    get_rid_block_addr(oldchildentry) = -1;

    inside_delete(pool, -1, root_off, rid, &oldchildentry, cmp, insert_handler, delete_handler);

    ctrlblock = (BCtrlBlock*)get_page(pool, 0);
    BNode *root_node = (BNode*)get_page(pool, root_off);

    if (root_node->n == 0 && !root_node->leaf) {
        ctrlblock->root_node = root_node->child[0];
        release(pool, root_off);
        root_node->next = ctrlblock->free_node_head;
        ctrlblock->free_node_head = root_off;
        release(pool, 0);
    } else {
        release(pool, root_off);
        release(pool, 0);
    }
}
