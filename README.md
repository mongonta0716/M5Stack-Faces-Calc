# M5Stack-Faces-Calc

 M5Stack FacesのCalculatorモジュールを利用した計算機です。
 
 Calculator for M5Stack Faces with Caluculator Module

# 使い方(Usage)
 Faces_Calc/build フォルダ配下にあるFaces_Calc.bin,jpgフォルダ,jsonフォルダをmicroSDにコピーして、[M5Stack LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher)から呼び出します。

 ## 操作

- AC: All Clear
- M: Mode Change（Calc Mode ⇔　Scroll Mode)

 ### Calc Mode
 プロンプトに数式を入力して「=」を押すと計算を実行して、表エリアに数式と答えがコピーされます。
- +/- : BackSpace
- % : Clear Prompt Line
- = ： Calc Execute

 ### Scroll Mode
 2,4,6,8キーで移動し、5を押すと赤い枠で囲われた行のデータがプロンプトに表示され編集できるようになります。

 # Requirement

 コンパイルする場合は以下のライブラリが必要です。SD-UpdaterはArduinoIDEのライブラリマネージャーからインストールします。
 Tiny Exprはtinyexpr.hとtinyexpr.cをFaces_Calc.inoと同じフォルダにコピーしてください。

 - [M5Stack-SD-Updater](https://github.com/tobozo/M5Stack-SD-Updater)
 - [TinyExpr](https://github.com/codeplea/tinyexpr)

# 細かい機能や制限事項等
履歴は最大99件保存できます。（電源を切ったり、リセットしても保持されます。）
表示桁数の関係で答えは999,999,999,999～-99,999,999,999の範囲を超えるとエラーになります。

## コンパイル
ArduinoIDEでコンパイル時に設定で警告を「なし」以外ではコンパイルできません。今後修正します、、、

# もし文字化けして起動しなくなった場合
一時ファイルをSPIFFSに持っていますが、文字化けすると起動後にハングするようになります。
その場合はM5StackCoreのBボタン（真ん中のボタン）を押しながら起動してください。


# Licence
[MIT](https://github.com/mongonta0716/M5Stack-Faces-Calc/blob/master/LICENSE)

# Author
[Takao Akaki](https://twitter.com/mongonta555)



