/**********************************************************************
   Programa para control del SERVO DIGITAL V4 con motor de paso-paso
				  Por Raimundo Alfonso Sánchez
		     Grupo de Control de Procesos (C.S.I.C.)
					 REF. PROGRAMA : 'SVMD3'

 Control de versiones:
 v4.00  : 07/09/2004
    - Version inicial

 v4.01  : 21/03/2005
    - Nuevo parametro para mandar posicion desde comunicaciones (modo posicionamiento)
 v4.01E  : 25/05/2006
    - Se cambia la constante MAX_POSICIONES para la valvula VICI de 6 posiciones (antes 8).


***********************************************************************/

#define VERSION         "v4.01"
#define REFERENCIA      "SVMD4"

#define FALSE   0
#define TRUE    1

#define V3_03
#define V3_04

#include <inlines.h>
#include <H83437f.h>
#include <math.h>


#define WDT_TCSR_W  (* (volatile unsigned short *)       ( 0xffa8 ))
#define WDT_TCNT_W  (* (volatile unsigned short *)       ( 0xffa8 ))
#define WDT_RESET WDT_TCNT_W = 0x5A00			// Reset WDT

#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NAK 0x15
#define CR  0x0D

// Definiciones para las tramas de comunicación
#define TAM_BUFFER_RX 		20  // Tamaño maximo de buffer para respuestas y tranmisiones
#define TAM_DIR 			2   // Tamaño cadena 'direccion'
#define DIR_ABS				00	// Direccion absoluta

#define RS485RX   		P5DR &= 0xFB;  	// Recepcion RS485
#define RS485TX   		P5DR |= 0x04;  	// Transmision RS485
#define LED_TX_ON  		P1DR |= 0x01  	// Led transmision RS485 ON
#define LED_TX_OFF 		P1DR &= 0xFE  	// Led transmision RS485 OFF
#define LED_RX_ON  		P1DR |= 0x04  	// Led recepcion RS485 ON
#define LED_RX_OFF 		P1DR &= ~0x04 	// Led recepcion RS485 OFF
#define LED_ERROR_ON  	P1DR |= 0x02  	// Led error RS485 ON
#define LED_ERROR_OFF 	P1DR &= ~0x02  	// Led error RS485 OFF
#define RELE_ON  		P1DR |= 0x80  	// Rele de alarma on
#define RELE_OFF 		P1DR &= ~0x80  	// Rele de alarma off
#define BUZZ_ON    		P8DR |= 0x40  	// Buzzer on
#define BUZZ_OFF   		P8DR &= ~0x40 	// Buzzer off
#define MODO_MOTOR	    ((P9DR & 0x08) == 0x08)				//Modo servo/posicionador
#define MODO_LOCAL      ((P9DR & 0x80) == 0x00)				//Modo nº vueltas remoto/local
#define MODO_L          ((P9DR & 0x80) == 0x00)				//Giro a la izquierda
#define NUMERO_VUELTAS_LOCAL	(((P9DR & 0x70) >> 4) + 1)	//Numero de vueltas seleccionado:
															// V2 V1 V0
															// --------------------
															// 0  0  0  : 1 vuelta
															// 0  0  1  : 2 vueltas
															// 0  1  0  : 3 vueltas
															// 0  1  1  : 4 vueltas
															// 1  0  0  : 5 vueltas
															// 1  0  1  : 6 vueltas
															// 1  1  0  : 7 vueltas
															// 1  1  1  : 8 vueltas
#define ENTRADAS_9_16	P7PIN
#define ENTRADA_ZERO	((P7PIN & 0x01) == 0x01)			// Señal externa de cero valvula
#define BOTON_ABRIR     ((P8DR & 0x02) == 0x02)				// Boton de abrir on
#define BOTON_CERRAR    ((P8DR & 0x01) == 0x01)				// Boton de cerrar on

// Definiciones para el driver del motor
#define MOTOR_ENABLE	P6DR |= 0x80
#define MOTOR_DISABLE	P6DR &= ~0x80
#define MOTOR_RESET_ON	P6DR &= ~0x40
#define MOTOR_RESET_OFF	P6DR |= 0x40
#define MOTOR_DIR_L		P6DR |= 0x02
#define MOTOR_DIR_R		P6DR &= ~0x02
#define MOTOR_CLK_H		P6DR |= 0x01
#define MOTOR_CLK_L		P6DR &= ~0x01
#define MOTOR_JP6		((P3DR & 0x40) == 0x40)
#define MOTOR_JP7		((P3DR & 0x80) == 0x80)
#define MOTOR_S0		0x04
#define MOTOR_S1		0x08
#define MOTOR_S2		0x10
#define MOTOR_S3		0x20
#define MOTOR_PASOS		25600						// Numero de pasos por revolucion
   													// Cambiado de 12500 a 25600 a 1/4/2003
#define MOTOR_HISTERESIS		2					// Histeresis en milivoltios (rango de 0 a 5 voltios)
#define MOTOR_DESVIACION_MAX	0.10 * MOTOR_PASOS	// SUPONIENDO QUE EL POTENCIOMETRO TIENE UNA TOLERANCIA DEL 1%...  (9.900K < 10K < 10.100K)
													// Maxima desviación permitida en el control de posicion (36º) (% por 1 de 360º)
													
#define POT_POSICION_INICIAL	400					// Posicion a la que va el motor cuando se seleccional el jumper DIR_6 (2 vuelta)
#define POT_LIMITE_SUP			1950				// Limite superior para el potenciometro (9.75 vuelta)
#define POT_LIMITE_INF			50					// Limite inferior para el potenciometro (0.25 vuelta)

// Definiciones para el modo posicionador
#define MAX_POSICIONES		6			// Numero máximo de posiciones
#define RELACION			80/12		// Relacion de desmultiplicacion por engranajes
#define ENGRA_P				12			// Numero de dientes engranaje motor
#define ENGRA_G				80			// Numero de dientes engranaje eje válvula
#define MOTOR_RAMPA			45			// Angulo de rampa incial y final (solo modo posicionador)


// Definiciones para el convertidor ADS1211
#define ADS1211_PUERTO  P2DR            // Puerto usado para el ADC
#define ADS1211_SCLK_L  P2DR |= 0x08    // Reloj
#define ADS1211_SCLK_H  P2DR &= ~0x08   // Reloj
#define ADS1211_SDIO_L  P2DR |= 0x10    // Datos de salida hacia el ADC
#define ADS1211_SDIO_H  P2DR &= ~0x10   // Datos de salida hacia el ADC
#define ADS1211_DRDY    0x40            // Datos listos
#define ADS1211_SDOUT   0x20            // Datos de entrada desde el ADC
#define GAINx1          0
#define GAINx2          1
#define GAINx4          2
#define GAINx8          3
#define GAINx16         4
#define ADS1211_NORMAL                  0
#define ADS1211_SELF_CALIBRATION        1
#define ADS1211_OFFSET_CALIBRATION      2
#define ADS1211_FULL_SCALE_CALIBRATION  3
#define ADS1211_PSEUDO_CALIBRATION      4
#define ADS1211_BACKGROUND_CALIBRATION  5
#define ADS1211_SLEEP                   6
//#define VALOR_DECIMACION    			391             // 50Hz
#define VALOR_DECIMACION    			195             // 100Hz
//#define VALOR_DECIMACION    			78              // 250Hz

// Definiciones y macros para memoria EEPROM AT24C64 en puerto 4
// P40 -> SCL
// P41 -> SDA
#define SCL_0   P4DR &= 0xFE
#define SCL_1   P4DR |= 0x01
#define SDA_0   P4DR &= 0xFD
#define SDA_1   P4DR |= 0x02
#define SDA_IN  modo_puerto_4 &= 0xFD   // SDA en modo entrada
#define SDA_IN1 P4DDR = modo_puerto_4   // SDA en modo entrada
#define SDA_OUT  modo_puerto_4 |= 0x02   // SDA en modo salida
#define SDA_OUT1 P4DDR = modo_puerto_4   // SDA en modo salida
#define P_EEPROM    P4DR        // Puerto utilizado para comunicacion con EEPROM
#define PC_EEPROM   P4DDR       // Registro de configuracion para el puerto EEPROM

// Maximos y minimos de parametros...
#define MIN_MOTOR_VUELTAS  	360
#define MAX_MOTOR_VUELTAS  	3240	// 9 vueltas
#define MAX_LIM				30000
#define MIN_LIM			    -30000
#define MIN_MOTOR_CERO		180 	// 0.5 vueltas
#define MAX_MOTOR_CERO		1800	// 5 vueltas

// Direcciones de parametros configurables en EEPROM...
// Valores de 1 byte...
#define DIR_TIPO_CANAL		0		//4*1 	= 4 bytes
#define DIR_COMM			4		//1*1   = 1 byte
#define DIR_REF				10		//5*1	= 5 bytes
#define DIR_VER				15    	//5*1	= 5 bytes
#define DIR_VALOR_VARIOS	20		//4*1	= 4 bytes
// valores de 2 bytes...
#define DIR_VALOR_MAXIMO	100		//4*2 	= 8 bytes
#define DIR_VALOR_MINIMO	108		//4*2 	= 8 bytes
#define DIR_VALOR_OFFSET	116		//4*2 	= 8 bytes
#define DIR_MOTOR_VUELTAS	124		//1*2	= 2 bytes
#define DIR_MOTOR_CERO		126		//1*2   = 2 bytes

// Variables globales
char modo_conf;
char modo_puerto_4;             // Modo para el puerto 4
unsigned int guiTimer;          // Temporizador 1 mSegundo int. comunicaciones
unsigned int guiTimer1;         // Temporizador 1 mSegundo 
unsigned int guiTimerSeg;       // Temporizador 1 mSegundo
char acBuffer[TAM_BUFFER_RX];
char acDirLocal[TAM_DIR];
unsigned int segundos = 0;      // Temporizador 1 Segundo
unsigned int timerZumb = 0;     // Temporizador para el zumbador
char error_rx = 0;              // Bandera de error de recepcion

//Variables de radam
char tipo_canal[4];             // Tipo de canal [canal]
float valor[4];                 // Valores de conversión [canal]
float valor_ant[4];				// Valores de conversión para filtro digital [canal]
signed int valor_minimo[4];  	// Tabla parametros valores minimos
signed int valor_maximo[4];  	// Tabla parametros valores maximos
signed int valor_offset[4];  	// Tabla parametros offset
char valor_varios[4];        	// Tabla parametros filtros (l)
                                //   Bits 0-3 : filtro de 0 a 10
// Valores de comunicacion
unsigned int TIME_OUT_BYTE;         // Time out para recepcion a nivel byte (mS)
unsigned int TIME_TXT;              // Tiempo de transmision entre caracteres (uS)
unsigned int TIME_RX_TX;            // Tiempo de espera entre recepcion y transmision (mS)
char comm;                      	// Comunicaciones          

// Variables para el control del motor
unsigned int motor_vueltas;			// Numero de vueltas máximas (en grados)
long posicion_abs = 0;				// Posicion inicial absoluta (en pasos)
long posicion_rel = 0;				// Posicion relativa (en pasos)
unsigned int motor_posicion = 0;	// Posicion absoluta del motor en grados
char error_motor = 0;				// Sin errores en el motor
unsigned int motor_cero = 0;		// Posicion del cero comprendido entre 0.5(0.1v) y 5(1v) vueltas.

