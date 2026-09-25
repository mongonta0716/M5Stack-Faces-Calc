# M5Stack-Faces-Calc

日本語 | [English](README_en.md)

 M5Stack FacesのCalculatorモジュールを利用した計算機です。
 
 Calculator for M5Stack Faces with Caluculator Module

# Faces Calculator3 対応
 [Faces Calculator3](https://docs.m5stack.com/en/faces/Faces_Calculator3) に対応しました。

- M5Stack Basic + Faces Calculator3（ファームウェア V03）で動作確認しています。
- キー入力はI2C（アドレス 0x08、M5Unifiedの `M5.In_I2C`）をポーリングして読み取ります。INTピンは使用しません。
- Calculator3の長押し操作に対応しています。
  - Aキー長押し：BackSpace
  - =キー長押し：Calc Execute

# 使い方(Usage)
 PlatformIOでビルドして書き込みます（後述の「コンパイル」を参照）。

 [M5Stack LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher)から起動する場合は、ビルドで生成された `.pio/build/m5stack-basic/firmware.bin` を `Faces_Calc.bin` という名前でmicroSDにコピーしてください。Aボタンを押しながら起動するとランチャー（menu.bin）に戻ります。

 ## 操作

- AC: All Clear
- M: Mode Change（Calc Mode ⇔　Scroll Mode)

 ### Calc Mode
 プロンプトに数式を入力して「=」を押すと計算を実行して、表エリアに数式と答えがコピーされます。
- \` (+/-) : BackSpace（Aキー長押しでも可）
- % : Clear Prompt Line
- = ： Calc Execute（長押しでも可）

 ### Scroll Mode
 2,4,6,8キーで移動し、5を押すと赤い枠で囲われた行のデータがプロンプトに表示され編集できるようになります。

 # Requirement

 コンパイルする場合は以下のライブラリが必要です。M5UnifiedとSD-UpdaterはArduinoIDEのライブラリマネージャーからインストールします。
 Tiny Exprはtinyexpr.hとtinyexpr.cをFaces_Calc.inoと同じフォルダにコピーしてください。（PlatformIOの場合は不要です）

 - [M5Unified](https://github.com/m5stack/M5Unified)
 - [M5Stack-SD-Updater](https://github.com/tobozo/M5Stack-SD-Updater)
 - [TinyExpr](https://github.com/codeplea/tinyexpr)

# 細かい機能や制限事項等
履歴は最大99件保存できます。（電源を切ったり、リセットしても保持されます。）
表示桁数の関係で答えは999,999,999,999～-99,999,999,999の範囲を超えるとエラーになります。

## コンパイル
### PlatformIO
リポジトリ直下の `platformio.ini` を使ってビルドできます。ライブラリは自動で取得され、TinyExprは `lib/tinyexpr` に同梱しています。

```
pio run                          # ビルド (M5Stack Basic)
pio run -t upload                # 書き込み
```

### ArduinoIDE
ArduinoIDEでコンパイル時に設定で警告を「なし」以外ではコンパイルできません。今後修正します、、、

# もし文字化けして起動しなくなった場合
一時ファイルをSPIFFSに持っていますが、文字化けすると起動後にハングするようになります。
その場合はM5StackCoreのBボタン（真ん中のボタン）を押しながら起動してください。


# Licence
[MIT](https://github.com/mongonta0716/M5Stack-Faces-Calc/blob/master/LICENSE)

# Author
[Takao Akaki](https://twitter.com/mongonta555)



