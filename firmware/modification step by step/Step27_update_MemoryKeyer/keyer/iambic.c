//
//  Iambic Keyer for SDR-3
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

#include "board/board.h"
#include "iambic.h"

#define RejectTime      5           //   5ms, chattering rejection time
#define AdjKeyTime      2           //   2ms, adjust key capture timing
#define RELAY_DLY      20           //  15ms, change over time for T/R relay when dot/dash keys are pressed
#define AdjDashTime     0           //   0ms, dash weight time
#define STATE_RX        0
#define STATE_DOT_PRE   1
#define STATE_DOT_ON    2
#define STATE_DOT_OFF   3
#define STATE_DASH_PRE  4
#define STATE_DASH_ON   5
#define STATE_DASH_OFF  6
#define STATE_TR_DLY    7

#define M_DOT           1
#define M_DASH          2
#define M_SPACE         3
#define M_WSPACE        4
#define M_END           0

int DotTime      =  50 ;           //  50ms, dot pulse width,  DotTime(ms) = 1200 / WPM
int DashTime     = 150 ;           //  DotTime * 3
int BreakinTime  = 300 ;           // 300ms, brake-in time after finished Tx
int keytype      =   1 ;           // 0: straight, 1: iambic

int cw_key_on ;                    // CW keying control
int cw_ptt_on ;                    // CW PTT control

char playbuf[320];                 // Morse code play buffer
char *playptr ;                    // play buffer pointer
int playmem = 0 ;                  // send message

static int state       = STATE_RX ;
static int state_delay = 0 ;
static int dotcnt      = 0 ;
static int dashcnt     = 0 ;
static int doton       = 0 ;
static int dashon      = 0 ;

static int m_doton( void ) ;
static int m_dashon( void ) ;
static void m_separator( void ) ;
static void morse_conversion( char *m, char *sentence ) ;
static int scan_morse( char c );


void iambic( void ) {
  if ( state==STATE_DOT_ON )
    dotcnt = 0 ;
  else if (!(( state==STATE_DOT_OFF )&&( state_delay >= ((DotTime>>2)+AdjKeyTime)) )) {
    if ( dotcnt < RejectTime ) {
      if ( !DOT_PRESSED )
        dotcnt = 0 ;
      else {
        ++dotcnt ;
        playmem = 0 ;              // メモリキーヤー送出停止
      }
    }
  }
  if (( state==STATE_DASH_ON ) && ( state_delay >= DotTime )) dotcnt = 0 ;
  doton  = (dotcnt==RejectTime) ;

  if ( keytype ) {
    if ( state==STATE_DASH_ON )    // 1: iambic
      dashcnt = 0 ;
    else if (!(( state==STATE_DASH_OFF )&&( state_delay >= ((DotTime>>2)+AdjKeyTime)) )) {
      if ( dashcnt < RejectTime ) {
        if ( !DASH_PRESSED )
          dashcnt = 0 ;
        else {
          ++dashcnt ;
          playmem = 0 ;            // メモリキーヤー送出停止
        }
      }
    }
    if (( state==STATE_DOT_ON ) && ( state_delay >= ((DotTime>>2)+AdjKeyTime) )) dashcnt = 0 ;
    dashon = (dashcnt==RejectTime) ;

  } else {
    if ( DASH_PRESSED ) {          // 0: straight
      if ( !dashon ) {
        dashcnt++ ;
        playmem = 0 ;              // メモリキーヤー送出停止
        if ( dashcnt > RejectTime) dashon = 1 ;
      } else {
        dashcnt =  RejectTime ;
      }
    } else if ( dashon ) {
      dashcnt-- ;
      if ( dashcnt == 0 ) dashon = 0 ;
    } else {
      dashcnt = 0 ;
    }
  }

  switch ( state ) {
    case STATE_RX :
      if ( doton || m_doton() ) {
        state = STATE_DOT_PRE ;
        state_delay = RELAY_DLY ;
      } else if ( dashon || m_dashon() ) {
        state = STATE_DASH_PRE ;
        state_delay = RELAY_DLY ;
      }
      break ;

    case STATE_DOT_PRE :
      if ( state_delay==0 ) {
        state = STATE_DOT_ON ;
        state_delay = DotTime ;
      } else {
        state_delay-- ;
      }
      break ;

    case STATE_DOT_ON :
      if ( state_delay==0 ) {
        state = STATE_DOT_OFF ;
        m_separator() ;
      } else {
        state_delay-- ;
      }
      break ;

    case STATE_DOT_OFF :
      if ( state_delay==0 ) {
        if ( dashon || m_dashon() ) {
          state = STATE_DASH_ON ;
          state_delay = DashTime + AdjDashTime ;
        } else if ( doton || m_doton() ) {
          state = STATE_DOT_ON ;
          state_delay = DotTime ;
        } else {
          state = STATE_TR_DLY ; 
          state_delay = BreakinTime ;
        }
      } else {
        state_delay-- ;
      }
      break ;

    case STATE_DASH_PRE :
      if ( state_delay==0 ) {
        state = STATE_DASH_ON ;
        state_delay = DashTime + AdjDashTime ;
      } else {
        state_delay--;
      }
      break ;

    case STATE_DASH_ON :
      if ( keytype ) {
        if ( state_delay==0 ) {     // 1: iambic
          state = STATE_DASH_OFF ;
          m_separator() ;
        } else {
          state_delay-- ;
        }
      } else {
        if ( !dashon ) {            // 0: straight
          state = STATE_TR_DLY ; 
          state_delay = BreakinTime ;
        }
      }
      break ;

    case STATE_DASH_OFF :
      if ( state_delay==0 ) {
        if ( doton || m_doton() ) {
          state = STATE_DOT_ON ;
          state_delay = DotTime ;
        } else if ( dashon || m_dashon() ) {
          state = STATE_DASH_ON ;
          state_delay = DashTime + AdjDashTime ;
        } else {
          state = STATE_TR_DLY ;
          state_delay = BreakinTime ;
        }
      } else {
        state_delay-- ;
      }
      break ;

    case STATE_TR_DLY :
      if ( state_delay==0 ) {
        state = STATE_RX ;
      } else if ( doton || m_doton() ) {
        state = STATE_DOT_ON ;
        state_delay =  DotTime ; //
      } else if ( dashon || m_dashon() ) {
        state = STATE_DASH_ON ;
        state_delay = DashTime + AdjDashTime ; //
      } else {
        state_delay-- ;
      }
      break ;
  }

  cw_ptt_on = !(state==STATE_RX) ;
  cw_key_on = ((state==STATE_DOT_ON) || (state==STATE_DASH_ON)) ;

  if (( cw_key_on==1 )&&( tx==1 )) {
    stm32_gpio_af( BEEP_GPIO, BEEP_PIN, BEEP_PWM_AF );    // SideTone出力ON
  } else {
    stm32_gpio_mode( BEEP_GPIO, BEEP_PIN, GPIO_MODE_IN ); // SideTone出力OFF
  }
}



