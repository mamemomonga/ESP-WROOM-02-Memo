#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include "config.h"
#include "htmldata.h"

MDNSResponder mdns;
ESP8266WebServer server(WEBSERVER_PORT);

// index.htmlをはき出す処理
CONFIG_HTML_DATA;

uint8_t leds1=0;
uint8_t leds2=0;

uint8_t btn_wht_last=1;
uint8_t btn_blu_last=1;

// LEDを10ミリ秒だけ点灯
void led_flash_short(uint8_t num) {
	leds2 |= (1<<num); set_leds();
	delay(10);
	leds2 &=~ (1<<num); set_leds();
}

// ポーリングの結果を返す
void resp_polling(void) {
	char message[128];
	snprintf( message, 128, "{ \"btn_wht\":%d, \"btn_blu\":%d }",
		( 1 & (leds2 >> BTN_WHT_LED )),
		( 1 & (leds2 >> BTN_BLU_LED ))
	);
	leds2 &=~ (1 << BTN_WHT_LED);	
	leds2 &=~ (1 << BTN_BLU_LED);	

	server.send(200, "text/json", message);
	led_flash_short(6);
}

// 8つのLED操作後のレスポンス
void resp_led(void) {
	char message[128];
	snprintf( message, 128, "{ \"led_value\":%d }", leds1 );
	server.send(200, "text/json", message);
	led_flash_short(6);
}

// 8つのLEDのリセットの処理
void resp_reset_led(void) {
	leds1=0; set_leds(); resp_led();
}

// 8つのLEDのトグル処理
void resp_toggle_led_one( uint8_t pos ) {
	leds1^=(1 << pos ); set_leds(); resp_led();
}

// 404 NotFound
void handle_not_found(void){
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET)?"GET":"POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i=0; i<server.args(); i++){
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
	led_flash_short(0);
}

// SPIを通じてLEDの状態を設定する
void set_leds( void ) {
	digitalWrite(SS,LOW);
	SPI.transfer(leds1);
	SPI.transfer(leds2);
	digitalWrite(SS,HIGH);
}

// セットアップ
void setup(void) {

	// シリアルの設定
	Serial.begin(115200);
	Serial.println("");
	Serial.println("");
	Serial.println("Setup");

	// SPIの設定
	pinMode(SS,OUTPUT);
	digitalWrite(SS,HIGH);

	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	SPI.setDataMode(SPI_MODE2);

	// かっこよくスプラッシュ
	leds1=0; leds2=0; set_leds(); delay(1000);
	for( uint8_t i=0; i<8; i++) { leds1=(1<<i); leds2=0; set_leds(); delay(30); }
	for( uint8_t i=0; i<8; i++) { leds2=(1<<i); leds1=0; set_leds(); delay(30); }
	leds1=0xFF; leds2=0xFF; set_leds(); delay(1000);
	leds1=0; leds2=0; set_leds(); delay(1000);

	// ボタンの設定
	pinMode(BTN_WHT,INPUT);
	pinMode(BTN_BLU,INPUT);

	// WiFiの接続
	WiFi.begin(WIFI_SSID, WIFI_PSK);
	Serial.print("Connecting");

	while (WiFi.status() != WL_CONNECTED) {
		delay(500); Serial.print(".");
		leds2 ^= 0b00000001;
		set_leds();
	}
	leds1 = 0b00000000;
	leds2 = 0b10000000;
	set_leds();

	Serial.println("");
	Serial.print("Connected to ");
	Serial.println( WIFI_SSID);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
  
	if (mdns.begin(MDNS_NAME, WiFi.localIP())) {
		Serial.println("MDNS responder started http://"MDNS_NAME".local/");
	}
 
	// ページの設定
	server.on("/", [](){ handle_send_index_html(); led_flash_short(6); });

	server.on("/0", [](){ resp_reset_led(); });
	server.on("/1", [](){ resp_toggle_led_one(0); });
	server.on("/2", [](){ resp_toggle_led_one(1); });
	server.on("/3", [](){ resp_toggle_led_one(2); });
	server.on("/4", [](){ resp_toggle_led_one(3); });
	server.on("/5", [](){ resp_toggle_led_one(4); });
	server.on("/6", [](){ resp_toggle_led_one(5); });
	server.on("/7", [](){ resp_toggle_led_one(6); });
	server.on("/8", [](){ resp_toggle_led_one(7); });
	server.on("/polling", [](){ resp_polling(); });
	server.on("/led",     [](){ resp_led(); });

	server.onNotFound( handle_not_found );
 
	// Webサーバの起動 
	server.begin();
	Serial.println("HTTP server started");

}

// 押しボタンの状態を検出
void button_detect( uint8_t btn, uint8_t *btn_last, void (*cb)(uint8_t) ) {
	if( btn != *btn_last ) {
		if( btn ) { cb(0); } else { cb(1); } 
		*btn_last = btn;
	}
}

// 白ボタンが押されたときのコールバック
void btn_wht_cb(uint8_t num) {
	if(num) {
		Serial.println("White Press");	
		leds2 |= (1 << BTN_WHT_LED);	
	} else {
		Serial.println("White Release");	
	}
	set_leds();
}

// 青ボタンが押されたときのコールバック
void btn_blu_cb(uint8_t num) {
	if(num) {
		Serial.println("Blue Press");	
		leds2 |= (1 << BTN_BLU_LED);	
	} else {
		Serial.println("Blue Release");	
	}
	set_leds();
}

// ループ処理
void loop(void) {
	server.handleClient();
	button_detect(digitalRead(BTN_WHT), &btn_wht_last, btn_wht_cb);
	button_detect(digitalRead(BTN_BLU), &btn_blu_last, btn_blu_cb);
}
