#pragma once
#include <stdint.h>

#define NFS_MAX_ENTRIES 128
#define NFS_NAME_LEN    32

enum nf_type {
    NF_FILE = 1,
    NF_DIR  = 2
};

void     nfs_init(void);

int      nfs_mkdir(const char* path);
int      nfs_create(const char* path);
int      nfs_write(const char* path, const void* data, uint32_t len);
int      nfs_read(const char* path, void* out, uint32_t max);
void     nfs_list(const char* path);
int nfs_remove(const char* path);

uint32_t nfs_serialize(uint8_t* buf, uint32_t max);
uint32_t nfs_deserialize(const uint8_t* buf, uint32_t len);