//--------------------------------------------------------------------
// メモリキーヤー起動
//--------------------------------------------------------------------
void memory_keyer_start( char *msg_ptr ) {
  morse_conversion(playbuf, msg_ptr) ;
  playptr = playbuf ;
  playmem = 1 ;
}

//--------------------------------------------------------------------
// メモリキーヤー停止
//--------------------------------------------------------------------
void memory_keyer_stop( void ) {
  playmem = 0 ;
}

//--------------------------------------------------------------------
// メモリキーヤー動作中チェック
//--------------------------------------------------------------------
int memory_keyer_status( void ) {
  return ( playmem ) ;
}

//--------------------------------------------------------------------
// メッセージのモールス符号判定
//--------------------------------------------------------------------
static int m_doton( void ) {
  if (( playmem == 1 )&&( *playptr == M_DOT )) {
    playptr++ ;
    return 1 ;
  } else {
    return 0 ;
  }
}

static int m_dashon( void ) {
  if (( playmem == 1 )&&( *playptr == M_DASH )) {
    playptr++ ;
    return 1 ;
  } else {
    return 0 ;
  }
}

static void m_separator( void ) {
  char s ;
  state_delay = DotTime ;
  if ( playmem == 1 ) {
    s = *playptr ;
    if ( s == M_SPACE ) {
      state_delay = DotTime * 3  ;
      playptr++ ;
    } else if ( s == M_WSPACE ) {
      state_delay = DotTime * 7  ;
      playptr++ ;
    } else if ( s == M_END ) {
      playmem = 0 ;
    }
  }
}


//--------------------------------------------------------------------
// 以下のソースコードは
// 月間FBニュース2014年8月号 JH1NRR氏の記事から流用＆変更
//--------------------------------------------------------------------
struct morse_s {
  const char c;
  const char code[8];
};

