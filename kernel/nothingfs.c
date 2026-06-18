#include "nothingfs.h"
#include "kprint.h"
#include "mm.h"

typedef struct nf_entry {
    char     name[NFS_NAME_LEN];
    uint8_t  type;          // NF_FILE or NF_DIR
    uint32_t size;          // file: bytes, dir: child count
    uint32_t parent;
    uint32_t first_child;
    uint32_t next_sibling;
    uint8_t* data;          // only for files
    uint8_t  used;
} nf_entry_t;

static nf_entry_t nfs_entries[NFS_MAX_ENTRIES];

static int nfs_alloc_entry(void);
static int nfs_lookup(const char* path);
static int nfs_find_child(int dir_idx, const char* name);
static void nfs_link_child(int parent, int child);
static int nfs_split_parent(const char* path, char* name, char* parent_path);

static void name_copy(char* dst, const char* src) {
    int i = 0;
    for (; i < NFS_NAME_LEN - 1 && src[i]; i++)
        dst[i] = src[i];
    dst[i] = 0;
}

static int name_eq(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

static const char* skip_slashes(const char* p) {
    while (*p == '/') p++;
    return p;
}

void nfs_init(void) {
    for (int i = 0; i < NFS_MAX_ENTRIES; i++) {
        nfs_entries[i].used = 0;
        nfs_entries[i].data = 0;
        nfs_entries[i].size = 0;
        nfs_entries[i].parent = (uint32_t)-1;
        nfs_entries[i].first_child = (uint32_t)-1;
        nfs_entries[i].next_sibling = (uint32_t)-1;
        nfs_entries[i].name[0] = 0;
    }

    // root directory at index 0
    nfs_entries[0].used = 1;
    nfs_entries[0].type = NF_DIR;
    nfs_entries[0].size = 0;
    nfs_entries[0].parent = (uint32_t)-1;
    nfs_entries[0].first_child = (uint32_t)-1;
    name_copy(nfs_entries[0].name, "/");
}

static int nfs_alloc_entry(void) {
    for (int i = 0; i < NFS_MAX_ENTRIES; i++) {
        if (!nfs_entries[i].used) {
            nfs_entries[i].used = 1;
            nfs_entries[i].size = 0;
            nfs_entries[i].first_child = (uint32_t)-1;
            nfs_entries[i].next_sibling = (uint32_t)-1;
            nfs_entries[i].data = 0;
            return i;
        }
    }
    return -1;
}

static int nfs_find_child(int dir_idx, const char* name) {
    uint32_t child = nfs_entries[dir_idx].first_child;
    while (child != (uint32_t)-1) {
        if (name_eq(nfs_entries[child].name, name))
            return child;
        child = nfs_entries[child].next_sibling;
    }
    return -1;
}

static void nfs_link_child(int parent, int child) {
    nf_entry_t* p = &nfs_entries[parent];
    nf_entry_t* c = &nfs_entries[child];

    c->parent = parent;
    c->next_sibling = (uint32_t)-1;

    if (p->first_child == (uint32_t)-1) {
        p->first_child = child;
    } else {
        uint32_t cur = p->first_child;
        while (nfs_entries[cur].next_sibling != (uint32_t)-1)
            cur = nfs_entries[cur].next_sibling;
        nfs_entries[cur].next_sibling = child;
    }

    p->size++;
}

static int nfs_lookup_internal(int start, const char* path) {
    const char* p = skip_slashes(path);
    int cur = start;

    if (*p == 0) return cur;

    char name[NFS_NAME_LEN];

    while (*p) {
        int i = 0;
        while (*p && *p != '/' && i < NFS_NAME_LEN - 1)
            name[i++] = *p++;
        name[i] = 0;

        p = skip_slashes(p);

        int child = nfs_find_child(cur, name);
        if (child < 0) return -1;
        cur = child;
    }

    return cur;
}

static int nfs_lookup(const char* path) {
    if (!path || path[0] != '/') return -1;
    return nfs_lookup_internal(0, path);
}

static int nfs_split_parent(const char* path, char* name, char* parent_path) {
    int len = 0;
    while (path[len]) len++;

    int last_slash = -1;
    for (int i = 0; i < len; i++)
        if (path[i] == '/') last_slash = i;

    if (last_slash <= 0) {
        parent_path[0] = '/';
        parent_path[1] = 0;
    } else {
        int j = 0;
        for (int i = 0; i < last_slash; i++)
            parent_path[j++] = path[i];
        parent_path[j] = 0;
    }

    int ni = 0;
    for (int i = last_slash + 1; i < len && ni < NFS_NAME_LEN - 1; i++)
        name[ni++] = path[i];
    name[ni] = 0;

    return name[0] ? 0 : -1;
}

int nfs_mkdir(const char* path) {
    char name[NFS_NAME_LEN];
    char parent_path[256];

    if (nfs_split_parent(path, name, parent_path) < 0)
        return -1;

    int parent = nfs_lookup(parent_path);
    if (parent < 0) return -2;

    if (nfs_find_child(parent, name) >= 0)
        return -3;

    int idx = nfs_alloc_entry();
    if (idx < 0) return -4;

    nf_entry_t* e = &nfs_entries[idx];
    e->type = NF_DIR;
    name_copy(e->name, name);

    nfs_link_child(parent, idx);
    return 0;
}

int nfs_create(const char* path) {
    char name[NFS_NAME_LEN];
    char parent_path[256];

    if (nfs_split_parent(path, name, parent_path) < 0)
        return -1;

    int parent = nfs_lookup(parent_path);
    if (parent < 0) return -2;

    if (nfs_find_child(parent, name) >= 0)
        return -3;

    int idx = nfs_alloc_entry();
    if (idx < 0) return -4;

    nf_entry_t* e = &nfs_entries[idx];
    e->type = NF_FILE;
    name_copy(e->name, name);

    nfs_link_child(parent, idx);
    return 0;
}

int nfs_write(const char* path, const void* data, uint32_t len) {
    int idx = nfs_lookup(path);
    if (idx < 0) return -1;

    nf_entry_t* e = &nfs_entries[idx];
    if (e->type != NF_FILE) return -2;

    if (e->data) kfree(e->data);

    e->data = kmalloc(len);
    if (!e->data) return -3;

    for (uint32_t i = 0; i < len; i++)
        e->data[i] = ((uint8_t*)data)[i];

    e->size = len;
    return 0;
}

int nfs_read(const char* path, void* out, uint32_t max) {
    int idx = nfs_lookup(path);
    if (idx < 0) return -1;

    nf_entry_t* e = &nfs_entries[idx];
    if (e->type != NF_FILE) return -2;

    uint32_t n = e->size < max ? e->size : max;
    for (uint32_t i = 0; i < n; i++)
        ((uint8_t*)out)[i] = e->data[i];

    return n;
}

void nfs_list(const char* path) {
    int idx = path ? nfs_lookup(path) : 0;
    if (idx < 0) {
        kprint("No such directory.\n");
        return;
    }

    nf_entry_t* dir = &nfs_entries[idx];
    if (dir->type != NF_DIR) {
        kprint("Not a directory.\n");
        return;
    }

    uint32_t child = dir->first_child;
    while (child != (uint32_t)-1) {
        nf_entry_t* e = &nfs_entries[child];
        if (e->type == NF_DIR)
            kprintf("[D] %s\n", e->name);
        else
            kprintf("[F] %s (%d bytes)\n", e->name, e->size);
        child = e->next_sibling;
    }
}

typedef struct {
    char     name[NFS_NAME_LEN];
    uint8_t  type;
    uint32_t size;
    uint32_t parent;
    uint32_t first_child;
    uint32_t next_sibling;
    uint32_t data_offset;
} nfs_img_entry_t;

typedef struct {
    char     magic[4];
    uint32_t entry_count;
} nfs_img_header_t;

uint32_t nfs_serialize(uint8_t* buf, uint32_t max) {
    nfs_img_header_t* hdr = (nfs_img_header_t*)buf;
    uint32_t offset = sizeof(nfs_img_header_t);
    nfs_img_entry_t* img = (nfs_img_entry_t*)(buf + offset);

    uint32_t count = 0;

    for (int i = 0; i < NFS_MAX_ENTRIES; i++) {
        if (!nfs_entries[i].used) continue;

        if (offset + sizeof(nfs_img_entry_t) > max) break;

        nfs_img_entry_t* e = &img[count];
        name_copy(e->name, nfs_entries[i].name);
        e->type = nfs_entries[i].type;
        e->size = nfs_entries[i].size;
        e->parent = nfs_entries[i].parent;
        e->first_child = nfs_entries[i].first_child;
        e->next_sibling = nfs_entries[i].next_sibling;
        e->data_offset = 0;

        offset += sizeof(nfs_img_entry_t);
        count++;
    }

    uint32_t data_offset = offset;

    for (uint32_t i = 0; i < count; i++) {
        if (img[i].type != NF_FILE) continue;

        int idx = -1;
        for (int j = 0; j < NFS_MAX_ENTRIES; j++) {
            if (!nfs_entries[j].used) continue;
            if (nfs_entries[j].type != NF_FILE) continue;
            if (!name_eq(nfs_entries[j].name, img[i].name)) continue;
            if (nfs_entries[j].parent != img[i].parent) continue;
            idx = j;
            break;
        }

        if (idx < 0) continue;

        nf_entry_t* e = &nfs_entries[idx];
        if (!e->data || e->size == 0) continue;

        if (data_offset + e->size > max) break;

        img[i].data_offset = data_offset;

        for (uint32_t b = 0; b < e->size; b++)
            buf[data_offset + b] = e->data[b];

        data_offset += e->size;
    }

    hdr->magic[0] = 'N';
    hdr->magic[1] = 'F';
    hdr->magic[2] = 'S';
    hdr->magic[3] = '1';
    hdr->entry_count = count;

    return data_offset;
}

uint32_t nfs_deserialize(const uint8_t* buf, uint32_t len) {
    if (len < sizeof(nfs_img_header_t)) return 0;

    const nfs_img_header_t* hdr = (const nfs_img_header_t*)buf;
    if (hdr->magic[0] != 'N' || hdr->magic[1] != 'F' ||
        hdr->magic[2] != 'S' || hdr->magic[3] != '1')
        return 0;

    uint32_t count = hdr->entry_count;
    uint32_t offset = sizeof(nfs_img_header_t);
    uint32_t need = offset + count * sizeof(nfs_img_entry_t);
    if (need > len) return 0;

    const nfs_img_entry_t* img = (const nfs_img_entry_t*)(buf + offset);

    nfs_init();

    for (uint32_t i = 0; i < count && i < NFS_MAX_ENTRIES; i++) {
        int idx = nfs_alloc_entry();
        if (idx < 0) break;

        nf_entry_t* e = &nfs_entries[idx];
        name_copy(e->name, img[i].name);
        e->type = img[i].type;
        e->size = img[i].size;
        e->parent = img[i].parent;
        e->first_child = img[i].first_child;
        e->next_sibling = img[i].next_sibling;
    }

    for (uint32_t i = 0; i < count && i < NFS_MAX_ENTRIES; i++) {
        if (img[i].type != NF_FILE) continue;
        if (img[i].data_offset == 0 || img[i].size == 0) continue;
        if (img[i].data_offset + img[i].size > len) continue;

        nf_entry_t* e = &nfs_entries[i];
        e->data = kmalloc(img[i].size);
        if (!e->data) continue;

        for (uint32_t b = 0; b < img[i].size; b++)
            e->data[b] = buf[img[i].data_offset + b];
    }

    return len;
}
int nfs_remove(const char* path) {
    int idx = nfs_lookup(path);
    if (idx < 0) return -1;

    nf_entry_t* e = &nfs_entries[idx];

    // cannot delete root
    if (idx == 0) return -2;

    // if directory and not empty
    if (e->type == NF_DIR && e->first_child != (uint32_t)-1)
        return -3;

    // unlink from parent
    int parent = e->parent;
    nf_entry_t* p = &nfs_entries[parent];

    uint32_t* link = &p->first_child;
    while (*link != (uint32_t)-1) {
        if (*link == idx) {
            *link = e->next_sibling;
            break;
        }
        link = &nfs_entries[*link].next_sibling;
    }

    // free file data
    if (e->type == NF_FILE && e->data)
        kfree(e->data);

    e->used = 0;
    return 0;
}
