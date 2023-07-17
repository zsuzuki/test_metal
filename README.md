# Metal 描画テスト

Appleの描画フレームワークのMetalをC++で使用するテストです。

[Appleのサンプル](https://developer.apple.com/jp/metal/cpp/)
をバラして、なるべく描画部分だけを触りやすくなるように再構成しています。

また、CoreTextを使用して文字列の描画が出来るようにもしています。

main.cppとshader/を見れば、何かしら描画出来るはず…

## ビルド

jpegファイルからのテクスチャ生成にlibjpegを使用します。homebrewなどでインストールしておいてください。

## 注意点

metal-cppのソースは同梱していません。上記appleのサイトにあるサンプルから抜き出してください。
また、実行時にはres/ディレクトリにlake.jpgというjpegファイルが置いてある必要もあります。
任意のjpegファイルをリネームして置いてください。
