#ifndef _CONFIG_H_
#define _CONFIG_H_

// ウェブサーバのポート
#define WEBSERVER_PORT 80

// WiFIのSSIDとパスワード
#define WIFI_SSID "SSID"
#define WIFI_PSK  "PASSWORD"

// mDNSの名前
// BonjourやmDNS対応のクライアントの場合、http://MDNS_NAME/ でアクセスできる
#define MDNS_NAME "esp8266"

// SPI入出力
// | 機能 | ESPWROOM02 | 74HC595 | SN74HC595 | 
// ===========================================
// | SCK  | GPIO14     | STCP    | RCLK      |
// | MISO | GPIO12     |         |           |
// | MOSI | GPIO13     | DS      | SER       |
// | SS   | GPIO15     | SHCP    | SRCLK     |   

#define SS 15

// ボタン
#define BTN_WHT  5
#define BTN_BLU 16

// ボタンとLEDの対応
#define BTN_WHT_LED 4
#define BTN_BLU_LED 3

#endif

