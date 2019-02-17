//
//  Edit Memory Keyer message for SDR-3
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
// (C) JI1UDD 2019

//--------------------------------------------------------------------
// メッセージ (最大47文字)
//--------------------------------------------------------------------
static char keyer_message[4][48] = {
  "CQ CQ CQ DE JA1QRZ JA1QRZ JA1QRZ PSE K",         // メッセージ 0 初期値
  "DE JA1QRZ K",                                    // メッセージ 1 初期値
  "QSL UR 5NN BK",                                  // メッセージ 2 初期値
  "TNX FB QSO CU AGN 73 TU :"                       // メッセージ 3 初期値
};
//          1         2         3         4      4
// 12345678901234567890123456789012345678901234567

//--------------------------------------------------------------------

static int m_msg_edit_mode = 0 ;    // メッセージエディットモード
static int m_num ;                  // エディット対象メモリ番号
static int m_column ;               // エディット対象文字位置
static int m_dsppos ;               // エディット表示開始位置

void m_edit_msg_disp( void ) ;
char * m_strcopy( char *src, char *dest, int n, int m ) ;

//--------------------------------------------------------------------
// CWモードか否かをチェック
//--------------------------------------------------------------------
int chk_cw_mode( void ) {
  return (( mod==3)||( mod==4 )) ;   // CWL or CWU
}

//--------------------------------------------------------------------
// メッセージエディットモードに入る
//--------------------------------------------------------------------
void m_enter_edit_mode( int num ) {
  char *ptr ;
  m_msg_edit_mode = 1 ;
  radio_buttons[N_BUTTON_DENOISE].status = 0;     // DENOISE 不使用
  radio_buttons[N_BUTTON_STEREO].status = 0;      // STEREO 不使用
  radio_buttons[N_BUTTON_FSTEP].status = 0;       // F.Step 不使用
  radio_buttons[N_BUTTON_MEDIT].status = 7;       // MEdit 使用中＆表示
  m_column = 0 ;
  m_dsppos = 0 ;
  m_edit_msg_disp() ;
}

//--------------------------------------------------------------------
// メッセージエディットモードから抜ける
//--------------------------------------------------------------------
void m_exit_edit_mode( void ) {
  m_msg_edit_mode = 0 ;
  radio_buttons[N_BUTTON_DENOISE].status = 7;     // DENOISE 使用中＆表示
  radio_buttons[N_BUTTON_STEREO].status = 7;      // STEREO 使用中＆表示
  radio_buttons[N_BUTTON_FSTEP].status = 7;       // F.Step 使用中＆表示
  radio_buttons[N_BUTTON_MEDIT].status = 0;       // Message Edit 不使用
  gui_update_all();
  spectrum_update();
}

//--------------------------------------------------------------------
// メッセージエディタ表示    style 76543210
//   カーソル付き左揃え表示    bit |||+++++--- cursor位置 (0～25)
//   style定義を変更して実現  定義 ||+--------  1: カーソル付左揃え表示
//                                 ++--------- 00: rect
//--------------------------------------------------------------------
void m_edit_msg_disp( void ) {
  m_strcopy( keyer_message[m_num], str_edit_msg, m_dsppos, 26 );  // メッセージを表示(Max 26文字)
  radio_buttons[N_BUTTON_MEDIT].style = ( 0x20 | ( m_column> 25 ? 25 : m_column )); // cursor位置設定
  gui_update_button( N_BUTTON_MEDIT );
}

//--------------------------------------------------------------------
// ロータリーエンコーダでメッセージ文字を選択
//--------------------------------------------------------------------
void m_sel_char( int rot ) {
  char c ;

  c = ( keyer_message[m_num][m_column] + rot ) ;
  if  ( c > 0x5A ) { 
    c = c - 0x2C ;
  } else if ( c < 0x2F ) {
    c = c + 0x2C ;
  }
  keyer_message[m_num][m_column] = c ;
  m_edit_msg_disp() ;
}

//--------------------------------------------------------------------
// ロータリーエンコーダでエディット対象文字位置を移動
//--------------------------------------------------------------------
void m_move_column( int rot ) {
  if ( rot==1 ) {
    if ( m_column < 46 ) {
      m_column++ ;
      if ( keyer_message[m_num][m_column]==0 ) {
        keyer_message[m_num][m_column]=0x20 ;
        keyer_message[m_num][m_column+1]=0 ;
      }
      if ( (m_column - m_dsppos)==26 ) {
        m_dsppos++ ;
      }
    }
  } else if ( rot==-1) {
    if ( m_column > 0 ) {
      m_column-- ;
      if ( m_dsppos > 0 ) {
         m_dsppos-- ;
      }
    }
  }
  m_edit_msg_disp() ;
}

//--------------------------------------------------------------------
// エディット対象位置にスペースを挿入
//--------------------------------------------------------------------
void m_ins_char( void ) {
  int n ;
  if ( m_column < 46 ) {
    n = strlength( keyer_message[m_num] )+1 ;  
    while ( n > m_column ) {
      keyer_message[m_num][n] = keyer_message[m_num][n-1] ;
      n-- ;
    }
    keyer_message[m_num][n]=0x20;
    m_edit_msg_disp() ;
  }
}

//--------------------------------------------------------------------
// エディット対象位置の文字を削除
//--------------------------------------------------------------------
void m_del_char( void ) {
  int n = m_column ;
  if ( keyer_message[m_num][n]!=0 ) {
    while( keyer_message[m_num][n+1] ) {
      keyer_message[m_num][n]=keyer_message[m_num][n+1] ;
      n++ ;
    }
    keyer_message[m_num][n]=0 ;
    m_edit_msg_disp() ;
  }
}

//--------------------------------------------------------------------
// 文字列コピー n文字目(0～) から 最大m文字分(1～)
//--------------------------------------------------------------------
char * m_strcopy( char *src, char *dest, int n, int m ) {
  src += n ;
  while( *src && m-- ) {
    *dest++ = *src++;
  }
  *dest = 0;
  return dest;
}
