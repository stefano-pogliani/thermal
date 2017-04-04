TODO
----
- Support nested ports identifier (for more accurate matches).
  * http://libusb.sourceforge.net/api-1.0/group__dev.html#gaa4b7b2b50a9ce2aa396b0af2b979544d

0.1.2
-----
- Clean dependencies when `make clean`.
- List USB devices bus and port numbers for use in config.
- SystemD service.
- Udev rules file to set USB device user permissions.

0.1.1
-----
- Print version number in help.
- Support cross-compiling.

0.1.0
-----
- Abstract sensors.
- Configuration loader/checker.
- Output temperatures to file or stdout.
- Output temperatures over HTTP in Prometheus text format.
- USB backed sensors.
- PcSensor Temper1F 1.3 support.
