set(COMPONENT_REQUIRES
	minibot
	settings
	storage
	tcpserverbase
)

set(COMPONENT_SRCS
	tcpserver.cpp
	reply.cpp
)

set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()