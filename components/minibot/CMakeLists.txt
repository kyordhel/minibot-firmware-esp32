# idf_component_register(
#     SRCS ""
#     INCLUDE_DIRS "."
#     REQUIRES
# )

set(COMPONENT_REQUIRES
	esp_driver_i2c
)

# set(COMPONENT_PRIV_REQUIRES
# )

set(COMPONENT_SRCS
	"i2c.cpp"
	"base.cpp"
	"sensors.cpp"
	"encoders.cpp"
	"minibot.cpp"
)

set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
