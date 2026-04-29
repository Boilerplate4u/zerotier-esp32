idf_component_register()
idf_component_get_property(zt_lib zerotier_node COMPONENT_LIB)
target_compile_definitions(${zt_lib} PRIVATE
    ZT_NETWORKCONFIG_DICT_CAPACITY_OVERRIDE=16384
    ZT_MAX_NETWORK_RULES=32
    ZT_MAX_NETWORK_CAPABILITIES=0
    ZT_MAX_NETWORK_TAGS=0
    ZT_MAX_CERTIFICATES_OF_OWNERSHIP=0
)
