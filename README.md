# Brainiac Firmware

This code repository holds Workshop12's Arduino powered accessory firmware.

At this time; the single accessory with firmware published here is the Relay Accessory Board.

## Relay Accessory Board

The Relay Accessory Board is assembled from several third party _off the shelf_ parts:
* An Arduino Uno
* An LC202 relay board
* Connector Wires
* USB A-B cable

To learn how to put these parts together as an accessory, or to see recommended sources, please see https://workshoptwelve.com/accessories/relay/ 

### LC202 Implementation Note (or Warning)

The LC202 is inverted compared to many other boards. The relays (and LED) turn on when driven by a logical `0`.
If you plan to use a different relay board that uses a logical `1`; you must change the implementation of `RelayChannel::invertedCheck()` located in `lc202.ino`.

## Other Accessories

See https://workshoptwelve.com/accessories/ for more information on accessory options.

# Source Code License

Grab it. Use it. Improve it. Enjoy.
