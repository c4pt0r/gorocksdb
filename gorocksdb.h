#include <stdlib.h>
#include "rocksdb/c.h"

// This API provides convenient C wrapper functions for rocksdb client.

/* Base */

extern void gorocksdb_destruct_handler(void* state);

/* CompactionFilter */

extern rocksdb_compactionfilter_t* gorocksdb_compactionfilter_create(void* state);

/* Comparator */

extern rocksdb_comparator_t* gorocksdb_comparator_create(void* state);

/* Filter Policy */

extern rocksdb_filterpolicy_t* gorocksdb_filterpolicy_create(void* state);
extern void gorocksdb_filterpolicy_delete_filter(void* state, const char* v, size_t s);

/* Merge Operator */

extern rocksdb_mergeoperator_t* gorocksdb_mergeoperator_create(void* state);
extern void gorocksdb_mergeoperator_delete_value(void* state, const char* v, size_t s);

/* Slice Transform */

extern rocksdb_slicetransform_t* gorocksdb_slicetransform_create(void* state);

extern void multi_seek(void* iter, const unsigned char* buf, const size_t buf_sz,
        unsigned char** ret_key_buf, size_t* ret_key_sz,
        unsigned char** ret_val_buf, size_t* ret_val_sz);

