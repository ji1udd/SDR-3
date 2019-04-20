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
// (C) JI1UDD 2018, 2019

#define       TRANSCEIVE 0x00
unsigned char SDR3ADRS = 0x94 ;       // 初期値：0x94 (IC-7300)
#define       CIVCOMLEN  16
unsigned char CIVCONTROLLER = 0xE0 ;

void freq_setting_civ( int f ) ;
void civ_cmd_03( void ) ;
void civ_cmd_04( void ) ;
void civ_cmd_05( unsigned char *ptr, int len ) ;
void civ_cmd_06( unsigned char *ptr, int len ) ;
void civ_cmd_07( unsigned char *ptr, int len ) ;
void civ_cmd_0f( unsigned char *ptr, int len ) ;
void civ_cmd_1a( unsigned char *ptr, int len ) ;
void civ_cmd_1c( unsigned char *ptr, int len ) ;
void civ_send_freq( void ) ;
void civ_send_mode( void ) ;
void civ_reply_ok( void ) ;
void civ_reply_ng( void ) ;
void civ_send( unsigned char *str, unsigned char dest ) ;
unsigned char * freq_bcd( int d, unsigned char *str ) ;
char * byte2str( unsigned char c, char *str ) ;

int civ_mode = 0 ;                    // 初期値：CI-V機能オフ
int civ_echo = 1 ;                    // 初期値：エコーバックあり
unsigned char civ_cmd[CIVCOMLEN] ;
unsigned char *civ_ptr ;
int civ_state = 0 ;
int civ_count = 0 ;
int civ_send_inh = 0 ;

// ---------------------------------------------------------
//  Dispatch CI-V command
// ---------------------------------------------------------
void civ_command_dispatch( void ) {
  int ch ;
  unsigned char c;

//  char str[8] ;                      // for debug
//  char *strp ;                       // for debug

  while( ( ch=uart1_getch() )>=0 ) {   // 戻り値はint, 受信データがなければERRを返す
    if (civ_echo) uart1_putch( ch );   // エコーバック

    c = (unsigned char)( ch & 0xFF) ;
//  byte2str( c, str );                // for debug
//  debugprint( str ) ;                // for debug

    switch( civ_state ){
      default:
      case 0 : if ( c == 0xFE )     civ_state = 1 ; break ;
      case 1 : if ( c == 0xFE )     civ_state = 2 ; else civ_state = 0 ; break ;
      case 2 : if ( c == SDR3ADRS ) civ_state = 3 ; else civ_state = 0 ; break ;
      case 3 : CIVCONTROLLER = c ;  civ_state = 4 ; civ_ptr = civ_cmd ; civ_count = 0; break ;
      case 4 : if ( c == 0xFD ) {
                 *civ_ptr = 0 ;
                 civ_ptr = civ_cmd;
                 switch( *civ_ptr++ ) {
                   case 0x03: civ_cmd_03(); break ;                       // Reads out operating frequency
                   case 0x04: civ_cmd_04(); break ;                       // Reads out operating mode
                   case 0x05: civ_cmd_05( civ_ptr, civ_count-1 ); break ; // Write operating frequency
                   case 0x06: civ_cmd_06( civ_ptr, civ_count-1 ); break ; // Write operating mode
                   case 0x07: civ_cmd_07( civ_ptr, civ_count-1 ); break ; // Select VFO mode
                   case 0x0F: civ_cmd_0f( civ_ptr, civ_count-1 ); break ; // Select/Reads out split mode
                   case 0x1A: civ_cmd_1a( civ_ptr, civ_count-1 ); break ; // Select/Reads out filter and DATA mode
                   case 0x1C: civ_cmd_1c( civ_ptr, civ_count-1 ); break ; // Select/Reads out Tx and Rx mode
                   default: break ;
                 }
                 civ_state = 0 ;  break ;
               } else {
                 *civ_ptr++ = c ;
                 civ_count++ ;
                 if ( civ_count == CIVCOMLEN ) {
                   civ_state = 0 ; break ;
                 }
               }
    }
  }
}


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