// Variables para el modo posicionador
char posicion = 0;						// Lugar de posicionamiento
signed int posicion_grados_actual;		// Posicion actual en grados
signed int posicion_grados_anterior;	// Posicion anterior en grados

// Declaraciónn de procedimientos 
void inicio (void);
void int_TMR0 (void);
void int_TMR1 (void);
void int_TMR16_OVF (void);
void int_recepcion_SCI0(void);
char calculaBCC (char *pBuffer , char cLongitud);
void decodificaMensaje_GASES(void);
void error_com (unsigned int numero);
void montaCadena_s (signed int numero, char longitud);
void montaCadena_i (unsigned int numero, char longitud);
void montaCadena_l (unsigned long numero, char longitud);
void escritura_correcta(void);
void itoa_ray(unsigned int uNumero, char *pCadena, char cLongitud);
void stoa_ray (signed int sNumero, char *pCadena, char cLongitud);
void btoa_ray(char cNumero, char *pCadena, char cLongitud);
char atoi_ray (char *pCadena, unsigned int *uNumero);
char atos_ray (char *pCadena, signed int *sNumero, char tamax);
void ltoa_ray (unsigned long lNumero, char *pCadena, char cLongitud);
char atol_ray (char *pCadena, unsigned long *uNumero);
void transmiteCadena (char *pCadena, char cLongitud);
void transmiteByte (char dato);
char recibeByte (char *dato);
void esperaMili (unsigned int);
void esperaMili1 (unsigned int);
void esperaMicro (unsigned int t);
void dectohex_8_ray (char numero, char *pCadena);
void filtrado_valores (unsigned int n);
void buzzer(unsigned int tiempo);
void comprueba_parametros (void);
unsigned int lee_entrada_analogica(char canal);
void motor_micropaso(unsigned int paso);
void calibra_motor(unsigned int);
void comprueba_configuracion_motor(void);
void mueve_motor(unsigned int grados);
char mueve_motor_zero(void);


/******************************************************************
    Funciones utilizadas para control de EEPROM externa serie
 ******************************************************************/
char w_byte_eeprom (char valor);
char r_byte_eeprom (void);
void eeprom_start(void);
void eeprom_stop(void);
void leeTipoEEprom     (unsigned int dir, void *param, char lg);
void escribeTipoEEprom (unsigned int dir, void *param, char lg);
void escribe_eeprom_24C16 (unsigned int dir, char dato);
char lee_eeprom_24C16 (unsigned int dir);
/*******************************************************************/

/******************************************************************
    Funciones utilizadas para control ADS1211 de BURR-BROWN
 ******************************************************************/
void inicializa_ADS1211(void);
char ADS1211_init(void);
char ADS1211_decimation(unsigned int valor);
char ADS1211_ganancia(char gain);
void ADS1211_modo(char modo);
unsigned long ADS1211_lee_valor(void);
char ADS1211_ready (void);
char ADS1211_canal (char canal);
void ADS1211_escribe (char dato);
char ADS1211_lee (void);


__inline__ void set_interrupt_mask (unsigned char mask)
{
  asm ("and.b #03,%0l"::"r"(mask));
  asm ("rotr.b r0l");
  asm ("rotr.b r0l");
  asm ("ldc r0l,ccr");
}

__inline__ void set_ccr (unsigned char mask)
{
  asm ("ldc %0l,ccr"::"r"(mask));
}


//***********************************//
//     Rutinas de Interrupcion       //
//***********************************//


#pragma interrupt
// Rutina de atencion a interrupcion error en recepcion serie SCI0
void int_error_recepcion_SCI0(void){
        error_rx = 1;           // Error en recepcion
        SCI0_SSR &= 0x8F;       // Borra flag de registro lleno
}

#pragma interrupt
// Rutina de atencion a interrupcion recepsion serie SCI0
void int_recepcion_SCI0(void){
        char cCaracter;         // Caracter de recepcion
        char cBCC;              // Caracter de BCC
        char cNumero = 0;       // Contador de caracteres

        SCI0_SCR &= 0xBF;       // Desactiva interrupcion recepcion
        set_ccr(0);             // Activa interrupciones generales

        LED_RX_ON;

        if(error_rx == 1){      // Si hay Overrun, framing, parity error...
            error_rx = 0;       // Anula error
            SCI0_SSR &= 0x8F;   // Borra flag de registro lleno
            SCI0_SCR |= 0x40;   // activa interrupcion recepcion
            LED_RX_OFF;
            return;             // Time out byte
        }

        // ****** Lectura de cadena completa  ******

        cCaracter = SCI0_RDR;   // Captura caracter
        SCI0_SSR &= 0x8F;       // Borra flag de registro lleno
        while(cCaracter == STX){
            acBuffer[cNumero] = cCaracter;
            while(cCaracter != ETX){            // Coge caracteres hasta ETX...
                guiTimer = 0;
                while(!(SCI0_SSR & 0x40)){      // Mientras no se reciba...
                    if(guiTimer >= TIME_OUT_BYTE){
                        SCI0_SSR &= 0x8F;       // Borra flag de registro lleno
                        SCI0_SCR |= 0x40;       // activa interrupcion recepcion
                        LED_RX_OFF;
                        return;                 // Time out byte
                    }
                }

                if(error_rx == 1){      // Si hay Overrun, framing, parity error...
                    error_rx = 0;       // Anula error
                    SCI0_SSR &= 0x8F;   // Borra flag de registro lleno
                    SCI0_SCR |= 0x40;   // activa interrupcion recepcion
                    LED_RX_OFF;
                    return;             // Time out byte
                }

                cCaracter=SCI0_RDR;             // Captura caracter
                SCI0_SSR &= 0x8F;               // Borra flag de registro lleno
                cNumero++;
                acBuffer[cNumero] = cCaracter;  // Guarda caracter
                if(cNumero >= (TAM_BUFFER_RX - 1)){     // Si hay desboradamiento en buffer
                         SCI0_SSR &= 0x8F;       // Borra flag de registro lleno
                        SCI0_SCR |= 0x40;       // activa interrupcion recepcion
                        LED_RX_OFF;
                        return;                 // Time out byte
                }
            }
        }

        // ****** Lectura de BCC ******
        guiTimer = 0;
        while(!(SCI0_SSR & 0x40)){      // Mientras no se reciba...
            if(guiTimer >= TIME_OUT_BYTE){
                SCI0_SSR &= 0x8F;       // Borra flag de registro lleno
                SCI0_SCR |= 0x40;       // activa interrupcion recepcion
                LED_RX_OFF;
                return;                 // Time out byte
            }
        }

        if(error_rx == 1){      // Si hay Overrun, framing, parity error...
            error_rx = 0;       // Anula error
            SCI0_SSR &= 0x8F;   // Borra flag de registro lleno
            SCI0_SCR |= 0x40;   // activa interrupcion recepcion
            LED_RX_OFF;
            return;             // Time out byte
        }

        cBCC=SCI0_RDR;          // Captura BCC
        SCI0_SSR &= 0x8F;       // Borra flag de registro lleno

        if(calculaBCC(acBuffer,cNumero) != cBCC){   // Si el BCC es incorrecto....
            SCI0_SSR &= 0x8F;   // Borra flag de registro lleno
            SCI0_SCR |= 0x40;   // activa interrupcion recepcion
            LED_RX_OFF;
            return;             // Time out byte
        }

        /****** Decodificacion del mensaje recibido ******/
        LED_RX_OFF;
        LED_TX_ON;
        decodificaMensaje_GASES();    // Decodificacion mensaje
        LED_TX_OFF;

        SCI0_SSR &= 0x8F;       // Borra flag de registro lleno
        SCI0_SCR |= 0x40;       // activa interrupcion recepcion
}

#pragma interrupt
// Rutina de reloj 1 mS para sincronizaci¢n del programa
void int_TMR0 (void){
    TMR0_TCSR &= 0x7F;      // Reset flag int. comparador B

	WDT_RESET;

    if(guiTimerSeg >= 1000){
        guiTimerSeg = 0;
        segundos++;
    }
    guiTimer++;             // incrementa timer int comunicaciones
    guiTimer1++;            // incrementa timer 
    guiTimerSeg++;          // incrementa timer segundos
}


#pragma interrupt
// Rutina de timer1 para control del motor (100uS)
void int_TMR1 (void){
    TMR1_TCSR &= 0x7F;      // Reset flag int. comparador B

    if(posicion_abs != posicion_rel){
		if(posicion_abs < posicion_rel){
			MOTOR_DIR_R;
	    	MOTOR_CLK_H;
	    	MOTOR_CLK_L;
			posicion_abs++;
		}else{
			MOTOR_DIR_L;
	    	MOTOR_CLK_H;
	    	MOTOR_CLK_L;
			posicion_abs--;
		}
	}
}

#pragma interrupt
// Rutina de atencion al overflow timer 16 bit
void int_TMR16_OVF (void){
	TCSR &= ~0x02;		// Borra flag overflow
}	


