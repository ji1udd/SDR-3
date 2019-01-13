//
//  Rig control(CI-V) for SDR-3
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// (C) JI1UDD 2018

#define TRANSCEIVE 0x00
#define CONTROLLER 0xE0
#define SDR3ADRS   0x10  // reuse 0x10 that is for IC-275 originally

void civ_send( unsigned char *str, unsigned char dest );
unsigned char * freq_bcd( int d, unsigned char *str );

int civ_mode = 0 ;


// ---------------------------------------------------------
//  Send command 00H ( Transfers operating frequency data )
// ---------------------------------------------------------
void civ_send_freq( void ) {
  unsigned char str[10];
  unsigned char *strp = str ;

  if ( civ_mode ) {
    *strp++ = 0x00 ; // Command
    strp = freq_bcd( freq, strp );
    *strp   = 0xFF ;
    civ_send( str, TRANSCEIVE ) ;
  }
}

// ---------------------------------------------------------
//  Send command 01H ( Transfer operating mode data )
// ---------------------------------------------------------
void civ_send_mode( void ) {
  unsigned char str[10];
  unsigned char *strp = str ;

  if ( civ_mode ) {
    *strp++ = 0x01 ; // Command
    *strp++ = (char)((mod==4)? 3: mod) ;
    *strp++ = 0x01 ;
    *strp   = 0xFF ;
    civ_send( str, TRANSCEIVE ) ;
  }
}


// ---------------------------------------------------------

void civ_send( unsigned char *str, unsigned char dest ) {
  uart1_putch( 0xFE );     // Preamble
  uart1_putch( 0xFE );     // Preamble
  uart1_putch( dest );     // Controller address
  uart1_putch( SDR3ADRS ); // SDR-3R address
  while( *str != 0xFF ) {
    uart1_putch( *str );
    str++;
  }
  uart1_putch( 0xFD );     // End of message
}


unsigned char * freq_bcd( int d, unsigned char *str ) {
  int  i ;
  char j ;

  for (i=0 ; i<5 ; i++) {
    j = ( d % 10 ) ;
    d = d/10;
    *str++ = ( ( d % 10 ) << 4 ) | j ;
    d = d/10;
  }
  return str;
}
