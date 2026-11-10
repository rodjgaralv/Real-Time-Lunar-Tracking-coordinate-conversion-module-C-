#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define DEBUG

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

// Segun Stellarium:
// N 42° 35' 59.99"
// W 5° 34' 13.08"
#define LATITUD_LEON 42.59999722
#define LONGITUD_LEON -5.5703

// Constantes para colorear el texto de printf
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/* puts a large angle in the correct range 0 - 360 degrees */
double range_degrees(double d)
{
  d = fmod( d, 360.);
    if(d<0.) d += 360.;
  return d;

}

/* puts a large angle in the correct range 0 - 2PI radians */
double range_radians (double r)
{
  r = fmod( r, 2.*M_PI );
    if (r<0.) r += 2.*M_PI;
  return r;

}


// Funciones auxiliares de conversion grados, radianes y horas
double degrees2Rad(double degrees) {

  double rads = degrees * M_PI / 180.0;

  rads = fmod (rads, 2 * M_PI);

  if (rads < 0)
    rads+= 2 * M_PI;

  return rads;
  }

  double rad2Degrees(double rad) {

  double degrees = rad / M_PI * 180.0;

  degrees = fmod (degrees, 360.0);

  if (degrees < 0)
    degrees += 360.0;

  return degrees;

}

double degrees2Hours (double degrees)  {


  double hours = degrees / 15.0;

  hours = fmod(hours, 24.0);

  if (hours < 0)
    hours += 24.0;

  return hours;

}

double rads2Hours (double rads) {

  double hours = rads * 12 / M_PI;

  hours = fmod(hours, 24.0);

  if (hours < 0)
    hours += 24.0;

  return hours;

}

// Imprime valor de un angulo en radianes en diferentes sistemas
void printAngleValue (char* angleName, double rads) {

  #ifdef DEBUG

  // El espaciador es para que los numeros negativos queden alineados
  /*char spacer = '\0';

  if (rads >= 0)
  spacer = ' ';
  */
  // Colores para los diferentes tipos de valores
  char* colores [5];

  colores [0] = ANSI_COLOR_RESET;
  colores [1] = ANSI_COLOR_RESET;
  colores [2] = ANSI_COLOR_RESET;
  colores [3] = ANSI_COLOR_RESET;
  colores [4] = ANSI_COLOR_RESET;

  if (strcmp(angleName,"ALT") == 0 ||
     strcmp(angleName,"AZ") == 0)
     colores [2] = ANSI_COLOR_CYAN;

  if (strcmp(angleName,"RA") == 0)
    colores [4] = ANSI_COLOR_GREEN;

  if (strcmp(angleName,"DEC") == 0)
    colores [2] = ANSI_COLOR_GREEN;

  // º:':''
  double totalDegrees = rad2Degrees (rads);
  int degrees = trunc (totalDegrees);
  int degreeMinutes = fabs(trunc((totalDegrees - degrees)*60));
  double degreeSeconds = (fabs(totalDegrees - degrees)*60 - degreeMinutes)*60;

  // h:m:s
  double totalHours = rads2Hours (rads);
  int h = trunc(totalHours);
  int m = trunc((totalHours - h)*60);
  double s = ((totalHours - h)*60 - m) * 60;

          printf(ANSI_COLOR_YELLOW "%6s:" ANSI_COLOR_RESET
      "%s\t%8.4f rad"
      "%s\t%8.4fº"
        "%s\t%4dº%2d'%5.2f''"
        "%s\t%8.2fh"
        "%s\t%2d:%2d:%5.2f\n"
        ANSI_COLOR_RESET,
      angleName,
      colores[0], rads,
      colores[1], totalDegrees,
      colores[2], degrees, degreeMinutes, degreeSeconds,
      colores[3], totalHours,
      colores[4], h, m, s);
  #endif

}

double coordLST(double* lst){
  double d;
  double LST;
  double UT;

  time_t now = time(&now);

  if (now == -1) {
    puts("The time() function failed");
  }

  struct tm *ptm = gmtime(&now);

  if (ptm == NULL) {
    puts("The gmtime() function failed");
  }

  d = ptm->tm_yday + (double)ptm->tm_hour/24.0 + (double)ptm->tm_min/1440.0 + (double)ptm->tm_sec/86400.0 + 7669.5;

  UT = 15*(ptm->tm_hour + ptm->tm_min/60.0 + ptm->tm_sec/3600.0);

  LST = 100.46 + 0.985647 * d + LONGITUD_LEON + UT;

  LST = degrees2Rad(LST);
  range_radians(LST);
  //printAngleValue ("LST", LST);

  // Devolvemos LST en radianes
  *lst = LST;

}

