## IoTaaP OS - Custom partitions
IoTaaP OS requires custom partitions since it uses local FAT filesystem for storing system parameters and certificates

## Two partition configuration files are available under **partitions** directory

- default_16MB_fat.csv - to be used with recomended ESP32 modules with 16MB flash
- default_fat.csv - to be used with standard ESP32 modules with 4MB flash

## Selecting custom partition

When using PlatformIO, custom partition configuration file (.csv) can be defined in `platformio.ini` file in the following way:

`board_build.partitions = partitions/default_16MB_fat.csv`

 Please note that `partitions` directory should be on the same level as your `src` directory, for the above configuration to work properly.
