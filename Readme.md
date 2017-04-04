Thermal
=======
A command line tool to access digital thermometer data.


Supported sensors
-----------------
Name | Vendor ID | Product ID
---- | --------- | ----------
PCSensor TEMPer1F | 0x0c45 | 0x7401


Building
--------
The program and its dependencies can be built with:
```bash
make clean build
```

Since version 0.1.1 thermal supports cross-compiling.
Configure the `CROSS_COMPILE` `make` variable should be set
to the toolchain prefix.

Keep in mind that cross-compiling will require libusb (which
in turns requires libudev) to be available to the cross-build
toolchain.

Because `promclient` uses `libonion` for the HTTP sever we
have to tell `cmake` which toolchain to use too.
The `CMAKE_TOOLCHAIN_FILE` `make` variable can be used for that.
See `promclient` readme for more on this.


Configuration
-------------
Configuration is done through JSON and the configuration
file is passed as the first and only argument to thermal.

See `config.example.json` for an example configuration.
