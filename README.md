# IOT thermometer

The project is a PlatformIO project that collect air temperature and
air humidity and send it to statsd server.

## Usage and Installation

To use this project you needs *espressif8266*-based board, *DHT22* sensor (or *DHT11*),
[platformio IDE](http://platformio.org/) and [statsd](https://github.com/etsy/statsd)-powered server.

Then you should connect the sensor with the board - I've used
[a instraction](https://learn.adafruit.com/dht/connecting-to-a-dhtxx-sensor).

The major step is setup the settings in `src/main.ino`:

- `ssid` - your wifi network name;
- `password` - your wifi network password (**It's a secret** - you should not commit it);
- `statsdsIP` - the ip address of the machine with statsd;
- `statsdsPort` - the udp port of statsd udp server, default: `8125`;
- `DHTPIN` - the board pin number connected to the sensor *data out* pin;
- `DHTTYPE` - the sensor model: DHT22 or DHT11;

Then save the file and build the project with PlatformIO.

Afterward, I must connect the board and upload the project to the board:

- initialize the project for your board (do it once);
- upload the project to the board;
- connect to the serial port with `115200` frequency (optional, used it for debugging);
- open the statsd graphite interface and graph `stats.gauges.temperature` and `stats.gauges.humidity`;

  Note: temperature is collected in Celsius!

