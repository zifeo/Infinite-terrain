
get_filename_component(PROJECTNAME icg_project NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

file(GLOB_RECURSE TEXTURES "*.tga")
copy_files_once(${TEXTURES})

file(GLOB_RECURSE SHADERS "*.glsl")
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${PROJECTNAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${PROJECTNAME} ${COMMON_LIBS})
