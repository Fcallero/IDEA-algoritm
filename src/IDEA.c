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


char* suma_modular_inversa(char*z1){
	char* resultado = malloc(17);
	uint16_t x1_num = string_to_uint(z1);
	uint32_t x2_num = 65536; //== 2^16

	uint16_t resultado_num = x2_num - x1_num;// == 2^16 - K_i mod(2^16)

	int_to_binary_string(resultado_num, &resultado, 16);

	return resultado;
}

//devuelve un numero x1 tal que (z1*x1) mod (2^16 + 1) == 1. Se usa el algorimo de euclides para ello
char* mul_modular_inversa(char*z1){
	char* resultado = malloc(17);
	uint16_t x1_num = string_to_uint(z1);
	uint16_t resultado_num = 0;

	 int32_t t = 0, new_t = 1;
	int32_t resto = 65537, nuevo_resto = x1_num;// r == 2^16 + 1
	int32_t temp;

	//algoritmo de euclides para allar el MCD entre z1 y 2^16 + 1
	while (nuevo_resto != 0) { // es == a parar si resto == 1
		int32_t q = resto / nuevo_resto;

		temp = t;
		t = new_t;
		new_t = temp - q * new_t;

		temp = resto;
		resto = nuevo_resto;
		nuevo_resto = temp - q * nuevo_resto;
	}


	// Si el inverso es negativo, lo ajustamos
	if (t < 0)
		t += 65537;//sumo 2^16 + 1 para que se vuelva positivo

	resultado_num = (uint16_t)t;


	int_to_binary_string(resultado_num, &resultado, 16);

	return resultado;
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

	//free(clave_ascii);

	//string_array_destroy(subclaves_array);
	return subclaves_ascii;
}

