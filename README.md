# bit.com C++ api sdk

### API request/response format
https://www.bit.com/docs/en-us/spot.html#order

### Guidelines for account mode
https://www.bit.com/docs/en-us/spot.html#guidelines-for-account-mode

### API Host:
https://www.bit.com/docs/en-us/spot.html#spot-api-hosts-production



# Tested OS 

* Ubuntu
* macOS

# C++ version

* C++ 17

# Dependencies

* Boost (This project use 1.88)
* Json lib: nlohmann json (https://github.com/nlohmann/json)
* Rest client: httplib (https://github.com/yhirose/cpp-httplib)


# How to run

## Setup env variables: 


* `BITCOM_REST_HOST`: Rest api host
* `BITCOM_WS_HOST`: Websocket host
* `BITCOM_AK`: Access-key
* `BITCOM_SK`: Private-key


## Build

```bash
cmake -S . -B out
cmake --build out
```


## Test REST api
```bash
make
./out/bit_client rest
```

## Test public websocket
```bash
./out/bit_client public-ws
```

## Test private websocket
```bash
./out/bit_client private-ws
```
