#include<stdlib.h>
#include<stdio.h>
#include<math.h>

void  generador(int dim,long int semilla,long double *random) ;
//void  lineal(float a,float b,float,c,int dim,float *x,float *y) ;
void  gauss(int n_disc,int nro_ran,long double *muestra,long double mu,long double sigma, long int semilla) ;
long double likelihood(int dim,long double *x,long double *y,long double a,long double b, long double c) ;
//int   priors(float min1,float max1,float v1,float min2,float max2,float v2) ;
long double alfa(long double l_can,long double l_ant) ;

int main(int argc, char *argv[])
{
  long double    *x,*y,a,b,*error,*yerror,random ;
  int            j,dim,l_cadena,p,acepta         ;
  long double    bc=10.,cc=10.                   ;
  long double    *l,l_ant,al,porce               ;
  long double    bposta, cposta                  ;
  long double    cociente,*cand_b,*cand_c        ;
  long int       semilla                         ;
  long double    xx                              ;
  long double    yy,ac=1e-18, aposta, *cand_a    ;
  FILE           *dat,*par                       ;

  //xy     =fopen("xy_teoricos.dat","w") ;
  //xyerr  =fopen("xy_ruidos.dat","w")   ;
  par      =fopen("parametros.dat","w")  ;  
  dat      =fopen("datos.txt","r")  ;  
  dim      =1000  ;
  l_cadena =50000 ;
  acepta   =0     ;
  
  x      =(long double *)malloc(dim*sizeof(long double)) ;//abscisas recta teórica
  y      =(long double *)malloc(dim*sizeof(long double)) ;//ordenadas recta //
  l      =(long double *)malloc(l_cadena*sizeof(long double)) ;//valor de log(likelihood)
  cand_a =(long double *)malloc(l_cadena*sizeof(long double)) ;//vector para el generador random, en este caso-->
  cand_b =(long double *)malloc(l_cadena*sizeof(long double)) ;//  de dimensión uno pues sólo necesito un valor
  cand_c =(long double *)malloc(l_cadena*sizeof(long double)) ;//  de dimensión uno pues sólo necesito un valor


  //generador(dim,1117,&x[0])                           ;//genero dominio de la recta
  //lineal(a=6,b=3,dim,x,y)                             ;//calculo ordenadas 
//  for(j=0;j<dim;j++) fprintf(xy,"%f %f \n",x[j],y[j]) ;//guardo valores teóricos
//  gauss(1000,dim,error,0,0.000005,1013)                      ;//calculo errores con distr. normal

  for(j=0;j<41;j++){
    fscanf(dat,"%lf %Le",&xx,&yy);
    x[j]=xx;
    //y[j]=yy;
    printf("%i %lf %Le \n",j,xx,yy);
  }

  srand48(1013)                            ;//inicializo drand48()

  semilla=lrand48()                        ;//me da un valor entero que uso como semilla en los candidatosi
  gauss(1000,l_cadena,cand_a,0,0.36,semilla) ;//genero candidato de una normal centrada en ac

  semilla=lrand48()                       ;//me da un valor entero que uso como semilla en los candidatos
  gauss(1000,l_cadena,cand_b,0,0.36,semilla) ;//idem en bc

  semilla=lrand48()                       ;//me da un valor entero que uso como semilla en los candidatos
  gauss(1000,l_cadena,cand_c,0,0.36,semilla) ;//idem en bc

  for(j=0;j<l_cadena;j++){
//    p=priors(0,10,ac,0,10,bc) ;//función priors, cuando ac y bc se encuentran en la región p=1.
    p=1;
    if(p==1){
      l[j]=likelihood(dim,x,y,ac,bc,cc) ;//calculo likelihood para candidatos ac y bc  
      if(j>0){
        //cociente=l[j]/l[j-1] ;
        al=alfa(l[j],l[j-1]) ;
        random=drand48()     ;
        //printf("%f\n",random) ;
        if(random<al){
        //if(cociente<1){
          fprintf(par,"%d %f %Le %f %f \n",j,l[j],ac,bc,cc) ;
          aposta=ac ;
          bposta=bc ;
          cposta=cc ;
          acepta=acepta+1 ;
          porce=(float)acepta*100/l_cadena ;
          printf("hola \n"); 
        }
      }
    } 
    ac=aposta+cand_a[j] ;//renombro candidatos
    bc=bposta+cand_b[j] ;
    cc=cposta+cand_c[j] ;
  }
  printf("Acepta un %f por ciento \n",porce) ;
  free(x)       ;
  free(y)       ;
  free(l)       ;
  free(cand_a)  ;
  free(cand_b)  ;
  free(cand_c)  ;
  fclose(par)   ;
}

