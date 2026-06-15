#ifndef TEMPS_LIB_H
#define TEMPS_LIB_H

#include <stdint.h>

/*	Overview:
 *
 *	Library for management temperature sensors, in control about every sensor:
 *	struct temp_sensor:
 *	RW	*sensor_data	sensor-specific data; need for read_temp() and request_temp()
 *	R	cur_temp	current temp, updating in background temps_lib_refresh() func
 *	R	prev_temp	previous temp, updating with every chage cur_temp:
 *					so, cur_temp will be as prev_temp, new val -> in cur_temp
 *	RW	tar_temp	just your value; library doesn't use this
 *	R	changes_timer	millis(), when was entered into .prev_temp;
 *	R	errors		if some errors in the read/cmp/other proccess, it's will be > 0
 *	RW	is_enable	if false, sensor is not serviced, other fields are not updated
 *	RW	req_interval	call request_temp() and read_temp() no more often than every
 *					req_interval ms
 *	RW	data		your private data; library doesn't use this; maybe replace to ptr?
 *	 W	read_temp	sensor-specific temperature read function pointer;
 *					the library thinks that you set this pointer;
 *					the library calls it automatcally every 'req_interval' ms
 *	 W	request_temp	sensor-specific temperature update function pointer;
 *					the library thinks that you set this pointer;
 *					the library calls it automatcally every 'req_interval' ms
 *
 *		// internal data, you don't need to change it:
 *		_read_timer	timer between temp requests
 *
 *	General structure:
 *	struct temps_service:
 *	RW	sensors		array of sensors
 *	RW	sensors_count	length of sensors array
 *		
 *	NOTE: R/W/RW - you can read/write this values
 *
 *	You can define:
 *		#define TEMPS_USE_DS18B20	// to use DS18B20 sensor
 *
 *
 *	Use on the example of DS18B20:
 *
 *	So, we have 2 DS18B20 sensors on D8 pin, and 3 on D10 pin;
 *	Sensors from D8 pin need set to 12 bit,
 *	Sensors from D10 pin - to 9 bit.
 *
 *	#define COUNT_OF_SENSORS 5
 *
 *	// here register DallasTemperature objects
 *	TEMPS_REGISTER_DS18B20_PIN(warn_sensors, 8);	// here 2 sensors
 *	TEMPS_REGISTER_DS18B20_PIN(def_sensors, 10);	// here 3
 *
 *	// create an arrays manually
 *	struct temp_sensor sensors_arr[COUNT_OF_SENSORS];
 *	struct ds18b20 sensors_data[COUNT_OF_SENSORS];
 *
 *	void init_my_ds18b20_sensors(struct temps_service *temps)
 *	{
 *		temps->sensors = sensors_arr;
 *		temps->sensors_count = COUNT_OF_SENSORS;
 *
 *		warn_sensors.begin();
 *		def_sensors.begin();
 *
 *		if (warn_sensor.getDeviceCount() != 2)
 *			return ERROR;
 *
 *		if (def_sensors.getDeviceCount() < 3)
 *			;	// do something
 *
 *		// set DallasTemperature objects
 *		sensors_data[0].obj = &def_sensors;
 *		sensors_data[1].obj = &def_sensors;
 *		sensors_data[2].obj = &def_sensors;
 *		sensors_data[3].obj = &ward_sensors;
 *		sensors_data[4].obj = &ward_sensors;
 *
 *		// find and set DS18B20 addresses
 *		sensors_data[0].obj->getAddress(sensors_data[0].address, 0);
 *		sensors_data[1].obj->getAddress(sensors_data[1].address, 1);
 *		sensors_data[2].obj->getAddress(sensors_data[2].address, 2);
 *		sensors_data[3].obj->getAddress(sensors_data[3].address, 0); // 0 because other pin
 *		sensors_data[4].obj->getAddress(sensors_data[4].address, 1);
 *
 *		// set resolutions
 *		sensors_data[0].obj->setResolution(sensors_data[0].address, 9);
 *		sensors_data[1].obj->setResolution(sensors_data[1].address, 9);
 *		sensors_data[2].obj->setResolution(sensors_data[2].address, 9);
 *		sensors_data[3].obj->setResolution(sensors_data[3].address, 12);
 *		sensors_data[4].obj->setResolution(sensors_data[4].address, 12);
 *
 *		// set structures
 *		temps->sensors[0].sensor_data = &sensor_data[0];
 *		temps->sensors[1].sensor_data = &sensor_data[1];
 *		temps->sensors[2].sensor_data = &sensor_data[2];
 *		temps->sensors[3].sensor_data = &sensor_data[3];
 *		temps->sensors[4].sensor_data = &sensor_data[4];
 *
 *		// set read/request functions
 *		temps->sensors[0].read_temp = &read_ds18b20;
 *		temps->sensors[0].request_temp = &request_ds18b20;
 *		temps->sensors[1].read_temp = &read_ds18b20;
 *		temps->sensors[1].request_temp = &request_ds18b20;
 *		temps->sensors[2].read_temp = &read_ds18b20;
 *		temps->sensors[2].request_temp = &request_ds18b20;
 *		temps->sensors[3].read_temp = &read_ds18b20;
 *		temps->sensors[3].request_temp = &request_ds18b20;
 *		temps->sensors[4].read_temp = &read_ds18b20;
 *		temps->sensors[4].request_temp = &request_ds18b20;
 *
 *		// set timeouts
 *		temps->sensors[0].req_interval = DS18B20_9_BIT_TIME;
 *		temps->sensors[1].req_interval = DS18B20_9_BIT_TIME;
 *		temps->sensors[2].req_interval = DS18B20_9_BIT_TIME;
 *		temps->sensors[3].req_interval = DS18B20_12_BIT_TIME;
 *		temps->sensors[4].req_interval = DS18B20_12_BIT_TIME;
 *
 *		// enable
 *		temps->sensors[0].is_enable = true;
 *		temps->sensors[1].is_enable = true;
 *		temps->sensors[2].is_enable = true;
 *		temps->sensors[3].is_enable = true;
 *		temps->sensors[4].is_enable = true;
 *
 *		// check
 *		for (int i = 0; i < temps->sensors_count; i++) {
 *			int ret = temps_lib_init(&temps->sensors[i]);
 *			if (ret)
 *				;	// do something
 *		}
 *
 *		// set the async mode
 *		warn_sensors.setWaitForConversion(false);
 *		def_sensors.setWaitForConversion(false);
 *	}
 *
 *	Next you can computing cur_temp, tar_temp, prev_temp, changes_timer in bg function,
 *	in interrupts, in callback... as you want, here without this
 */

