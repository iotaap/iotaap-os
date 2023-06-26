## [5.1.0] - 2023-06-26

IoTaaP OS v5.1.0 introduces new functions and integration of IoTaaP Services.
  
### Added
- customPublish function for publishing to topics of other users (Topic Access Control)
- storageServiceStore function to support IoTaaP Storage service 
- smsServiceSend function to support IoTaaP SMS service 
- Example of services usage is now added to examples folder

## [5.0.0] - 2021-11-22

IoTaaP OS v5.0.0 introduces performance and usability improvements, stability updates, improved web configurator and various bug fixes. This version also
intruduces batch data publishing to ensure fast and reliable publishing of high frequency data. 
  
### Added
- MQTT Batch implemented
- Support for unsecured MQTT connection (without encryption)
- Subscribing to any topic is now supported 
- Backup before reset
- 

### Changed
- Using WiFi events instead of basic functional approach 
- Certificate is now using dynamic allocation of the memory
- Web configurator imrovements and new fields
- OTA server configuration improvement 
- NTP servers are removed, IoTaaP Heartbeat is now used to sync time with server 
- MQTT client now accepts IP addres for server
- 3rd party libraries updated
- Task timing optimized
- Configurator internal handling

### Fixed

- WiFi password generator bug
- Starting web server dinamically 
- Static allocation is replaced by dynamic
- Fixed rebooting issue with local configurator
- High memory usage is now optimized  
- Queue is fixed and replaced by native solution
- Timestamp was reseting after restart
- Linked list


## [4.0.1] - 2021-04-01
   
### Fixed

- Fixed wrong ID printing to serial during boot

## [4.0.0] - 2021-03-31

**Breaking changes**

IoTaaP OS v4.0.0 introduces filesystem changes, bug fixes, various cosmetic changes and performance and usability improvements. Please read the 
[Official IoTaaP OS documentation](https://docs.iotaap.io/docs-iotaap-os/) to catch up easily.
  
### Added

- Implemented remote update trigger for devices
- Implemented remote update trigger for devices in group
- Added development feature for monitoring free stack in internal tasks (disabled by default)
- Added `start()` function, **which now has to be called at the beginning in order to start IoTaaP OS**

### Changed

- System will not start unitl `start()` function is called
- Updated initial code (example)
- Implemented internal FAT filesystem, SD is not mandatory to start the system. If SD is not present, logging and data backup will be disabled
- Removed `getUserParameter` function. Custom parameters are not supported due to switching to internal FAT FS for configuration (better implementation of custom parameters will be implemented in the future releases)
- CA certificate is now stored on internal filesystem, not on the SD card
- SD card is not mandatory and file structure will be automatically created during the first boot (manual SD flashing is not required anymore)
- Improved system logging and serial output

### Fixed

- IoTaaP hostname (during Web Configurator AP) is now generated from device MAC, and it is the same as the SSID
- Fixed resetting synced time if WiFi connection is lost (now it will work with previously synced time)
- Internal tasks performance optimizations

## [3.1.0] - 2021-03-17
  
### Added

- Added checking and creating FS at startup
- Creating default config file (template) at startup if not present
- Web configurator
- CoC and issue templates to repository

### Changed

- Stack size for WiFi and MQTT tasks
- Queue size for logs and local messages
- Status publishing period (to 5000ms)
- Logging (introduced SYSTEM tag and reduced writing of INFO tag log to SD card)

### Fixed 

- Showing error message if FS is not available, even if serial debug print is disabled

## [3.0.3] - 2021-03-08
  
### Fixed

- Fixed Duplicated Certification reuse
- Syntax & Grammar cleanup on comments

## [3.0.2] - 2021-02-10

### Changed

- Updated CONTRIBUTING.md, new PR procedure

## [3.0.1] - 2021-02-05

### Added

- Added device_id to uDeviceCloudPublishParam
   
### Fixed

- Fixed unix_ms (systemTime)  issue caused by ARDUINOJSON_USE_LONG_LONG

## [3.0.0] - 2021-01-29
  
IoTaaP OS v3.0.0 - Forked from stable branch

### Added

### Changed
   
### Fixed