//
//  draw waterfall for SDR-3
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

//----------------------------------------------------------
// Waterfall用色データ
//----------------------------------------------------------
#define WF_COLOR_NUM  112 // 112階調
const uint16 wf_color_table[WF_COLOR_NUM] = {
0x0000,0x0002,0x0004,0x0006,0x0008,0x000A,0x000C,0x000E, //   0
0x0010,0x0012,0x0014,0x0016,0x0018,0x001A,0x001C,0x001E, //   8
0x003F,0x00BF,0x013F,0x01BF,0x023F,0x02BF,0x033F,0x03BF, //  16
0x043F,0x04BF,0x053F,0x05BF,0x063F,0x06BF,0x073F,0x07BF, //  24
0x07FE,0x07FC,0x07FA,0x07F8,0x07F6,0x07F4,0x07F2,0x07F0, //  32
0x07EE,0x07EC,0x07EA,0x07E8,0x07E6,0x07E4,0x07E2,0x07E0, //  40
0x0FE0,0x1FE0,0x2FE0,0x3FE0,0x4FE0,0x5FE0,0x6FE0,0x7FE0, //  48
0x8FE0,0x9FE0,0xAFE0,0xBFE0,0xCFE0,0xDFE0,0xEFE0,0xFFE0, //  56
0xFFA0,0xFF20,0xFEA0,0xFE20,0xFDA0,0xFD20,0xFCA0,0xFC20, //  64
0xFBA0,0xFB20,0xFAA0,0xFA20,0xF9A0,0xF920,0xF8A0,0xF820, //  72
0xF801,0xF803,0xF805,0xF807,0xF809,0xF80B,0xF80D,0xF80F, //  80
0xF811,0xF813,0xF815,0xF817,0xF819,0xF81B,0xF81D,0xF81F, //  88
0xF85F,0xF09F,0xF0DF,0xE91F,0xE95F,0xE19F,0xE1DF,0xDA1F, //  96
0xDA5F,0xD29F,0xD2DF,0xCB1F,0xCB5F,0xC39F,0xC3DF,0xBC1F  // 104
};

//----------------------------------------------------------
// LCDにWaterfallを描画
//----------------------------------------------------------
void lcd_draw_waterfall( int x, int y, int w, int h, unsigned char *buf, int bufoffset, int leveloffset, int step ) {
  int cnt, index ;
  unsigned char *ptr , *limit ;
  uint16 color ;

  lcd_setadrs( x, y, x+w-1, y+h-1 );
  LCD_CSX_L();
  int tmp = (LCD_DATA_PORT->ODR)&(~LCD_DATA_PORT_MASK);

  ptr = buf + ( w * bufoffset ) ; 
  limit = buf + ( w * h ) - 1 ;
  cnt = w * h ;

  while (cnt--) {
    index = leveloffset + ( *ptr++ * step );

    if ( ptr > limit ) ptr = buf ; 
    if ( index < 0 ) {
      index = 0 ; 
    }else if ( index > WF_COLOR_NUM-1 ) {
      index = WF_COLOR_NUM-1 ;
    }

    color = wf_color_table[index];

    LCD_WRX_L();
    (LCD_DATA_PORT->ODR) = tmp | ((color>>8)&LCD_DATA_PORT_MASK);
    WRX_DLY();
    LCD_WRX_H();
    WRX_DLY();

    LCD_WRX_L();
    (LCD_DATA_PORT->ODR) = tmp | ((color)&LCD_DATA_PORT_MASK);
    WRX_DLY();
    LCD_WRX_H();
  }
  LCD_CSX_H();
}
