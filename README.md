# SDR-3 firmware
Modification step by step.

### Step1:
- おじさん工房のHPに掲載されているパッチの適用

### Step2:
- CWモード(CW Lower と CW Upper)を追加
- LCD上に表示されている周波数が送信周波数となる
- PTTを押すと送信となる
- CW波のON/OFF時に窓関数は使用していない(CW波は急峻に立ち上がり/立下りする)。

### Step3:
- CWキーイング機能を追加
- CW key に PA5 を 使用。R2 に 3.3kΩを実装。
- PTT で送信状態にして、PA5をGNDに落とすとCW波の送信となる。
- CW波のON/OFFは窓関数(raised cosine)を使用(CW波は緩やかに立ち上がり/立下りする)。

### Step4:
- iambicキーヤー機能を追加
- Dot key に PA5 を、 Dash key に PA6 を使用。 R3 に 3.3kΩを実装する
- PA5/PA6をGNDに落とすとDot/DashのCW波が送信される (PTTの操作は不要)。
- USB端子(PA11)にSidetone信号が出力される(500Hz 矩形波)ので圧電スピーカーをつないでモニタできる。PA11と圧電スピーカとの間には直列に抵抗(1kΩなど)を入れる。
- Keyerのスピードとブレークイン時間についてメニューは未実装。変更したい場合は、 ソースコードの define の値を修正する必要がある。
- keyerフォルダは、SSB_trxフォルダに入れる

### Step5:
- iambicキーヤーのSidetone周波数を600Hzに変更(受信時のトーン周波数と一致させた)
- SidetoneをオーディオコーデックIC経由のヘッドフォン出力に変更
- PA11の出力をRCのLPFを通してオーディオコーデックのIN1L/Rに入力する。
[_RC LPF Schematic_](hardware/SDR-3_SideTone.pdf)
[_Photo_](hardware/SDR-3_sidetone.JPG)
- Sidetoneの音量についてメニューは未実装。変更したい場合は、 tlv320aic3204.c の修正内容を変更する必要がある。
- Step5フォルダ中にあるkeyerフォルダを使う (keyerフォルダを差し替える必要あり。内容更新あり)

### Step6:
- settingメニューに、下記のiambicキーヤーの設定項目を追加
- Speed：10～40 WPM
- Break-in 時間： 50～500 ms (実際は、数十msの誤差あり)
- Sidetone 音量： 0 ～ 117
- Step6フォルダ中にあるkeyerフォルダを使う (keyerフォルダを差し替える必要あり。内容更新あり)
- radio.c における CWモード追加時のバグ修正

### Step7:
- キーヤーにバグキー/ストレートキーの機能を追加
- settingメニューで iambic として使用するか バグキー/ストレートキとして使用するかを選択できる
- ストレートキーを使用する場合は、ストレートキーを Dash key (PA6) 側に接続する
- Step7フォルダ中にあるkeyerフォルダを使う (keyerフォルダを差し替える必要あり。内容更新あり)

