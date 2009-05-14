/*
 * Archivo: scanner.h
 * ------------------
 * Descripcion: Este archivo es la interfaz a un modulo que exporta un TAD para
 * facilitar dividir un string 
 * en unidades logicas llamadas "tokens", formados por strings de letras y
 * digitos consecutivos representando palabras, o strings de un caracter
 * represando signos de puntuacion o separadores. Uso: El siguiente fragmento
 * de codigo ejemplifica la obtencion de cada token del string inputString:
 *		scanner = NewScanner();
 *		SetScannerString(scanner, inputString);
 *		while (MoreTokensExist(scanner))
 *			token = ReadToken(scanner);
 *			...
*/

#ifndef _scanner_h
#define _scanner_h

/*
 * IMPORTANTE:  Los caracteres especiales son los siguiente el espacio,
 *              y el enter.
*/

/*
 * Tipo: scannerADT
 * ----------------
 * Este es el tipo abstracto usado para representar una instancia particular de
 * scanner.
*/
typedef struct scannerCDT *scannerADT;

typedef enum {PreserveSeparators, IgnoreSeparators} separatorOptionT;


/* Funcion:         NewScanner
 * Uso:             NewScanner scanner;
 *                  scanner=NewScanner();
 *-----------------------------------------------------------------------------
 * Descripcion:     Crea un nuevo scanner. Devuelve NULL en caso contrario.
 *-----------------------------------------------------------------------------
 * Precondicion:    -
 * Postcondicion:   Scanner creado.
*/
scannerADT NewScanner(void);


/* Funcion:         FreeScanner
 * Uso:             FreeScanner(scanner);
 *-----------------------------------------------------------------------------
 * Descripcion:     Libera el los recursos alocados para el funcionamiento del
 *                  scanner.
 *-----------------------------------------------------------------------------
 * Precondicion:    Scanner creado satisfactoriamente.
 * Postcondicion:   -
*/
void FreeScanner(scannerADT scanner);


/* Funcion:         SetScannerString
 * Uso:             SetScannerString(scanner,string);
 *-----------------------------------------------------------------------------
 * Descripcion:     Setea el scanner que se desea dividir en tokens.
 *-----------------------------------------------------------------------------
 * Precondicion:    Scanner creado satisfactoriamente.
 * Postcondicion:   Scanner con el string seteado.
*/
void SetScannerString(scannerADT scanner, char * str);


/* Funcion:         ReadToken
 * Uso:             token=ReadToken(scanner);
 *-----------------------------------------------------------------------------
 * Descripcion:     ReadToken retorna el siguiente token desde el scanner. Si
 *                  es llamada cuando no hay tokens disponibles, retorna una
 *                  cadena vacia. El token devuelto por ReadToken se encuentra
 *                  alocado en el heap, por lo que el cliente puede llamar a la
 *                  funciin free cuando no se lo necesita mas.
 *-----------------------------------------------------------------------------
 * Precondicion:    Scanner creado satisfactoriamente.
 * Postcondicion:   Devuelve un string.
*/
char * ReadToken(scannerADT scanner);


/* Funcion:         MoreTokensExist
 * Uso:             MoreTokensExist(scanner);
 *-----------------------------------------------------------------------------
 * Descripcion:     La funcion devuelve 1 si existen mas substring dentro del
 *                  string seteado al inicio. 0 en caso contrario.
 *-----------------------------------------------------------------------------
 * Precondicion:    Scanner creado satisfactoriamente.
 * Postcondicion:   Numero entero (0 o 1).
*/
int MoreTokensExist(scannerADT scanner);


/* Funcion:         SaveToken
 * Uso:             SaveToken(scanner,token);
 *-----------------------------------------------------------------------------
 * Descripcion:     Guarda el token en la estructura de datos del scanner de
 *                  manera tal que al llamar nuevamente a ReadToken, devuelva
 *                  ese msimo token.
 *-----------------------------------------------------------------------------
 * Precondicion:    Scanner creado satisfactoriamente.
 * Postcondicion:   String
*/
void SaveToken(scannerADT, char * token);


/* Funcion:         SetScannerSeparatorOption
 * Uso:             SetScannerSeparatorOption(scanner,PreserveSeparators);
 *-----------------------------------------------------------------------------
 * Descripcion:     La funcion setea si debe considerar los caracteres
 *                  especiales como separadores o como tokens validos.
 *-----------------------------------------------------------------------------
 * Precondicion:    Scanner creado satisfactoriamente.
 * Postcondicion:   String
*/
void SetScannerSeparatorOption(scannerADT scanner, separatorOptionT option);


/* Funcion:         GetScannerSeparatorOption
 * Uso:             GetScannerSeparatorOption(scanner,PreserveSeparators);
 *-----------------------------------------------------------------------------
 * Descripcion:     La funcion devuelve si el scanner considera los caracteres
 *                  especiales como separadores o como tokens validos.
 *-----------------------------------------------------------------------------
 * Precondicion:    Scanner creado satisfactoriamente.
 * Postcondicion:   Numero entero.
*/
separatorOptionT GetScannerSeparatorOption(scannerADT scanner);

#endif
