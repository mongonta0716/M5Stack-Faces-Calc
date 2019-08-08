# M5Stack-Faces-Calc

 M5Stack FacesのCalculatorモジュールを利用した計算機です。
 Calculator for M5Stack Faces CaluculatorModule

# 使い方(Usage)
 Faces_Calc/build フォルダ配下にあるFaces_Calc.bin,jpgフォルダ,jsonフォルダをmicroSDにコピーして、[M5Stack LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher)から呼び出します。

 ## 操作
 AC: All Clear
 M: Mode Change（Calc Mode ⇔　Scroll Mode)
 +/-： Not use

 ### Calc Mode
 プロンプトに数式を入力して「=」を押すと計算を実行して、表エリアに数式と答えがコピーされます。
- . : BackSpace
- % : Clear Prompt Line

 ### Scroll Mode
 2,4,6,8キーで移動し、5を押すと赤い枠で囲われた行のデータがプロンプトに表示され編集できるようになります。

 # Requirement

 コンパイルする場合は以下のライブラリが必要です。SD-UpdaterはArduinoIDEのライブラリマネージャーからインストールします。
 Tiny Exprはtinyexpr.hとtinyexpr.cをFaces_Calc.inoと同じフォルダにコピーしてください。

 - [M5Stack-SD-Updater](https://github.com/tobozo/M5Stack-SD-Updater)
 - [TinyExpr](https://github.com/codeplea/tinyexpr)

# 制限事項等
履歴は最大99件保存できます。（電源を切ると消えます。）
表示桁数の関係で答えは999,999,999,999～-99,999,999,999の範囲を超えるとエラーになります。

# Licence
[MIT](https://github.com/mongonta0716/M5Stack-Faces-Calc/blob/master/LICENSE)

# Author
[Takao Akaki](https://twitter.com/mongonta555)



