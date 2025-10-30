/*
 * Utils.h
 *
 *  Created on: Oct 29, 2025
 *      Author: utnso
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <commons/string.h>
#include <stdlib.h>

int32_t string_size(char *string );

void binary_to_hex_string(char* bin_string, char* hex_string) ;
void hex_to_binary(char *hex_str, char **ascii_str) ;
char* hex_to_ascii(char* hex_str);

uint16_t string_to_uint(char* num);
void int_to_binary_string(uint32_t num, char **buffer, int cant_bits);

#endif /* SRC_UTILS_H_ */
