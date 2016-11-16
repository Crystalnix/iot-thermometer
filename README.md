# IOT thermometer

This PlatformIO project collects air temperature and
air humidity and sends it to the statsd server.

## Usage and Installation

To use this project you need an *espressif8266*-based board, *DHT22* sensor (or *DHT11*),
[platformio IDE](http://platformio.org/), and [statsd](https://github.com/etsy/statsd)-powered server.

Then you should connect the sensor with the board - I've used
[this instruction](https://learn.adafruit.com/dht/connecting-to-a-dhtxx-sensor).

The major step is the settings setup in `src/main.ino`:

- `ssid` - your WiFi network name;
- `password` - your WiFi network password (**It's a secret** - you should not commit it);
- `statsdsIP` - the IP address of the machine with statsd;
- `statsdsPort` - the UDP port of statsd UDP server, default: `8125`;
- `DHTPIN` - the board pin number connected to the sensor *data out* pin;
- `DHTTYPE` - the sensor model: DHT22 or DHT11;

Then save the file and build the project with PlatformIO.

Afterward, I must connect the board and upload the project to the board:

- Initialize the project for your board (do it once);
- Upload the project to the board;
- Connect to the serial port with `115200` frequency (optional, used it for debugging);
- Open the statsd graphite interface and graph `stats.gauges.temperature` and `stats.gauges.humidity`;

  Note: temperature is collected in Celsius!

