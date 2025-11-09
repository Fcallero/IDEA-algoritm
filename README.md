# Algoritmo IDEA

IDEA es un bloc cipher que emplea una clave de 128 bits y bloques de 64 bits. Utiliza 8 rondas en total mas una "media ronda" que es una transformación final. En Cada ronda utiliza las operaciones XOR, suma módulo 2^16 y multiplicacion módulo 2^16+1.

## Ejemplo de uso
Desencriptar:
```bash
./IDEA.out resultado_cifrado.txt 1234ACB980154D89F1B123F011208965 0 resultado_descifrado.txt
```
Encriptar:
```bash
./IDEA.out contenido.txt 1234ACB980154D89F1B123F011208965 1 resultado_cifrado.txt
```
En general:
```bash
./IDEA.out <ruta-archivo-origen> <clave-128-bits-en-hexa> <1/0> <ruta-archivo-destino>
```
Donde 1 equivale a encriptar y 0 a desencriptar.

## Windows
Si se quiere ejecutar en windows se debe usar el makefile que se encuentra bajo la carpeta llamada "windows".
Se requiere de instalar MSYS y mingw para usar gcc y make.
