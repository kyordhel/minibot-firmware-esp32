set(COMPONENT_REQUIRES
	esp_wifi
	nvs_flash
	settings
	storage
)

# set(COMPONENT_PRIV_REQUIRES
# )

set(COMPONENT_SRCS
	"wifi.cpp"
)

set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
