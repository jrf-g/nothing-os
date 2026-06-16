#include "ramfs.h"
#include "kprint.h"
#include "mm.h"   // for kmalloc

typedef struct {
    char     name[RAMFS_NAME_LEN];
    uint32_t size;
    uint8_t* data;
    uint8_t  used;
} ramfs_file_t;

static ramfs_file_t files[RAMFS_MAX_FILES];

void ramfs_init(void) {
    for (int i = 0; i < RAMFS_MAX_FILES; i++) {
        files[i].used = 0;
        files[i].data = 0;
        files[i].size = 0;
        files[i].name[0] = 0;
    }
}

static int name_eq(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

static void name_copy(char* dst, const char* src) {
    int i = 0;
    for (; i < RAMFS_NAME_LEN - 1 && src[i]; i++)
        dst[i] = src[i];
    dst[i] = 0;
}

static ramfs_file_t* find_file(const char* name) {
    for (int i = 0; i < RAMFS_MAX_FILES; i++) {
        if (files[i].used && name_eq(files[i].name, name))
            return &files[i];
    }
    return 0;
}

int ramfs_create(const char* name) {
    if (find_file(name))
        return -1; // already exists

    for (int i = 0; i < RAMFS_MAX_FILES; i++) {
        if (!files[i].used) {
            files[i].used = 1;
            files[i].size = 0;
            files[i].data = (uint8_t*)kmalloc(RAMFS_FILE_SIZE);
            if (!files[i].data) {
                files[i].used = 0;
                return -2;
            }
            name_copy(files[i].name, name);
            return 0;
        }
    }
    return -3; // full
}

int ramfs_write(const char* name, const char* data, uint32_t len) {
    ramfs_file_t* f = find_file(name);
    if (!f) return -1;
    if (len > RAMFS_FILE_SIZE) len = RAMFS_FILE_SIZE;
    for (uint32_t i = 0; i < len; i++)
        f->data[i] = (uint8_t)data[i];
    f->size = len;
    return 0;
}

int ramfs_read(const char* name, char* out, uint32_t max) {
    ramfs_file_t* f = find_file(name);
    if (!f) return -1;
    uint32_t n = f->size < max ? f->size : max;
    for (uint32_t i = 0; i < n; i++)
        out[i] = (char)f->data[i];
    return (int)n;
}

void ramfs_list(void) {
    kprint("RAMFS files:\n");
    for (int i = 0; i < RAMFS_MAX_FILES; i++) {
        if (files[i].used) {
            kprintf("  %s  (%d bytes)\n", files[i].name, (int)files[i].size);
        }
    }
}
typedef struct {
    char     name[RAMFS_NAME_LEN];
    uint32_t size;
} ramfs_header_entry_t;

typedef struct {
    uint32_t file_count;
    ramfs_header_entry_t entries[RAMFS_MAX_FILES];
    // followed by raw file data in same order
} ramfs_image_header_t;

uint32_t ramfs_serialize(uint8_t* buf, uint32_t max) {
    ramfs_image_header_t* hdr = (ramfs_image_header_t*)buf;
    uint32_t count = 0;
    uint32_t offset = sizeof(ramfs_image_header_t);

    for (int i = 0; i < RAMFS_MAX_FILES; i++) {
        if (!files[i].used) continue;
        if (offset + files[i].size > max) break;

        name_copy(hdr->entries[count].name, files[i].name);
        hdr->entries[count].size = files[i].size;

        for (uint32_t j = 0; j < files[i].size; j++)
            buf[offset + j] = files[i].data[j];

        offset += files[i].size;
        count++;
    }

    hdr->file_count = count;
    return offset; // total image size
}

uint32_t ramfs_deserialize(const uint8_t* buf, uint32_t len) {
    const ramfs_image_header_t* hdr = (const ramfs_image_header_t*)buf;
    uint32_t offset = sizeof(ramfs_image_header_t);

    ramfs_init();

    for (uint32_t i = 0; i < hdr->file_count; i++) {
        const char* name = hdr->entries[i].name;
        uint32_t size = hdr->entries[i].size;
        if (offset + size > len) break;

        if (ramfs_create(name) == 0) {
            ramfs_write(name, (const char*)(buf + offset), size);
        }
        offset += size;
    }

    return offset;
}
