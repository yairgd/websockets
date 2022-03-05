# Websockets in c++

This project is the implementation of WebSockets in c++ using lib WebSockets - [lws](https://github.com/warmcat/libwebsockets.git). I wrote [post](https://yairgadelov.me/websockets-with-c-/) about it in my blog.


## Download & install libwebsockets 
The project uses CMake as a build system. Download the project from here:

```bash
git clone git@github.com:yairgd/websockets.git
```

And use this to build it:
```bash
cd websockets &&  mkdir Debug && cmake -DCMAKE_BUILD_TYPE=Debug ..
```

