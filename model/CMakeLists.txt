file(GLOB sources LIST_DIRECTORIES false *.qmodel)
message("models are :" ${sources})
add_custom_target(models ${sources})
