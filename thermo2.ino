#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS A0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 252);
EthernetServer server(80);
String HTTP_req;

void setup() {
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.begin(9600);
    sensors.begin();
}

void loop() {
    EthernetClient client = server.available();
    if (client) { 
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) { 
                char c = client.read();
                HTTP_req += c;
                if (c == '\n' && currentLineIsBlank) {
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println();
                    if (HTTP_req.indexOf("ajax_switch") > -1) {
                        getTemperature(client);
                    } else {
                        client.println("<!DOCTYPE html>");
                        client.println("<html>");
                        client.println("<head>");
                        client.println("<title>Masterl1nk's Arduino web server</title>");
                        client.println("<link href=\'http://fonts.googleapis.com/css?family=Roboto+Condensed\' rel=\'stylesheet\' type=\'text/css\'>");
                        client.println("<style type=\"text/css\">p#switch_txt {font-size:37px;margin:0px} body {text-align:center; font-family: \'Roboto Condensed\', sans-serif;}</style>");
                        client.println("<script>");
                        client.println("function getTemperature() {");
                        client.println("nocache = \"&nocache=\"\
                                                         + Math.random() * 1000000;");
                        client.println("var request = new XMLHttpRequest();");
                        client.println("request.onreadystatechange = function() {");
                        client.println("if (this.readyState == 4) {");
                        client.println("if (this.status == 200) {");
                        client.println("if (this.responseText != null) {");
                        client.println("document.getElementById(\"switch_txt\")\
.innerHTML = this.responseText;");
                        client.println("}}}}");
                        client.println(
                        "request.open(\"GET\", \"ajax_switch\" + nocache, true);");
                        client.println("request.send(null);");
                        client.println("setTimeout('getTemperature()', 1000);");
                        client.println("}");
                        client.println("</script>");
                        client.println("</head>");
                        client.println("<body onload=\"getTemperature()\">");
                        client.println("<h1>Masterl1nk's Arduino web server</h1>");
                        client.println(
                        "<p id=\"switch_txt\">Obteniendo...</p>");
                        client.println("<p>This server is located in Argentina</p>");
                        client.println("<p>Contact me at rey.nico@gmail.com</p>");
                        client.println("</body>");
                        client.println("</html>");
                    }
                    Serial.print(HTTP_req);
                    HTTP_req = "";
                    break;
                }
            }
        }
        delay(1);
        client.stop();
    }
}

void getTemperature(EthernetClient cl) {
    sensors.requestTemperatures();
    cl.print("Temperatura: ");
    cl.print(sensors.getTempCByIndex(0));
    cl.print(" C");
}