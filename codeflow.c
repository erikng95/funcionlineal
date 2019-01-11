Funcion Dibujar tachado
{
/* uchar f,f2,f3
int  n,i,m,j,k
int  punt,punt_ant
float a
uchar huge *ptr
uchar huge *ptr1
long yy,yyy */ Inicializar variables;
   /*   SET_BNK_ASM (BANCO_IRAM);*/Set Banco RAM 2;
   switch (Rotacion)
     {
     case 1:
      /* y = v_pos
       x = (uchar)((h_pos-anch)/8)
       nbit = (uchar)((h_pos-anch)%8)*/Definir X,Y y posicion del primer bit (offset en X);
       if (h_pos < anch)
         return;
Intercambiar ancho y largo;
       break;
     case 2:
       /*y = v_pos-anch
       x = (uchar)((h_pos-l)/8)
       nbit = (uchar)((h_pos-l)%8)*/Definir X,Y y posicion del primer bit(offset en X,Y);
       if (v_pos < anch  ||  h_pos < l)
         return;
       break;
     case 3:
      /* y = v_pos-l
       x = (uchar)(h_pos/8)
       nbit = (uchar)(h_pos%8)*/Definir X,Y y posicion del primer bit(offset en Y);
 Intercambiar ancho y largo;		
       break;
     case 0:
      /* y = v_pos
       x = (uchar)(h_pos/8)
       nbit = (uchar)(h_pos%8)*/
       Definir X,Y y posicion del primer bit(default);
       break;
     }
      if (ancho > largo)
     {
 Intercambiar Largo y Ancho
    
     Subir Flag ancho>largo;
     }
Calculo de la pendiente;
Pointer apuntar a IMAGEN
Comprobar banco de RAM
   Posicion inicial del Pointer;
      for (i = 0;i < grosor;i++)
     {
/*m = nbit
ptr1 = ptr
yyy = yy
punt_ant = 0
flag cambio de RAM = FALSE*/ Definir variables;
for (j=0;j <= largo;j++)
         {
           /*         punt = pend * j
         punt = (int)(punt+0.5)*/
          Calculo de punto segun la pendiente;
         if (punt - punt_ant)
           {
           if (Flag anch>largo)
             {
             /*m++*/ Incrementar posicion del bit;
             }
           else
             {
             if (Flag linea inversa )
               {
               /*ptr1 -= bytlin
               yyy -= bytlin*/ Puntero Salto de linea inversa;
               if (yyy < 0x00000)
                 {
                 Set banco RAM 2
                 Flag cambio a RAM 2 subida;
                 }
               }
             else
               {
               /*ptr1 += bytlin
               yyy += bytlin*/ Puntero salto de linea;
               if (yyy >= 0x20000)
                 {
                 /*selbnk()
                 yyy -= 0x20000
                 ptr1 = (uchar huge *) IMAGEN
                 ptr1 += yyy
                 ptr1 += 32
                 f = TRUE*/ Set banco RAM 3
                 flag cambio a RAM 3 subida;
                 }
               }
             }
           }
         punt_ant = punt;
         if ((m == 8)
           {
           /*m = 0
           ptr1++
           yyy++ */Cambio de byte;
           if (yyy >= 0x20000)
             {
             Set banco RAM 3
             flag cambio a RAM 3 subida;
             }
           }
                    /*masc1 = tblbit[m]
         *ptr1 = (*ptr1 & ~masc1) | masc1*/
           Aplicar mascara segun posicion de m;
         if (flag ancho>largo)
           {
           if (flag linea inversa)
             {
 Pointer salto de linea inversa;
             if (yyy < 0x00000)
               {
                 /*               yyy += 0x20000
               ptr1 = (uchar huge *) IMAGEN
               ptr1 -= yyy
               ptr1 -= 32
               f2 = TRUE*/
               Set banco de RAM 2
               flag cambio a RAM 2 subida;
               }
             }
           else
             {
 Pointer salto de linea;
             if (yyy >= 0x20000)
               {

               Set banco de RAM 3
               flag cambio a RAM 3 subida;
               }
             }
           }
         else
/*m++*/Incrementar posicion del bit; 
         }
                if (Flag cambio a banco 3)
            Set Banco RAM 2;
       if (Flag cambio a banco 2)
            Set Banco RAM 3;
  }
}         