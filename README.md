# AVR向けOV7670ドライバー
カメラデバイスOV7670のドライバー。AVR（ATmega旧シリーズ）用です。

## 概要
カメラデバイスOV7670のドライバーです。コードはArduino用ではなくAVR-GCCで記述しています。  
解像度はQVGA/QQVGAのみ対応です。カラーモードはRGB565のみ対応です。  
シンプルなアプリケーションとして、カメラから画像をキャプチャし、LCDへ描画することができます（サンプルコード）。  
複雑なアプリケーションとしては上記に加え、スイッチ操作により解像度を切り替えたり、外付けフラッシュメモリーへ画像を保存したりすることができます（実証済み）。  
また、Linux版のレジスタ設定の最適化を試みています。  
参考:Linux版レジスタ設定 https://github.com/torvalds/linux/blob/master/drivers/media/i2c/ov7670.c  
その他、詳細はソースコード、および、ソースコード中のコメントを見てください。

## 前提
ここで公開している成果物はATmega328Pで動作確認済みです。  

## 開発環境
Windows10, Microchip Studio 7, OV7670, ATmega328P, LCD:ST7735/ST7789, フラッシュメモリー:IS25LP040

## ビルド環境
プロジェクトフォルダ(例：mysample)にファイルをフラットに配置し、ビルドします。  
<code>
mysample
│  main.c
│  OV7670.c
│  OV7670.h
│  OV7670Register.c
│  OV7670Register.h
│  mysample.atsln              ※Microchip Studio 固有ファイル
│  mysample.componentinfo.xml  ※Microchip Studio 固有ファイル
│  mysample.cproj              ※Microchip Studio 固有ファイル
│  spilib.c
│  spilib.h
│  ST77XXlib.c
│  ST77XXlib.h
│  twilib.c
│  twilib.h
</code>
