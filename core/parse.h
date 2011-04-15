
#ifndef CORE_PARSE_H
#define CORE_PARSE_H

#include <core/common.h>

bool_t scan_bool(const char *str, bool_t *res);

bool_t scan_dec(const char *str, long int *res);
bool_t scan_ldec(const char *str, long long int *res);
bool_t scan_hex(const char *str, long int *res);
bool_t scan_lhex(const char *str, long long int *res);

bool_t scan_byte(const char *str, uint8_t *res);

bool_t scan_uint8(const char *str, uint8_t *res);
bool_t scan_uint16(const char *str, uint16_t *res);
bool_t scan_uint32(const char *str, uint32_t *res);

bool_t scan_ptr(const char *str, void **res);

#endif /* !CORE_PARSE_H */
