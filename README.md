# About this project

このプロジェクトはやねうら王を利用して将棋のランダム局面を生成することを目的とします。

開発環境はVisual Studio 2017です。

[開発者twitter](https://twitter.com/tibigame)

# 開発予定

- 序盤のランダム局面(未定)
- 終盤の詰む詰まないのランダム局面(実装中)
- 終盤の入玉形のランダム局面(未定)

# ビルドについて

- cudaのライブラリがリンクされる設定になっています。
- 「my_rand.h」内にて「#define CUDA」しない限り不要です。
- 現状はCUDAの生成は遅いので使っていませんが、マルチスレッドでCPUをフルに使った場合は変わってくるかも。
- 「int_board.h」内の「#define AVX512」を外せばAVX512が無くても動くはず。

# ライブラリについて

- グラフィックライブラリ、フォントレンダリングに「GLFW3」「FreeType」「sdl2」を使用しています。NuGet でインストールしてください。
- 他にインクルードファイルやdllが入っていますが、現状どうリンクされているのか把握していません。
- graphic_common.hのGLFW3の定義をコメントアウトすることでグラフィック系をオフにしてビルドできます。

# resourseについて

- ttfフォントとテクスチャを起動時に読み込みます。 (現状ファイル名決め打ち)
- 他のフォントでも動きますが、レンダリング位置がずれるかも。

# AVX512について

- 現状はSkylake-Xをターゲットとしています。
- 2レジスタの連結シフトが使えれば累積和がより効率的に求まる。
- 任意3変数のビット演算、バイト単位のpopcntも使える可能性がある。

# やねうら王からの変更点

- 新規実装コードはほぼ「user-engine」ディレクトリ内に閉じています。
- shogi.cpp: guiスレッドの起動コードを追加。
- position.h: 一部privateの項目をpublicに変更。
- position.cpp: 盤面を空で初期化する関数set_blank()の実装。
- position_sfen_fast.cpp: Position::set_fast(), Position::sfen_fast()の実装。
- Bitboard.h: cross00StepEffectBB, cross45StepEffectBBテーブルの作成。
- Bitboard.cpp: cross00StepEffectBB, cross45StepEffectBBテーブルの初期化。
