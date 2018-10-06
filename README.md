# SDR-3 firmware
Modification step by step.

##Step1:
- おじさん工房のHPに掲載されているパッチの適用

##Step2:
- CWモード(CW Lower と CW Upper)を追加
- LCD上に表示されている周波数が送信周波数となる
- PTTを押すと送信となる
- CW波のON/OFF時に窓関数は使用していない(CW波は急峻に立ち上がり/立下りする)。

##Step3:
- CWキーイング機能を追加
- CW key に PA5 を 使用。R2 に 3.3kΩを実装。
- PTT で送信状態にして、PA5をGNDに落とすとCW波の送信となる。
- CW波のON/OFFは窓関数(raised cosine)を使用(CW波は緩やかに立ち上がり/立下りする)。

##Step4:
- iambicキーヤー機能を追加
- Dot key に PA5 を、 Dash key に PA6 を使用。 R3 に 3.3kΩを実装する
- PA5/PA6をGNDに落とすとDot/DashのCW波が送信される。
- USB端子(PA11)にSidetone信号が出力される(500Hz 矩形波)ので圧電スピーカーをつないでモニタできる。PA11と圧電スピーカとの間には直列に抵抗(1kΩなど)を入れる。
- Keyerのスピードとブレークイン時間についてメニューは未実装。変更したい場合は、 ソースコードの define の値を修正する必要がある。
- keyerフォルダは、SSB_trxフォルダに入れる
