#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>

// Initialize storage system (set base path for SD card)
bool storage_init(const char* base_path);

// Cleanup storage system
void storage_cleanup(void);

// File operations
bool storage_write_file(const char* filename, const void* data, size_t size);
bool storage_read_file(const char* filename, void* buffer, size_t max_size, size_t* bytes_read);
bool storage_file_exists(const char* filename);
bool storage_delete_file(const char* filename);
long storage_get_file_size(const char* filename);

#endif