void generador(int dim,long int semilla,long double *random)
{
  int j;
  srand48(semilla) ; 
  for(j=0 ; j<dim ; j++)random[j]=drand48() ;
}

//void lineal(float a,float b,int dim,float *x,float *y)
//{
//  int i ;
//  for(i=0 ; i<dim ; i++)y[i]=a*x[i]+b ;
//}

long double likelihood(int dim,long double *x,long double *y,long double a,long double b,long double c)
{
  int j                 ;
  long double suma,l    ;
  suma=0. ;
  for(j=0 ; j<dim ; j++)  
  {
    l=suma-pow((a*exp(pow(b*x[j],c))-y[j]),2) ;
    //printf("%d %f %f %f %f %f\n",j,l,x[j],yerror[j],a,b);
    suma=l ;
  }
  return (l) ;
}
 
int priors(float min1,float max1,float v1,float min2,float max2,float v2)
{
  int p ;
  if(v1<= max1 && v1>min1 && v2<= max2 && v2>min2) p=1;
  else p=0 ;
  //printf("%d\n",p) ;
  return (p) ;
}

long double alfa(long double l_can,long double l_ant)
{
  long double c,alfa   ;
  c=l_can/l_ant  ;
  if(c<1) alfa=1 ;
  else alfa=1./c ;
  return (alfa)  ;
} 


void gauss(int n_disc,int nro_ran,long double *muestra,long double mu,long double sigma, long int semilla)
//devuelve una muestra de una distribución normal, crea la teórica y entrega la muestra.
{
  int   j,i                                        ;
  long double *x_gauss,*y_gauss,*p_acumu,a,b,rango_nodos ;
  long double *xrr,xi,suma,sumay,pi,x                    ; 
  FILE  *gauss,*nodos,*mues                        ;

  gauss=fopen("gaussiana.dat","w")     ;
  nodos=fopen("probabilidad.dat","w")  ;
  mues =fopen("muestra_gauss.dat","w") ;

  x_gauss =(long double *)malloc(n_disc*sizeof(long double))  ;
  y_gauss =(long double *)malloc(n_disc*sizeof(long double))  ;
  p_acumu =(long double *)malloc(n_disc*sizeof(long double))  ;
  xrr     =(long double *)malloc(nro_ran*sizeof(long double)) ;

  rango_nodos =8*sigma       ; //4 sigmas a c/ lado de la media
  xi          =mu-4*sigma    ; //punto inicio, cálculo de nodos
  suma        =0.            ;
  sumay       =0.            ; //sumo sobre todos los y'es
  pi          =acos(-1.)     ;

  for(i=0 ; i<n_disc ; i++)
  {
    x_gauss[i] =xi+(rango_nodos*i)/n_disc                                             ; //nodos en x
    y_gauss[i] =(1./(sigma*sqrt(2*pi)))*exp(-pow((x_gauss[i]-mu),2)/(2.*pow(sigma,2))) ; //en y 
    p_acumu[i] =(suma + y_gauss[i])                                                    ; //p acumulada
    suma       =p_acumu[i]                                                             ;
    fprintf(gauss,"%f %f\n",x_gauss[i], y_gauss[i]) ;
  }
  suma=0. ;
  for(i=0 ; i<n_disc ; i++)
  {
    sumay =suma+y_gauss[i] ;
    suma =sumay            ;
  }
  suma=0. ;
  for(i=0 ; i<n_disc ; i++)
  {
    p_acumu[i] =p_acumu[i]/sumay                                   ; //normalizo
    fprintf(nodos,"%f %f %f \n",x_gauss [i],y_gauss[i],p_acumu[i]) ;
  }

  generador(nro_ran,semilla,&xrr[0]) ;

  for(j=0 ; j<nro_ran ; j++)
  {	
    if(xrr[j] >= 0. && xrr[j] <= p_acumu[0]) //printf("bien!\n") ;
    {
      a	     =p_acumu[i]/x_gauss[i]  ; //recta de interpolación	para el primer nodo
      b	     =0.                     ;
      muestra[j] =(xrr[j]-b)/a           ;
    }		
    for(i=0 ; i<n_disc ; i++)
    { 
      a =p_acumu[i+1]-p_acumu[i]/(x_gauss[i+1]-x_gauss[i]) ; //recta probabilidad acumulada
      b	=p_acumu[i]-a*x_gauss[i]                           ; //entre nodos j y j+1
      if(xrr[j] > p_acumu[i] && xrr[j] <= p_acumu[i+1]) muestra[j] =(xrr[j]-b)/a ; 
    }
  }
  for(i=0 ; i<nro_ran ; i++) fprintf(mues,"%f \n",muestra[i]) ;
  fclose(gauss);
  fclose(nodos);
  fclose(mues) ;
}

