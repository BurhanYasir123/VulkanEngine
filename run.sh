mkdir ./build
mkdir ./build/shaders
glslc -fshader-stage=vert ./src/shaders/vertex.glsl -o ./build/shaders/vert.spv
glslc -fshader-stage=frag ./src/shaders/fragment.glsl -o ./build/shaders/frag.spv
g++ *.cpp -I./src -lvulkan -lglfw -o ./build/app
echo --------------------------------
pushd build
./app
popd