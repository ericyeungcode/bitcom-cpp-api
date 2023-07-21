# bit.com C++ api sdk


# OS 

* MacOs Ventura

# C++ version

* C++ 17

# Dependencies

* Json lib: nlohmann json (https://github.com/nlohmann/json)
* Rest client: httplib (https://github.com/yhirose/cpp-httplib)
* Websocket client: websocketpp (https://github.com/zaphoyd/websocketpp)


# How to run

* setup env variables: `BITCOM_REST_HOST`, `BITCOM_WS_HOST`, `BITCOM_AK`, `BITCOM_SK`


## Test REST api
```bash
make
./main rest
```

## Test public websocket
```bash
./main public-ws
```

## Test private websocket
```bash
./main private-ws
```