// Rutina de inicio de perifericos y sistema
void inicio (void){
    // Configuraci¢n puertos
    // 0 -> entrada
    // 1 -> salida
    P5DDR |= 0x04;          // P52 salida 485-in/out
    // Puerto 1 utilizado para el control de led y salidas reles de alarmas.
    P1DDR = 0xff;           // Bit 0 : salida led TX-RS485         (LTX)
                            // Bit 1 : salida led RX-RS485         (LRX)
                            // Bit 2 : salida led ERROR            (ERR)
                            // Bit 3 : salida rele R1     		   (R1_ON)
                            // Bit 4 : salida rele R2              (R2_ON)
                            // Bit 5 : salida rele R3              (R3_ON)
                            // Bit 6 : salida rele R4              (R4_ON)
                            // Bit 7 : salida relé de alarma	
    // Puerto 2 utilizado integramente para el control del ADS1211
    P2DDR = 0x18;           // Bit 0 : entrada posicion 7			(P20)
                            // Bit 1 : entrada posicion 8			(P21)
                            // Bit 2 : N/C                         
                            // Bit 3 : salida SCLK ADS1211         (SCLK)
                            // Bit 4 : salida SDIO ADS1211         (SDIO)
                            // Bit 5 : entrada SDOUT ADS1211       (SDOUT)
                            // Bit 6 : entrada DRDY ADS1211        (DRDY)
                            // Bit 7 : N/C
    P2DR = 0x00;            // Desactiva salidas
    // Puerto 3 utilizado integramente para las direcciones.
    P3DDR = 0x00;           // Bit 0 : entrada DIR0
                            // Bit 1 : entrada DIR1
                            // Bit 2 : entrada DIR2
                            // Bit 3 : entrada DIR3
                            // Bit 4 : entrada DIR4
                            // Bit 5 : entrada DIR5
                            // Bit 6 : entrada CONFIGURACION VUELTA INICIAL POR DEFECTO
                            // Bit 7 : entrada CONFIGURACION DEL CERO
    P3PCR = 0xFF;           // Activa Pull-up de todos los bits
    // Puerto 4 utilizado para la memoria EEPROM.
    P4DDR = 0x03;           // Bit 0 : salida para SCL              (SCL)
                            // Bit 1 : salida para SDA              (SDA)
                            // Bit 2 : entrada posicion 1			(P42)
                            // Bit 3 : entrada posicion 2			(P43)
                            // Bit 4 : entrada posicion 3			(P44)
                            // Bit 5 : entrada posicion 4			(P45)
                            // Bit 6 : entrada posicion 5			(P46)
                            // Bit 7 : entrada posicion 6			(P47)
    modo_puerto_4 = 0x03;
    P4DR  |= 0x03;          // SDA y SCL activados

    // Puerto 6 utilizado para el control del motor.
    P6DDR = 0xff;           // Bit 0 : salida pulsos            	(CLK)
                            // Bit 1 : salida direccion        		(DIR)
                            // Bit 2 : salida s0					(S0)
                            // Bit 3 : salida s1					(S1)
                            // Bit 4 : salida s2					(S2)
                            // Bit 5 : salida s3					(S3)
                            // Bit 6 : salida reset driver			(RST_MOT)
                            // Bit 7 : salida enable driver			(ENABLE)
    P6DR = 0x00;            // Driver desactivado
    
    // Puerto 7 utilizado para leer posiciones 9 a 16
    				        // Bit 0 : entrada posicion 9			(P70)
                            // Bit 1 : entrada posicion 10			(P71)
                            // Bit 2 : entrada posicion 11			(P72)
                            // Bit 3 : entrada posicion 12			(P73)
                            // Bit 4 : entrada posicion 13			(P74)
                            // Bit 5 : entrada posicion 14			(P75)
                            // Bit 6 : entrada posicion 15			(P76)
                            // Bit 7 : entrada posicion 16			(P77)
    // Puerto 8 utilizado para programacion y salida buzzer
    P8DDR = 0x70;           // Bit 0 : BOTON DE CERRAR
                            // Bit 1 : BOTON DE ABRIR
                            // Bit 2 : N/C
                            // Bit 3 : N/C
                            // Bit 4 : salida programacion         (TXP84)
                            // Bit 5 : entrada programacion        (RXP85)
                            // Bit 6 : salida  BUZZER			   (BUZZER)
    P8DR = 0x00;            
    // Puerto 9 utilizado para entradas por interrupcion de alarmas y seleccion de numero de vueltas.
    P9DDR = 0x00;           // Bit 0 : Entrada FULL STEP MOTOR 		(FULL_ST)
                            // Bit 1 : Entrada FAULT MOTOR  		(FAULT)
                            // Bit 2 : 
                            // Bit 3 : Entrada SERVO/POSICIONADOR	(P93)
                            // Bit 4 : Entrada V0					(P94)
                            // Bit 5 : Entrada V1					(P95)
                            // Bit 6 : Entrada V2					(P96)
                            // Bit 7 : Entrada R/L (REMOTE)			(P97)

    // Inicializacion puerto SCI0
    SCI0_SMR = 0x00;        // Modo de comunicaciones
    SCI0_SCR = 0x70;        // Registro de control serie n,8,1, Int RX enable
    SCI0_SSR &= 0x84;       // Registro de estado serie

    // Lee jumper de configuraci¢n...
    if(!(P9DR & 0x01))
        modo_conf = TRUE;
    else
        modo_conf = FALSE;

	// Lee velocidad de comunicaciones
	leeTipoEEprom (DIR_COMM , &comm,1);
	if(modo_conf == FALSE){
        // Selecciona velocidad dependiendo de jumper por puerto 3
		switch (comm){
			case 4:
                TIME_OUT_BYTE = 2;      // Time out para recepcion a nivel byte (mS)
                TIME_TXT      = 150;    // Tiempo de transmision entre caracteres (uS)
                TIME_RX_TX    = 1;      // Tiempo de espera entre recepcion y transmision (mS)
                SCI0_BRR = 3;           // 115200 baudios (14.7456 Mhz)
    			break;
        	case 3:
        	    TIME_OUT_BYTE = 3;      // Time out para recepcion a nivel byte (mS)
            	TIME_TXT      = 200;    // Tiempo de transmision entre caracteres (uS)
	            TIME_RX_TX    = 1;      // Tiempo de espera entre recepcion y transmision (mS)
	            SCI0_BRR = 7;           // 57600  baudios (14.7456 Mhz)
	            break;
			case 2:
	            TIME_OUT_BYTE = 4;      // Time out para recepcion a nivel byte (mS)
    	        TIME_TXT      = 300;    // Tiempo de transmision entre caracteres (uS)
        	    TIME_RX_TX    = 2;      // Tiempo de espera entre recepcion y transmision (mS)
            	SCI0_BRR = 11;          // 38400  baudios (14.7456 Mhz)
	            break;
			case 1:
	            TIME_OUT_BYTE = 5;      // Time out para recepcion a nivel byte (mS)
    	        TIME_TXT      = 400;    // Tiempo de transmision entre caracteres (uS)
        	    TIME_RX_TX    = 3;      // Tiempo de espera entre recepcion y transmision (mS)
            	SCI0_BRR = 23;          // 19200  baudios (14.7456 Mhz)
	            break;
			case 0:
	            TIME_OUT_BYTE = 10;     // Time out para recepcion a nivel byte (mS)
    	        TIME_TXT      = 1000;   // Tiempo de transmision entre caracteres (uS)
        	    TIME_RX_TX    = 4;      // Tiempo de espera entre recepcion y transmision (mS)
            	SCI0_BRR = 47;          // 19200  baudios (14.7456 Mhz)
	            break;
    	}
	}else{
        TIME_OUT_BYTE = 5;      // Time out para recepcion a nivel byte (mS)
        TIME_TXT      = 400;    // Tiempo de transmision entre caracteres (uS)
        TIME_RX_TX    = 3;      // Tiempo de espera entre recepcion y transmision (mS)
        SCI0_BRR = 23;          // 19200  baudios (14.7456 Mhz)
    }


    // Inicializacion de Timer 8 bit canal 0 (TIMER 1mS)
   	STCR |= 0x01;           // TIMER0=>CLK/256
                            // TIMER1=>CLK/8
    TMR0_TCORB = 58;        // Valor del comparador B (1mS)
    TMR0_TCR = 0x93;        // Int por comparador B y reset por comp B, clk/256
    TMR0_TCSR &= 0x7F;      // Reset flag int. comparador B
	
	// Activa DAC's
	//DA_DACR |= 0xE0;			
	/*
	// Inicialización del timer de 16 Bits
	TCR = 0x03;				// External clock source FTCI
	TIER |= 0x02;			// Activa interrupcion por overflow
	*/



// Inicializacion de variables
    btoa_ray((((~P3DR)& 0x3F) + DIR_ABS),acBuffer,3);   // Convierte direccion seleccionada
    acDirLocal[0]=acBuffer[1];  	// Direccion inicial local 
    acDirLocal[1]=acBuffer[2];

// Lectura de parametros en EEPROM de variables y constantes...
    leeTipoEEprom (DIR_VALOR_MAXIMO , &valor_maximo,8);
	leeTipoEEprom (DIR_VALOR_MINIMO , &valor_minimo,8);
	leeTipoEEprom (DIR_VALOR_OFFSET , &valor_offset,8);	
	leeTipoEEprom (DIR_TIPO_CANAL , &tipo_canal,4);
	leeTipoEEprom (DIR_VALOR_VARIOS , &valor_varios,4);
	leeTipoEEprom (DIR_MOTOR_VUELTAS , &motor_vueltas,2);
	leeTipoEEprom (DIR_MOTOR_CERO , &motor_cero,2);
	
#ifdef V3_03
	// Comprueba el modo de numero de vueltas, si remoto o local...
 	if(MODO_LOCAL){
    	// Si el modo es local...
		motor_vueltas = (NUMERO_VUELTAS_LOCAL * 360);
	}
#endif
		
	LED_ERROR_OFF;
	LED_TX_OFF;
	LED_RX_OFF;
	BUZZ_OFF;

    // Configuracion Watchdog
    WDT_TCSR_W = 0xA57F;       // Modo Watchdog timer
                                   // Modo Watchdog timer
                                   // Funcion reset
                                   // Aprox. overflow = 90 mS
                                   // Activa WDT !!!
        
    // General
    set_interrupt_mask(0);      // activa interrupciones
}


