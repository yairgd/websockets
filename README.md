# Websockets in c++

This project is the implementation of WebSockets in c++ using lib WebSockets - [lws](https://github.com/warmcat/libwebsockets.git). I wrote [post](https://yairgadelov.me/websockets-with-c-/) about it in my blog.

## Download & install libwebsockets

The project uses CMake as a build system.

Clone this repository and then fetch the git submodule dependencies:

```bash
git submodule update --init --recursive
```

And use this to build it:

```bash
cd websockets &&  mkdir Debug && cd Debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make
```

## Add as a CMake dependency

In your own project that uses CMake as a build system, clone this repository and add the following in CMakeLists.txt

```cmake
target_link_libraries(${TARGET} PRIVATE websockets)
target_link_libraries(${TARGET} PRIVATE WebSockets)
add_subdirectory(websockets EXCLUDE_FROM_ALL)
```
