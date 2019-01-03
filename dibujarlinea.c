/************************************************************************/
/* DIBLIN                               02-Dic-04                       */
/* Dibujar linea.                                                       */
/************************************************************************/
void diblin(int l,int g)
{
   uchar f;
   int  n,i,m,j;
   uchar huge *ptr;
   uchar huge *ptr1;
   long yy,yyy;

/* banco de imagen */
   SET_BNK_ASM (BANCO_IRAM);
   switch (rotacion)
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
   yy = cal_yy();
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
   m = n/8;              				/* Numero de bytes sin el primero */
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
       *ptr = (*ptr & ~masc1) | masc1; 	/* 1er byte */
       ptr1 = ptr;
       yyy = yy;
       if (m > 0)
         {
         f = FALSE;
         for (j=0;j<m;j++)
           {
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
           *ptr1 = 0xff;
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