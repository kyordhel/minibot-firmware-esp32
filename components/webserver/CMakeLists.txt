set(COMPONENT_REQUIRES
	minibot
	settings
	storage
	tcpserverbase
)

set(COMPONENT_SRCS
	"webserver.cpp"
	"helpers.cpp"
)

set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()