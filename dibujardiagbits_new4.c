tblmasl[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01}
tblmas4[8] = {0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE}

/************************************************************************/
/* DIBLIN                               02-Dic-04                       */
/* Dibujar linea.                                                       */
/************************************************************************/
void dibdiag(int l,int g, float a,float b,uchar flag_inv)
{
   uchar f;
   int  n,i,m,j,k,punt,punt_ant;
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
       ptr1 = ptr;
       yyy = yy;
       m = nbit;  /*el paso de bit en bit y hacer el cambio de byte*/     
       if (n > 0) /* n es el numero de bits sin el primero */
         {
         f = FALSE;
         punt_ant = 0; /*es el punto del bit anterior del que se esta calculando*/
         for (j=0;j<l;j++) /* numero de bits en la linea */
           {    
           punt = calc_punt(a,j);   /* calculo de puntos en la funcion de linea */
           if((m%8) == 0) /* el bit actual esta en el mismo byte que el anterior */
           {
               ptr1++;    /*cambio de byte*/          
           }
           for(punt_inter=0;punt_inter<(punt-punt_ant);punt_inter++)
           {   
             ptr1 += bytlin;
             masc1 = tblmasl[m+1];          
            *ptr = (*ptr & ~masc1) | masc1;        
           }    
          punt_ant = punt;
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
          if(m>7)
                m = 0;            
            masc1 = tblmasl[m];          
            *ptr = (*ptr & ~masc1) | masc1;
            m++;            
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
    a = ((ancho - 0) / (largo - 0)); /* pendiente = (yfin - yini) / (xfin - xini)*/
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