#ifndef TEMPS_USE_DS18B20
#define TEMPS_USE_DS18B20
#endif

/**
 * Alias to float, one digit after the decimal point
 * 255 mean 25.5, 777 mean 77.7, 1115 -> 111.5
 * maximum is 6553.6
 */
typedef uint16_t fl_t;	/* unsigned */


#ifdef TEMPS_USE_DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20_MAX_TEMP 125
#define DS18B20_MIN_TEMP -55

#define DS18B20_9_BIT_TIME 95
#define DS18B20_10_BIT_TIME 190
#define DS18B20_11_BIT_TIME 350
#define DS18B20_12_BIT_TIME 750

/**
 * Register new OneWire, DallasTemperature objects to management DS18B20 sensors
 * Call once for each pin
 *
 * @name - name for new DallasTemperature object
 * @pin - pin on which the sensor/sensors are located
 */
#define TEMPS_REGISTER_DS18B20_PIN(name, pin)			\
	static OneWire name ##_wire((pin));			\
	static DallasTemperature name(&(name ## _wire))


struct ds18b20 {
	DallasTemperature *obj;
	DeviceAddress address;
};

fl_t read_ds18b20(struct temp_sensor *sensor);
void request_ds18b20(struct temp_sensor *senfor);

#endif /* TEMPS_USE_DS18B20 */


#define TEMPS_SENSOR_DATA_INVALID 22
#define TEMPS_READ_TEMP_INVALID 23
#define TEMPS_REQUEST_TEMP_INVALID 24


struct temp_sensor {
	/**
	 * sensor-specific data;
	 * store here everything you need for read_temp() and request_temp() funtions
	 */
	void *sensor_data;

	fl_t cur_temp;
	fl_t prev_temp;
	fl_t tar_temp;
	uint32_t changes_timer;		/* millis(), when value was entered into 'prev_temp' */
	uint32_t data;			/* user data about this sensor */
	uint32_t req_interval;		/* request temp no more often than every req_interval ms */
	uint8_t errors;
	bool is_enable;
	uint32_t _read_timer;

	/**
	 * In these functions, you don't need to change the fields in the 'struct temp_sensor',
	 * the library does it all for you. You just need to return the correct temperature
	 * and request a new value.
	 * The library calls them in temps_lib_refresh(), so make them as fast as possible.
	 * And of cource, it's in your best interest for them to be 'asynchronous'.
	 */
	fl_t (*read_temp)(struct temp_sensor *sensor); /* func to read finished temp */
	void (*request_temp)(struct temp_sensor *sensor); /* func to request new temp */
};

struct temps_service {
	struct temp_sensor *sensors;		/* arr */
	uint8_t sensors_count;
};


uint8_t temps_lib_init(struct temp_sensor *sensor);
uint8_t temps_lib_refresh(struct temps_service *service);
uint8_t *temps_lib_convert(fl_t num, uint8_t buff[5], uint8_t is_float);


#endif
