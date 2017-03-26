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
```bash
make clean build
```


Configuration
-------------
Configuration is done through JSON and the configuration
file is passed as the first and only argument to thermal.

See `config.example.json` for an example configuration.
