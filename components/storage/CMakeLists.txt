set(COMPONENT_REQUIRES
	vfs
	fatfs
)

set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_SRCS
	storage.cpp
)

register_component()

# fatfs_create_spiflash_image(
fatfs_create_partition_image(
	storage
	${CMAKE_SOURCE_DIR}/partitions/storage
	FLASH_IN_PROJECT
	PRESERVE_TIME
)

