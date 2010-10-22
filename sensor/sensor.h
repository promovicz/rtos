#ifndef SENSOR_SENSOR_H
#define SENSOR_SENSOR_H



struct sensor {
	char *s_name;

	int s_quantity;
	int s_unit;
	int s_prefix;

	int s_min_value;
	int s_max_value;
};

#endif /* !SENSOR_SENSOR_H */
