file(GLOB sources LIST_DIRECTORIES false *.cpp)

# Every soure file in this directory is a excutable
foreach(src ${sources})
    get_filename_component(src_name ${src} NAME_WE)
    add_executable(${src_name} ${src_name}.cpp)
    target_compile_definitions(${src_name} PRIVATE APP_NAME="${src_name}")
endforeach(src sources)