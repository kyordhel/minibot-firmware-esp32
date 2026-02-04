#pragma once

#include <esp_vfs.h>
#include <esp_vfs_fat.h>

namespace minibot::storage{

void init();
void realpath(char* realpath, const char* file);

}