struct morse_s morse[] = {
  { 'A', { M_DOT,  M_DASH, M_END } },
  { 'B', { M_DASH, M_DOT,  M_DOT,  M_DOT,  M_END } },
  { 'C', { M_DASH, M_DOT,  M_DASH, M_DOT,  M_END } },
  { 'D', { M_DASH, M_DOT,  M_DOT,  M_END } },
  { 'E', { M_DOT,  M_END } },
  { 'F', { M_DOT,  M_DOT,  M_DASH, M_DOT,  M_END } },
  { 'G', { M_DASH, M_DASH, M_DOT,  M_END } },
  { 'H', { M_DOT,  M_DOT,  M_DOT,  M_DOT,  M_END } },
  { 'I', { M_DOT,  M_DOT,  M_END } },
  { 'J', { M_DOT,  M_DASH, M_DASH, M_DASH, M_END } },
  { 'K', { M_DASH, M_DOT,  M_DASH, M_END } },
  { 'L', { M_DOT,  M_DASH, M_DOT,  M_DOT,  M_END } },
  { 'M', { M_DASH, M_DASH, M_END } },
  { 'N', { M_DASH, M_DOT,  M_END } },
  { 'O', { M_DASH, M_DASH, M_DASH, M_END } },
  { 'P', { M_DOT,  M_DASH, M_DASH, M_DOT,  M_END } },
  { 'Q', { M_DASH, M_DASH, M_DOT,  M_DASH, M_END } },
  { 'R', { M_DOT,  M_DASH, M_DOT,  M_END } },
  { 'S', { M_DOT,  M_DOT,  M_DOT,  M_END } },
  { 'T', { M_DASH, M_END } },
  { 'U', { M_DOT,  M_DOT,  M_DASH, M_END } },
  { 'V', { M_DOT,  M_DOT,  M_DOT,  M_DASH, M_END } },
  { 'W', { M_DOT,  M_DASH, M_DASH, M_END } },
  { 'X', { M_DASH, M_DOT,  M_DOT,  M_DASH, M_END } },
  { 'Y', { M_DASH, M_DOT,  M_DASH, M_DASH, M_END } },
  { 'Z', { M_DASH, M_DASH, M_DOT,  M_DOT,  M_END } },
  { '0', { M_DASH, M_DASH, M_DASH, M_DASH, M_DASH, M_END } },
  { '1', { M_DOT,  M_DASH, M_DASH, M_DASH, M_DASH, M_END } },
  { '2', { M_DOT,  M_DOT,  M_DASH, M_DASH, M_DASH, M_END } },
  { '3', { M_DOT,  M_DOT,  M_DOT,  M_DASH, M_DASH, M_END } },
  { '4', { M_DOT,  M_DOT,  M_DOT,  M_DOT,  M_DASH, M_END } },
  { '5', { M_DOT,  M_DOT,  M_DOT,  M_DOT,  M_DOT,  M_END } },
  { '6', { M_DASH, M_DOT,  M_DOT,  M_DOT,  M_DOT,  M_END } },
  { '7', { M_DASH, M_DASH, M_DOT,  M_DOT,  M_DOT,  M_END } },
  { '8', { M_DASH, M_DASH, M_DASH, M_DOT,  M_DOT,  M_END } },
  { '9', { M_DASH, M_DASH, M_DASH, M_DASH, M_DOT,  M_END } },
  { ':', { M_DOT,  M_DOT,  M_DOT,  M_DASH, M_DOT,  M_DASH, M_END } }, // ~SK
  { ';', { M_DOT,  M_DASH, M_DOT,  M_DASH, M_DOT,  M_END } },         // ~AR
  { '<', { M_DASH, M_DOT,  M_DASH, M_DASH, M_DOT,  M_END } },         // ~KN
  { '=', { M_DASH, M_DOT,  M_DOT,  M_DOT,  M_DASH, M_END } },         // ~BT
  { '>', { M_DOT,  M_DASH, M_DOT,  M_DOT,  M_DOT,  M_END } },         // ~AS
  { '?', { M_DOT,  M_DOT,  M_DASH, M_DASH, M_DOT,  M_DOT,  M_END } }, // ~IMI
  { '/', { M_DASH, M_DOT,  M_DOT,  M_DASH, M_DOT,  M_END } },         // ~DN
  { '\0', { M_END } },
};

//--------------------------------------------------------------------
// メッセージ文字列をモールス符号列に変換
//--------------------------------------------------------------------
static void morse_conversion( char *m, char *message ) {
  char c ;
  int  i, j ;

  c = *message++ ;
  while ( c ) {              // 1文字読み出してデリミタ(=0)でなければモールス符号に変換する
    i = scan_morse( c ) ;
    c = *message++ ;
    if (i >= 0) {
      for (j = 0; morse[i].code[j]; j++) {    // M_ENDが見つかるまで、モールス符号をコピー
        *m++ = morse[i].code[j] ;
      }
      if ( c == ' ' ) {      // 次が空白なら単語間スペース
        *m++ = M_WSPACE ;
      } else {
        *m++ = M_SPACE ;     // それ以外なら文字間スペース
      }
    }
  }
  *m = M_END ;
}

static int scan_morse( char c ) {
  int i ;
  for (i = 0; morse[i].c; i++) {
    if ( morse[i].c == c ) {
      return ( i ) ;
    }
  }
  return ( -1 ) ; // 符号が存在しない場合
}
