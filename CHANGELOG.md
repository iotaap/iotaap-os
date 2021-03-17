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