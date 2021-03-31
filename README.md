# IoTaaP OS
> Open Source IoT Operating System

[![Build Status](https://jenkins.iotaap.io/buildStatus/icon?job=iotaap-os%2Fmaster)](https://jenkins.iotaap.io/job/iotaap-os/job/master/)

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

## Hardware

For development you can use official [IoTaaP Hardware](https://www.iotaap.io/)

* [IoTaaP Boards](https://www.iotaap.io/boards/)
* [IoTaaP Modules](https://www.iotaap.io/iot-modules/)

## Tutorials

Follow [official tutorials](https://docs.iotaap.io/docs-tutorials/) for even faster IoT development process!

## Documentation

Official [documentation](https://docs.iotaap.io)

## Roadmap

Check out our [Roadmap](https://community.iotaap.io/t/roadmap/77), request features and improvements

## Community

Join [IoTaaP community](https://community.iotaap.io) today!

## Authors

* [IoTaaP Team](https://www.iotaap.io) – contact@iotaap.io
* IoTaaP Community

## Contributing

See [CONTRIBUTING](./CONTRIBUTING.md) for more information.

## License

Apache License 2.0, see [LICENSE](./LICENSE.md)