//  if ( civ_mode ) {
  if ( civ_mode && ( civ_send_inh == 0 ) ) {
    *strp++ = 0x01 ; // Command
    *strp++ = (char)((mod==4)? 3: mod) ;
    *strp++ = 0x01 ;
    *strp   = 0xFF ;
    civ_send( str, TRANSCEIVE ) ;
  }
  civ_send_inh = 0 ;
}

// ---------------------------------------------------------
//  Reply command 03H ( Reads out operating frequency data )
// ---------------------------------------------------------
void civ_cmd_03( void ) {
  unsigned char str[10];
  unsigned char *strp = str ;

  *strp++ = 0x03 ; // Command
  strp = freq_bcd( freq, strp );
  *strp   = 0xFF ;
  civ_send( str, CIVCONTROLLER ) ;
}

// ---------------------------------------------------------
//  Reply command 04H ( Reads out operating mode data )
// ---------------------------------------------------------
void civ_cmd_04( void ) {
  unsigned char str[10];
  unsigned char *strp = str ;

  *strp++ = 0x04 ; // Command
  *strp++ = (char)((mod==4)? 7: mod) ;
  *strp++ = 0x01 ;
  *strp   = 0xFF ;
  civ_send( str, CIVCONTROLLER ) ;
}

// -----------------------------------------------------------
//  Receive command 05H ( Write operating frequency data )
// -----------------------------------------------------------
void civ_cmd_05( unsigned char *ptr, int len ) {
  int i, f ;
  unsigned char c ;

  f = 0 ;
  ptr = ptr + len - 1 ;

  for (i=0 ; i<len ; i++) {
    c = *ptr-- ;
    f = f*100 + ((c>>4)&0x0F)*10 + (c&0x0F) ;
  }
  freq_setting_civ( f ) ; 
  civ_reply_ok() ;
}

void freq_setting_civ( int f ) {
  freq = f ;                // スマート周波数処理なし
  limit( (int *)&freq, FREQ_MIN, FREQ_MAX );
  spectrum_update();
  freq_str( str_freq );     // 周波数を文字列にする
  freq_update();

  if( tx ) {
    if( tx_freq_check() ) trx_setting();
  }
  gui_update();
}

// -----------------------------------------------------------
//  Receive command 06H ( Write operating mode data )
// -----------------------------------------------------------
void civ_cmd_06( unsigned char *ptr, int len ) {
  int sel = (int) *ptr ;
  if ( sel == 4 ) sel = 5 ;      // RTTY -> ignore
  else if ( sel == 7 ) sel = 4 ; // CW-R -> CW-U

  if ( (( len == 2 )||( len == 1 )) && ( sel >= 0 ) && ( sel < 5 ) ) {
    civ_send_inh = 1 ;
    menus[N_MENU_MOD].sel = sel ; 
    mod_setting() ;
    button_settingstr();
    gui_update_button( 0 );      // update Mode button
    civ_reply_ok() ;             // send OK
  } else {
    civ_reply_ng() ;             // ignore and send NG
  }
}

// -----------------------------------------------------------
//  Receive command 07H ( Select the VFO mode )
// -----------------------------------------------------------
void civ_cmd_07( unsigned char *ptr, int len ) {
  civ_reply_ok() ;               // ignore but send OK
}

// -----------------------------------------------------------
//  Receive command 0FH ( Select/Reads out split mode )
// -----------------------------------------------------------
void civ_cmd_0f( unsigned char *ptr, int len ) {
  unsigned char str[10];
  unsigned char *strp = str ;

  if ( len == 0 ) {               // Reads out ?
    *strp++ = 0x0F ;              // Command
    *strp++ = 0x00 ;              // Split OFF state
    *strp   = 0xFF ;
    civ_send( str, CIVCONTROLLER ) ;
  } else {
    if ( *ptr == 0x00 ) {         // set split off command
      civ_reply_ok() ;            // Send OK
    } else {
      civ_reply_ng() ;            // ignore and send NG
    }
  }
}

