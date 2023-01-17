# BLERCCar client

## Compile

```shell
cd BLERCcar/BLERCcar_client/lib/spdlog
$ cd spdlog && mkdir build && cd build
$ cmake .. && make -j
# Or compiling against static library of spdlog
$ cmake .. -DSPDLOG_ENABLE_PCH=ON && make -j
```