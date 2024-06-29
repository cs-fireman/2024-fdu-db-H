```c
-----------file_io.h-------------------
PAGE_SIZE 128
long long off_t;
struct {
    FILE *fp;  /* file pointer */
    off_t length;  /* file length */
} FileInfo;
struct {
  char data[PAGE_SIZE];  /* placeholder only */
} Page;
enum {
    FILE_IO_SUCCESS,
    FILE_IO_FAILED,
    INVALID_LEN,  /* file length is not a multiple of PAGE_SIZE */
    INVALID_ADDR,  /* address is not a multiple of PAGE_SIZE */
    ADDR_OUT_OF_RANGE,
} FileIOResult;
FileIOResult open_file(FileInfo *file, const char *filename);
如果存在，打开，否则创建。指针file的fp和length都赋值。返回IOResult。
    
FileIOResult close_file(FileInfo *file);
//关闭文件。
    
FileIOResult read_page(Page *page, const FileInfo *file, off_t addr);
从file的addr处读一个Page，读到的内容用指针page指向。
    
FileIOResult write_page(const Page *page, FileInfo *file, off_t addr);
在file的addr处写一个Page，内容为指针page指向的内容。
```

以上是一个文件的结构，每个文件对应一个缓冲池（可以理解为cache）

```c
------------buffer_pool.h--------------
CACHE_PAGE 8
struct {
  FileInfo file;           // 文件信息
  Page pages[CACHE_PAGE];  // 缓冲的页面  8个
  off_t addrs[CACHE_PAGE]; // 每个缓冲的页面在文件中的偏移
  size_t cnt[CACHE_PAGE];  // 每个缓冲页面的访问计数
  size_t ref[CACHE_PAGE];  // 每个缓冲页面的引用计数
} BufferPool;
void init_buffer_pool(const char *filename, BufferPool *pool);
//对于一个文件名进行初始化
打开（创建)文件 read_page。
初始化缓冲池的各个信息：
    file指针指向刚刚open_page的第一个参数
    addr = -1
    cnt, ref = 0
    
void close_buffer_pool(BufferPool *pool);
关掉缓冲区的时候把其中的所有信息都写入pool->file里面 write_page。
关掉文件 close_file。
    
Page *get_page(BufferPool *pool, off_t addr);
把pool中偏移为addr的页找出来，否则从文件中找出来放到pool中，并return。
LRU：替换cnt最大的。
每次访问，所有页的年龄（cnt）++，如果在pool中，这页的cnt清零，ref++。
否则先找到所有ref为0的，
对其遍历找到cnt最大的，
替换 write_page 先把缓冲区中的写入文件
    然后 read_page 的第一个参数是刚刚找到的要替换的pool->page[x]。
返回结果。

void release(BufferPool *pool, off_t addr);
所有的引用计数--。
```

---

一个数据文件由若干个块（页）构成，每个块由Header, ItemId, Item构成.

```C
------------block.h--------------
struct {
    /* header section */
    short n_items;  /* number of allocated ItemPtr's */
    short head_ptr;  /* free space begin */
    short tail_ptr;  /* free space end */
    /* ItemID section */
    /* Item section */
    char data[PAGE_SIZE - 3 * sizeof(short)];  /* placeholder only */
} Block;
/*
 * ItemID: 32-bit unsigned int
 * 31-th bit: not used, always be 0
 * 30-th bit: availability (0: unavailable (because it has been used); 1: available (for future use))
 * 29~15 bit: offset
 * 14~ 0 bit: size
 */
typedef unsigned int ItemID;
typedef char *ItemPtr;
#define get_item_id_availability(item_id) (((item_id) >> 30) & 1)
#define get_item_id_offset(item_id) (((item_id) >> 15) & ((1 << 15) - 1))
#define get_item_id_size(item_id) ((item_id) & ((1 << 15) - 1))

#define compose_item_id(availability, offset, size) ((((availability) & 1) << 30) | (((offset) & ((1 << 15) - 1)) << 15) | ((size) & ((1 << 15) - 1)))

#define get_item_id(block, idx) (*(ItemID*)((block)->data + sizeof(ItemID) * (idx)))

void init_block(Block *block);
//初始化块
    
/* idx should not be out of range */
ItemPtr get_item(Block *block, short idx);
//返回block第idx项Item的起始地址

/* there should be enough space */
short new_item(Block *block, ItemPtr item, short item_size);
//向block插入大小为item_size，起始地址为item的Item，返回插入后的项号（idx）
先找有没有unused的ItemId，如果有的话head_ptr不用变，tail_ptr -= item_size。
否则head-ptr += sizeof(ItemId)，tail_ptr -= item_size。
设置好ItemId的值，把item memcpy 到 tail_ptr那里。
    
/* idx should not be out of range */
void delete_item(Block *block, short idx);
//删除block第idx项Item
ItemId如果在末尾: head_ptr -= sizeof(ItemId), block->n_items --
否则把availability置1。
把原来tail_ptr开始的大小为 offset - tail_ptr 的内存
移动（memmove）到tail_ptr + size处，
同时修改之前的所有ItemId的偏移值。
最后tail_ptr += size。
```

