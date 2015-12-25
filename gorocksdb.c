#include "gorocksdb.h"
#include <stdio.h>
#include <sys/time.h>
#include "buflist.h"
#include "_cgo_export.h"

/* Base */

void gorocksdb_destruct_handler(void* state) { }

/* Comparator */

rocksdb_comparator_t* gorocksdb_comparator_create(void* state) {
    return rocksdb_comparator_create(
            state,
            gorocksdb_destruct_handler,
            (int (*)(void*, const char*, size_t, const char*, size_t))(gorocksdb_comparator_compare),
            (const char *(*)(void*))(gorocksdb_comparator_name));
}

/* CompactionFilter */

rocksdb_compactionfilter_t* gorocksdb_compactionfilter_create(void* state) {
    return rocksdb_compactionfilter_create(
            state,
            gorocksdb_destruct_handler,
            (unsigned char (*)(void*, int, const char*, size_t, const char*, size_t, char**, size_t*, unsigned char*))(gorocksdb_compactionfilter_filter),
            (const char *(*)(void*))(gorocksdb_comparator_name));
}

/* Filter Policy */

rocksdb_filterpolicy_t* gorocksdb_filterpolicy_create(void* state) {
    return rocksdb_filterpolicy_create(
            state,
            gorocksdb_destruct_handler,
            (char* (*)(void*, const char* const*, const size_t*, int, size_t*))(gorocksdb_filterpolicy_create_filter),
            (unsigned char (*)(void*, const char*, size_t, const char*, size_t))(gorocksdb_filterpolicy_key_may_match),
            gorocksdb_filterpolicy_delete_filter,
            (const char *(*)(void*))(gorocksdb_filterpolicy_name));
}

void gorocksdb_filterpolicy_delete_filter(void* state, const char* v, size_t s) { }

/* Merge Operator */

rocksdb_mergeoperator_t* gorocksdb_mergeoperator_create(void* state) {
    return rocksdb_mergeoperator_create(
            state,
            gorocksdb_destruct_handler,
            (char* (*)(void*, const char*, size_t, const char*, size_t, const char* const*, const size_t*, int, unsigned char*, size_t*))(gorocksdb_mergeoperator_full_merge),
            (char* (*)(void*, const char*, size_t, const char* const*, const size_t*, int, unsigned char*, size_t*))(gorocksdb_mergeoperator_partial_merge_multi),
            gorocksdb_mergeoperator_delete_value,
            (const char* (*)(void*))(gorocksdb_mergeoperator_name));
}

void gorocksdb_mergeoperator_delete_value(void* id, const char* v, size_t s) { }

/* Slice Transform */

rocksdb_slicetransform_t* gorocksdb_slicetransform_create(void* state) {
    return rocksdb_slicetransform_create(
            state,
            gorocksdb_destruct_handler,
            (char* (*)(void*, const char*, size_t, size_t*))(gorocksdb_slicetransform_transform),
            (unsigned char (*)(void*, const char*, size_t))(gorocksdb_slicetransform_in_domain),
            (unsigned char (*)(void*, const char*, size_t))(gorocksdb_slicetransform_in_range),
            (const char* (*)(void*))(gorocksdb_slicetransform_name));
}

// timing helper
static struct timeval tm1;
static inline void start() { gettimeofday(&tm1, NULL); }
static inline void stop()
{
    struct timeval tm2;
    gettimeofday(&tm2, NULL);
    unsigned long long t = 1000 * (tm2.tv_sec - tm1.tv_sec)
        + (tm2.tv_usec - tm1.tv_usec) / 1000;
    printf("%llu ms\n", t);
}

static inline void _dbg(const unsigned char* buf, size_t sz) {
    size_t i;
    for (i = 0;i < sz;i++) {
        printf("%02x ", (unsigned char)buf[i]);
    }
    printf("\n");
}

void multi_seek(void* iter, const unsigned char* buf, const size_t buf_sz,
        unsigned char** ret_key_buf, size_t* ret_key_sz,
        unsigned char** ret_val_buf, size_t* ret_val_sz) {
    rocksdb_iterator_t* it = (rocksdb_iterator_t*)iter;

    buflist_t* start_keys
        = buflist_new_from_buf(buf, (uint32_t)buf_sz);

    buflist_t* ret_keys = buflist_new();
    buflist_t* ret_vals = buflist_new();
    buflist_iter_t* sit = buflist_iter_new(start_keys);
    for (; buflist_iter_valid(sit); buflist_iter_next(sit)) {
        unsigned char* start_key;
        uint32_t sz = 0;
        buflist_iter_cur(sit, &start_key, &sz);

        rocksdb_iter_seek(it, (const char*)start_key, (size_t)sz);
        if (rocksdb_iter_valid(it)) {
            size_t key_sz = 0;
            size_t val_sz = 0;
            unsigned char* key = (unsigned char*)rocksdb_iter_key(it, &key_sz);
            unsigned char* val
                = (unsigned char*)rocksdb_iter_value(it, &val_sz);
            if (key != NULL && val != NULL) {
                buflist_push(ret_keys, key, key_sz);
                buflist_push(ret_vals, val, val_sz);
            }
        }
        else {
            unsigned char s[] = "\0";
            buflist_push(ret_keys, s, 1);
            buflist_push(ret_vals, s, 1);
        }
    }
    *ret_key_buf = ret_keys->buf;
    *ret_key_sz  = ret_keys->size;
    *ret_val_buf = ret_vals->buf;
    *ret_val_sz  = ret_vals->size;
}
