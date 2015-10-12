CLI-Contest
====

コマンドラインからサンプル入出力で一括テストしたり提出したりするツール。

## 使い方

### init
    $ python3 cli-contest.py init http://codeforces.com/contest/1
コンテストページのURLから、サンプル入出力の取得などの初期化処理を行います。
init以外の処理を実行するには以前にカレントディレクトリでinitが実行されている必要があります。

### test
    $ python3 cli-contest.py test A ./a.out
サンプル入出力を用いてテストを行います。
入力は標準入力から与えられ、標準出力への出力がサンプル出力と比較されます。

#### テストケースの追加
テストケースは [initしたディレクトリ]/[問題ID]/\*\*.(in|out) として保存されています。
規則に従ってファイルを追加するとtestコマンド実行時に処理されるようになります。

### submit
    $ python3 cli-contest.py submit A A.cpp
指定された問題の回答としてファイルを提出します。
また、設定ファイルで前処理を行うように指定していた場合にはそれを反映したものを提出します。

## 設定ファイル
- $HOME/.cli-contest.ini
  - コンテストをまたいだ設定情報が保存されます。言語選択や前処理の有無など。
- $HOME/.cli-contest-session
  - Cookieなどのセッション情報が保存されます。