// Transforma de coordenadas ALT AZ a RA DEC.
// Los valores de ALT y AZ deben estar en grados, en el intervalo 0 - 360
// Los valoes de RA DEC se devuelven en radianes
void altAz2RaDec (double alt, double az, double* ra, double* dec) {

  double HA = 0;
  double LST = 0;
  double LAT = 0;

  // Variables de salida
  double RA = 0;
  double DEC = 0;

  // Definimos LAT para poder poner en radianes LATITUD_LEON
  LAT = degrees2Rad(LATITUD_LEON);

  // Pasamos a radianes
  double ALT = degrees2Rad(alt);
  range_radians(ALT);
  printAngleValue ("ALT", ALT);

  double AZ  = degrees2Rad(az);
  range_radians(AZ);
  printAngleValue ("AZ", AZ);

  // Calculamos LST
  double lst = 0;
  coordLST(&lst);
  LST = lst;
  range_radians(LST);

  // Obtenemos DEC

  DEC = (asin(sin(ALT)*sin(LAT)+cos(ALT)*cos(LAT)*cos(AZ)));
  range_radians(DEC);

  // Obtenemos HA
  HA = acos(((sin(ALT)-(sin(DEC)*sin(LAT)))/(cos(DEC)*cos(LAT))));
  range_radians(HA);
  //printAngleValue ("HA", HA);


  // Obtenemos RA
  RA = LST - HA;
  range_radians(RA);

  // No tocar de aqui en adelante
  printAngleValue("RA", RA);
  printAngleValue("DEC", DEC);

  // Devolvemos los valores
  *ra  = RA;
  *dec = DEC;

}

// Transforma de coordenadas RA (recta ascension) DEC a ALT AZ (.
// Los valores de RA y DEC deben estar en grados, en el intervalo 0 - 360
// Los valoes de ALT AZ se devuelven en radianes
void raDec2AltAz (double ra, double dec, double* alt, double* az) {

  double LAT = 0;
  double HA = 0;

  // Variables de salida
  double ALT = 0;
  double AZ = 0;

  // Pasamos a grados entre 0 y 360
  ra = range_degrees(ra);
  dec = range_degrees(dec);

  // Obtencion de LST, en radianes
  double lst = 0;
  coordLST(&lst);
  double LST = lst;

  // Pasamos a radianes
  double RA = degrees2Rad(ra);
  range_radians(RA);
  printAngleValue ("RA", RA);

  double DEC  = degrees2Rad(dec);
  range_radians(DEC);
  printAngleValue ("DEC", DEC);

  // Calculamos HA
  HA = LST - RA;

  if(HA < 0){
     HA += 2*M_PI;
  }

  range_radians(HA);
  //printAngleValue ("HA", HA);

  // Definimos LAT para poder poner en radianes LATITUD_LEON
  LAT = degrees2Rad(LATITUD_LEON);

  // Obtencion ALT
  ALT = asin(sin(DEC)*sin(LAT)+cos(DEC)*cos(LAT)*cos(HA));
  range_radians(ALT);

  // Obtencion AZ
  AZ = acos((sin(DEC) - sin(ALT)*sin(LAT))/(cos(ALT)*cos(LAT)));
  if(sin(HA) > 0){
    AZ = 2*M_PI - AZ;
  }
  range_radians(AZ);

  // No tocar de aqui en adelante
  printAngleValue("ALT", ALT);
  printAngleValue("AZ", AZ);

  // Devolvemos los valores
  *alt = ALT;
  *az = AZ;

}

// main. No tocar (salvo para cambiar los valores de prueba)
void main() {

  // Variables para los angulos a convertir (tienen que estar en grados)
  double azPrueba  = 253.24;
  double altPrueba = 24.59;

  double raPrueba  = 114.818;
  double decPrueba = 5.2177;

  // De aqui en adelante, no tocar.

  // Variables destino donde almacenar los angulos convertidos
  double alt;
  double az;

  double ra;
  double dec;

  // Pruebas
  printf ("\nProbando conversor ALT AZ -> RA DEC:\n\n");
  altAz2RaDec (altPrueba, azPrueba, &ra, &dec);

  printf ("\nProbando conversor RA DEC -> ALT AZ:\n\n");
  raDec2AltAz (raPrueba, decPrueba, &alt, &az);
  printf("\n");

}
