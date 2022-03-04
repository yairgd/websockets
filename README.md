# Websockets in c++

This project is the implementation of WebSockets in c++ using lib WebSockets (lws). Refer  [here](https://yairgadelov.me/websockets-with-c-/) in my blog for more details.


## Download & install libwebsockets 
The project uses CMake as a build system, and it builds the *lws*, it could refer on my previous [post]({{< ref "/blog/rest_server_with_cpp.md" >}}). So here is how to build *lws* in CMake.

```cmake
	ExternalProject_Add(websockets_external
		GIT_REPOSITORY https://github.com/warmcat/libwebsockets.git
		GIT_TAG v3.2.0
		CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DHAVE_SSL=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/${3rd_part}/websockets/${CMAKE_BUILD_TYPE}/
		DEPENDS websockets
		)
```

Download the project from here:
```bash
git clone git@github.com:yairgd/websockets.git
```

Ans use this to build it:
```bash
cd websockets &&  mkdir Debug && cmake -DCMAKE_BUILD_TYPE=Debug ..
```

