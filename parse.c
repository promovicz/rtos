
#include "parse.h"

bool_t scan_bool(const char *str, bool_t *res)
{
	if((!strcmp(str, "true"))
	   || (!strcmp(str, "t"))
	   || (!strcmp(str, "1"))
	   || (!strcmp(str, "on"))) {
		*res = BOOL_TRUE;
		return BOOL_TRUE;
	}

	if((!strcmp(str, "false"))
	   || (!strcmp(str, "f"))
	   || (!strcmp(str, "0"))
	   || (!strcmp(str, "off"))) {
		*res = BOOL_FALSE;
		return BOOL_TRUE;
	}

	return BOOL_FALSE;
}

bool_t scan_dec(const char *str, long int *res)
{
	char *end;
	long int v;

	v = strtol(str, &end, 10);

	if(!*end) {
		*res = v;
		return BOOL_TRUE;
	}

	return BOOL_FALSE;
}

bool_t scan_ldec(const char *str, long long int *res)
{
	char *end;
	long int v;

	v = strtoll(str, &end, 10);

	if(!*end) {
		*res = v;
		return BOOL_TRUE;
	}

	return BOOL_FALSE;
}

bool_t scan_hex(const char *str, long int *res)
{
	char *end;
	long int v;

	v = strtol(str, &end, 16);

	if(!*end) {
		*res = v;
		return BOOL_TRUE;
	}

	return BOOL_FALSE;
}

bool_t scan_lhex(const char *str, long long int *res)
{
	char *end;
	long int v;

	v = strtoll(str, &end, 16);

	if(!*end) {
		*res = v;
		return BOOL_TRUE;
	}

	return BOOL_FALSE;
}

bool_t scan_byte(const char *str, uint8_t *res)
{
	long int v;

	if(scan_hex(str, &v)) {
		if(v >= 0 && v <= 255) {
			*res = v & 0xFF;
			return BOOL_TRUE;
		}
	}

	return BOOL_FALSE;
}

bool_t scan_uint8(const char *str, uint8_t *res)
{
	long int v;

	if(scan_hex(str, &v)) {
		if(v >= 0 && v <= UINT8_MAX) {
			*res = v & 0xFF;
			return BOOL_TRUE;
		}
	}

	return BOOL_FALSE;
}

bool_t scan_uint16(const char *str, uint16_t *res)
{
	long int v;

	if(scan_hex(str, &v)) {
		if(v >= 0 && v <= UINT16_MAX) {
			*res = v & 0xFFFF;
			return BOOL_TRUE;
		}
	}

	return BOOL_FALSE;
}

bool_t scan_uint32(const char *str, uint32_t *res)
{
	long long int v;

	if(scan_lhex(str, &v)) {
		if(v >= 0 && v <= UINT32_MAX) {
			*res = v & 0xFFFFFFFF;
			return BOOL_TRUE;
		}
	}

	return BOOL_FALSE;
}

bool_t scan_ptr(const char *str, void **res)
{
	long long int v;

	if(scan_lhex(str, &v)) {
		if(v >= 0 && v <= UINT32_MAX) {
			*res = v & 0xFFFFFFFF;
			return BOOL_TRUE;
		}
	}

	return BOOL_FALSE;
}
