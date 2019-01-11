void com_diag(void);

uchar	tblbit[8]   = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };

/************************************************************************/
/* DIBDIAG                                                              */
/* Dibujar linea diagonal.                                              */
/************************************************************************/
void dibdiag(int l,int anch,int g)
{
   uchar f,f2,f3;
   int  n,i,m,j,k;
   int  punt,punt_ant;
   float a;
   uchar huge *ptr;
   uchar huge *ptr1;
   long yy,yyy;
   
/* banco de imagen */
   SET_BNK_ASM (BANCO_IRAM);
   switch (rotacion)
     {
     case 1:
       y = v_pos;
       x = (uchar)((h_pos-anch)/8);
       nbit = (uchar)((h_pos-anch)%8);
       if (h_pos < anch)
         return;
       n = l;		/* guarda el largo */
       l = anch;		/* el largo pasa a ser el ancho */
       anch = l;		/* el ancho pasa a ser el largo */
       break;
     case 2:
       y = v_pos-anch;
       x = (uchar)((h_pos-l)/8);
       nbit = (uchar)((h_pos-l)%8);
       if (v_pos < anch  ||  h_pos < l)
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
   if (a > l)
     {
     n=l;
     l=a;
     a=l;
     f3 = TRUE;
     }
   pend = a / l;  /* pendiente = (yfin - yini) / (xfin - xini)*/
   ptr = (uchar huge *) IMAGEN;
   yy = cal_yy();
   if (yy >= 0x20000)
     {
     SET_BNK_ASM (BANCO_IRAM+1);
     yy -= 0x20000;
     y = (yy - x) / bytlin;  
     }
   ptr += ((long) y * bytlin) + x;
   if (flg_inv == TRUE)  /* cuando hay que hacer la inversa, mover el pointer a lo ancho */
     {
       ptr += a*bytlin;
       yy += a*bytlin;
       if (yy >= 0x20000)
         {
         SET_BNK_ASM (BANCO_IRAM+1);
         yy -= 0x20000;
         y = (yy - x) / bytlin;
         }
     }
   for (i=0;i<g;i++)
     {
     if (flgbln == TRUE)				/* En blanco */
       {
       for (k=0;k<=a;k++)
         memset((ptr),0x00,l);
       }
     else
       {
       m = nbit;  /* la posicion del 1er bit */
       ptr1 = ptr;
       yyy = yy;
       f = FALSE;
       punt_ant = 0;
       for (j=0;j<=l;j++)
         {
         punt = pend * j; /* y = a*x */
         punt = (int)(punt+0.5); /* redondeo */
         if (punt - punt_ant) /* cambio de linea */
           {
           if (f3 == TRUE)
                m++;
           else
             {
             if (flg_inv == TRUE)
               {
               ptr1 -= bytlin;
               yyy -= bytlin;
               if (yyy < 0x00000)
                 {
                 SET_BNK_ASM(BANCO_IRAM);
                 yyy += 0x20000;
                 ptr1 = (uchar huge *) IMAGEN;
                 ptr1 -= yyy;
                 ptr1 -= 32;
                 f2 = TRUE;
                 }
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
           }
         punt_ant = punt;
         if ((m == 8)
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
         masc1 = tblbit[m];
         *ptr1 = (*ptr1 & ~masc1) | masc1;
         if (f3 == TRUE)
           {
           if (flg_inv == TRUE)
             {
             ptr1 -= bytlin;
             yyy -= bytlin;
             if (yyy < 0x00000)
               {
               SET_BNK_ASM(BANCO_IRAM);
               yyy += 0x20000;
               ptr1 = (uchar huge *) IMAGEN;
               ptr1 -= yyy;
               ptr1 -= 32;
               f2 = TRUE;
               }
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
         else
            m++; 
         }
       if (f == TRUE)
            SET_BNK_ASM (BANCO_IRAM);
       if (f2 == TRUE)
            SET_BNK_ASM (BANCO_IRAM+1);
       }
     if (flg_inv == TRUE)
       {
       ptr -= bytlin;
       yy -= bytlin;
       if (yy < 0x00000)
         {
         SET_BNK_ASM (BANCO_IRAM);
         yy += 0x20000;
         ptr = (char *) IMAGEN;
         ptr -= yy;
         ptr -= 32;
         }
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
    uchar flag_inv;
	largo = rd_int_buffer_parser();     /* Leer largo */
	get_ctr_parser();                   /* Leer "," */
    ancho = rd_int2_buffer_parser();    /* Leer ancho */
	get_ctr_parser();                   /* Leer "," */
	grosor = rd_int_buffer_parser();    /* Leer grosor */
    largo2 = rd_int_buffer_parser();     /* Leer largo 2 */
	get_ctr_parser();                   /* Leer "," */
    ancho2 = rd_int2_buffer_parser();    /* Leer ancho 2 */
	get_ctr_parser();                   /* Leer "," */
	grosor2 = rd_int_buffer_parser();    /* Leer grosor 2 */
	if (grosor == 0  ||  largo < grosor || ancho < grosor)
		return;
    flag_inv = FALSE;
	dibdiag(largo,ancho,grosor);         		/* Dibujar linea diagonal */
	if (grosor2 == 0  ||  largo2 < grosor2 || ancho2 < grosor2)
		return;
    flag_inv = TRUE;
	dibdiag(largo2,ancho2,grosor2);               /* Dib linea diagonal inversa */
  #endif
}