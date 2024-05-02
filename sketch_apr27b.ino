#define AP_SSID "HUAWEI-5dJS"
#define AP_PASS "5SFhuL4s"

#include <GyverNTP.h>
#include <GyverHub.h>
GyverHub hub;
GyverNTP ntp(3);  // часовой пояс в часах и период обновления в секундах
uint32_t t;
bool flag, window, light;
void btn() {
  flag = !flag;
  hub.sendUpdate("pomp", flag);
  if (flag) hub.update("pomp_btn").label("Выключить помпу");
  else hub.update("pomp_btn").label("Включить помпу");
}
void btn_window() {
  window = !window;
  hub.sendUpdate("window", window);
  if (window) hub.update("btn_window").label("Закрыть окно");
  else hub.update("btn_window").label("Открыть окно");
}


void build(gh::Builder& b) {
  b.Menu("Инфо о датчиках и автомат;Ручной режим");
  b.show(b.menu() == 1);
  b.beginRow();
  if (flag) b.Button_("pomp_btn").attach(btn).label("Выключить помпу");
  else b.Button_("pomp_btn").attach(btn).label("Включить помпу");
  if (window) b.Button_("btn_window").attach(btn_window).label("Закрыть окно");
  else b.Button_("btn_window").attach(btn_window).label("Открыть окно");
  b.endRow();
    b.show(b.menu() == 0);
  b.beginRow();
  // b.widget();
  b.Gauge_("TEMP").range(0, 100, 5).value(50).label("TEMP °С");
  b.Gauge_("hum").range(0, 100, 5).value(50).label("Влажность %");
  b.Gauge_("hum_soil").range(0, 100, 5).value(50).label("Влажность почвы %");
  b.endRow();
  b.beginRow();
  b.LED_("pomp").value(flag).label("Статус полива");
  b.LED_("window").value(window).label("Статус открытия форточки");
  b.endRow();
  b.beginRow();
  b.Label_("time").text(ntp.timeString()).label("Время сервера");
  b.LED_("time_status").value(false).label("Статус запроса времени");
  b.endRow();
  b.beginRow();
  b.LED_("stats_light").label("Свет").value(light);
  b.endRow();
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  hub.mqtt.config("test.mosquitto.org", 1883);
  hub.config(F("MyDevices"), F("Теплица"));
  ntp.begin();
  ntp.updateNow();
  hub.onBuild(build);
  hub.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  hub.tick();
  ntp.tick();
  if (millis() - t >= 1500) {
    hub.sendUpdate("TEMP", random(0, 100));
    t = millis();
  }
  hub.sendUpdate("time", ntp.timeString());
  if (ntp.status() < 1) {
    hub.sendUpdate("time_status", true);
  } else {
    hub.sendUpdate("time_status", false);
  }
}
