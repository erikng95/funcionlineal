void com_diag(void);
float calc_pend(int largo,int ancho);
float calc_b(int xini,int yini,float a);
int calc_punt(float a,float b,int x);
void pasar_byte(int n);

uchar	tblbit[8]   = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };

/************************************************************************/
/* DIBDIAG                                                              */
/* Dibujar linea diagonal.                                              */
/************************************************************************/
void dibdiag(int l,int anch,int g)
{
   uchar f;
   int  n,i,m,j,k,o;
   int  punt,punt_inter,punt_ant,dif,dif_med;
   float a;
   uchar huge *ptr;
   uchar huge *ptr1;
   long yy,yyy;
   
/* banco de imagen */
  #if CF3000
   SET_BNK_ASM (bnkiram);
  #else
   SET_BNK_ASM (BANCO_IRAM);
  #endif
   switch (rotacion) /* y = v_pos x = (uchar)(h_pos/8) nbit = (uchar)(h_pos%8) */
     {
     case 1:
       y = v_pos;
  #if DOT_12
       x = (h_pos-g)/8;
  #else
       x = (uchar)((h_pos-g)/8);
  #endif       
       nbit = (uchar)((h_pos-g)%8);
       if (h_pos < g)
         return;
       n = l;		/* guarda el largo */
       l = anch;		/* el largo pasa a ser el ancho */
       anch = l;		/* el ancho pasa a ser el largo */
       break;
     case 2:
       y = v_pos-g;
  #if DOT_12
       x = (h_pos-l)/8;
  #else
       x = (uchar)((h_pos-l)/8);
  #endif
       nbit = (uchar)((h_pos-l)%8);
       if (v_pos < g  ||  h_pos < l)
         return;
       break;
     case 3:
       y = v_pos-l;
  #if DOT_12
       x = h_pos/8;
  #else
       x = (uchar)(h_pos/8);
  #endif
       nbit = (uchar)(h_pos%8);
       n = l;		/* guarda el largo */
       l = anch;		/* el largo pasa a ser el ancho */
       anch = l;		/* el ancho pasa a ser el largo */
       break;
     default:
       y = v_pos;
  #if DOT_12
       x = h_pos/8;
  #else
       x = (uchar)(h_pos/8);
  #endif
       nbit = (uchar)(h_pos%8);
       break;
     }
   a = calc_pend(l,anch);  
   ptr = (uchar huge *) IMAGEN;
   yy = cal_yy(); /* return (((long)y*bytlin) + x) */
  #if DOT_12
   flg_b3 = FALSE;
   if (yy >= 0x40000)
     {
     SET_BNK_ASM (BANCO_IRAM+2);
     yy -= 0x40000;
     y = (yy - x) / bytlin;
     flg_b3 = 2;
     }
  #endif   
   if (yy >= 0x20000)
     {
     SET_BNK_ASM (BANCO_IRAM+1);
     yy -= 0x20000;
     y = (yy - x) / bytlin;
  #if DOT_12
     flg_b3 = TRUE;
  #endif     
     }
   ptr += ((long) y * bytlin) + x;
  #if DOT_12
   flg = FALSE;
  #endif
  if (flg_inv == TRUE)
     {
     for (k=0;k<anch;k++)
       {
       ptr += bytlin;
       yy += bytlin;
       if (yy >= 0x20000)
         {
         SET_BNK_ASM (BANCO_IRAM+1);
         yy -= 0x20000;
         y = (yy - x) / bytlin;
         f2 == TRUE;
         }
       }
     }
   for (i=0;i<g;i++)
     {
     if (flgbln == TRUE)				/* En blanco */
       {
       for (o=0;o<=anch;o++)
         memset((ptr),0x00,l);
       }
     else
       {
       m = nbit;  /* la posicion del bit actual */
       ptr1 = ptr;
       yyy = yy;
       f = FALSE;
       punt_ant = 0; /* es el punto del bit anterior del que se esta calculando */
       for (j=0;j<=l;j++) /* numero de bits en la linea */
         {
         punt = calc_punt(a,j);   /* calculo de puntos en la funcion de linea */
         dif = punt - punt_ant; /* diferencia entre el punto del bit actual y el anterior */
         if (dif == 1) /* cambio de linea cuando el bit actual esta en la siguiente linea que el anterior bit */
           {
           if (flg_inv == TRUE)
             {
             ptr1 -= bytlin;
             yyy -= bytlin;
             }
           else
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
           }
         if (dif > 1) /* mirar si rellenar bits intermedios */
           {
           dif_med = (dif/2);
           for (punt_inter=0;punt_inter<dif_med;punt_inter++) /* rellenar bits intermedios con la mascara del bit actual */
             {
             if (flg_inv == TRUE)
               {
               ptr1 -= bytlin;
               yyy -= bytlin;
               }
             else
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
             masc1 = tblbit[m-1];
             *ptr1 = (*ptr1 & ~masc1) | masc1;
             }
           dif_med = ((dif/2) + (dif%2));
           if ((dif_med) > 1) /* mirar si hay que rellenar mas bits */
             {
             m = pasar_byte(int m);
             for (punt_inter=0;punt_inter<(dif_med-1);punt_inter++) /* rellenar bits intermedios con la mascara del siguiente bit */
                {
                if (flg_inv == TRUE)
                  {
                  ptr1 -= bytlin;
                  yyy -= bytlin;
                  }
                else
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
                masc1 = tblbit[m];
                *ptr1 = (*ptr1 & ~masc1) | masc1;
                }                   
             }
           }
         punt_ant = punt;
         m = pasar_byte(m); /* si m == 8, pasar a 0 e incrementar pointer*/
         masc1 = tblbit[m];
         *ptr1 = (*ptr1 & ~masc1) | masc1;
         m++;
         }
       if (f == TRUE)
            SET_BNK_ASM (BANCO_IRAM);
       }
     if (flg_inv == TRUE)
       {
       ptr -= bytlin;
       yy -= bytlin;
       }
     else
       {
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
    uchar rot_aux,flag_inv;
	largo = rd_int_buffer_parser();     /* Leer largo */
	get_ctr_parser();                   /* Leer "," */
    ancho = rd_int2_buffer_parser();    /* Leer ancho */
	get_ctr_parser();                   /* Leer "," */
	grosor = rd_int_buffer_parser();    /* Leer grosor */
	if (grosor == 0  ||  largo < grosor || ancho < grosor)
		return;
    flag_inv = FALSE;
	dibdiag(largo,ancho,grosor);         		/* Dibujar linea diagonal */
    flag_inv = TRUE;
	dibdiag(largo,ancho,grosor);               /* Dib linea diagonal inversa */
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

int pasar_byte(int m)
{
 if ((m == 8) /* el bit actual esta en el siguiente byte */
 {
  m = 0;
  ptr1++;
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
    return(m);
}
