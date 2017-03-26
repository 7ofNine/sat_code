#include <stdio.h>
#include <string.h>
#include <math.h>
#include "norad.h"
#include "norad_in.h"

/* Example code to add BSTAR data using Ted Molczan's method.  It just
   reads in TLEs,  computes BSTAR if possible,  then writes out the
   resulting modified TLE.

   Add the '-v' (verbose) switch,  and it also writes out the orbital
   period and perigee/apogee distances.  Eventually,  I'll probably
   set it up to dump other data that are not immediately obvious
   just by looking at the TLEs... */

int main( int argc, const char **argv)
{
   FILE *ifile;
   const char *filename;
   char line1[100], line2[100];
   int verbose = 0;

   if( !strcmp( argv[argc - 1], "-v"))
      {
      verbose = 1;
      argc--;
      }
   filename = (argc == 1 ? "all_tle.txt" : argv[1]);
   ifile = fopen( filename, "rb");
   if( !ifile)
      {
      fprintf( stderr, "Couldn't open '%s': ", filename);
      perror( "");
      return( -1);
      }
   while( fgets( line1, sizeof( line1), ifile))
      if( *line1 == '1' && fgets( line2, sizeof( line2), ifile)
                  && *line2 == '2')
         {
         tle_t tle;

         if( parse_elements( line1, line2, &tle) >= 0)
            {
            char obuff[200];
            double params[N_SGP4_PARAMS], c2;

            SGP4_init( params, &tle);
            c2 = params[0];
            if( c2 && tle.xno)
               tle.bstar = tle.xndt2o / (tle.xno * c2 * 1.5);
            write_elements_in_tle_format( obuff, &tle);
            printf( "%s", obuff);
            if( verbose)
               {
               const double a1 = pow(xke / tle.xno, two_thirds);  /* in Earth radii */

               printf( "   Perigee: %.4f km\n",
                    (a1 * (1. - tle.eo) - 1.) * earth_radius_in_km);
               printf( "   Apogee: %.4f km\n",
                    (a1 * (1. + tle.eo) - 1.) * earth_radius_in_km);
               printf( "   Orbital period: %.4f min\n",
                    2. * pi / tle.xno);
               }
            }
         }
      else
         printf( "%s", line1);
   fclose( ifile);
   return( 0);
}
