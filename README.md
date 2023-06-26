# IoTaaP OS - Open Source IoT Operating System

## Build status
[![IoTaaP OS Build](https://github.com/iotaap/iotaap-os/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/iotaap/iotaap-os/actions/workflows/build.yml)

## Usage

Include IoTaaP_OS.h to your project and define IoTaaP OS object

## Usage example

```cpp
#include <IoTaaP_OS.h>

IoTaaP_OS iotaapOs("1.0.1");

...

void setup()
{
  iotaapOs.start(); // Start IoTaaP OS
  iotaapOs.startWifi(); // Connect to WiFi
  iotaapOs.startMqtt(callback); // Connect to MQTT broker

}

...
```

## Tutorials

Follow [official tutorials](https://docs.iotaap.io/en/tutorials/introdcution) for even faster IoT development process!

## Documentation

Official [documentation](https://docs.iotaap.io)

## Community

Join [IoTaaP community](https://community.iotaap.io) today!

## Authors

* [IoTaaP Team](https://www.iotaap.io) – contact@iotaap.io
* IoTaaP Community

## Contributing

See [CONTRIBUTING](./CONTRIBUTING.md) for more information.

## License

Apache License 2.0, see [LICENSE](./LICENSE.md)
