glslc -fshader-stage=vert ./src/shaders/vertex.glsl -o ./src/vert.spv
glslc -fshader-stage=frag ./src/shaders/fragment.glsl -o ./src/frag.spv
g++ *.cpp -I./src -lvulkan -lglfw -o app
echo --------------------------------
./app
