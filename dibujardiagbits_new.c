tblmasl[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01}
tblmas4[8] = {0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE}

/************************************************************************/
/* DIBLIN                               02-Dic-04                       */
/* Dibujar linea.                                                       */
/************************************************************************/
void dibdiag(int l,int g, float a,float b,uchar flag_inv)
{
   uchar f;
   int  n,i,m,j,k,punt,punt_aux;
   uchar huge *ptr;
   uchar huge *ptr1;
   long yy,yyy;
   
/* banco de imagen */
   SET_BNK_ASM (BANCO_IRAM);
   switch (rotacion) /* y = v_pos x = (uchar)(h_pos/8) nbit = (uchar)(h_pos%8) */
     {
     case 1:
       y = v_pos;
       x = (uchar)((h_pos-g)/8);
       nbit = (uchar)((h_pos-g)%8);
       if (h_pos < g)
         return;
       n = l;		/* guarda el largo */
       l = g;		/* el grosor pasa a ser el largo */
       g = n;		/* el largo pasa a ser el grosor */
       break;
     case 2:
       y = v_pos-g;
       x = (uchar)((h_pos-l)/8);
       nbit = (uchar)((h_pos-l)%8);
       if (v_pos < g  ||  h_pos < l)
         return;
       break;
     case 3:
       y = v_pos-l;
       x = (uchar)(h_pos/8);
       nbit = (uchar)(h_pos%8);
       n = l;		/* guarda el largo */
       l = g;		/* el grosor pasa a ser el largo */
       g = n;		/* el largo pasa a ser el grosor */
       break;
     default:
       y = v_pos;
       x = (uchar)(h_pos/8);
       nbit = (uchar)(h_pos%8);
       break;
     }
   ptr = (uchar huge *) IMAGEN;
   yy = cal_yy(); /* return (((long)y*bytlin) + x) */
   if (yy >= 0x20000)
     {
     SET_BNK_ASM (BANCO_IRAM+1);
     yy -= 0x20000;
     y = (yy - x) / bytlin;
     }
   ptr += ((long) y * bytlin) + x;
   n = l - 8 + nbit;              	/* Bits que faltan */
   if (l < 8)
     {
     masc1 = tblmas4[l] >> nbit;
     masc2 = tblmas4[l] << (8-nbit);
     }
   else
     {
     masc1 = tblmasl[nbit];
     masc2 = ~tblmasl[n%8];
     }
   for (i=0;i<g;i++)
     {
     if (flgbln == TRUE)				/* En blanco */
       {
       *ptr = *ptr & ~masc1; 			/* 1er byte */
       if (m > 0)
         memset((ptr+1),0x00,m);     	/* Bytes centrales */
       if (masc2 != 0)
         *(ptr+m+1) = *(ptr+m+1) & ~masc2;
       }
     else
       {
       masc1 = tblmasl[nbit];       
       *ptr = (*ptr & ~masc1) | masc1; /*1er bit*/
       ptr1 = ptr;
       yyy = yy;
       k = nbit;       
       if (n > 0) /* n es el numero de bits sin el primero */
         {
         f = FALSE;
         punt_aux = 0;
         for (j=0;j<n;j++) /* numero de bits en la linea */
           {
           punt = calc_punt(a,j);   /* calculo de puntos en la funcion de linea */ /*0*/
           if((n%8) != 0) /* el bit esta en el mismo byte que el anterior */
           {
           for(punt_inter=0;punt_inter<(punt-punt_aux);punt_inter++)
           {
             ptr1 += bytlin;
             masc1 = tblmasl[k];          
            *ptr = (*ptr & ~masc1) | masc1;        
           }                   
           }
           else /*cuando n es multiplo de 8, cambio de byte*/
           {
           ptr1++; /* cambio de byte */
           for(punt_inter=0;punt_inter<(punt-punt_aux);punt_inter++)
           {
             ptr1 += bytlin;
             masc1 = tblmasl[k];          
            *ptr = (*ptr & ~masc1) | masc1;        
           }               
           }
           punt_aux = punt;
           yyy++;              
           if (yyy >= 0x20000)
             {
             selbnk();
             yyy -= 0x20000;
             ptr1 = (uchar huge *) IMAGEN;
             ptr1 += yyy;
             ptr1 += 32;
             f = TRUE;
             }
            k++;
            if(k>7)
                k = 0;            
            masc1 = tblmasl[k];          
            *ptr = (*ptr & ~masc1) | masc1;             
           }
         if (f == TRUE)
           SET_BNK_ASM (BANCO_IRAM);
         }
       if (masc2 != 0)
         {
         ptr1++;
         yyy++;
         f = FALSE;
         if (yyy >= 0x20000)
           {
           selbnk();
           yyy -= 0x20000;
           ptr1 = (uchar huge *) IMAGEN;
           ptr1 += yyy;
           ptr1 += 32;
           f = TRUE;
           }
         *ptr1 = (*ptr1 & ~masc2) | masc2;
         if (f == TRUE)
           SET_BNK_ASM (BANCO_IRAM);
         }
       }
     ptr += bytlin;
     yy += bytlin;
     if (yy >= 0x20000)
       {
       selbnk();
       yy -= 0x20000;
       ptr = (char *) IMAGEN;
       ptr += yy;
       ptr += 32; 
       }
     _WATCHDOG;
     tstmon();                      	/* Mirar si llamar a monitor */
     }
   SET_BNK_ASM (0);
}

void com_diag(void)
{
  #if !CPU_485_M
	int h;
    float pend;
    uchar rot_aux,flag_inv;
	largo = rd_int_buffer_parser();     /* Leer largo */
	get_ctr_parser();                   /* Leer "," */
    ancho = rd_int2_buffer_parser();    /* Leer ancho */
	get_ctr_parser();                   /* Leer "," */
	grosor = rd_int_buffer_parser();    /* Leer grosor */
	if (grosor == 0  ||  largo < grosor || ancho < grosor)
		return;
    pend = calc_pend(largo,ancho);
    b = calc_b(h_pos,v_pos,pend);
    flag_inv = 0;
	dibdiag(largo,grosor,pend,b,flag_inv);         		/* Dibujar linea diagonal */
    flag_inv = 1;
	dibdiag(largo,grosor,pend,flag_inv);               /* Dib linea diagonal inversa */
  #endif
}

float calc_pend(int largo,int ancho)
{
    float a;
    a = ((ancho - v_pos) / (largo - h_pos)); /* pendiente = (yfin - yini) / (xfin - xini)*/
    return (a);
}

float calc_b(int xini,int yini,float a)
{
    float b;
    b = yini - (a*xini); /* pendiente = (yfin - yini) / (xfin - xini)*/
    return (b);
}

int calc_punt(float a,float b,int x) /* f(x) = a*x + b */
{
    int punt;
    float d;
    
    d = a * j + b; 
    punt = (d + 0.5); /* redondear (pasar de float a int)*/
    
    return (punt);
}


/* 

int calc_punt(float a,int x)
{ 

float d;
d = a * x;
return ((int)d+ 0.5);

} 

*/