char** generar_subclaves_desencriptar(char *clave){
	char **subclaves_array = generar_subclaves(clave);//genero las 52 claves normalmente
	char** subclaves_array_desencriptar = string_array_new();

	printf("Subclaves para la desencripción:\n");

	//subclaves ronda 1
	string_array_push(&subclaves_array_desencriptar, mul_modular_inversa(subclaves_array[48])); // 48 = 6* 8 es decir, k1 de la ronda 9

	string_array_push(&subclaves_array_desencriptar, suma_modular_inversa(subclaves_array[49])); // 49 = 6* 8 + 1 es decir, k2 de la ronda 9

	string_array_push(&subclaves_array_desencriptar, suma_modular_inversa(subclaves_array[50])); // 50 = 6* 8 + 2 es decir, k3 de la ronda 9

	string_array_push(&subclaves_array_desencriptar, mul_modular_inversa(subclaves_array[51])); // 51 = 6* 8 + 3 es decir, k4 de la ronda 9

	string_array_push(&subclaves_array_desencriptar, subclaves_array[46]); // 46 = 6* 7 + 4 es decir, k5 de la ronda 8

	string_array_push(&subclaves_array_desencriptar, subclaves_array[47]); // 47 = 6* 7 + 5 es decir, k6 de la ronda 8

	//subclaves de la ronda 2 a la 8
	for(int ronda = 1; ronda < 8; ronda++){

		string_array_push(&subclaves_array_desencriptar, mul_modular_inversa(subclaves_array[6*(10-ronda-2)]));// k1 de la ronda (10-ronda)

		string_array_push(&subclaves_array_desencriptar, suma_modular_inversa(subclaves_array[6*(10-ronda-2) + 2])); // k3 de la ronda (10-ronda)

		string_array_push(&subclaves_array_desencriptar, suma_modular_inversa(subclaves_array[6*(10-ronda-2) + 1])); // k2 de la ronda (10-ronda)

		string_array_push(&subclaves_array_desencriptar, mul_modular_inversa(subclaves_array[6*(10-ronda-2) + 3]));// k4 de la ronda (10-ronda)

		string_array_push(&subclaves_array_desencriptar, subclaves_array[6*(9-ronda-2) + 4]);// k5 de la ronda (9-ronda)

		string_array_push(&subclaves_array_desencriptar, subclaves_array[6*(9-ronda-2) + 5]);// k6 de la ronda (9-ronda)
	}
	//subclaves ronda 9

	string_array_push(&subclaves_array_desencriptar, mul_modular_inversa(subclaves_array[0]));// 0 = 6* 0  es decir, k1 de la ronda 1

	string_array_push(&subclaves_array_desencriptar, suma_modular_inversa(subclaves_array[1])); // 1 = 6* 0 + 1 es decir, k2 de la ronda 1

	string_array_push(&subclaves_array_desencriptar, suma_modular_inversa(subclaves_array[2])); // 2 = 6* 0 + 2 es decir, k3 de la ronda 1

	string_array_push(&subclaves_array_desencriptar, mul_modular_inversa(subclaves_array[3]));// 3 = 6* 0+3  es decir, k4 de la ronda 1

	return subclaves_array_desencriptar;
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
void agregar_relleno(char*** bloques_array, int es_cifrado){
	int cant_bloques = string_array_size(*bloques_array);

	char* ultimo_bloque = (*bloques_array)[cant_bloques-1];

	if(string_size(ultimo_bloque) == 64 && es_cifrado){
		char* nuevo_ultimo_bloque = malloc(65);
		nuevo_ultimo_bloque[0]= '\0';//inicializo vacio

		bits_a_rellenar((uint32_t) 64,0, 64,&nuevo_ultimo_bloque);

		string_array_push(bloques_array, nuevo_ultimo_bloque);
	} else if(es_cifrado) {

		int bits_faltantes = 64 - string_size(ultimo_bloque);
		int tam_ultimo_bloque = string_size(ultimo_bloque);

		ultimo_bloque = realloc(ultimo_bloque, 65);
		ultimo_bloque[tam_ultimo_bloque]= '\0';//inicializo vacio

		bits_a_rellenar((uint32_t) bits_faltantes,tam_ultimo_bloque, 64,&ultimo_bloque);

		string_array_replace(*bloques_array,cant_bloques-1, ultimo_bloque);
	}//si se decifra, no habria que agregar relleno!!

	int nueva_cant_bloques = string_array_size(*bloques_array);
	for(int i = 0; i<nueva_cant_bloques ; i++){
		printf("Relleno bloque nro %i: %s\n", i, (*bloques_array)[i]);
	}

}

void sacar_bytes_relleno(char** descifrado_completo){

	int tam_descifrado = string_size(*descifrado_completo);
	int indice_ultimo_bloque =tam_descifrado-64;
	char* ultimo_bloque = string_substring_from(*descifrado_completo, indice_ultimo_bloque);
	uint8_t * nums = malloc(8*(sizeof(uint8_t)));
	for(int i = 0; i< 8; i++){
	  char* caracter_n = string_substring(ultimo_bloque, 8*i,8);
	  printf("caracter_n es: %s\n", caracter_n);
	  nums[i] = (uint8_t) string_to_uint(caracter_n);
	}

	if(nums[7] == 64){
		char* temp = string_substring_until(*descifrado_completo, tam_descifrado-64);
		free(*descifrado_completo);
		*descifrado_completo=temp;
	} else if( nums[7] <= 7) {
		int count = 0;
		for(int i = nums[7]; nums[i]==nums[7] ; i--){
			count++;
		}
		if(count == nums[7]){
			char* temp = string_substring_until(*descifrado_completo, tam_descifrado-(nums[7]*8));
			free(*descifrado_completo);
			*descifrado_completo=temp;
		}
	}
	printf("Quedó el descrifrado como: %s\n", *descifrado_completo);
	printf("ultimo bloque es: %s\n", ultimo_bloque);

}

char** obtener_bloques(char* contenido, int es_cifrado){
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
	agregar_relleno(&bloques_array, es_cifrado);


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

    if(argc < 5){
    	printf("Se debe indicar la clave, el nombre del archivo txt, si se quiere encriptar (1) o desencriptar(0) y la ruta al archivo con el resultado (debe ser txt) !!\n");
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

    char* es_encriptar = strdup(argv[3]);
    int es_cifrado = strcmp(es_encriptar, "1") == 0 ? 1 : 0;

    if(es_cifrado){
    	printf("Cifrado:\n");
    }else{
    	printf("Decifrado:\n");
    }

    printf("Clave: %s\n",clave);

   contenido = malloc( stat_file.st_size + 1);
   fread(contenido, stat_file.st_size, 1, f_contenido);

   contenido[stat_file.st_size] = '\0';//agrego fin de caracter

   if(contenido == NULL){
	   printf("No se pudo leer el archivo !!\n");
	   return -1;
   }

   char** subclaves;
   if(es_cifrado){
	   printf("Contenido a cifrar:\n %s\n", contenido);


	   subclaves = generar_subclaves(clave);//genero las 52 subclaves
   }else {
	   printf("Contenido a decifrar:\n %s\n", contenido);
	   subclaves = generar_subclaves_desencriptar(clave);//genero las 52 subclaves
   }

   int cant_subclaves = string_array_size(subclaves);

   printf("En total hay: %i subclaves\n", cant_subclaves);

   	for(int i = 0; i<52; i++){
   		printf("subclave numero %i (en total %i bits): %s\n",i, string_size(subclaves[i]), subclaves[i]);
   	}

	char** bloques = obtener_bloques(contenido, es_cifrado);//separo el contenido en bloques de 64 bits


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
			if(es_cifrado){
				printf("cifrado subbloque nro %i: %s\n", count, subbloque_n);
			} else {
				printf("descifrado subbloque nro %i: %s\n", count, subbloque_n);
			}

			string_append(&cifrado_completo, subbloque_n);
			count++;
		}

		string_iterate_lines(subbloques_cifrado_n, append_and_print_cifrado);
		printf("Siguiente bloque ...\n");

		string_array_destroy(subbloques_cifrado_n);
	}

	string_array_destroy(bloques);

	if(!es_cifrado){
		sacar_bytes_relleno(&cifrado_completo);
	}

	char* cifrado_hex = malloc(string_size(cifrado_completo));
	binary_to_hex_string(cifrado_completo, cifrado_hex);

	free(cifrado_completo);

	char* cifrado_ascii = hex_to_ascii(cifrado_hex);

	if(es_cifrado){
		printf("el texto cifrado en hexa es: %s\n",cifrado_hex);
		printf("el texto cifrado en ascii es: %s\n",cifrado_ascii);
	}else {
		printf("el texto descifrado en hexa es: %s\n",cifrado_hex);
		printf("el texto descifrado en ascii es: %s\n",cifrado_ascii);
	}

	fclose(f_contenido);
	char* path_resultado = strdup(argv[4]);
	FILE* f_contenido_resultado = fopen(path_resultado, "w");

	fwrite(cifrado_ascii,1,string_size(cifrado_ascii), f_contenido_resultado);

	fclose(f_contenido_resultado);

	printf("Resultado guardado en: %s\n", path_resultado);


	free(cifrado_ascii);
	free(path);
	free(cifrado_hex);
	free(contenido);
	free(es_encriptar);

	string_array_destroy(subclaves);
    return 0;
}
