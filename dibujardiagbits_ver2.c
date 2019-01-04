tblmasd[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01}

/************************************************************************/
/* DIBLIN                               02-Dic-04                       */
/* Dibujar linea.                                                       */
/************************************************************************/
void dibdiag(int l,int anch,int g)
{
   uchar f;
   int  n,i,m,j,k;
   int  punt,punt_ant,dif,dif_med;
   float a;
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
       l = anch;		/* el largo pasa a ser el ancho */
       anch = l;		/* el ancho pasa a ser el largo */
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
       l = anch;		/* el largo pasa a ser el ancho */
       anch = l;		/* el ancho pasa a ser el largo */
       break;
     default:
       y = v_pos;
       x = (uchar)(h_pos/8);
       nbit = (uchar)(h_pos%8);
       break;
     }
   a = calc_pend(l,anch);  
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
       if (l > 0)
         memset((ptr),0x00,l);     
       }
     else
       {
         ptr1 = ptr;
         yyy = yy;
         m = nbit;  /*el paso de bit en bit y hacer el cambio de byte*/     
         f = FALSE;
         punt_ant = 0; /*es el punto del bit anterior del que se esta calculando*/
         for (j=0;j<l;j++) /* numero de bits en la linea */
            {    
             punt = calc_punt(a,j);   /* calculo de puntos en la funcion de linea */
             if ((m%8) == 0) /* el bit actual esta en el siguiente byte */
             {
               ptr1++;    /*cambio de byte*/          
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
             }
             dif = punt-punt_ant;
             if (dif == 1)
             {
              ptr1 += bytlin;
              yyy += bytlin;
              if (yyy >= 0x20000)
              {
                selbnk();
                yyy -= 0x20000;
                ptr1 = (uchar huge *) IMAGEN;
                ptr1 += yyy;
                ptr1 += 32;
                f = TRUE;
              }               
             }             
             if (dif > 1)
             {
              dif_med = (dif/2);   
              for(punt_inter=0;punt_inter<dif_med;punt_inter++)
              {
               ptr1 += bytlin;
               yyy += bytlin;
               if (yyy >= 0x20000)
               {
                selbnk();
                yyy -= 0x20000;
                ptr1 = (uchar huge *) IMAGEN;
                ptr1 += yyy;
                ptr1 += 32;
                f = TRUE;
               }                     
               masc1 = tblmasd[m];          
               *ptr = (*ptr & ~masc1) | masc1;        
              }
             dif_med = ((dif/2) + (dif%2));
             for(punt_inter=0;punt_inter<(dif_med-1);punt_inter++)
              {
               ptr1 += bytlin;
               yyy += bytlin;
               if (yyy >= 0x20000)
               {
                selbnk();
                yyy -= 0x20000;
                ptr1 = (uchar huge *) IMAGEN;
                ptr1 += yyy;
                ptr1 += 32;
                f = TRUE;
               }               
               masc1 = tblmasd[m+1];          
               *ptr = (*ptr & ~masc1) | masc1;        
              }
             }
             punt_ant = punt;            
             if (m>7)
                m = 0;            
             masc1 = tblmasd[m];          
             *ptr = (*ptr & ~masc1) | masc1;
             m++;            
            }
         if (f == TRUE)
           SET_BNK_ASM (BANCO_IRAM);
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
   /* float pend; */
    uchar rot_aux,flag_inv;
	largo = rd_int_buffer_parser();     /* Leer largo */
	get_ctr_parser();                   /* Leer "," */
    ancho = rd_int2_buffer_parser();    /* Leer ancho */
	get_ctr_parser();                   /* Leer "," */
	grosor = rd_int_buffer_parser();    /* Leer grosor */
	if (grosor == 0  ||  largo < grosor || ancho < grosor)
		return;
   /* pend = calc_pend(largo,ancho);
    b = calc_b(x_off,y_off,pend); */
    flag_inv = FALSE;
	dibdiag(largo,ancho,grosor,flag_inv);         		/* Dibujar linea diagonal */
    flag_inv = TRUE;
	dibdiag(largo,ancho,grosor,flag_inv);               /* Dib linea diagonal inversa */
  #endif
}

float calc_pend(int largo,int ancho)
{
    float a;
    a = ((ancho - 0) / (largo - 0)); /* pendiente = (yfin - yini) / (xfin - xini)*/
    return (a);
}

float calc_b(int xini,int yini,float a) /*Xini e Yini serian opciones de offset*/
{
    float b;
    b = yini - (a*xini); /* pendiente = (yfin - yini) / (xfin - xini)*/
    return (b);
}

int calc_punt(float a,float b,int x) /* f(x) = a*x + b */
{
    int punt;
    float d;
    
    d = a * x + b; 
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