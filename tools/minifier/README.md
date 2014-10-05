Minifier
====

ライブラリ部分をいい感じに展開するツール。

```#include "libcomp/***.hpp"``` で読み込んだライブラリコードから必要な部分のみを抽出します。

## 使い方
### ビルド
- LLVM/Clangに依存しています。
  - Debian Wheezy + llvm.orgのaptリポジトリからインストールできるLLVM/Clang 3.5で開発しています。
- 必要なパッケージがそろっていればmakeするだけでbin/minifierが生成されるはずです。

### オプションなど
    $ minifier -include=<ライブラリのルートへのパス> <入力ソースファイル>
- ```-include```オプションは複数指定できます。
