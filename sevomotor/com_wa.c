	// ***** Comando : escritura de velocidad de comunicaciones
    if(acBuffer[4] == 'C' && acBuffer[5] == 'O' && acBuffer[6] == 'M'){ 
    	acBuffer[3] = ACK;
        acBuffer[8] = '\0';
        // Convierte a signed int la cadena y comprueba errores...
        if(atoi_ray (&acBuffer[7],&iTemp) == 1){	// Error en la conversion...     
    	    error_com(4);							// Error de escritura incorrecto
            return;
    	}            
        if(iTemp > 4){								// Comprueba limites
    	    error_com(4);							// Error de escritura incorrecto        
            return;
    	}            
        escritura_correcta();
        comm = (char)iTemp;
        // Ahora, guarda los parametros en la EEprom...
   	    escribeTipoEEprom (DIR_COMM, &comm, 1);
        return;
	}

    // ***** Comando : escritura de tipo de canal
    if(acBuffer[4] == 'C' && acBuffer[5] == 'T'){ 
    	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
    		error_com(5);
    		return;
    	}					
    	if((n > 3) || (n < 1)){					//Error en numero de canal
    		error_com(5);
    		return;    		
    	}		
        acBuffer[3] = ACK;
        acBuffer[9] = '\0';
        // Convierte a signed int la cadena y comprueba errores...
        if(atoi_ray (&acBuffer[7],&iTemp) == 1){	// Error en la conversion...     
	        error_com(4);							// Error de escritura incorrecto
            return;
		}            
        if(iTemp > 21 || iTemp < 10){				// Comprueba limites
	        error_com(4);							// Error de escritura incorrecto        
            return;
		}            
		if(n<3)
	        tipo_canal[n-1+2] = (char)iTemp;
        else
	        tipo_canal[1] = (char)iTemp;			// Si es canal de referencia
        escritura_correcta();
        // Ahora, guarda los parametros en la EEprom...
        escribeTipoEEprom (DIR_TIPO_CANAL, &tipo_canal, 4);
        return;
    }

    // ***** Comando : escritura de minimo de escala
    if(acBuffer[4] == 'M' && acBuffer[5] == 'I'){ 
    	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
    		error_com(5);
    		return;
    	}					
    	if((n > 2) || (n < 1)){					//Error en numero de canal
    		error_com(5);
    		return;    		
    	}		
        acBuffer[3] = ACK;
        // Convierte a signed int la cadena y comprueba errores...
        if(atos_ray (&acBuffer[7],&sTemp,6) == 1){	// Error en la conversion...     
	        error_com(4);							// Error de escritura incorrecto
            return;
		}            
        if((sTemp > MAX_LIM) || (sTemp < MIN_LIM)){	// Comprueba limites
	        error_com(4);							// Error de escritura incorrecto        
            return;
		}            
        valor_minimo[n-1+2] = sTemp;
        escritura_correcta();
        // Ahora, guarda los parametros en la EEprom...
        escribeTipoEEprom (DIR_VALOR_MINIMO, &valor_minimo, 8);
        return;
    }

    // ***** Comando : escritura de maximo de escala
    if(acBuffer[4] == 'M' && acBuffer[5] == 'A'){ 
    	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
    		error_com(5);
    		return;
    	}					
    	if((n > 2) || (n < 1)){					//Error en numero de canal
    		error_com(5);
    		return;    		
    	}		
        acBuffer[3] = ACK;
        // Convierte a signed int la cadena y comprueba errores...
        if(atos_ray (&acBuffer[7],&sTemp,6) == 1){	// Error en la conversion...     
	        error_com(4);							// Error de escritura incorrecto
            return;
		}            
        if((sTemp > MAX_LIM) || (sTemp < MIN_LIM)){	// Comprueba limites
	        error_com(4);							// Error de escritura incorrecto        
            return;
		}            
        valor_maximo[n-1+2] = sTemp;
        escritura_correcta();
        // Ahora, guarda los parametros en la EEprom...
        escribeTipoEEprom (DIR_VALOR_MAXIMO, &valor_maximo, 8);
        return;
    }
    
    // ***** Comando : escritura de offset
    if(acBuffer[4] == 'O' && acBuffer[5] == 'T'){ 
    	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
    		error_com(5);
    		return;
    	}					
    	if((n > 2) || (n < 1)){					//Error en numero de canal
    		error_com(5);
    		return;    		
    	}		
        acBuffer[3] = ACK;
        // Convierte a signed int la cadena y comprueba errores...
        if(atos_ray (&acBuffer[7],&sTemp,6) == 1){	// Error en la conversion...     
	        error_com(4);							// Error de escritura incorrecto
            return;
		}            
        if((sTemp > MAX_LIM) || (sTemp < MIN_LIM)){	// Comprueba limites
	        error_com(4);							// Error de escritura incorrecto        
            return;
		}            
        valor_offset[n-1+2] = sTemp;
        escritura_correcta();
        // Ahora, guarda los parametros en la EEprom...
        escribeTipoEEprom (DIR_VALOR_OFFSET, &valor_offset, 8);
        return;
    }

    // ***** Comando : escritura de filtro digital
    if(acBuffer[4] == 'F' && acBuffer[5] == 'L'){ 
    	if(atos_ray(&acBuffer[6],&n,1) == 1){	//Error en numero de canal
    		error_com(5);
    		return;
    	}					
    	if((n > 2) || (n < 1)){					//Error en numero de canal
    		error_com(5);
    		return;    		
    	}		
        acBuffer[3] = ACK;
        acBuffer[9] = '\0';
        // Convierte a signed int la cadena y comprueba errores...
        if(atoi_ray (&acBuffer[8],&iTemp) == 1){	// Error en la conversion...     
	        error_com(4);							// Error de escritura incorrecto
            return;
		}            
        if(iTemp > 10){								// Comprueba limites
	        error_com(4);							// Error de escritura incorrecto        
            return;
		}            
        valor_varios[n-1+2] = valor_varios[n-1+2] & 0xf0;
        valor_varios[n-1+2] = valor_varios[n-1+2] | (char)iTemp;
        escritura_correcta();
        // Ahora, guarda los parametros en la EEprom...
        escribeTipoEEprom (DIR_VALOR_VARIOS, &valor_varios, 4);
        return;
    }
    
    // ***** Comando : escritura de numero máximo de vueltas
    if(acBuffer[4] == 'm' && acBuffer[5] == 'a' && acBuffer[6] == 'x'){ 
        acBuffer[3] = ACK;
        acBuffer[11] = '\0';
        // Convierte a int la cadena y comprueba errores...
        if(atoi_ray (&acBuffer[7],&iTemp) == 1){	// Error en la conversion...     
	        error_com(4);							// Error de escritura incorrecto
            return;
		}            
        if((iTemp > MAX_MOTOR_VUELTAS) || (iTemp < MIN_MOTOR_VUELTAS)){	// Comprueba limites
	        error_com(4);							// Error de escritura incorrecto        
            return;
		}            
		if(!MODO_LOCAL){
	        motor_vueltas = iTemp;
        }
        escritura_correcta();
        // Ahora, guarda los parametros en la EEprom...
        if(!MODO_LOCAL){
	        escribeTipoEEprom (DIR_MOTOR_VUELTAS, &motor_vueltas, 2);
        }
       	if(!MODO_LOCAL){
	        posicion_rel = 0;
			posicion_abs = 0;
	        calibra_motor(motor_cero);
        }
        return;
    }


    // ***** Comando : escritura del cero 
    if(acBuffer[4] == 'Z' && acBuffer[5] == 'E' && acBuffer[6] == 'R'){ 
        acBuffer[3] = ACK;
        acBuffer[11] = '\0';
        // Convierte a int la cadena y comprueba errores...
        if(atoi_ray (&acBuffer[7],&iTemp) == 1){	// Error en la conversion...     
	        error_com(4);							// Error de escritura incorrecto
            return;
		}            
        if((iTemp > MAX_MOTOR_CERO) || (iTemp < MIN_MOTOR_CERO)){	// Comprueba limites
	        error_com(4);							// Error de escritura incorrecto        
            return;
		}            
        motor_cero = (unsigned int)(((long)iTemp*2000)/3600);
        escritura_correcta();
        // Ahora, guarda los parametros en la EEprom...
        escribeTipoEEprom (DIR_MOTOR_CERO, &motor_cero, 2);
        posicion_rel = 0;
		posicion_abs = 0;
        calibra_motor(motor_cero);
        return;
    }

    // ***** Comando : escritura del cero 
    if(acBuffer[4] == 'P' && acBuffer[5] == 'O' && acBuffer[6] == 'V'){ 
        acBuffer[3] = ACK;
        acBuffer[9] = '\0';
        // Convierte a int la cadena y comprueba errores...
        if(atoi_ray (&acBuffer[7],&iTemp) == 1){	// Error en la conversion...     
	        error_com(4);							// Error de escritura incorrecto
            return;
		}            
        if((iTemp > 8) || (iTemp < 1)){				// Comprueba limites
	        error_com(4);							// Error de escritura incorrecto        
            return;
		}            
        posicion = (char)(iTemp - 1);
        escritura_correcta();
        return;
    }
