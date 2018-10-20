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
// (C) JI1UDD 2018

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

int DotTime      =  50 ;           //  50ms, dot pulse width,  DotTime(ms) = 1200 / WPM
int DashTime     = 150 ;           //  DotTime * 3
int BreakinTime  = 300 ;           // 300ms, brake-in time after finished Tx
int keytype      =   1 ;           // 0: straight, 1: iambic

int cw_key_on ;                    // CW keying control
int cw_ptt_on ;                    // CW PTT control

static int state       = STATE_RX ;
static int state_delay = 0 ;
static int dotcnt      = 0 ;
static int dashcnt     = 0 ;
static int doton       = 0 ;
static int dashon      = 0 ;

void iambic( void ) {
  if ( state==STATE_DOT_ON )
    dotcnt = 0 ;
  else if (!(( state==STATE_DOT_OFF )&&( state_delay >= ((DotTime>>2)+AdjKeyTime)) )) {
    if ( dotcnt < RejectTime ) {
      if ( !DOT_PRESSED )
        dotcnt = 0 ;
      else
        ++dotcnt ;
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
        else
          ++dashcnt ;
      }
    }
    if (( state==STATE_DOT_ON ) && ( state_delay >= ((DotTime>>2)+AdjKeyTime) )) dashcnt = 0 ;
    dashon = (dashcnt==RejectTime) ;

  } else {
    if ( DASH_PRESSED ) {          // 0: straight
      if ( !dashon ) {
        dashcnt++ ;
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
      if ( doton ) {
        state = STATE_DOT_PRE ;
        state_delay = RELAY_DLY ;
      } else if ( dashon ) {
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
        state_delay = DotTime ;
      } else {
        state_delay-- ;
      }
      break ;

    case STATE_DOT_OFF :
      if ( state_delay==0 ) {
        if ( dashon ) {
          state = STATE_DASH_ON ;
          state_delay = DashTime + AdjDashTime ;
        } else if ( doton ) {
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
          state_delay = DotTime ;
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
        if ( doton ) {
          state = STATE_DOT_ON ;
          state_delay = DotTime ;
        } else if ( dashon ) {
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
      } else if ( doton ) {
        state = STATE_DOT_ON ;
        state_delay =  DotTime ; //
      } else if ( dashon ) {
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
    stm32_gpio_af( BEEP_GPIO, BEEP_PIN, BEEP_PWM_AF );    // SideToneèoóÕON
  } else {
    stm32_gpio_mode( BEEP_GPIO, BEEP_PIN, GPIO_MODE_IN ); // SideToneèoóÕOFF
  }
}
