#pragma once
#include <stdint.h>

#define RAMFS_MAX_FILES 32
#define RAMFS_NAME_LEN  32
#define RAMFS_FILE_SIZE 4096  // per file

void      ramfs_init(void);
int       ramfs_create(const char* name);
int       ramfs_write(const char* name, const char* data, uint32_t len);
int       ramfs_read(const char* name, char* out, uint32_t max);
void      ramfs_list(void);

// serialize whole FS into a flat image
uint32_t  ramfs_serialize(uint8_t* buf, uint32_t max);
uint32_t  ramfs_deserialize(const uint8_t* buf, uint32_t len);
