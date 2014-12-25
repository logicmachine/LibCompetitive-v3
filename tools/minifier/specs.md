minifier-specification
====

## 定義 (Decl) について

### グローバル変数 (VarDecl)
- 残して初期化に用いられる式をコールグラフに追加する

### グローバル関数 (FunctionDecl)
- コールグラフ上で到達可能な場合のみ残す

### 構造体定義 (RecordDecl)
- コールグラフ上で到達可能な場合のみ残す
  - 子となるメンバ変数をすべてコールグラフに追加する

### 構造体・クラス定義 (CXXRecordDecl)
- コールグラフ上で到達可能な場合のみ残す
  - bases, vbases: 基底クラスをすべてコールグラフに追加
  - isLambda: ラムダ式もCXXRecordDeclになる?
    - 要調査
  - hasUserDeclaredDestructor: デストラクタをコールグラフに追加する

### メンバ変数 (FieldDecl)
- 親となる構造体・クラス定義が残っている場合は必ず残す
  - hasInClassInitialization: 初期化式をコールグラフに追加
  - isBitField: ビット幅指定に用いた式をコールグラフに追加

### メンバ関数 (CXXMethodDecl)
- コールグラフ上で到達可能な場合のみ残す

### コンストラクタ (CXXConstructorDecl)
- コールグラフ上で到達可能な場合のみ残す

### デストラクタ (CXXDestructorDecl)
- 存在する場合は必ず残す
  - コールグラフ上に呼び出しが残らない場合があるので注意

### 名前空間 (NamespaceDecl)
- 子に残っている要素が存在する場合のみ残す


## 型 (Type) について

### 配列 (ArrayType)
- getElementType: 要素の型を追加する

### 可変サイズ配列 (VariableLengthArray)
- getSizeExpr: サイズ計算式をコールグラフに追加

### 属性つきの型 (AttributedType)
- __attribute__((aligned(16))) とか
- getModifiedType: 修飾された型を追加する

### auto (AutoType)
- getDeducedType: 解決された先の型を追加する

### decltype (DecltypeType)
- getUnderlyingExpr: 評価された式をコールグラフに追加
- getUnderlyingType: 解決された先の型を追加する

### メンバへのポインタ (MemberPointerType)
- getClass: クラスの型を追加
- getPointeeType: 指している要素の型を追加

### ポインタ (PointerType)
- getPointeeType: ポインタの指している要素の型を追加

### 参照 (ReferenceType)
- getPointeeType: 参照の指している要素の型を追加

### 列挙型 (EnumType)

### 構造体・クラス (RecordType)

### typedef (TypedefType)
- getDecl: 別名を付けられた型を追加

### TemplateSpecializationType