//***********************************//
//     Procedimiento principal       //
//***********************************//
void main (void){
    unsigned int n; // Contador
    unsigned int m; // Contador
    float xTemp;	// Valor temporal para calculos de escalado
    float fTemp,fTemp1,fTemp2,fTemp3;	// Temporal de lectura de datos
    float valor_anterior = 0;
	long pasos = 0;
	long pasos_maximos = 0;
	signed int diferencia_posicion;
	long temp_desviacion;

    RS485RX         // Macro RS485 en recepcion
        
    inicio();       // Inicializa uC
       
    RELE_OFF;
    comprueba_parametros();
        
    MOTOR_RESET_ON;
    BUZZ_ON;
    esperaMili1(500);
    BUZZ_OFF;
    esperaMili1(500);
    MOTOR_RESET_OFF;
    motor_micropaso(MOTOR_PASOS);
    MOTOR_ENABLE;
    
    //buzzer(500);

#ifdef V3_04    
    if(MODO_MOTOR){	// Si el modo del motor es el de posicionador......
    	// Incio del modo del motor en posicionador...
        SCI0_SCR &= 0xBF;       // Desactiva interrupcion recepcion
	    if(MODO_L)
	    	MOTOR_DIR_L;
	    else
	    	MOTOR_DIR_R;
	    	
		if(mueve_motor_zero() == 1){
			while(1){
		       	P8DR ^= 0x40;	//Buzzer ON por alarma...
				P1DR ^= 0x02;	//Led error on
				MOTOR_DISABLE;
				esperaMili1(1000);
			}
		}
        SCI0_SCR |= 0x40;       // activa interrupcion recepcion
		posicion_grados_actual = 0;
	    while(1){
		    if((P4DR & 0x04) == 0x04){
			    esperaMili1(50);
				if((P4DR & 0x04) == 0x04)
					posicion = 0;
		    }
		    if((P4DR & 0x08) == 0x08){
			    esperaMili1(50);
				if((P4DR & 0x08) == 0x08)
					posicion = 1;
		    }
		    if((P4DR & 0x10) == 0x10){
			    esperaMili1(50);
				if((P4DR & 0x10) == 0x10)
					posicion = 2;
		    }
		    if((P4DR & 0x20) == 0x20){
			    esperaMili1(50);
				if((P4DR & 0x20) == 0x20)
					posicion = 3;
		    }
		    if((P4DR & 0x40) == 0x40){
			    esperaMili1(50);
				if((P4DR & 0x40) == 0x40)
					posicion = 4;
		    }
		    if((P4DR & 0x80) == 0x80){
			    esperaMili1(50);
				if((P4DR & 0x80) == 0x80)
					posicion = 5;
		    }

//			Solo hasta 6 posiciones
		    if((P2DR & 0x01) == 0x01){
			    esperaMili1(50);
				if((P2DR & 0x01) == 0x01)
					posicion = 6;
		    }
		    if((P2DR & 0x02) == 0x02){
			    esperaMili1(50);
				if((P2DR & 0x02) == 0x02)
					posicion = 7;
		    }

		    posicion_grados_actual = (signed int)(posicion * ((360 * ENGRA_G) / (MAX_POSICIONES * ENGRA_P)));
		    diferencia_posicion = posicion_grados_actual - posicion_grados_anterior;
		    if(diferencia_posicion > 0){	// Si la posicion es posterior...
		        SCI0_SCR &= 0xBF;       // Desactiva interrupcion recepcion
			    mueve_motor(diferencia_posicion);
			    SCI0_SCR |= 0x40;       // activa interrupcion recepcion
			    posicion_grados_anterior = posicion_grados_actual;
			}
			if(diferencia_posicion < 0){	// Si la posicion es anterior...
	            SCI0_SCR &= 0xBF;       // Desactiva interrupcion recepcion
				if(mueve_motor_zero() == 1){
					while(1){
				       	P8DR ^= 0x40;	//Buzzer ON por alarma...
						P1DR ^= 0x02;	//Led error on
						MOTOR_DISABLE;
						esperaMili1(1000);
					}
				}
				SCI0_SCR |= 0x40;       // activa interrupcion recepcion
				esperaMili1(500);
		        SCI0_SCR &= 0xBF;       // Desactiva interrupcion recepcion
			    mueve_motor(posicion_grados_actual);
			    SCI0_SCR |= 0x40;       // activa interrupcion recepcion
			    posicion_grados_anterior = posicion_grados_actual;
			}
			// Espera 0.5 segundos...
			esperaMili1(500);
		}
	    
	}
#endif      
    
    // Inicio del modo del motor en servo....
    // Inicializacion de Timer 8 bit canal 1, (TIMER para contador de pulsos para motor)
    TMR1_TCORB = 184;       // Valor del comparador B (100 uS)
    TMR1_TCR = 0x91;        // Int por comparador B y reset por comp B, clk/8    
    TMR1_TCSR &= 0x7F;      // Reset flag int. comparador B
	
	
	while(ADS1211_decimation(VALOR_DECIMACION) == TRUE){	// Decimación
		inicializa_ADS1211();
	}
	
	// Lee jumpers de configuracion...
	comprueba_configuracion_motor();
	
	// Inicia el proceso de calibracion del motor para posicionar el cero
	calibra_motor(motor_cero);
    motor_micropaso(MOTOR_PASOS);

	//Valores iniciales a cero...
    for(m=0;m<4;m++){
            valor[m] = 0;
            valor_ant[m] = 0;
	}            
	
    while(1){
		guiTimer1 = 0;
        //**************************************************************
        //                  Control de tarjeta radam2
        //**************************************************************
		while(ADS1211_decimation(VALOR_DECIMACION) == TRUE){	// Decimación
			inicializa_ADS1211();
		}
        for(n=0;n<4;n++){           
			// P1DR ^= 0x04;
        	// Canales...
            while(ADS1211_canal((char)n) == TRUE){   // Selecciona canal...
            	inicializa_ADS1211();                
            }
            // Lectura de conversores A/D.............
            switch(tipo_canal[n]){
                case 10:            // Canal +/- 5.0000...
                    while(ADS1211_ganancia(GAINx1) == TRUE){    // Selecciona ganancia x1
                        inicializa_ADS1211();             
                    }
                    ADS1211_modo(ADS1211_SELF_CALIBRATION);
                    // Lee valor canal analógico y escala...
                    xTemp = ((float)(valor_maximo[n] - valor_minimo[n]) / (float)0x7fffff);
                    fTemp = (float)valor_offset[n] + (float)valor_minimo[n] + (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
                    valor[n] = fTemp;
   					filtrado_valores(n);		//Filtra señal
                    break;

                case 11:            // Canal +/- 2.5000...
                    while(ADS1211_ganancia(GAINx2) == TRUE){    // Selecciona ganancia x2
                        inicializa_ADS1211();             
                    }
                    ADS1211_modo(ADS1211_SELF_CALIBRATION);
                    // Lee valor canal analógico y escala...
                    xTemp = ((float)(valor_maximo[n] - valor_minimo[n]) / (float)0x7fffff);
                    fTemp = (float)valor_offset[n] + (float)valor_minimo[n] + (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
                    valor[n] = fTemp;
   					filtrado_valores(n);		//Filtra señal
                    break;

                case 12:            // Canal +/- 1.2500...
                    while(ADS1211_ganancia(GAINx4) == TRUE){    // Selecciona ganancia x4
                        inicializa_ADS1211();             
                    }
                    ADS1211_modo(ADS1211_SELF_CALIBRATION);
                    // Lee valor canal analógico y escala...
                    xTemp = ((float)(valor_maximo[n] - valor_minimo[n]) / (float)0x7fffff);
                    fTemp = (float)valor_offset[n] + (float)valor_minimo[n] + (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
					valor[n] = fTemp;
   					filtrado_valores(n);		//Filtra señal
                    break;

                case 13:            // Canal +/- 0.6250...
                    while(ADS1211_ganancia(GAINx8) == TRUE){    // Selecciona ganancia x8
                        inicializa_ADS1211();             
                    }
                    ADS1211_modo(ADS1211_SELF_CALIBRATION);
                    // Lee valor canal analógico y escala...
                    xTemp = ((float)(valor_maximo[n] - valor_minimo[n]) / (float)0x7fffff);
                    fTemp = (float)valor_offset[n] + (float)valor_minimo[n] + (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
                    valor[n] = fTemp;
   					filtrado_valores(n);		//Filtra señal
                    break;

                case 14:            // Canal +/- 0.3125...
                    while(ADS1211_ganancia(GAINx16) == TRUE){   // Selecciona ganancia x16
                        inicializa_ADS1211();             
                    }
                    ADS1211_modo(ADS1211_SELF_CALIBRATION);
                    // Lee valor canal analógico y escala...
                    xTemp = ((float)(valor_maximo[n] - valor_minimo[n]) / (float)0x7fffff);
                    fTemp = (float)valor_offset[n] + (float)valor_minimo[n] + (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
   					valor[n] = fTemp;
   					filtrado_valores(n);		//Filtra señal
                    break;
                    
                case 20:            // Canal +/- 4/20mA...(+/- 2.5000v)
                    while(ADS1211_ganancia(GAINx2) == TRUE){    // Selecciona ganancia x2
                        inicializa_ADS1211();             
                    }
                    ADS1211_modo(ADS1211_SELF_CALIBRATION);
                    // Lee valor canal analógico y escala...
                    xTemp = ((float)(valor_maximo[n] - valor_minimo[n]) / (float)0x51EB85);
                    fTemp = (float)valor_offset[n] + (float)valor_minimo[n] + (((float)ADS1211_lee_valor() - (float)0x947AE1) * xTemp);
                    valor[n] = fTemp;
   					filtrado_valores(n);		//Filtra señal
                    break;

                case 21:            // Canal +/- 0/20mA... (+/- 2.5000v)
                    while(ADS1211_ganancia(GAINx2) == TRUE){    // Selecciona ganancia x2
                        inicializa_ADS1211();             
                    }
                    ADS1211_modo(ADS1211_SELF_CALIBRATION);
                    // Lee valor canal analógico y escala...
                    xTemp = ((float)(valor_maximo[n] - valor_minimo[n]) / (float)0x666666); //(2v->20mA)
                    fTemp = (float)valor_offset[n] + (float)valor_minimo[n] + (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
                    valor[n] = fTemp;
   					filtrado_valores(n);		//Filtra señal
                    break;
            }	// Switch case
        }	// For n
		
		//Comprueba rangos...
		if(valor[1] < 0) valor[1] = 0;
		
		//Comprueba que el numero de vueltas establecido no puede ser porque hay demasiado cero...
		if((unsigned int)(((long)motor_vueltas * 2000) / 3600) > (POT_LIMITE_SUP - motor_cero)){
			P1DR ^= 0x02;	//Led error on
			//Limita la posicion a este valor...
			if(valor[1] > (float)(((long)(POT_LIMITE_SUP - motor_cero) * 5000) / (((long)motor_vueltas * 2000) / 3600))){
				valor[1] = (float)(((long)(POT_LIMITE_SUP - motor_cero) * 5000) / (((long)motor_vueltas * 2000) / 3600));
			}
		}

		// Calcula nueva posicion...
		pasos_maximos = (long)(((float)motor_vueltas/(float)360)*(float)MOTOR_PASOS); 
		if((abs(valor[1] - valor_anterior) > MOTOR_HISTERESIS) && (error_motor == 0)){			// Control de histeresis y errores
			pasos = ((pasos_maximos * (long)valor[1]) / 5000);
			posicion_rel = pasos;	
			valor_anterior = valor[1];
		}
		motor_posicion = (unsigned int)(((long)posicion_abs * (long)motor_vueltas)/pasos_maximos);

		// Comprobación de posicion....
		if(posicion_abs == posicion_rel){
			// Canal 0(potenciometro)...
		   	while(ADS1211_canal(0) == TRUE){   // Selecciona canal...
		    	inicializa_ADS1211();                
			}
		    while(ADS1211_ganancia(GAINx1) == TRUE){    // Selecciona ganancia x1
		        inicializa_ADS1211();             
		    }
		    ADS1211_modo(ADS1211_SELF_CALIBRATION);
		    // Lee potenciometro...
		    xTemp = ((float)(5000) / (float)0x7fffff);
			fTemp = 2000 - (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
			
			// Implementacion del filtro...		
			// Valores dentro del +/-1%...
			fTemp1 = 2000 - (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
			esperaMicro(10000);
			fTemp2 = 2000 - (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
			esperaMicro(10000);
			fTemp3 = 2000 - (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
			if((fTemp < (fTemp1 + 20)) && (fTemp > (fTemp1 - 20))){
				fTemp = fTemp1;
			}
			if((fTemp < (fTemp2 + 20)) && (fTemp > (fTemp2 - 20))){
				fTemp = fTemp2;
			}
			if((fTemp < (fTemp3 + 20)) && (fTemp > (fTemp3 - 20))){
				fTemp = fTemp3;
			}
/*			
			// Calculo de la desviación máxima...
			// La desviación máxima depende de la posicion máxima a la que puede alcanzar el potenciometro
			// Este valor se calcula sumando la posicion inicial del cero + el numero de vueltas programado:
			temp_desviacion = (long)((((((float)motor_cero * 36) / 20) + (float)motor_vueltas) / 3600) * MOTOR_DESVIACION_MAX);
			
		    if( (((((long)fTemp - motor_cero) * MOTOR_PASOS)) / 200) > (posicion_abs + temp_desviacion)){
		    	error_motor = 1;	// error en posicionamiento
		    	MOTOR_DISABLE;
		    }
		    if( (((((long)fTemp - motor_cero) * MOTOR_PASOS)) / 200) < (posicion_abs - temp_desviacion)){
		    	error_motor = 1;	// error en posicionamiento
		    	MOTOR_DISABLE;
		    }
*/

		}

		// Comprueba error de posicionamiento...
		if(error_motor > 0){
		 	P8DR ^= 0x40;	//Buzzer ON por alarma...
			P1DR ^= 0x02;	//Led error on
			RELE_ON;
			MOTOR_DISABLE;
		}
		
		// Lee jumpers de configuracion...
		comprueba_configuracion_motor();
	
		// while(guiTimer1 <= 10);	// Espera hasta 1 segundo...
    }	// While
}

//***********************************//
//***********************************//
//     Procedimientos generales      //
//***********************************//
//***********************************//

//***************************************//
// Calculo del BCC                       //
// Parametros:                           //
// - *pBuffer  : puntero a pBuffer       //
// - cLongitud : longitud del buffer - 1 //
// Devuelve: caracter BCC calculado      //
//***************************************//
char calculaBCC (char *pBuffer , char cLongitud)
{
    char temp=0;    // Valor temporal del BCC
    char i;         // Contador 

    for (i=0;i<=cLongitud;i++)
        temp ^= pBuffer[i];
    return(temp);
}

//******************************************************//
// Procedimento de decodificacion para                  //
// protocolo TOHO (modificado):                         //
// STX+DIR1+DIR2+R/W+C1+V1+V2+V3+V4+V5+V6+ETX           //
// Parametros:                                          //            
// - acBuffer  : buffer de recepcion global             //
//******************************************************//
void decodificaMensaje_GASES(void){
    unsigned int iTemp;
    signed int sTemp;
    unsigned long lTemp;
    signed int n;					// Temporal con numero de canal

    // Comprobaci¢n de direccion
    if((acBuffer[1] != acDirLocal[0]) || (acBuffer[2] != acDirLocal[1])){
        // Si las direcciones son incorrectas....salte
        return;
    }
	if(acBuffer[3] == 'R'){	// Comandos de lectura...
		#include "com_ra.c"			
    }
    if(acBuffer[3] == 'W'){	// Comandos de escritura...
		#include "com_wa.c"			
    }
    error_com(1);					// Comando no reconocido
}

/*****************************************************************
  Rutina de preparacion y envio de la trama de error
  Por @Ray2001
  ****************************************************************/
void error_com (unsigned int numero){
	acBuffer[3] = NAK;
	itoa_ray(numero,&acBuffer[4],2);		//Convierte numero a cadena
	acBuffer[6] = ETX;
	acBuffer[7] = calculaBCC(acBuffer,6);  	//Calcula BCC
	esperaMili(TIME_RX_TX);             	//Espera tiempo entre RX y TX
	transmiteCadena (acBuffer,7);   		//Transmite cadena
}	    

/*****************************************************************
  Rutina de preparacion y envio de la trama de respuesta para
  numeros con signo...
  El parametro longitud no incluye los signos
  Por @Ray2001
  ****************************************************************/
void montaCadena_s (signed int numero, char longitud){
    // Montaje de cadena de respuesta...
    acBuffer[3] = ACK;
    stoa_ray (numero,&acBuffer[4],longitud);
    acBuffer[longitud+5]  = ETX;
    acBuffer[longitud+6] = calculaBCC(acBuffer,(longitud+5));  	//Calcula BCC
    esperaMili(TIME_RX_TX);                 					//Espera tiempo entre RX y TX
    transmiteCadena (acBuffer,(longitud+6));          			//Transmite cadena
}    

/*****************************************************************
  Rutina de preparacion y envio de la trama de respuesta para
  numeros sin signo...
  Por @Ray2001
  ****************************************************************/
void montaCadena_i (unsigned int numero, char longitud){
    // Montaje de cadena de respuesta...
    acBuffer[3] = ACK;
    itoa_ray (numero,&acBuffer[4],longitud);
    acBuffer[longitud+4]  = ETX;
    acBuffer[longitud+5] = calculaBCC(acBuffer,(longitud+4));  	//Calcula BCC
    esperaMili(TIME_RX_TX);                 					//Espera tiempo entre RX y TX
    transmiteCadena (acBuffer,(longitud+5));          			//Transmite cadena
}    

/*****************************************************************
  Rutina de preparacion y envio de la trama de respuesta para
  numeros largos sin signo...
  Por @Ray2001
  ****************************************************************/
void montaCadena_l (unsigned long numero, char longitud){
    // Montaje de cadena de respuesta...
    acBuffer[3] = ACK;
    ltoa_ray (numero,&acBuffer[4],longitud);
    acBuffer[longitud+4]  = ETX;
    acBuffer[longitud+5] = calculaBCC(acBuffer,(longitud+4));  	//Calcula BCC
    esperaMili(TIME_RX_TX);                 					//Espera tiempo entre RX y TX
    transmiteCadena (acBuffer,(longitud+5));          			//Transmite cadena
}    

/*****************************************************************
  Rutina de preparacion y envio de la trama de respuesta de
  escritura correcta.
  Por @Ray2001
  ****************************************************************/
void escritura_correcta(void){  
    acBuffer[4]  = ETX;
    acBuffer[5] = calculaBCC(acBuffer,4);   // Calcula BCC
    esperaMili(TIME_RX_TX);                 // Espera tiempo entre RX y TX
    transmiteCadena (acBuffer,5);           // Transmite cadena
}    


/*******************************************************
  Conversion de entero sin signo a cadena. Por @Ray2000.
  Parametros:                           
  - uNumero   : numero entero sin signo a convertir
  - *pCadena  : puntero a cadena
  - cLongitud : longitud de la conversion
                si cLongitud es 0, la longitud no es fija
  *******************************************************/
void itoa_ray (unsigned int uNumero, char *pCadena, char cLongitud){
    char i;                     //Contador temporal
    char temp;

    if(cLongitud > 5) return;   // Comprueba si la longitud est  fuera de rango...

    for (i=cLongitud;i>0;i--){
        temp = (char)((uNumero % 10) + 0x30);
        pCadena[i-1] = temp;
        uNumero /= 10;
    }
    pCadena[cLongitud] = '\0';
}

/*******************************************************
  Conversion de entero con signo a cadena. Por @Ray2000.
  El signo se añade siempre al principio de la cadena
  Parametros:                           
  - sNumero   : numero entero con signo a convertir
  - *pCadena  : puntero a cadena
  - cLongitud : longitud de la conversion excluidos los signos
                si cLongitud es 0, la longitud no es fija
  *******************************************************/
void stoa_ray (signed int sNumero, char *pCadena, char cLongitud){
    char i;                     //Contador temporal
    char temp;

	if(sNumero < 0){
        pCadena[0] = '-';
        sNumero = -sNumero;
    }else{
		pCadena[0] = '+';
    }

    if(cLongitud > 6) return;   // Comprueba si la longitud est  fuera de rango...

    for (i=cLongitud;i>0;i--){
        temp = (char)((sNumero % 10) + 0x30);
        pCadena[i] = temp;
        sNumero /= 10;
    }
    pCadena[cLongitud+1] = '\0';
}

/*******************************************************
  Conversion de byte sin signo a cadena. Por @Ray2000.
  Parametros:                           
  - cNumero   : numero byte sin signo a convertir
  - *pCadena  : puntero a cadena
  - cLongitud : longitud de la conversion
  *******************************************************/
void btoa_ray (char cNumero, char *pCadena, char cLongitud){
    char i;                     //Contador temporal
    char temp;

    if(cLongitud > 3) return;   // Comprueba si la longitud est  fuera de rango...

    for (i=cLongitud;i>0;i--){
        temp = ((cNumero % 10) + 0x30);
        pCadena[i-1] = temp;
        cNumero /= 10;
    }
    pCadena[cLongitud] = '\0';
}



/********************************************************
  Conversion de cadena a unsigned int. Por @Ray2000.
  Parametros:                           
  - *pCadena  : puntero de cadena a convertir
  - *uNumero  : puntero a numero donde dejar la conversion
  Devuelve:
  - atoi_ray  : 0 - conversion correcta
                1 - conversion incorrecta
  *******************************************************/
char atoi_ray (char *pCadena, unsigned int *uNumero){
    char i = 0;             // Contador temporal
    unsigned int t;         // Contador temporal multiplicador

    // Comprueba si todos los caracteres son n£meros...
    while((pCadena[i] != '\0') && (i < 5)){
        if((pCadena[i] > '9') || (pCadena[i] < '0'))
            return(1);          // Conversi¢n incorrecta
        i++;
    }
    *uNumero = 0;
    // Parte entera...
    for (t = 1 ; i > 0 ; i--,t *= 10){
        *uNumero = *uNumero + (unsigned int)(t * (pCadena[i-1] - 0x30));
    }
    return(0);      // Conversion correcta
}

/********************************************************
  Conversion de cadena a signed int. Por @Ray2000.
  Parametros:                           
  - *pCadena  : puntero de cadena a convertir
  - *sNumero  : puntero a numero donde dejar la conversion
  - tamax     : numeto de caracteres maximos a escoger (incluyendo el '-' y el '+'
  Devuelve:
  - atos_ray  : 0 - conversion correcta
                1 - conversion incorrecta
  *******************************************************/
char atos_ray (char *pCadena, signed int *sNumero, char tamax){
    char i = 0;             // Contador temporal
    signed int t;           // Contador temporal multiplicador
	char negata = 0;		// inicialmente el numero es positivo

   	// Comprueba si todos los caracteres son n£meros...
   	if(pCadena[0] == '-'){	// Numero negativo
		pCadena[0] = '0';
		negata = 1;
	}
   	if(pCadena[0] == '+'){	// Numero positivo
		pCadena[0] = '0';          
		negata = 0;
	}
   	while((pCadena[i] != '\0') && (i < tamax)){
        if((pCadena[i] > '9') || (pCadena[i] < '0'))
            return(1);          // Conversi¢n incorrecta
        i++;
    }
    *sNumero = 0;
    // Parte entera...
    for (t = 1 ; i > 0 ; i--,t *= 10){
        *sNumero = *sNumero + (signed int)(t * (pCadena[i-1] - 0x30));
    }
    if(negata == 1)
    	*sNumero = -(*sNumero);
    return(0);      // Conversion correcta
}

/*******************************************************
  Conversion de long sin signo a cadena. Por @Ray2000.
  Parametros:                           
  - lNumero   : numero long sin signo a convertir
  - *pCadena  : puntero a cadena
  - cLongitud : longitud de la conversion
                si cLongitud es 0, la longitud no es fija
  *******************************************************/
void ltoa_ray (unsigned long lNumero, char *pCadena, char cLongitud){
    char i;                     //Contador temporal
    char temp;

    if(cLongitud > 10) return;	// Comprueba si la longitud est  fuera de rango...

    for (i=cLongitud;i>0;i--){
        temp = (char)(lNumero % 10) + 0x30;
        pCadena[i-1] = temp;
        lNumero /= 10;
    }
    pCadena[cLongitud] = '\0';
}

/********************************************************
  Conversion de cadena a unsigned long. Por @Ray2001.
  Parametros:                           
  - *pCadena  : puntero de cadena a convertir
  - *uNumero  : puntero a numero donde dejar la conversion
  Devuelve:
  - atoi_ray  : 0 - conversion correcta
                1 - conversion incorrecta
  *******************************************************/
char atol_ray (char *pCadena, unsigned long *uNumero){
    char i = 0;             // Contador temporal
    unsigned long t;         // Contador temporal multiplicador

    // Comprueba si todos los caracteres son n£meros...
    while((pCadena[i] != '\0') && (i < 10)){
        if((pCadena[i] > '9') || (pCadena[i] < '0'))
            return(1);          // Conversi¢n incorrecta
        i++;
    }
    *uNumero = 0;
    // Parte entera...
    for (t = 1 ; i > 0 ; i--,t *= 10){
        *uNumero = *uNumero + (unsigned long)(t * (pCadena[i-1] - 0x30));
    }
    return(0);      // Conversion correcta
}

/********************************************************
  Transmision de una cadena por puerto SCI0. Por @Ray2000.
  Parametros:                           
  - *pCadena  : puntero a cadena
  - cLongitud : longitud de la transmision
  *******************************************************/
void transmiteCadena (char *pCadena, char cLongitud){
    char i;         // Contador temporal

    RS485TX          // Macro RS485 en transmision
    esperaMicro(50); // Espera 50uS a estabilizacion de cambio RX/TX
    for(i=0;i<=cLongitud;i++){
        while(!(SCI0_SSR & 0x80));  // Espera a Buffer de salida vacio
        SCI0_TDR  = pCadena[i];     // Transmite caracter
        SCI0_SSR &= 0x7F;           // Borra TDRE para trasmitir

        esperaMicro(TIME_TXT);      // Espera tiempo entre caracteres
    }
    RS485RX             // Macro RS485 en recepcion
    esperaMicro(50);    // Espera 50uS a estabilizacion de cambio TX/RX
}

/********************************************************
  Transmision de byte por puerto SCI0. Por @Ray2000.
  Modo sincrono.
  Parametros:                           
  - dato  : dato a transmitir
  *******************************************************/
void transmiteByte (char dato){
    SCI0_SCR |= 0x20;           // TX enable
//    while((SCI0_SSR & 0x80) == 0);   // Espera a Buffer de salida vacio
    SCI0_SSR &= 0x84;           // Borra errores
    SCI0_TDR  = dato;           // Transmite dato
    SCI0_SSR &= 0x7F;           // Borra TDRE para trasmitir
    while((SCI0_SSR & 0x04) == 0);      // Espera a terminar transmision
    SCI0_SCR &= ~(0x20);        // TX disable
}

/********************************************************
  Lectura de byte por puerto SCI0. Por @Ray2000.
  Modo sincrono.
  Parametros:                           
  - dato  : puntero a byte a recibir
  Devuelve:
  - recibeByte = 0 : todo correcto
  - recibeByte = 1 : error
  *******************************************************/
char recibeByte (char *dato){
    unsigned int timeOut;

    SCI0_SCR |= 0x10;       // RX enable

    timeOut = 1000;
    do{
        if((SCI0_SSR & 0x20) == 0x20){
            SCI0_SSR &= 0x84;           // Borra RDRF
            SCI0_SCR &= 0xEF;           // RX disable
            return(1);          // Error overrun
        }
        timeOut--;
    } while(((SCI0_SSR & 0x40) == 0x00) && (timeOut != 0)); // Espera a que se llene el buffer o time out...

    if(timeOut == 0x00){
        SCI0_SSR &= 0x84;           // Borra RDRF
        SCI0_SCR &= 0xEF;           // RX disable
        return(1);          // Error time out
    }

    *dato = SCI0_RDR;           // Transmite dato
    SCI0_SSR &= 0x84;           // Borra RDRF
    SCI0_SCR &= 0xEF;           // RX disable
    return(0);
}


/********************************************************
  Procedimiento para espera de milisegundos. Por @Ray2000.
  Esta es utilizada para la interrupcion de comunicaciones
  Parametros:
  - unsigned int : tiempo en milisegunsos
  NOTA: esto usa el timer
  *******************************************************/
void esperaMili (unsigned int tiempo){
        guiTimer = 0;
        while(guiTimer <= tiempo);   // Espera tiempo entre caracteres
}

/********************************************************
  Procedimiento para espera de milisegundos. Por @Ray2000.
  Parametros:                           
  - unsigned int : tiempo en milisegunsos
  NOTA: esto usa el timer
  *******************************************************/
void esperaMili1 (unsigned int tiempo){
        guiTimer1 = 0;
        while(guiTimer1 <= tiempo);   // Espera tiempo entre caracteres
}


/********************************************************
  Procedimiento para espera de uSegundos. Por @Ray2000.
  Parametros:                           
  - unsigned int : tiempo en uSegundos
  NOTA: esto NO usa el timer, depende del cristal
  *******************************************************/
void esperaMicro (unsigned int t){
#ifdef XTAL_16
    t = t / 8;
#endif
    for(;t>0;t--);
}


// Funcion para escribir byte en eeprom.(No utilizar esta directamente!!!)
// Devuelve 1 si no es reconocido. 0 si reconocido.
char w_byte_eeprom (char valor){
    char i;                 // Contador temporal
    for(i=0;i<8;i++){
        if(valor & 0x80)    // Si es uno...
            SDA_1;
        esperaMicro(2);
        SCL_1;
        esperaMicro(2);
        SCL_0;
        esperaMicro(2);
        SDA_0;
        esperaMicro(2);
        valor <<= 1;        // Desplaza a la izquierda valor
    }

    // Lectura de ACK... (por ahora no lo hago)
    
    SDA_IN;                 // SDA en modo entrada
    SDA_IN1;
    SCL_1;
    esperaMicro(2);
    if(P_EEPROM & 0x02){    // Si SDA es 1, entonces no se ha reconocido...
        SCL_0;
        SDA_OUT;            // SDA en modo salida
        SDA_OUT1;
        SDA_0;
        return(1);          // No reconocido
    }
    SDA_OUT;                // SDA en modo salida
    SDA_OUT1;
    SCL_0;

    SDA_0;
    return(0);              // Reconocido
}

// Funcion para leer byte de eeprom.(No utilizar esta directamente!!!)
char r_byte_eeprom (void){
    char i;                 // Contador temporal
    char temp = 0;          // Temporal
    
    SDA_IN;                 // SDA en modo entrada
    SDA_IN1;
    SCL_1;
    esperaMicro(2);
    for(i=0;i<8;i++){
        temp <<= 1;
        SCL_1;
        esperaMicro(2);
        if(P_EEPROM & 0x02) // Si SDA es 1...
           temp |= 0x01;
        SCL_0;
        esperaMicro(2);
    }
    SDA_OUT;                // SDA en modo salida
    SDA_OUT1;
    SDA_0;
    return(temp);
}


// Condicion START
void eeprom_start(void){
    SDA_0;
    esperaMicro(2);
    SCL_0;
    esperaMicro(2);
}

// Condicion STOP
void eeprom_stop(void){
    SCL_1;
    esperaMicro(2);
    SDA_1;
    esperaMicro(2);
}

/********************************************************
  Lectura de valor de EEPROM.  Por @Ray2000.
  Parametros:                           
  - dir        : direccion de memoria EEprom a leer
  - *param     : puntero al valor float a rellenar
  - lg         : longitud del tipo
  *******************************************************/
void leeTipoEEprom (unsigned int dir, void *param, char lg){
    char i;             //Contador temporal
    char temp;
    char *ptemp;        // Puntero temporal a char para lectura individual de bytes
                        // en un tipo float
    ptemp = param;

    for(i=0;i<lg;i++){
        *(ptemp+i) = lee_eeprom_24C16 (dir+i);
        temp = *(ptemp+i);
    }
}


/********************************************************
  Escritura de valor a EEPROM.    Por @Ray2000.
  Parametros:                           
  - dir        : direccion de memoria EEprom a escribir
  - *param     : puntero al valor float a leer
  - lg         : longitud del tipo
  *******************************************************/
void escribeTipoEEprom (unsigned int dir, void *param, char lg){
    char i;             //Contador temporal
    char temp;
    char *ptemp;        // Puntero temporal a char para lectura individual de bytes
                        // en un tipo float
    ptemp = param;
    for(i=0;i<lg;i++){
        temp = *(ptemp+i);
        do{
	        escribe_eeprom_24C16(dir+i , temp);
        }while(lee_eeprom_24C16(dir+i) != temp);
    }
}


/********************************************************
  Escritura en EEPROM 24C16. Por @Ray2000.
  Parametros:                           
  - dir        : direccion de memoria a leer
  - dato       : dato a esbribir
  *******************************************************/
void escribe_eeprom_24C16 (unsigned int dir, char dato){
    char *byte;             // Puntero char
    char temp;              // parte alta de pagina temporal

    // Comprueba primero si los datos a grabar son los mismos.
    // Si es as¡, no grabes y sal...
    if(lee_eeprom_24C16(dir) == dato)
        return;

    byte = (char *)&dir;    // *byte contiene el byte alto de 'dir'
    //byte = &dir;          // *byte contiene el byte alto de 'dir'
    temp = *byte;           // Parte alta de direccion
    temp <<= 1;             // Rota a la izquierda
    temp &= 0x0E;           // Mascara para eliminar direcciones erroneas;
    temp = temp | 0xA0;     // escritura

    eeprom_start();         // Condicion START
    w_byte_eeprom(temp);        // Direccion de dispositivo / escritura
    w_byte_eeprom(*(byte+1));   // Byte bajo de direccion de memoria
    w_byte_eeprom(dato);        // escribe dato
    eeprom_stop();          // Condicion STOP
    esperaMili(10);         // Espera a escritura de eeprom
}

/********************************************************
  Lectura de EEPROM 24C16. Por @Ray2000.
  Parametros:                           
  - dir        : direccion de memoria a leer
  Devuelve:
  - lee_eeprom : valor leido
  *******************************************************/
char lee_eeprom_24C16 (unsigned int dir){
    char *byte;             // Puntero char
    char byte_leido;
    char temp;              // parte alta de pagina temporal

    byte = (char *)&dir;    // *byte contiene el byte alto de 'dir'
    temp = *byte;           // Parte alta de direccion
    temp <<= 1;             // Rota a la izquierda
    temp &= 0x0E;           // Mascara para eliminar direcciones erroneas;
    temp = temp | 0xA0;     // escritura

    eeprom_start();         // Condicion START
    w_byte_eeprom(temp);        // Direccion de dispositivo / escritura
    w_byte_eeprom(*(byte+1));   // Byte bajo de direccion de memoria
    eeprom_stop();          // Condicion STOP

    esperaMicro(10);

    eeprom_start();         // Condicion START
    temp |= 1;              // Lectura
    w_byte_eeprom(temp);    // Direccion de dispositivo / lectura
    byte_leido = r_byte_eeprom();
    eeprom_stop();          // Condicion STOP
    return(byte_leido);
}

/********************************************************
  Conversion de byte a hexadecimal. Por @Ray2000.
  Parametros:
  - numero    : numero char a convertir
  - *pCadena  : puntero de cadena convertida
  *******************************************************/
void dectohex_8_ray (char numero, char *pCadena){
    pCadena[0] = numero / 16;
    pCadena[1] = numero % 16;
    pCadena[0] += (pCadena[0] > 9)? 0x37 : 0x30;
    pCadena[1] += (pCadena[1] > 9)? 0x37 : 0x30;
}


/*************************************************************
  Rutinas de control para el convertidor de BURR-BROWN ADS1211
  Por @Ray2000. Control de Procesos.
  ************************************************************/
// Inicializa los parametros iniciales del ADS1211
// Devuelve TRUE si hay error
void inicializa_ADS1211(void){
    ADS1211_init();
    ADS1211_canal(0);
    ADS1211_decimation(VALOR_DECIMACION);       // Decimaci¢n a 10 Hz
    ADS1211_modo(ADS1211_SELF_CALIBRATION);
}

// Inicializa ADS1211
// Devuelve TRUE si hay error
char ADS1211_init(void){
    unsigned int reintentos = 4;
    do{
        if(ADS1211_ready()==TRUE)
        	return(TRUE);	// Ha habido error
        ADS1211_escribe(0x04);
        ADS1211_escribe(0x62);

        if(ADS1211_ready()==TRUE)
        	return(TRUE);	// Ha habido error
        ADS1211_escribe(0x84);
        reintentos--;
    }while(((ADS1211_lee() & 0xfe) != 0x62) && (reintentos > 0));

    if(reintentos == 0)
        return(TRUE);       // Ha habido error
    else
        return(FALSE);
}

// Cambio del ratio de decimaci¢n del ADS1211
char ADS1211_decimation(unsigned int valor){
    char *pValor;
    char temp;
    char reintentos = 10;

    pValor = &valor;
	temp = *(pValor+1);
    do{
        //ADS1211_ready();
        ADS1211_escribe(0x07);          // Command register byte 0
        ADS1211_escribe(temp);   		// Parte baja de valor

        //ADS1211_ready();
        ADS1211_escribe(0x87);
        reintentos--;
    }while((ADS1211_lee() != temp) && (reintentos > 0));

    if(reintentos == 0)
        return(TRUE);       // Ha habido error

    reintentos = 10;
    //ADS1211_ready();
    ADS1211_escribe(0x86);
    temp = ADS1211_lee();               // Lee command register byte 1
    temp = (temp & 0xE0) | *(pValor);   // Parte alta de valor

    do{
        //ADS1211_ready();
        ADS1211_escribe(0x06);          // Command register byte 1
        ADS1211_escribe(temp);          // Parte alta de valor

        //ADS1211_ready();
        ADS1211_escribe(0x86);
        reintentos--;
    }while((ADS1211_lee() != temp) && (reintentos > 0));

    if(reintentos == 0)
        return(TRUE);       // Ha habido error
    else
        return(FALSE);

}

// Cambio de ganancia del ADS1211
// Devuelte TRUE si hay error
char ADS1211_ganancia(char gain){
    char temp;
    char reintentos = 4;

    //ADS1211_ready();
    ADS1211_escribe(0x85);
    temp = ADS1211_lee();           // Lee command register byte 2
    temp = (temp & 0xE3) | (gain << 2);    // Inserta la ganancia

    do{
        //ADS1211_ready();
        ADS1211_escribe(0x05);      // Command register byte 2
        ADS1211_escribe(temp);      // Escribe valor

        //ADS1211_ready();
        ADS1211_escribe(0x85);
        reintentos--;
    }while((ADS1211_lee() != temp) && (reintentos > 0));
    ADS1211_ready();
    ADS1211_ready();

    if(reintentos == 0)
        return(TRUE);       // Ha habido error
    else
        return(FALSE);
}

// Cambio de modo del ADS1211
void ADS1211_modo(char modo){
    char temp;

    //ADS1211_ready();
    ADS1211_escribe(0x85);
    temp = ADS1211_lee();           // Lee command register byte 2
    temp = (temp & 0x1F) | (modo << 5);    // Inserta el modo
 
    //ADS1211_ready();
    ADS1211_escribe(0x05);      // Command register byte 2
    ADS1211_escribe(temp);      // Escribe valor
    ADS1211_ready();
    ADS1211_ready();
    ADS1211_ready();
}

// Lectura de valor anal¢gico
unsigned long ADS1211_lee_valor(void){
    unsigned long valor_temp;
    char *pchar;

    pchar = &valor_temp;

    valor_temp = 0;
    ADS1211_ready();
    ADS1211_escribe(0xC0);          // Lee 3 bytes
    *(pchar+1) = ADS1211_lee();
    *(pchar+2) = ADS1211_lee();
    *(pchar+3) = ADS1211_lee();

    return(valor_temp);
}

// Devuelve FALSE si hay error...
char ADS1211_ready (void){
	unsigned int reintentos = 35000;	// 100mS

    // Espera a que est‚ lista la conversi¢n...
    while(((ADS1211_PUERTO & ADS1211_DRDY) == ADS1211_DRDY) && (reintentos > 0)){
	    reintentos--;	    
	}	    
	if(reintentos == 0)
		return(TRUE);

	reintentos = 35000;		
	
	while(((ADS1211_PUERTO & ADS1211_DRDY) == 0) && (reintentos > 0)){
	    reintentos--;
	}	    
	if(reintentos == 0)
		return(TRUE);
	else
		return(FALSE);		
}

char ADS1211_canal (char canal){
    char temp;
    char reintentos = 4;

    //ADS1211_ready();
    ADS1211_escribe(0x85);
    temp = ADS1211_lee();           // Lee command register byte 2
    temp = ((temp & 0xFC) | canal); // Selecciona el canal

    do{
        //ADS1211_ready();
        ADS1211_escribe(0x05);      // Command register byte 2
        ADS1211_escribe(temp);      // Escribe valor

        //ADS1211_ready();
        ADS1211_escribe(0x85);
        reintentos--;
    }while((ADS1211_lee() != temp) && (reintentos > 0));
    ADS1211_ready();
    ADS1211_ready();

    if(reintentos == 0)
        return(TRUE);       // Ha habido error
    else
        return(FALSE);
}

void ADS1211_escribe (char dato){
    char n;
    esperaMicro(2);
    for (n=0;n<8;n++){
        ADS1211_SCLK_H;
        esperaMicro(1);
        if((dato & 0x80) == 0x80){  //Lee MSB
            ADS1211_SDIO_H;    
        }else{
            ADS1211_SDIO_L;    
        }
        dato <<= 1;
        ADS1211_SCLK_L;
        esperaMicro(1);
    }
    ADS1211_SDIO_L;    
}

char ADS1211_lee (void){
    char n;
    char temp = 0;
    esperaMicro(2);
    for (n=0;n<8;n++){
        temp <<= 1;
        ADS1211_SCLK_H;
        esperaMicro(1);
        if((ADS1211_PUERTO & ADS1211_SDOUT) == 0){  //Lee MSB del ADC
            temp |= 0x01;    
        }
        ADS1211_SCLK_L;
    }
    return(temp);
}


/*****************************************************************
  Rutina de lectura de canal de entrada analógica...
  Devuelve el valor en 10 bits
  Por @Ray2001
  ****************************************************************/
unsigned int lee_entrada_analogica(char canal){
    unsigned int valorAD;
    
    // Conversion A/D canal 0, grupo 0
    AD_ADCSR = (canal & 0x07);
    esperaMicro(50);
    AD_ADCSR |= 0x20;               // Inicio conversion
    while(!(AD_ADCSR & 0x80));      // Espera a fin de conversion (8.4 uS)
    switch (canal & 0x03){
		case 0:
	        valorAD = AD_ADDRAH * 0xff;     // Lee valor conversion alto
		    valorAD = valorAD + AD_ADDRAL;  // Lee valor conversion bajo
		    break;
		case 1:
	        valorAD = AD_ADDRBH * 0xff;     // Lee valor conversion alto
		    valorAD = valorAD + AD_ADDRBL;  // Lee valor conversion bajo
		    break;
		case 2:
	        valorAD = AD_ADDRCH * 0xff;     // Lee valor conversion alto
		    valorAD = valorAD + AD_ADDRCL;  // Lee valor conversion bajo
		    break;
		case 3:
	        valorAD = AD_ADDRDH * 0xff;     // Lee valor conversion alto
		    valorAD = valorAD + AD_ADDRDL;  // Lee valor conversion bajo
		    break;
	}
    valorAD >>= 6;                  // Reforma el valor
    return(valorAD);
}



/*****************************************************************
  Rutina de filtrado digital de valores
  Valores:
  n : numero de canal de 0 a 3
  Por @Ray2001
  ****************************************************************/
void filtrado_valores (unsigned int n){
	float filtro = 1;
	
	switch (valor_varios[n] & 0x0f){	//Seleccioina valor del filtro...
		case 0:		// No hay filtro...
			filtro = 1;
			break;
		case 1:		// Filtro 1...
			filtro = 0.9;
			break;
		case 2:		// Filtro 2...
			filtro = 0.8;
			break;
		case 3:		// Filtro 3...
			filtro = 0.7;
			break;
		case 4:		// Filtro 4...
			filtro = 0.6;
			break;
		case 5:		// Filtro 5...
			filtro = 0.5;
			break;
		case 6:		// Filtro 6...
			filtro = 0.4;
			break;
		case 7:		// Filtro 7...
			filtro = 0.3;
			break;
		case 8:		// Filtro 8...
			filtro = 0.2;
			break;
		case 9:		// Filtro 9...
			filtro = 0.1;
			break;
		case 10:	// Filtro 10...
			filtro = 0.08;
			break;
	}		
	
	valor[n] = ((1-filtro) * valor_ant[n]) + (filtro * valor[n]);
    valor_ant[n] = valor[n];
}        


/*****************************************************************
  Rutina para activar el buzzer por un tiempo
  Está relacionada con el timer1
  Tiempo en mS
  Por @Ray2001
  ****************************************************************/
void buzzer(unsigned int tiempo){
    timerZumb = tiempo;
	BUZZ_ON;
}	

/*****************************************************************
  Rutina de filtrado digital del oscilador
  Por @Ray2002
  ****************************************************************/
void motor_micropaso(unsigned int paso){
	char a;
	
	a = P6DR;
	a &= ~(MOTOR_S0 | MOTOR_S1 | MOTOR_S2 | MOTOR_S3);
	switch (paso){	//Seleccioina valor de paso...
		case 400:
			break;
		case 800:
			a |= MOTOR_S0;
			break;
		case 1600:
			a |= MOTOR_S1;
			break;
		case 3200:
			a |= (MOTOR_S0 | MOTOR_S1);
			break;
		case 6400:
			a |= MOTOR_S2;
			break;
		case 12800:
			a |= (MOTOR_S0 | MOTOR_S2);
			break;
		case 25600:
			a |= (MOTOR_S1 | MOTOR_S2);
			break;
		case 51200:
			a |= (MOTOR_S0 | MOTOR_S1 | MOTOR_S2);
			break;
		case 1000:
			a |= MOTOR_S3;
			break;
		case 2000:
			a |= (MOTOR_S0 | MOTOR_S3);
			break;
		case 5000:
			a |= (MOTOR_S1 | MOTOR_S3);
			break;
		case 10000:
			a |= (MOTOR_S0 | MOTOR_S1 | MOTOR_S3);
			break;
		case 25000:
			a |= (MOTOR_S2 | MOTOR_S3);
			break;
		case 50000:
			a |= (MOTOR_S0 | MOTOR_S2 | MOTOR_S3);
			break;
	}		
	P6DR = a;
}        

void calibra_motor(unsigned int posicion){
	float xTemp,fTemp;
	float medida = 0;
	long contador;
	
	posicion = 2000 - posicion;

	while(ADS1211_decimation(78) == TRUE){	// Decimación a 250 Hz
		inicializa_ADS1211();
	}
    // Canal 0(potenciometro)...
   	while(ADS1211_canal(0) == TRUE){   // Selecciona canal...
    	inicializa_ADS1211();                
	}
    while(ADS1211_ganancia(GAINx1) == TRUE){    // Selecciona ganancia x1
        inicializa_ADS1211();             
    }
    ADS1211_modo(ADS1211_SELF_CALIBRATION);
    // Lee potenciometro...
    xTemp = ((float)(5000) / (float)0x7fffff);
	fTemp = (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
	medida = fTemp;				// Captura valor inicial del potenciometro
	if(fTemp < POT_LIMITE_INF || fTemp > POT_LIMITE_SUP){
		error_motor = 1;		
		return;
	}
	if(fTemp < posicion){
        MOTOR_DIR_L;    
        motor_micropaso(400);
        contador = 0;
	    while(fTemp < (posicion-10)){
		    xTemp = ((float)(5000) / (float)0x7fffff);
		    fTemp = (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
		    MOTOR_CLK_H;
		    MOTOR_CLK_L;

		    if(contador > 50){				// Si en 50 pasos(1/8 de vuelta a 400ppv) no se ha movido 1/8 de vuelta...
		    	if(fabs(fTemp - medida) < 5){ 
			    	error_motor = 1;
			    	return;
	    		}
		    }

	    	if(fTemp < POT_LIMITE_INF || fTemp > POT_LIMITE_SUP){	// Comprueba limites
				error_motor = 1;			
				return;
			}
			contador++;
		}
	    motor_micropaso(12800);
	    // calibra a 0.5 vueltas...
	    xTemp = ((float)(5000) / (float)0x7fffff);
 	    fTemp = (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
	    medida = fTemp;
	    contador = 0;
	    while(fTemp < posicion){
		    xTemp = ((float)(5000) / (float)0x7fffff);
		    fTemp = (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
		    MOTOR_CLK_H;
		    MOTOR_CLK_L;
		    if(contador > 1600){				// Si en 1600 pasos(1/8 de vuelta a 12800ppv) no se ha movido 1/8 de vuelta...
		    	if(fabs(fTemp - medida) < 5){
			    	error_motor = 1;
			    	return;
		    	}
		    }
		    if(fTemp < POT_LIMITE_INF || fTemp > POT_LIMITE_SUP){	// Comprueba limites
				error_motor = 1;				
				return;
			}
			contador++;
		}
	}
	if(fTemp > posicion){
	    motor_micropaso(400);
	    MOTOR_DIR_R;   
	    contador = 0 ;
	    while(fTemp > (posicion+10)){
		    xTemp = ((float)(5000) / (float)0x7fffff);
		    fTemp = (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
		    MOTOR_CLK_H;
		    MOTOR_CLK_L;
		    if(contador > 50){				// Si en 50 pasos(1/8 de vuelta a 400ppv) no se ha movido 1/8 de vuelta...
		    	if(fabs(fTemp - medida) < 5){
			    	error_motor = 1;
			    	return;
		    	}
		    }
		    if(fTemp < POT_LIMITE_INF || fTemp > POT_LIMITE_SUP){	// Comprueba limites
				error_motor = 1;	
				return;
			}
			contador++;
		}
	    motor_micropaso(12800);
	    // calibra.....
	    xTemp = ((float)(5000) / (float)0x7fffff);
	    fTemp = (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
	    medida = fTemp;
	    contador = 0;
	    while(fTemp > posicion){
		    xTemp = ((float)(5000) / (float)0x7fffff);
		    fTemp = (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
		    MOTOR_CLK_H;
		    MOTOR_CLK_L;
		    if(contador > 1600){				// Si en 1600 pasos(1/8 de vuelta a 12800ppv) no se ha movido 1/8 de vuelta...
		    	if(fabs(fTemp - medida) < 5){
			    	error_motor = 1;
			    	return;
		    	}
		    }
		    if(fTemp < POT_LIMITE_INF || fTemp > POT_LIMITE_SUP){	// Comprueba limites
				error_motor = 1;		
		    	return;
			}
			contador++;
		}
	}
}

void comprueba_configuracion_motor(void){
    float xTemp;	// Valor temporal para calculos de escalado
    float fTemp;	// Temporal de lectura de datos

	if(!MOTOR_JP7){				// Si se selecciona el jumper del cero...
		MOTOR_DISABLE;
	    motor_micropaso(MOTOR_PASOS);
		while(!MOTOR_JP7){		// Mientras esperas a que se vuelva al estado normal....
			// Lee botones de cerrar o abrir...
			if(BOTON_ABRIR){
				MOTOR_DIR_R;
			    MOTOR_ENABLE;
			    mueve_motor(5);	// Mueve motor 5º en el sentido de abrir...
			    MOTOR_DISABLE;
			    esperaMili1(100);
			}			
			if(BOTON_CERRAR){
				MOTOR_DIR_L;
			    MOTOR_ENABLE;
			    mueve_motor(5);	// Mueve motor 5º en el sentido de abrir...
			    MOTOR_DISABLE;
			    esperaMili1(100);
			}			
		}
		
		
		// Canal 0(potenciometro)...
	   	while(ADS1211_canal(0) == TRUE){   // Selecciona canal...
	    	inicializa_ADS1211();                
		}
	    while(ADS1211_ganancia(GAINx1) == TRUE){    // Selecciona ganancia x1
	        inicializa_ADS1211();             
	    }
	    ADS1211_modo(ADS1211_SELF_CALIBRATION);
	    // Lee potenciometro...
	    xTemp = ((float)(5000) / (float)0x7fffff);
		fTemp = 2000 - (((float)ADS1211_lee_valor() - (float)0x800000) * xTemp);
		motor_cero = (unsigned int)fTemp;
	    escribeTipoEEprom (DIR_MOTOR_CERO, &motor_cero, 2);
		posicion_rel = 0;
		posicion_abs = 0;
	    MOTOR_RESET_ON;
	    esperaMili1(500);
	    MOTOR_RESET_OFF;
	    motor_micropaso(MOTOR_PASOS);
	    MOTOR_ENABLE;
   		calibra_motor(motor_cero);
	}
	if(!MOTOR_JP6){				// Si se selecciona el jumper del la posicion inicial por defecto...
		while(!MOTOR_JP6);		// Espera a que se vuelva al estado normal...
		motor_cero = POT_POSICION_INICIAL;
	    escribeTipoEEprom (DIR_MOTOR_CERO, &motor_cero, 2);
		calibra_motor(motor_cero);
	}
}

/*****************************************************************
  Rutina para comprobar los parametros iniciales de la EEPROM
  Si la EEPROM es nueva, cambia la referencia o la version del
  programa, se reestableceran los parametros por defecto.
  Por @Ray2001
  ****************************************************************/
void comprueba_parametros (void){
	const char *cadena;
	unsigned int i;
	char temp;
	char no_coincide = FALSE;

	// Comprueba referecia...
	cadena = REFERENCIA;	
	for(i = 0;cadena[i] != '\0';i++){
        leeTipoEEprom (DIR_REF + i, &temp, 1);
        if(temp != cadena[i])
        	no_coincide = TRUE;
	}        	

	// Comprueba version...
	cadena = VERSION;	
	for(i = 0;cadena[i] != '\0';i++){
        leeTipoEEprom (DIR_VER + i, &temp, 1);
        if(temp != cadena[i])
        	no_coincide = TRUE;
	}        	

	if(no_coincide == TRUE){
		// Carga valores por defecto...
		for(i=0;i<4;i++) valor_maximo[i] = 5000;
		for(i=0;i<4;i++) valor_minimo[i] = 0;
		for(i=0;i<4;i++) valor_offset[i] = 0;
		for(i=0;i<4;i++) valor_varios[i] = 0;
		for(i=0;i<4;i++) tipo_canal[i]   = 10;	// +/- 5.000v
		tipo_canal[1] = 20;		// Para la referencia es 4/20mA
		comm = 4;				// 115200 baudios
		motor_vueltas = 1800; 	// 5 vueltas por defecto
		motor_cero = POT_POSICION_INICIAL;		// Cero inicial por defecto
	    
		// Escribe valores por defecto en EEPROM...
		escribeTipoEEprom (DIR_REF, REFERENCIA, 5);
		escribeTipoEEprom (DIR_VER, VERSION, 5);
		escribeTipoEEprom (DIR_VALOR_MAXIMO, &valor_maximo, 8);
	    escribeTipoEEprom (DIR_VALOR_MINIMO, &valor_minimo, 8);
	    escribeTipoEEprom (DIR_VALOR_OFFSET, &valor_offset, 8);
	    escribeTipoEEprom (DIR_TIPO_CANAL, &tipo_canal, 4);
	    escribeTipoEEprom (DIR_VALOR_VARIOS, &valor_varios, 4);
	    escribeTipoEEprom (DIR_MOTOR_VUELTAS, &motor_vueltas, 2);
  	    escribeTipoEEprom (DIR_MOTOR_CERO, &motor_cero, 2);
    	escribeTipoEEprom (DIR_COMM, &comm, 1);
	}
}    


/*****************************************************************
  Rutina para mover el motor paso-paso un numero determinado de 
  pasos y con rampa de aceleracion y desaceleracion
  Por @Ray2003
  ****************************************************************/
void mueve_motor(unsigned int grados){
	unsigned long n;
	unsigned int t;
	unsigned long rampa_inicio;
	unsigned long rampa_final;
	unsigned long n_pasos;
	
	n_pasos = ((unsigned long)grados * (unsigned long)MOTOR_PASOS) / 360;
	rampa_inicio = (unsigned long)((MOTOR_RAMPA * (unsigned long)MOTOR_PASOS) / 360);			// Angulo de rampa incial
	rampa_final = (unsigned long)(n_pasos - ((MOTOR_RAMPA * (unsigned long)MOTOR_PASOS) / 360));	// Angulo de rampa final
	t = (unsigned int)((MOTOR_RAMPA * (unsigned long)MOTOR_PASOS) / 360);
	for(n=0;n<n_pasos;n++){
	    MOTOR_CLK_H;
	    MOTOR_CLK_L;
	    if(n < rampa_inicio) t--;
	    if(n >= rampa_final) t++;
		esperaMicro(20 + (t/50));
	}
}

/*****************************************************************
  Rutina para mover el motor hasta el zero
  Devuelve 1 si no ha encontrado el cero
  Devuelve 0 si lo encuentra
  Por @Ray2003
  ****************************************************************/
char mueve_motor_zero(void){
	unsigned long n; 
	unsigned long n_pasos;
	unsigned int t;
	unsigned int rampa_inicio;

	n_pasos = (unsigned long)((370L * 80L * MOTOR_PASOS) / (360L * 12L));
	rampa_inicio = (unsigned int)((MOTOR_RAMPA * (unsigned long)MOTOR_PASOS) / 360);			// Angulo de rampa incial
	t = (unsigned int)((MOTOR_RAMPA * (unsigned long)MOTOR_PASOS) / 360);
	
	// Si en 370º de giro no ha encontrado el zero...
	for(n=0;n<n_pasos;n++){
	    MOTOR_CLK_H;
	    MOTOR_CLK_L;
	    if(n < rampa_inicio) t--;
		esperaMicro(20 + (t/50));
	    
  		if(ENTRADA_ZERO){
	  		esperaMili1(50);
			if(ENTRADA_ZERO) 
			   	return(0);
	  	}
	}
  	return(1);
}
