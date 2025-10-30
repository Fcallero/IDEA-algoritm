#include "IDEA.h"

#define round	8



//desplazamiento hacia izquierda cíclico
void cyclic_left_shift(char *bit_string, int length, int shift_count) {
    if (length <= 1 || shift_count == 0) {
        return;
    }

    // Asegura que el conteo de desplazamiento esté dentro del rango de la longitud de la cadena
    shift_count %= length;
    if (shift_count < 0) {
        shift_count += length;
    }
    if (shift_count == 0) {
        return;
    }

    // Asigna memoria para una copia temporal de la cadena original
    char *temp_string = (char *)malloc(length );

    /* Copiamos solo los length bytes (no necesitamos terminar con '\0' si
           trabajamos con buffers binarios o strings sin usar funciones que
           esperen terminador). Pero si tu bit_string es C-string, podés usar length+1 y '\0'. */
        memcpy(temp_string, bit_string, length);

    // Realiza el desplazamiento cíclico
    for (int i = 0; i < length; i++) {
        int new_index = (i + shift_count) % length;
        bit_string[i] = temp_string[new_index];
    }

    free(temp_string);
}

char** generar_subclaves(char *clave){
	char **subclaves_array = string_array_new();

	//obtengo las primeras ocho subclaves
	for(int i =0; clave[i] != '\0'; i = i+4){
		char* subclave_n = string_from_format("%c%c%c%c", clave[i], clave[i+1], clave[i+2], clave[i+3]);
		string_array_push(&subclaves_array, subclave_n);
	}


	// se pasa de hexa a ascii (decimal)

	char **subclaves_ascii = string_array_new();

	void convertir_a_ascii(char* subclave_en_hexa){
		char *ascii_str = string_new();
		hex_to_binary(subclave_en_hexa, &ascii_str);
		string_array_push(&subclaves_ascii, ascii_str);
	}
	string_iterate_lines(subclaves_array, convertir_a_ascii);

	string_array_destroy(subclaves_array);

	//unificar la clave

	char *clave_ascii = string_new();

	void append_array(char* subclave_en_ascii){

		string_append(&clave_ascii, subclave_en_ascii);
	}


	string_iterate_lines(subclaves_ascii, append_array);


	printf("clave en bits (en total %i bits): %s\n", string_size(clave_ascii), clave_ascii);

	//dezplazamiento a la izquierda cíclico de 25 bits

	cyclic_left_shift(clave_ascii,128,25);

	//partir en 8 y agregarlos al array de subclaves

	for(int i = 0; i< 8; i++){
		char * subclave_n= string_substring(clave_ascii,i*16,16);

		string_array_push(&subclaves_ascii, subclave_n);
	}

	//repito los pasos para generar las 52 subclaves en total que necesito
	int len = string_array_size(subclaves_ascii);

	while(len < 56){
		//printf("clave en bits (en total %i bits): %s\n", string_size(clave_ascii), clave_ascii);
		cyclic_left_shift(clave_ascii,128,25);
		//printf("clave en bits dezplazado 25 veces(en total %i bits): %s\n", string_size(clave_ascii), clave_ascii);

		//partir en 8 y agregarlos al array de subclaves

		for(int i = 0; i< 8; i++){
			char * subclave_n= string_substring(clave_ascii,i*16,16);
			//printf("subclave numero %i(en total %i bits): %s\n",i, string_size(subclave_n), subclave_n);

			string_array_push(&subclaves_ascii, subclave_n);
		}

		len = string_array_size(subclaves_ascii);
	}

	printf("En total hay: %i subclaves\n", len);// debe ser 56 donde las ultimas 4 no se usan porque asi es el algoritmo

	for(int i = 0; i<52; i++){
		printf("subclave numero %i (en total %i bits): %s\n",i, string_size(subclaves_ascii[i]), subclaves_ascii[i]);
	}
	free(clave_ascii);

	//string_array_destroy(subclaves_array);
	return subclaves_ascii;
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

void bits_a_rellenar(uint32_t numero_relleno, int cant_disponible, int bits_tam_total, char** buffer){
	char* bits_a_multiplicar = malloc(9);//cada numero son 8 bits
	uint32_t numero_relleno_bytes = numero_relleno /8;
	int_to_binary_string(numero_relleno_bytes, &bits_a_multiplicar, 8);

	int cant_a_repetir = (bits_tam_total - cant_disponible)/8;//lo paso en bytes

	for(int i = 0;i< cant_a_repetir;i++){
		strcat(*buffer, bits_a_multiplicar);
	}
	free(bits_a_multiplicar);
}

//relleno de bits usando la estrategia PKCS5
void agregar_relleno(char*** bloques_array){
	int cant_bloques = string_array_size(*bloques_array);

	char* ultimo_bloque = (*bloques_array)[cant_bloques-1];

	if(string_size(ultimo_bloque) == 64){
		char* nuevo_ultimo_bloque = malloc(65);
		nuevo_ultimo_bloque[0]= '\0';//inicializo vacio

		bits_a_rellenar((uint32_t) 64,0, 64,&nuevo_ultimo_bloque);

		string_array_push(bloques_array, nuevo_ultimo_bloque);
	} else {

		int bits_faltantes = 64 - string_size(ultimo_bloque);
		int tam_ultimo_bloque = string_size(ultimo_bloque);

		ultimo_bloque = realloc(ultimo_bloque, 65);
		ultimo_bloque[tam_ultimo_bloque]= '\0';//inicializo vacio

		bits_a_rellenar((uint32_t) bits_faltantes,tam_ultimo_bloque, 64,&ultimo_bloque);

		string_array_replace(*bloques_array,cant_bloques-1, ultimo_bloque);
	}

	int nueva_cant_bloques = string_array_size(*bloques_array);
	for(int i = 0; i<nueva_cant_bloques ; i++){
		printf("Relleno bloque nro %i: %s\n", i, (*bloques_array)[i]);
	}

}

char** obtener_bloques(char* contenido){
	char** bloques_array = string_array_new();

	char* contenido_bits = string_new();
	int len = string_size(contenido);

	printf("tam contenido: %i\n", len);

	//paso el contenido ascii a bits


	for(int i = 0; i<len ; i++){
		char* bits_caracter_n = malloc(9);
		convert_ascii_to_bits(contenido[i], bits_caracter_n);
		printf("caracter %i: %s\n",i, bits_caracter_n);
		string_append(&contenido_bits,bits_caracter_n);

		free(bits_caracter_n);
	}

	//subdividir el contenido en bloques de 64 bits
	int tam_contenido_bits = string_size(contenido_bits);
	int cant_bloques = tam_contenido_bits /64;


	if(tam_contenido_bits % 64 != 0){
		cant_bloques++;
	}


	for(int i = 0; i< cant_bloques ; i++){
		char * bloque_n= string_substring(contenido_bits,i*64,64);
		string_array_push(&bloques_array, bloque_n);
	}
	free(contenido_bits);
	agregar_relleno(&bloques_array);


	return bloques_array;
}

char**obtener_subbloques(char* bloque_n){
	char** subbloques = string_array_new();

	for(int i = 0; i< 4 ; i++){//son 4 subbloques
		char * subbloque_n= string_substring(bloque_n,i*16,16);
		string_array_push(&subbloques, subbloque_n);
	}

	return subbloques;
}


char*mul_modular(char *x1, char* x2){
	char* resultado = malloc(17);

	uint16_t x1_num = string_to_uint(x1);
	uint16_t x2_num = string_to_uint(x2);
	uint16_t resultado_num;


	uint32_t c = (uint32_t) x1_num * (uint32_t) x2_num;

	// Obtenemos los 16 bits superiores (hi) y los 16 bits inferiores (lo)
	uint16_t hi = (uint16_t)(c >> 16);
	uint16_t lo = (uint16_t)(c & 0xFFFF);

	if (lo >= hi) {
		resultado_num = lo - hi;
	} else {
		// Manejamos el caso de resultado negativo sumando el módulo
		resultado_num = lo - hi + 65537; // 2^16 + 1
	}

	if (resultado_num == 65536) {
		resultado_num = 0;
	}

	int_to_binary_string(resultado_num, &resultado, 16);

	return resultado;
}

char*suma_modular(char *x1, char* x2){
	char* resultado = malloc(17);
	uint16_t x1_num = string_to_uint(x1);
	uint16_t x2_num = string_to_uint(x2);

	uint16_t resultado_num = x1_num +  x2_num;

	int_to_binary_string(resultado_num, &resultado, 16);

	return resultado;
}

char*operacion_xor(char *x1, char* x2){
	char* resultado = malloc(17);

	uint16_t x1_num = string_to_uint(x1);
	uint16_t x2_num = string_to_uint(x2);
	uint16_t resultado_num = x1_num ^ x2_num;

	int_to_binary_string(resultado_num, &resultado, 16);
	return resultado;
}

void ronda(char** subbloques_n, int ronda_num, char** subclaves){

	//a.

	subbloques_n[0] = mul_modular(subbloques_n[0], subclaves[6*ronda_num]);

	subbloques_n[3] = mul_modular(subbloques_n[3], subclaves[6*ronda_num + 3]);

	subbloques_n[1] = suma_modular(subbloques_n[1], subclaves[6*ronda_num + 1]);

	subbloques_n[2] = suma_modular(subbloques_n[2], subclaves[6*ronda_num + 2]);

	//b.

	char* temp_0 = mul_modular( subclaves[6*ronda_num + 4] ,operacion_xor(subbloques_n[0], subbloques_n[2]));

	char* temp_1 = mul_modular( subclaves[6*ronda_num + 5] , suma_modular(temp_0, operacion_xor(subbloques_n[1], subbloques_n[3])));

	char* temp_2 = suma_modular(temp_0, temp_1);

	//c.

	subbloques_n[0] = operacion_xor(subbloques_n[0], temp_1);

	subbloques_n[3] = operacion_xor(subbloques_n[3], temp_2);

	char* a = operacion_xor(subbloques_n[1], temp_2);
	subbloques_n[1] = operacion_xor(subbloques_n[2], temp_1);

	subbloques_n[2] = a;

}

char** media_ronda(char** subbloques_n, char** subclaves){
	char** subbloque_cifrado = string_array_new();

	string_array_push(&subbloque_cifrado, mul_modular(subbloques_n[0], subclaves[48])); //6*8 = 48 que es k1 de la ronda 9

	string_array_push(&subbloque_cifrado, suma_modular(subbloques_n[2], subclaves[49]));  //6*8+1 = 49 que es k2 de la ronda 9

	string_array_push(&subbloque_cifrado, suma_modular(subbloques_n[1], subclaves[50]));  //6*8+2 = 50 k3 de la ronda 9

	string_array_push(&subbloque_cifrado, mul_modular(subbloques_n[3], subclaves[51]));  //6*8+3 = 51 que es k4 de la ronda 9

	return subbloque_cifrado;
}


int main(int argc, char** argv){

    if(argc < 3){
    	printf("Se debe indicar la clave y el nombre del texto !!\n");
        return -1;
    }

    char* path = strdup(argv[1]);
    FILE* f_contenido = fopen(path, "r");

    struct stat stat_file;
	stat(path, &stat_file);//obtengo info del archivo

    char* clave = argv[2];
    char* contenido ;

    if(f_contenido == NULL){
        printf("No se pudo leer el archivo %s\n", argv[1]);
        return -1;
    }


    if( string_size(clave)!= 32){
    	printf("La clave debe ser de 128 bits!! (que son 32 caracteres en hexa)\n");
    	return -1;
    }

    printf("Clave: %s\n",clave);


   contenido = malloc( stat_file.st_size + 1);
   fread(contenido, stat_file.st_size, 1, f_contenido);

   if(contenido == NULL){
	   printf("No se pudo leer el archivo !!\n");
	   return -1;
   }

	printf("Contenido a cifrar:\n %s\n", contenido);

	char** subclaves = generar_subclaves(clave);//genero las 52 subclaves

	char** bloques = obtener_bloques(contenido);//separo el contenido en bloques de 64 bits


	char* cifrado_completo = string_new();

	//cifro cada bloque
	int cant_bloques = string_array_size(bloques);


	printf("Comienzo el cifrado:\n");
	for(int i = 0; i<cant_bloques; i++){

		printf("bloque nro %i: %s\n", i, bloques[i]);

		char** subbloques_n = obtener_subbloques(bloques[i]);//separo el bloque en 4 subbloques de 16 bits


		for(int k = 0; k<4; k++){
			printf("subbloque %i: %s\n", k, subbloques_n[k]);
		}

		//aplico las 8 rondas
		for(int j = 0; j < round ; j++){

			ronda(subbloques_n, j, subclaves);

		}
		//transformación final (media ronda)
		char** subbloques_cifrado_n = media_ronda(subbloques_n, subclaves);

		string_array_destroy(subbloques_n);
		//unifico el array de subbloques cifrado a un unico string
		int count = 0;
		void append_and_print_cifrado(char* subbloque_n){
			printf("cifrado subbloque nro %i: %s\n", count, subbloque_n);

			string_append(&cifrado_completo, subbloque_n);
			count++;
		}

		string_iterate_lines(subbloques_cifrado_n, append_and_print_cifrado);
		printf("Siguiente bloque ...\n");

		string_array_destroy(subbloques_cifrado_n);
	}

	string_array_destroy(bloques);

	char* cifrado_hex = malloc(string_size(cifrado_completo));
	binary_to_hex_string(cifrado_completo, cifrado_hex);

	free(cifrado_completo);

	printf("el texto cifrado en hexa es: %s\n",cifrado_hex);

	char* cifrado_ascii = hex_to_ascii(cifrado_hex);
	printf("el texto cifrado en ascii es: %s\n",cifrado_ascii);

	free(cifrado_ascii);
	free(path);
	free(cifrado_hex);
	free(contenido);

	string_array_destroy(subclaves);
    return 0;
}
