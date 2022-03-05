# Websockets in c++

This project is the implementation of WebSockets in c++ using lib WebSockets - [lws](https://github.com/warmcat/libwebsockets.git). 


## Download & install libwebsockets 
The project uses CMake as a build system, and it builds the *lws*, it could refer on my previous [post](https://yairgadelov.me/rest-api-server-with-c-/#download--install-restbed). So here is how to build *lws* in CMake.


Download the project from here:
```bash
git clone git@github.com:yairgd/websockets.git
```

And use this to build it:
```bash
cd websockets &&  mkdir Debug && cmake -DCMAKE_BUILD_TYPE=Debug ..
```

