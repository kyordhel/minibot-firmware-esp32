# Edit following two lines to set component requirements (see docs)

# set(COMPONENT_REQUIRES )

set(COMPONENT_PRIV_REQUIRES
	minibot
	storage
	settings
	tcpserver
	webserver
	wifi
)

set(COMPONENT_SRCS
	"behaviors.cpp"
	"main.cpp"
)

set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()


# idf_component_register(
# 	SRCS "main.cpp"
# 	INCLUDE_DIRS "."
# 	REQUIRES ${COMPONENT_REQUIRES}
# 	PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
# )

