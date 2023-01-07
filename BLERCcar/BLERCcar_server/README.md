# BLERCCar server

This server runs on the ESP32.
Exposing A BLE service with the following characteristics:

* Car service UUID: `10b47b82-89d8-11ed-a1eb-0242ac120002`

The client and the server will use same `BLERCcar/common/common.hpp` for characteristics usage

## Drive characteristics

`10b47dee-89d8-11ed-a1eb-0242ac120002`

* Write
* Wait-for-respond
* Expected (Driving mode, accel amount)

## Steer characteristics

`10b481ae-89d8-11ed-a1eb-0242ac120002`

* Write
* Wait fro respond
* Expected (Direction, steer_percentage)
* 