//imagen.c
#if LINEA_OBLICUA
void dibobli(int l,int a,int g);
#endif
/************************************************************************/
/* DIBOBLI                               05-Feb-2019                    */
/* Dibujar linea oblicua.                                               */
/************************************************************************/
#if LINEA_OBLICUA
void dibobli(int l,int a,int g)
{
   uchar f,f2,f3;
   int  n,i,m,j,k;
   int  punt,punt_ant,pend;
   int  dir_y,dir_x;
   uchar huge *ptr;
   uchar huge *ptr1;
   long yy,yyy;
   
/* banco de imagen */
   SET_BNK_ASM (BANCO_IRAM);
   y = v_pos;
   x = (uchar)(h_pos/8);
   nbit = (uchar)(h_pos%8);
   dir_x = 1;
   dir_y = 1;
   if (a < 0)
     {
     dir_y = -1;
     a = abs(a);
     }
   if (l < 0)
     {
     dir_x = -1;
     l = abs(l);   
     }
   if (a > l) /* se cambian los parametros antes de calcular para conseguir 1/m */
     {
     n=l;
     l=a;
     a=n; 
     f3 = TRUE;
     }
   pend = (a-1)*100 / (l-1);  /* m = (yfin) / (xfin), cuando el ancho>largo el valor es 1/m */
   ptr = (uchar huge *) IMAGEN;
   yy = cal_yy();
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
       for (k=0;k<=a;k++)
         memset((ptr),0x00,(l/8)+(l%8));
       }
     else
       {
       m = nbit;  /* la posicion del 1er bit */
       ptr1 = ptr;
       yyy = yy;
       f2 = FALSE;  
       f = FALSE;
       punt_ant = 0;
       for (j=0;j<l;j++)
         {
         punt = pend * j; /* y = m*x, cuando ancho>largo es x = y * 1/m */
         punt = (punt+50)/100;
         if (punt - punt_ant) == 1) /* cambio de linea */
           {
           if (f3 == TRUE)
                m += dir_x;
           else
             {
               ptr1 += (bytlin*dir_y);
               yyy += (bytlin*dir_y);
               if (yyy >= 0x20000)
                 {
                 selbnk();
                 yyy -= 0x20000;
                 ptr1 = (uchar huge *) IMAGEN;
                 ptr1 += yyy;
                 ptr1 += 32;
                 f = TRUE;
                 }
               if (yyy < 0x00000)
                 {
                 SET_BNK_ASM(BANCO_IRAM);
                 yyy += 0x1FFFF;
                 ptr1 = (uchar huge *) IMAGEN;
                 ptr1 -= yyy;
                 ptr1 -= 32;
                 f2 = TRUE;
                 }				 
             }
           }
         punt_ant = punt;
         if (m == 8)
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
         if (m < 0)
           {
           m = 7;
           ptr1--;
           yyy--;
           if (yyy < 0x00000)
             {
             SET_BNK_ASM(BANCO_IRAM);
             yyy += 0x1FFFF;
             ptr1 = (uchar huge *) IMAGEN;
             ptr1 -= yyy;
             ptr1 -= 32;
             f2 = TRUE;
             }             
           }                      
         masc1 = tblbit[m];
         *ptr1 = *ptr1 | masc1;
         if (f3 == TRUE) 
           {
             ptr1 += (bytlin*dir_y);
             yyy += (bytlin*dir_y);
             if (yyy >= 0x20000)
               {
               selbnk();
               yyy -= 0x20000;
               ptr1 = (uchar huge *) IMAGEN;
               ptr1 += yyy;
               ptr1 += 32;
               f = TRUE;
               }
             if (yyy < 0x00000)
               {
               SET_BNK_ASM(BANCO_IRAM);
               yyy += 0x1FFFF;
               ptr1 = (uchar huge *) IMAGEN;
               ptr1 -= yyy;
               ptr1 -= 32;
               f2 = TRUE;
               }
           }
         else
            m += dir_x; 
         }
       if (f == TRUE)
            SET_BNK_ASM (BANCO_IRAM);
       if (f2 == TRUE)
            SET_BNK_ASM (BANCO_IRAM+1);
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
#endif

// Parser.c
#if LINEA_OBLICUA
extern void dibobli(int l,int a,int g);
#endif

#if LINEA_OBLICUA
void com_M(void);
#endif

#if LINEA_OBLICUA
void com_M(void)
{
  #if !CPU_485_M
	int h;
	largo = rd_int2_buffer_parser();     /* Leer largo */
	get_ctr_parser();                   /* Leer "," */
    ancho = rd_int2_buffer_parser();    /* Leer ancho */
	get_ctr_parser();                   /* Leer "," */
	grosor = rd_int_buffer_parser();    /* Leer grosor */
	if (grosor == 0  ||  largo < grosor || ancho < grosor)
		return;
	largo -= hposter;
	ancho -= v_pos;
	dibobli(largo,ancho,grosor);         		/* Dibujar linea oblicua */
  #endif
}
#endif
// tablas0.tbl
#if LINEA_OBLICUA
extern void com_M (void);
#endif

#if LINEA_OBLICUA
	com_M,      /* '^Mxxx,yyyy,zzz' */            	/*   tachado                  */
#else
    com_nad,    /* */
#endif

//includes/defines.h

#define LINEA_OBLICUA 0