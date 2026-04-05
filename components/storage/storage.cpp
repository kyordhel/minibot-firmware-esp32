#include "storage.h"
#include <cstring>
#include <esp_err.h>

namespace minibot::storage{

// Mount path for the partition
static const char *base_path = "/storage";

static const char* TAG = "[STOR]";
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

static esp_err_t mount_rw();
static esp_err_t mount_ro();

void init(){
	esp_err_t err;
	if( (err = mount_rw()) == ESP_OK ) return;
	ESP_LOGW(TAG, "Failed to mount FATFS in read/write mode (%s)", esp_err_to_name(err));
	if( (err = mount_ro()) == ESP_OK )
		ESP_LOGW(TAG, "FATFS mounted in read-only mode");
	else
		ESP_LOGE(TAG, "Failed to mount FATFS in read-only mode (%s)", esp_err_to_name(err));
}


void realpath(char* realpath, const char* file){
	sprintf(realpath, "%s/%s", base_path, file);
}

static esp_err_t mount_rw(){
    const
    esp_vfs_fat_mount_config_t mount_config = {
		.format_if_mount_failed = false,   // If true, try to format the partition if mount fails
		.max_files = 5,                    // Number of files that can be open at a time
		.allocation_unit_size = CONFIG_WL_SECTOR_SIZE, // Size of allocation unit, cluster size.
		.disk_status_check_enable = false, // Check errors on external SD volumes
		.use_one_fat = false,              // Use only one FAT table (reduce memory usage), but decrease reliability of file system in case of power failure.
	};

	return esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage", &mount_config, &s_wl_handle);
}


static esp_err_t mount_ro(){
    const
    esp_vfs_fat_mount_config_t mount_config = {
		.format_if_mount_failed = false,   // If true, try to format the partition if mount fails
		.max_files = 5,                    // Number of files that can be open at a time
		.allocation_unit_size = CONFIG_WL_SECTOR_SIZE, // Size of allocation unit, cluster size.
		.disk_status_check_enable = false, // Check errors on external SD volumes
		.use_one_fat = false,              // Use only one FAT table (reduce memory usage), but decrease reliability of file system in case of power failure.
	};

	return esp_vfs_fat_spiflash_mount_ro(base_path, "storage", &mount_config);
}

}