// ---------------------------------------------------------------
//  Receive command 1AH ( Select/Reads out filter and DATA mode )
// ---------------------------------------------------------------
void civ_cmd_1a( unsigned char *ptr, int len ) {
  unsigned char str[10];
  unsigned char *strp = str ;

  switch( *ptr ){                 // Sub command ?
    case 0x03 :                   // filter band width
      if ( len == 1 ) {           // Reads out command ?
        *strp++ = 0x1A ;          // Command
        *strp++ = 0x03 ;          // Sub Command
        *strp++ = 34 ;            // LPF 3kHz
        *strp   = 0xFF ;
        civ_send( str, CIVCONTROLLER ) ;
      } else {
        civ_reply_ok() ;          // ignore and send OK
      } break ;

    case 0x06 :                   // DATA mode
      if ( len == 1 ) {           // Reads out command ?
        *strp++ = 0x1A ;          // Command
        *strp++ = 0x06 ;          // Sub Command
        *strp++ = 0x00 ;          // OFF
        *strp++ = 0x00 ;          // OFF
        *strp   = 0xFF ;
        civ_send( str, CIVCONTROLLER ) ;
      } else {
        civ_reply_ok() ;          // ignore and send OK
      } break ;

    default:                      // ohters
      break ;                     // ignore and no response
  }
}

// -----------------------------------------------------------
//  Receive command 1CH ( Select/Reads out Tx and Rx mode )
// -----------------------------------------------------------
void civ_cmd_1c( unsigned char *ptr, int len ) {
  unsigned char str[10];
  unsigned char *strp = str ;

  if ( *ptr++ == 0x00 ) {         // Tx-Rx sub command ?
    if ( len == 1 ) {             // Reads out ?
      *strp++ = 0x1C ;            // Command
      *strp++ = 0x00 ;            // Sub command
      *strp++ = (unsigned char) tx ;
      *strp   = 0xFF ;
      civ_send( str, CIVCONTROLLER ) ;

    } else {                      // set Tx-Rx command
      civ_send_inh = 1 ;
      tx = (int) ( *ptr & 0x01 ) ;
      tx_freq_check();
      trx_setting();
      gui_update_button( N_BUTTON_TRX );
      gui_update_button( N_BUTTON_SQLVL );
      gui_update_button( N_BUTTON_SQBSY );
      civ_reply_ok() ;            // send OK
    }
  }                               // ignore other sub command
}

// ---------------------------------------------------------
//  Reply OK message
// ---------------------------------------------------------
void civ_reply_ok( void ) {
  unsigned char str[] = { 0xFB, 0xFF }; // OK code
  civ_send( str, CIVCONTROLLER ) ;
}

// ---------------------------------------------------------
//  Reply NG message
// ---------------------------------------------------------
void civ_reply_ng( void ) {
  unsigned char str[] = { 0xFA, 0xFF }; // NG code;
  civ_send( str, CIVCONTROLLER ) ;
}

// ---------------------------------------------------------
//  Send CI-V message
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

// ---------------------------------------------------------
//  Convert frequency to BCD
// ---------------------------------------------------------
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

// ---------------------------------------------------------
//  Convert 1 Byte to ASCII
// ---------------------------------------------------------
char * byte2str( unsigned char c, char *str ) {
  char n ;
  n = ( c>>4 ) & 0x0F ;
  *str++ = n>9 ? n+0x37 : n+0x30 ; // upper nible
  n = c & 0x0F ;
  *str++ = n>9 ? n+0x37 : n+0x30 ; // lower nible
//*str++ = 0x20 ;                  // space
  *str = 0x0 ;
  return( str ) ;
}