一个文件有128个块，想要知道这些块里面空闲空间的情况，就需要hash_map来管理。

第0块是控制块，第1~n_directory_blocks块是哈希表目录块，剩下是哈希表块。

```C
-------------hash_map.h----------------
struct {
    // 控制块
    off_t free_block_head;  // 空闲块链表头指针（的偏移）
    off_t n_directory_blocks; 
    // 第1~n_directory_blocks块是哈希表目录块
    // 构成哈希表目录，按空闲空间大小分类保存块的地址。
    off_t max_size; // number of hashmapblock
} HashMapControlBlock;

#define HASH_MAP_DIR_BLOCK_SIZE (PAGE_SIZE / sizeof(off_t))
struct {
    // 哈希表目录块：一个包含目录项的数组
    off_t directory[HASH_MAP_DIR_BLOCK_SIZE];
} HashMapDirectoryBlock;

#define HASH_MAP_BLOCK_SIZE ((PAGE_SIZE - 2 * sizeof(off_t)) / sizeof(off_t))
struct {
    // 哈希表块，以链表形式组织，保存哈希表块链表头指针。
    off_t next; // 链表中下一个哈希块的偏移
    off_t n_items; // 当前哈希块中的项数
    off_t table[HASH_MAP_BLOCK_SIZE]; // 哈希表，存的是addr（偏移）
} HashMapBlock;
/* if hash table has already existed, it will be re-opened and n_directory_blocks will be ignored */
void hash_table_init(const char *filename, BufferPool *pool, off_t n_directory_blocks);
如果哈希表已经存在（文件长度不为0），忽略。
否则初始化（将相应的变量写到文件中去）：
    控制块：比较常用，可以放在缓冲区中。
    哈希表目录块：值全为-1
    哈希表块：最后一块指向-1
    
void hash_table_close(BufferPool *pool);
关掉缓冲池
    
/* there should not be no duplicate addr */
void hash_table_insert(BufferPool *pool, short size, off_t addr);
// 标记地址为addr的块有size的空闲空间
先看有没有对应size的目录块，
如果有：
    找到没有满的哈希表块，在这个块的table数组末尾加一个addr，n_items ++；
    如果都满了，新建一个哈希表块，插在开头，n_items = 1，table[0] = addr，
    控制块的max_size ++，新的块信息写到文件中。
如果没有：
    从空闲块链表中获取一个空闲哈希块，更新空闲块链表头，设置哈希块的第一个数据项，
    并将其地址存储到目录项中；
    如果没有空闲哈希块，分配一个新的哈希块，设置其第一个数据项，并将其地址存储到目录	项中，同时更新控制块中的 max_size。

/* if there is no suitable block, return -1 */
off_t hash_table_pop_lower_bound(BufferPool *pool, short size);
// 返回至少包含size空闲空间的块地址
从对应size的位置开始遍历每个哈希表块，如果有就返回table的最后一项，然后pop。
否则返回-1。

/* addr to be poped must exist */
void hash_table_pop(BufferPool *pool, short size, off_t addr);
// 删除地址为addr的块的记录，该块有size的空闲空间
相当于链表的删除，注意如果哈希表块删完了，需要更新空闲链表和目录块指向的链表。
```

**新建指针的时候可能会是NULL，不妨新建一个变量，在变量上取地址**

**如果要在一个变量本来的地方修改它，用指针或者解引用指针** 

---

保存一个表需要有两个文件，数据文件和空闲空间映射文件。

同时操作之前的哈希表和数据块，table提供了对两个文件的更高级的抽象。

RID结构体表示记录的标识符，包含两个部分：记录所在块的地址和记录在块中的索引。

RID存储在Item里面。

```C
--------table.h--------
struct {
    BufferPool data_pool;// 用于存储数据的缓冲池
    BufferPool fsm_pool;// 用于自由空间管理的缓冲池
} Table;
typedef struct {
    /* off_t block_addr */
    /* short idx */
    char data[sizeof(off_t) + sizeof(short)];  /* 10 bytes */
} RID;  /* record id */
#define get_rid_block_addr(rid) (*(off_t*)(&(rid)))
#define get_rid_idx(rid) (*(short*)(((char*)(&(rid))) + sizeof(off_t)))
void table_init(Table *table, const char *data_filename, const char *fsm_filename);
//初始化表，创建数据和空闲空间的缓冲区。

void table_close(Table *table);
//关掉缓冲区。

off_t table_get_total_blocks(Table *table);
//返回数据文件的总块数。

short table_block_get_total_items(Table *table, off_t block_addr);
//返回数据文件偏移为addr的块的总项数

void table_read(Table *table, RID rid, ItemPtr dest);
//根据rid，将数据读入dest，需要确保dest拥有适当的大小
根据rid找到对应block中的相应的项，将它memmove到dest。

RID table_insert(Table *table, ItemPtr src, short size);
//插入大小为size，起始地址为src的Item，返回RID。
要求空闲空间至少为size，用hash_table_pop_lower_bound找到对应的块。
如果找到了：就new_item到块中，并且hash_insert；
否则：新建一个块，new_item，写入文件，并且hash_insert。
    
void table_delete(Table *table, RID rid);
//根据rid，删除相应Item
找到相应的块的相应的项，删除，hash_pop，hash_insert
```

