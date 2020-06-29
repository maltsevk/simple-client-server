# simple-client-server
Simple client/server application based on linux sockets and supporting multithreading

### Requirements
- g++ 5.4
- CMake 3.17.0

### Build
Server
```
$ mkdir server/build
$ cd server/build
$ cmake ../
$ make
```
Client
```
$ mkdir client/build
$ cd client/build
$ cmake ../
$ make
```

### Usage
Launch server
```
$ ./server
```
Launch client
```
$ ./client [-tcp|-udp]
```
