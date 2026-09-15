set(LOCAL_LIB_NAME dbengine)

add_library(${LOCAL_LIB_NAME}  
  db_engine.cpp
)
  
add_library(data::dbengine-lib-static ALIAS ${LOCAL_LIB_NAME})

set_property(TARGET ${LOCAL_LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_features(${LOCAL_LIB_NAME} PUBLIC cxx_std_20)

target_include_directories(${LOCAL_LIB_NAME} PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
  )

target_link_libraries(${LOCAL_LIB_NAME} PUBLIC
  data::storage-lib-static
)