---

文件中的每一个block（页）中的Item，就字符串而言是一个StringChunk

包含了rid, size, data（变长）。

每个字符串是分块存储的，string_record记录了一个string的分块存储情

况。

```C
--------------str.h---------------
#define STR_CHUNK_MAX_SIZE ((PAGE_SIZE) / 4)
#define STR_CHUNK_MAX_LEN (STR_CHUNK_MAX_SIZE - sizeof(RID) - sizeof(short))

struct {
    /*
     * strings are split into chunks organized in singly linked list
     * RID rid: next chunk
     * short size: size of the current chunk
     * char data[]: chunk data (of variable length)
     */
    char data[STR_CHUNK_MAX_SIZE];  /* placeholder only */
} StringChunk;

#define get_str_chunk_rid(chunk) (*(RID*)(chunk))
#define get_str_chunk_size(chunk) (*(short*)(((char*)(chunk)) + sizeof(RID)))
#define get_str_chunk_data_ptr(chunk) (((char*)(chunk)) + sizeof(RID) + sizeof(short))
#define calc_str_chunk_size(len) ((len) + sizeof(RID) + sizeof(short))

struct {
    /* you can ADD anything in this struct */
    StringChunk chunk;  /* current chunk */
    short idx;  /* current idx */
} StringRecord;

void read_string(Table *table, RID rid, StringRecord *record);
//根据rid，读取字符串至record
相当于根据rid去table_read, idx初始化为0, 表示下一个字符是第一个字符。

int has_next_char(StringRecord *record);
//判断record是否还有下一个字符
如果下一个字符的位置比当前chunk的长度长，并且下一块的地址为-1，那么没有下一个字符，否则有。(true为1, false为0)

char next_char(Table *table, StringRecord *record);
//获取record的下一个字符
如果下一个字符的位置在这个chunk内，读并且idx++。
否则read_string下一个chunk，读并且idx++。

int compare_string_record(Table *table, const StringRecord *a, const StringRecord *b);

RID write_string(Table *table, const char *data, off_t size);
//将长度为size的字符串data写入table表，并返回rid
将字符串分割成多个chunk，然后逐个插入到表中。

void delete_string(Table *table, RID rid);
//根据rid，删除对应字符串
不断读下一个rid，table_read，table_delete

/* load up to max_size chars to dest from record, the number of loaded chars are returned.
   note '\0' should be appended manually  */
size_t load_string(Table *table, const StringRecord *record, char *dest, size_t max_size);
如果has_next_char，就把next_char放在dest指针的地方，dest++，cnt++
```
B-树

第0块为控制块，包含根节点地址与空闲块链表头指针。

其余块为B-树节点块。

```C
------b_tree.h-------
#define DEGREE (((PAGE_SIZE) - sizeof(size_t) - sizeof(off_t) - sizeof(char) + sizeof(RID)) / 2 / (sizeof(off_t) + sizeof(RID)))

struct {
  size_t n;                      // 当前节点中存储的键数量
  off_t next;                    // 下一个节点的地址（用于链表结构）
  off_t child[2 * DEGREE];       // 孩子节点的地址数组
  RID row_ptr[2 * DEGREE - 1];   // 存储键的指针数组
  char leaf;                     // 是否是叶子节点（1表示是叶子节点，0表示不是，f表示空闲节点）
} BNode;

struct {
  off_t root_node;         // 根节点的地址
  off_t free_node_head;    // 空闲节点链表的头节点地址
  off_t max_size;          // 新增：空闲节点数量
} BCtrlBlock;

void b_tree_init(const char *filename, BufferPool *pool);
//初始化
初始化控制块，以及max_size个空闲块。

RID b_tree_search(BufferPool *pool, void *key, size_t size, b_tree_ptr_row_cmp_t cmp);
// 根据key搜索RID，并且要符合cmp
先获取根节点信息，然后在B-树中递归搜索得到节点。在这个节点的row_ptr数组中搜索符合cmp的值，有就返回rid，没有就返回(-1,0)的rid。
```







