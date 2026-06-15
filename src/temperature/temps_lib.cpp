#include <stdint.h>

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "temperature/temps_lib.h"


/*
 * The sensor initialization function.
 * Run it after you've set all the values in your 'struct temp_sensor'.
 * It will help identify issues with unset pointers
 *
 * @sensor - pointer to your sensor
 *
 * Return 0 if everything is fine, else TEMPS_*_INVALID macro
 */
uint8_t temps_lib_init(struct temp_sensor *sensor)
{
	if (!sensor->sensor_data)
		return TEMPS_SENSOR_DATA_INVALID;

	if (!sensor->read_temp)
		return TEMPS_READ_TEMP_INVALID;

	if (!sensor->request_temp)
		return TEMPS_REQUEST_TEMP_INVALID;

	return 0;
}


static uint8_t _refresh_sensor(struct temp_sensor *sensor)
{
/* TODO: handle possible read errors */
	uint8_t ret = 0;

	/* exit if sensor disable */
	if (!sensor->is_enable)
		return ret;

	if (sensor->_read_timer &&
	    (millis() - sensor->_read_timer >= sensor->req_interval)) {
		/* temp ready, read it, set timer to 0 */
		fl_t new_temp;

		/* 10 - to convert float to fl_t: (77.7 * 10) = 777 that good */
		/* new_temp = (fl_t)(sensor->obj->getTempC(sensor->address) * 10); */
		new_temp = sensor->read_temp(sensor);
		if (new_temp != sensor->cur_temp) {
			sensor->prev_temp = sensor->cur_temp;
			sensor->changes_timer = millis();
		}

		sensor->cur_temp = new_temp;
		sensor->_read_timer = 0;	/* neccessary */
	}

	if (!sensor->_read_timer) {
		/* request temp, set timer */
		/* sensor->obj->requestTemperaturesByAddress(sensor->address); */
		sensor->request_temp(sensor);
		sensor->_read_timer = millis();
	}

	return ret;
}


/* Here only read/set temperatures, without comparing tar/cur */
uint8_t temps_lib_refresh(struct temps_service *service)
{
/* TODO: here read the temps, update the timers, etc... */
	struct temp_sensor *sensor;
	uint8_t ret = 0;

	for (int i = 0; i < service->sensors_count; i++) {
		sensor = &(service->sensors[i]);
		ret = _refresh_sensor(sensor);
		if (ret)
			sensor->errors++;
	}

	return ret;
}


/*
 * Convert unsigned fl_t to str
 * Right-aligned buffer, replace all buffer items to ' ' before result
 *
 * @num - number for convert (10000 > num > 10)
 * @buff[5] - str for result, it is assumed that it will fit in it
 * @is_float - flag, 0 - if num is simple number, !0 - if num is a float num
 *
 * char *res = "aaaaa";
 * temps_lib_convert(333, res, 1);
 * -> res == " 33.3"
 * temps_lib_convert(333, res, 0);
 * -> res == "333aa"
 *
 *  max:
 *	temps_lib_convert(999, res, 1)
 *	-> "999.9"
 *	temps_lib_convert(999, res, 0)
 *	-> "999  "
 *
 *  err:
 *	temps_lib_convert(10000, res, 1)
 *	-> "aeraa"
 *	temps_lib_convert(10000, res, 0)
 *	-> "aeraa"
 */
uint8_t *temps_lib_convert(fl_t num, uint8_t buff[5], uint8_t is_float)
{
	uint32_t tmp;
	int data;

	if (num >= 10000) {
		buff[1] = 'e';
		buff[2] = 'r';

		return NULL;
	}

	if (is_float) {
		buff[0] = ' ';
		buff[1] = ' ';
		buff[2] = ' ';
		buff[3] = ' ';
		buff[4] = ' ';

		buff[4] = (num % 10) + '0';
		num /= 10;
		buff[3] = '.';

		tmp = num % 10;
		num /= 10;
		buff[2] = tmp + '0';

		if (!num)
			return buff + 2;

		tmp = num % 10;
		num /= 10;
		buff[1] = tmp + '0';

		if (!num)
			return buff + 1;

		tmp = num % 10;
		num /= 10;
		buff[0] = tmp + '0';

		return buff;
	}

	/* so as num must be < 10000 -> max is 999.9, if no float, then just 999
	 * that 3 symbols, because last [3] and [4] don't change
	 */
	buff[0] = ' ';
	buff[1] = ' ';
	buff[2] = ' ';

	num /= 10;

	if (!num)
		return buff + 3;

	buff[2] = num % 10 + '0';
	num /= 10;

	if (!num)
		return buff + 2;

	buff[1] = num % 10 + '0';
	num /= 10;

	if (!num)
		return buff + 1;

	buff[0] = num % 10 + '0';
	num /= 10;

	return buff;
}


#ifdef TEMPS_USE_DS18B20
fl_t read_ds18b20(struct temp_sensor *sensor)
{
	struct ds18b20 *sensor_data = sensor->sensor_data;
	DallasTemperature *obj = sensor_data->obj;

	/* 10 - to convert float to fl_t: (77.7 * 10) = 777 that good */
	return (fl_t)(obj->getTempC(sensor_data->address) * 10);
}


void request_ds18b20(struct temp_sensor *sensor)
{
	struct ds18b20 *sensor_data = sensor->sensor_data;
	DallasTemperature *obj = sensor_data->obj;

	obj->requestTemperaturesByAddress(sensor_data->address);
}
#endif /* TEMPS_USE_DS18B20 */
