#include "Utils.h"

int32_t string_size(char *string ){
	return (int32_t) strlen(string);
}

//convierte una cadena de caracteres binarios a una cadena hexadecimal.
void binary_to_hex_string(char* bin_string, char* hex_string) {
    long length = strlen(bin_string);
    int i, j = 0;

    // Tabla de conversión para cada nibble (4 bits)
    char hex_digits[] = "0123456789ABCDEF";

    // Si la longitud no es múltiplo de 4, procesa el primer grupo de bits
    int remainder = length % 4;
    if (remainder != 0) {
        int nibble_val = 0;
        for (i = 0; i < remainder; i++) {
            nibble_val = (nibble_val << 1) | (bin_string[i] - '0');
        }
        hex_string[j++] = hex_digits[nibble_val];
    }

    // Procesa el resto de la cadena en grupos de 4 bits
    for (i = remainder; i < length; i += 4) {
        int nibble_val = 0;
        nibble_val = (bin_string[i] - '0') << 3 |
                     (bin_string[i+1] - '0') << 2 |
                     (bin_string[i+2] - '0') << 1 |
                     (bin_string[i+3] - '0');
        hex_string[j++] = hex_digits[nibble_val];
    }

    // Agrega el terminador nulo a la cadena hexadecimal
    hex_string[j] = '\0';
}

//convierte un caracter en hexa a un string de caracteres que son los 4 bits correspondientes en ascii
void hex_char_to_int(char hex_char, char **binary_output) {
    switch (hex_char) {
        case '0': string_append(binary_output, "0000"); break;
        case '1': string_append(binary_output, "0001"); break;
        case '2': string_append(binary_output, "0010"); break;
        case '3': string_append(binary_output, "0011"); break;
        case '4': string_append(binary_output, "0100"); break;
        case '5': string_append(binary_output, "0101"); break;
        case '6': string_append(binary_output, "0110"); break;
        case '7': string_append(binary_output, "0111"); break;
        case '8': string_append(binary_output, "1000"); break;
        case '9': string_append(binary_output, "1001"); break;
        case 'A':
        case 'a': string_append(binary_output, "1010"); break;
        case 'B':
        case 'b': string_append(binary_output, "1011"); break;
        case 'C':
        case 'c': string_append(binary_output, "1100"); break;
        case 'D':
        case 'd': string_append(binary_output, "1101"); break;
        case 'E':
        case 'e': string_append(binary_output, "1110"); break;
        case 'F':
        case 'f': string_append(binary_output, "1111"); break;
        default: string_append(binary_output, "NULL"); break; // si es invalido solo digo NULL
    }
}


// convierte string en hexa a string en ascii
void hex_to_binary(char *hex_str, char **ascii_str) {
    int len = string_length(hex_str);

    for (int i = 0; i < len; i++) {

    	hex_char_to_int(hex_str[i], ascii_str);
    }
}

char* hex_to_ascii(char* hex_str, int* out_len) {
    if (hex_str == NULL) return NULL;

    int len = string_size(hex_str);
    if (len % 2 != 0) {
        // Debe tener una cantidad par de caracteres
        return NULL;
    }

    // Cantidad de bytes que representará (cada 2 caracteres hex = 1 byte)
    *out_len = len / 2;

    // Reservamos espacio (+1 para el terminador '\0')
    char* ascii_str = malloc(*out_len + 1);

    for (int i = 0; i < *out_len; i++) {
        char byte_hex[3] = { hex_str[2 * i], hex_str[2 * i + 1], '\0' };
        ascii_str[i] = (char) strtol(byte_hex, NULL, 16);
    }

    ascii_str[*out_len] = '\0';
    return ascii_str;
}


uint16_t string_to_uint(char* num){
	char *endptr;

	return (uint16_t) strtoul(num,&endptr,2);
}

void int_to_binary_string(uint32_t num, char **buffer, int cant_bits) {
    int i;

    (*buffer)[cant_bits] = '\0';
    // si cant_bits fuera de 32 bits =>
    // 	iteramos de 31 a 0 para rellenar el buffer de derecha a izquierda
    for (i = cant_bits - 1; i >= 0; i--) {
        // Usamos el operador de desplazamiento a la derecha y el bitwise AND para obtener cada bit
        (*buffer)[i] = (num & 1) ? '1' : '0';
        num >>= 1;
    }
}

// convierte un caracter en un string de bits (ejemplo si caracter='h' => buffer="01101000")
void convert_ascii_to_bits(char caracter, char*buffer){
	  buffer[8] = '\0'; // Asegura el terminador nulo al final

	    for (int i = 7; i >= 0; i--) {
	        // Usa un operador bit a bit para verificar cada bit del caracter
	        // Se desplaza 1 a la izquierda i veces para obtener una máscara con un 1 en la posición correcta
	        // Y se usa el operador AND (&) para aislar ese bit
	        if ((caracter >> i) & 1) {
	            buffer[7 - i] = '1';
	        } else {
	            buffer[7 - i] = '0';
	        }
	    }
}
