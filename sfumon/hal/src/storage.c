#include "storage.h"
#include <stdio.h>
#include <string.h>

static char base_path[256] = "./";

bool storage_init(const char* path) {
    if (path != NULL) {
        strncpy(base_path, path, sizeof(base_path) - 1);
        base_path[sizeof(base_path) - 1] = '\0';
    }
    printf("Storage: Initialized with base path '%s'\n", base_path);
    return true;
}

void storage_cleanup(void) {
    printf("Storage: Cleanup\n");
}

bool storage_write_file(const char* filename, const void* data, size_t size) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, filename);
    
    FILE* file = fopen(full_path, "wb");
    if (!file) {
        printf("Storage: Failed to open '%s' for writing\n", full_path);
        return false;
    }
    
    size_t written = fwrite(data, 1, size, file);
    fclose(file);
    
    if (written != size) {
        printf("Storage: Write error for '%s'\n", full_path);
        return false;
    }
    
    printf("Storage: Wrote %zu bytes to '%s'\n", size, full_path);
    return true;
}

bool storage_read_file(const char* filename, void* buffer, size_t max_size, size_t* bytes_read) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, filename);
    
    FILE* file = fopen(full_path, "rb");
    if (!file) {
        printf("Storage: Failed to open '%s' for reading\n", full_path);
        return false;
    }
    
    size_t read = fread(buffer, 1, max_size, file);
    fclose(file);
    
    if (bytes_read) {
        *bytes_read = read;
    }
    
    printf("Storage: Read %zu bytes from '%s'\n", read, full_path);
    return true;
}

bool storage_file_exists(const char* filename) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, filename);
    
    FILE* file = fopen(full_path, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

bool storage_delete_file(const char* filename) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, filename);
    
    if (remove(full_path) == 0) {
        printf("Storage: Deleted '%s'\n", full_path);
        return true;
    }
    
    printf("Storage: Failed to delete '%s'\n", full_path);
    return false;
}

long storage_get_file_size(const char* filename) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, filename);
    
    FILE* file = fopen(full_path, "rb");
    if (!file) {
        return -1;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    
    return size;
}