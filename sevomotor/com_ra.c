// ***** Comando común : lectura de Referencia
if(acBuffer[4] == 'R' && acBuffer[5] == 'E' && acBuffer[6] == 'F'){ 
    // Montaje de cadena de respuesta...
    acBuffer[3] = ACK;
    strcpy(&acBuffer[4],REFERENCIA);
    acBuffer[9]  = ETX;
    acBuffer[10] = calculaBCC(acBuffer,9);  //Calcula BCC
    esperaMili(TIME_RX_TX);                 //Espera tiempo entre RX y TX
    transmiteCadena (acBuffer,10);          //Transmite cadena
    return;
}

// ***** Comando común: lectura de Version
if(acBuffer[4] == 'V' && acBuffer[5] == 'E' && acBuffer[6] == 'R'){ 
    // Montaje de cadena de respuesta...
    acBuffer[3] = ACK;
    strcpy(&acBuffer[4],VERSION);
    acBuffer[9]  = ETX;
    acBuffer[10] = calculaBCC(acBuffer,9);  //Calcula BCC
    esperaMili(TIME_RX_TX);                 //Espera tiempo entre RX y TX
    transmiteCadena (acBuffer,10);          //Transmite cadena
    return;
}

// ***** Comando común: lectura de velocidad de comunicaciones
if(acBuffer[5] == 'C' && acBuffer[6] == 'O' && acBuffer[7] == 'M'){ 
	montaCadena_i ((unsigned int)comm,1);
    return;
}

// ***** Comando : lectura de tipo de canal
if(acBuffer[4] == 'C' && acBuffer[5] == 'T'){ 
	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
		error_com(5);
		return;
	}					
	if((n > 3) || (n < 1)){					//Error en numero de canal
		error_com(5);
		return;    		
	}		
	if(n<3)
		montaCadena_i ((unsigned int)(tipo_canal[n-1+2]),2);	//Envia respuesta
	else		
		montaCadena_i ((unsigned int)(tipo_canal[1]),2);	//Envia respuesta del canal de referencia
    return;
}

// ***** Comando : lectura de canal
if(acBuffer[4] == 'C' && acBuffer[5] == 'H'){ 
    if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
		error_com(5);
		return;
	}					
	if((n > 2) || (n < 1)){					//Error en numero de canal
		error_com(5);
		return;    		
	}		
	//montaCadena_s ((signed int)valor[n-1],5);	//Envia respuesta
	montaCadena_s ((signed int)valor[n-1+2],5);	//Envia respuesta
    return;
}

// ***** Comando : lectura de minimo de escala
if(acBuffer[4] == 'M' && acBuffer[5] == 'I'){ 
	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
		error_com(5);
		return;
	}					
	if((n > 2) || (n < 1)){					//Error en numero de canal
		error_com(5);
		return;    		
	}		
	montaCadena_s (valor_minimo[n-1+2],5);	//Envia respuesta
    return;
}

// ***** Comando : lectura de maximo de escala
if(acBuffer[4] == 'M' && acBuffer[5] == 'A'){ 
	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
		error_com(5);
		return;
	}					
	if((n > 2) || (n < 1)){					//Error en numero de canal
		error_com(5);
		return;    		
	}		
	montaCadena_s (valor_maximo[n-1+2],5);	//Envia respuesta
    return;
}

// ***** Comando : lectura de offset
if(acBuffer[4] == 'O' && acBuffer[5] == 'T'){ 
	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
		error_com(5);
		return;
	}					
	if((n > 2) || (n < 1)){					//Error en numero de canal
		error_com(5);
		return;    		
	}		
	montaCadena_s (valor_offset[n-1+2],5);	//Envia respuesta
    return;
}

// ***** Comando : lectura de filtro digital
if(acBuffer[4] == 'F' && acBuffer[5] == 'L'){ 
	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
		error_com(5);
		return;
	}					
	if((n > 2) || (n < 1)){					//Error en numero de canal
		error_com(5);
		return;    		
	}		
	montaCadena_i ((unsigned int)(valor_varios[n-1] & 0x0f),2);	//Envia respuesta
    return;
}

// ***** Comando : lectura de estado alarma
if(acBuffer[4] == 'E' && acBuffer[5] == 'R' && acBuffer[6] == 'R'){ 
	montaCadena_i ((unsigned int)error_motor,2);	//Envia respuesta
    return;
}

// ***** Comando : lectura de numero de vueltas
if(acBuffer[4] == 'm' && acBuffer[5] == 'a' && acBuffer[6] == 'x'){ 
	montaCadena_i (motor_vueltas,4);		//Envia respuesta
    return;
}

// ***** Comando : lectura de posicion del eje en grados
if(acBuffer[4] == 'P' && acBuffer[5] == 'V' && acBuffer[6] == ' '){ 
	montaCadena_i (motor_posicion,4);		//Envia respuesta
    return;
}

// ***** Comando : lectura del cero o posicion inicial
if(acBuffer[4] == 'Z' && acBuffer[5] == 'E' && acBuffer[6] == 'R'){ 
	montaCadena_i ((unsigned int)(((long)motor_cero*3600)/2000),4);		//Envia respuesta
    return;
}

// ***** Comando : lectura de posicion del eje en % respecto al numero de vueltas
if(acBuffer[4] == 'P' && acBuffer[5] == 'O' && acBuffer[6] == 'S'){ 
	montaCadena_i ((unsigned int)(((long)motor_posicion * (long)100)/motor_vueltas),3);		//Envia respuesta
    return;
}

// ***** Comando común: lectura de posicion (modo posicionamiento)
if(acBuffer[4] == 'P' && acBuffer[5] == 'O' && acBuffer[6] == 'V'){ 
	montaCadena_i ((unsigned int)posicion + 1,2);
    return;
}
