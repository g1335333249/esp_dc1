#include <ESP8266WebServer.h>
#include <flash_hal.h>
#include <FS.h>
#include "StreamString.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "Http.h"
#include "Module.h"
#include "Rtc.h"
#include "HttpHtml.h"

ESP8266WebServer *Http::server;
bool Http::isBegin = false;

void Http::handleRoot()
{
    if (captivePortal())
    {
        return;
    }
    if (!checkAuth())
    {
        return;
    }

    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    server->send(200, F("text/html"), "");
    server->sendContent_P(HTTP_HTML_HEAD_START);
    server->sendContent(module ? module->getModuleCNName() : F("修复模式"));
    server->sendContent_P(HTTP_HTML_HEAD_END);
    server->sendContent(module ? module->getModuleCNName() : F("修复模式"));
    server->sendContent_P(HTTP_HTML_NAV_START);
#ifdef WEB_LOG_SIZE
    server->sendContent_P(PSTR("<button onclick='tab(6)'>日志</button>"));
#endif
    server->sendContent_P(HTTP_HTML_NAV_END);

    // TAB 1 Start
    uint8_t mode = WiFi.getMode();
    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<div id='tab'>"
                    "<div id='tab1' style='display: block;'>"
                    "<table class='gridtable'><thead><tr><th colspan='2'>WiFi状态</th></tr></thead><tbody>"
                    "<tr><td>主机名</td><td>%s</td></tr>"
                    "<tr><td>WiFi模式</td><td>%s</td></tr>"
                    "<tr><td>SSID</td><td>%s</td></tr>"),
               UID,
               (mode == WIFI_STA ? PSTR("STA") : (mode == WIFI_AP ? PSTR("AP") : PSTR("AP STA"))),
               WiFi.SSID().c_str());
    server->sendContent_P(tmpData);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>RSSI</td><td>%ddBm</td></tr>"
                    "<tr><td>开机时间</td><td id='uptime'>%s</td></tr>"
                    "<tr><td>空闲内存</td><td><span id='free_mem'>%d</span> kB</td></tr>"
                    "<tr><td>IP地址</td><td>%s</td></tr>"
                    "<tr><td>DHCP</td><td>%s</td></tr>"
                    "</tbody></table>"
                    "</div>"),
               WiFi.RSSI(), Rtc::msToHumanString(millis()).c_str(), ESP.getFreeHeap() / 1024,
               WiFi.localIP().toString().c_str(), (globalConfig.wifi.is_static ? PSTR("静态IP") : PSTR("DHCP")));
    server->sendContent_P(tmpData);
    // TAB 1 End

    // TAB 2 Start
    server->sendContent_P(HTTP_HTML_TAB2_WIFI);

    server->sendContent_P(HTTP_HTML_TAB2_DHCP_START);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr class='dhcp_hide'><td>静态IP</td><td><input type='text' name='static_ip' value='%s'></td></tr>"
                    "<tr class='dhcp_hide'><td>子网掩码</td><td><input type='text' name='static_netmask' value='%s'></td></tr>"
                    "<tr class='dhcp_hide'><td>网关</td><td><input type='text' name='static_gateway' value='%s'></td></tr>"),
               globalConfig.wifi.ip, globalConfig.wifi.sn, globalConfig.wifi.gw);
    server->sendContent_P(tmpData);

    server->sendContent_P(HTTP_HTML_TAB2_DHCP_END);

#ifndef DISABLE_MQTT
    server->sendContent_P(HTTP_HTML_TAB2_MQTT_START);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>地址</td><td><input type='text' name='mqtt_server' value='%s'></td></tr>"
                    "<tr><td>端口</td><td><input type='number' min='0' max='65535' name='mqtt_port' required value='%d'>&nbsp;&nbsp;&nbsp;&nbsp;0为不启动mqtt</td></tr>"),
               globalConfig.mqtt.server, globalConfig.mqtt.port);
    server->sendContent_P(tmpData);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>用户名</td><td><input type='text' name='mqtt_username' value='%s'></td></tr>"
                    "<tr><td>密码</td><td><input type='password' name='mqtt_password' value='%s'></td></tr>"),
               globalConfig.mqtt.user, globalConfig.mqtt.pass);
    server->sendContent_P(tmpData);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>主题</td><td><input type='text' name='mqtt_topic' value='%s' style='min-width:90%'></td></tr>"
                    "<tr><td>心跳上报间隔</td><td><input type='number' min='0' max='3600' name='interval' required value='%d'>&nbsp;秒&nbsp;&nbsp;0为不上报</td></tr>"),
               globalConfig.mqtt.topic, globalConfig.mqtt.interval);
    server->sendContent_P(tmpData);

    server->sendContent_P(HTTP_HTML_TAB2_MQTT_RETAIN);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>状态</td><td id='mqttconnected'>%s</td></tr>"
                    "<tr><td colspan='2'><button type='submit' class='btn-info'>保存</button></td></tr>"
                    "</tbody></table></form>"),
               Mqtt::mqttClient.connected() ? PSTR("已连接") : PSTR("未连接"));
    server->sendContent_P(tmpData);

#ifndef DISABLE_MQTT_DISCOVERY
    server->sendContent_P(HTTP_HTML_TAB2_DISCOVERY_START);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>自发现状态</td><td id='discovery'>%s</td></tr>"
                    "<tr><td>自发现前缀</td><td><input type='text' name='discovery_prefix' required value='%s'></td></tr>"),
               globalConfig.mqtt.discovery ? PSTR("已启动") : PSTR("未启动"), globalConfig.mqtt.discovery_prefix);
    server->sendContent_P(tmpData);

    server->sendContent_P(HTTP_HTML_TAB2_DISCOVERY_END);
#endif
#endif

    server->sendContent_P(HTTP_HTML_TAB2_END);
    // TAB 2 End

    // TAB 3 Start
    server->sendContent_P(HTTP_HTML_TAB3_START);

    if (module)
    {
        module->httpHtml(server);
    }

    server->sendContent_P(HTTP_HTML_TAB3_MODULE_START);

    snprintf_P(tmpData, sizeof(tmpData), PSTR("<tr><td>主机名</td><td><input type='text' name='uid' value='%s'>&nbsp;具有唯一性，留空默认</td></tr>"), UID);
    server->sendContent_P(tmpData);

    server->sendContent_P(HTTP_HTML_LOG_OUTPUT_START);
#ifdef USE_SYSLOG
    server->sendContent_P(PSTR("<label class='bui-radios-label'><input type='checkbox' name='log_syslog' value='1'/><i class='bui-radios' style='border-radius:20%'></i> syslog</label>&nbsp;&nbsp;&nbsp;&nbsp;"));
#endif
#ifdef WEB_LOG_SIZE
    server->sendContent_P(PSTR("<label class='bui-radios-label'><input type='checkbox' name='log_web' value='1'/><i class='bui-radios' style='border-radius:20%'></i> web</label>&nbsp;&nbsp;&nbsp;&nbsp;"));
#endif
    server->sendContent_P(HTTP_HTML_LOG_OUTPUT_END);

#ifdef USE_SYSLOG
    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>syslog服务器</td><td>"
                    "<input type='text' name='log_syslog_host' style='width:150px' value='%s'> : "
                    "<input type='number' name='log_syslog_port' value='%d' min='0' max='65000' style='width:50px'>"
                    "</td></tr>"),
               globalConfig.debug.server, globalConfig.debug.port);
    server->sendContent_P(tmpData);
#endif

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>NTP服务器</td><td>"
                    "<input type='text' name='ntp' style='width:150px' value='%s'> 建议在获取时间失败时才填写"
                    "</td></tr>"),
               globalConfig.wifi.ntp);
    server->sendContent_P(tmpData);

    server->sendContent_P(HTTP_HTML_TAB3_MODULE_END);
    // TAB 3 End

    // TAB Security Start
    server->sendContent_P(HTTP_HTML_TAB_SECURITY_START);

    snprintf_P(tmpData, sizeof(tmpData), PSTR("<tr><td>Web用户名</td><td><input type='text' name='http_username' value='%s' maxlength='14'>&nbsp;留空关闭认证</td></tr>"
                                               "<tr><td>Web密码</td><td><input type='password' name='http_password' value='%s' maxlength='14'></td></tr>"),
               globalConfig.http.user, globalConfig.http.pass);
    server->sendContent_P(tmpData);

    server->sendContent_P(HTTP_HTML_TAB_SECURITY_END);
    // TAB Security End

    // TAB 4 Start
    server->sendContent_P(HTTP_HTML_TAB4_START);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>ESP芯片ID</td><td>%d</td></tr>"
                    "<tr><td>Flash芯片 ID</td><td>%d</td></tr>"
                    "<tr><td>Flash大小</td><td>%d kB</td></tr>"
                    "<tr><td>固件Flash大小</td><td>%d kB</td></tr>"
                    "<tr><td>固件大小</td><td>%d kB</td></tr>"),
               ESP.getChipId(), ESP.getFlashChipId(), ESP.getFlashChipRealSize() / 1024, ESP.getFlashChipSize() / 1024, ESP.getSketchSize() / 1024);
    server->sendContent_P(tmpData);

    uint8_t mac[6];
    wifi_get_macaddr(STATION_IF, mac);
    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>空闲程序空间</td><td>%d kB</td></tr>"
                    "<tr><td>内核和SDK版本</td><td>" ARDUINO_ESP8266_RELEASE "%s</td></tr>"
                    "<tr><td>重启原因</td><td>%s</td></tr>"
                    "<tr><td>MAC地址</td><td>%02X:%02X:%02X:%02X:%02X:%02X</td></tr>"
                    "</tbody></table>"),
               ESP.getFreeSketchSpace() / 1024, ESP.getSdkVersion(), ESP.getResetReason().c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    server->sendContent_P(tmpData);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<table class='gridtable'><thead><tr><th colspan='2'>固件升级</th></tr></thead><tbody>"
                    "<tr><td>当前版本</td><td>%s</td></tr>"
                    "<tr><td>编译时间</td><td>%s</td></tr>"),
               module ? module->getModuleVersion().c_str() : PSTR("0"), Rtc::GetBuildDateAndTime().c_str());
    server->sendContent_P(tmpData);

    server->sendContent_P(HTTP_HTML_TAB4_OTA);
    // TAB 4 End

#ifdef WEB_LOG_SIZE
    // TAB 5 Start
    server->sendContent_P(HTTP_HTML_TAB5);
// TAB 5 End
#endif

    server->sendContent_P(HTTP_HTML_FOOTER);

    // TAB 2
    snprintf_P(tmpData, sizeof(tmpData), PSTR("<script type='text/javascript'>%ssetRadioValue('dhcp', '%d');dhcponchange(null);"),
               WiFi.isConnected() ? PSTR("") : PSTR("scanWifi();"), globalConfig.wifi.is_static ? 2 : 1);
    server->sendContent_P(tmpData);

#ifndef DISABLE_MQTT
    snprintf_P(tmpData, sizeof(tmpData), PSTR("setRadioValue('retain', '%d');"), globalConfig.mqtt.retain ? 1 : 0);
    server->sendContent_P(tmpData);

#ifndef DISABLE_MQTT_DISCOVERY
    if (globalConfig.mqtt.discovery)
    {
        server->sendContent_P(HTTP_HTML_SCRIPT_DISCOVERY_BTN);
    }
#endif
#endif
    // TAB 3
    if ((1 & globalConfig.debug.type) == 1)
    {
        server->sendContent_P(HTTP_HTML_SCRIPT_LOG_SERIAL);
    }
#ifdef USE_SYSLOG
    if ((2 & globalConfig.debug.type) == 2)
    {
        server->sendContent_P(HTTP_HTML_SCRIPT_LOG_SYSLOG);
    }
#endif
#ifdef WEB_LOG_SIZE
    if ((4 & globalConfig.debug.type) == 4)
    {
        server->sendContent_P(HTTP_HTML_SCRIPT_LOG_WEB);
    }
#endif
    if ((8 & globalConfig.debug.type) == 8)
    {
        server->sendContent_P(HTTP_HTML_SCRIPT_LOG_SERIAL1);
    }
    server->sendContent_P(HTTP_HTML_SCRIPT_END);
}

#ifndef DISABLE_MQTT
void Http::handleMqtt()
{
    if (!checkAuth())
    {
        return;
    }
    String topic = server->arg(F("mqtt_topic"));
    if (topic.length() == 0)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"MQTT主题不能为空\"}"));
        return;
    }
    if (topic.indexOf("%prefix%/") == 0)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"MQTT主题必须包含【%prefix%/】\"}"));
        return;
    }
    strcpy(globalConfig.mqtt.server, server->arg(F("mqtt_server")).c_str());
    globalConfig.mqtt.port = server->arg(F("mqtt_port")).toInt();
    globalConfig.mqtt.retain = server->arg(F("retain")) == F("1");
    strcpy(globalConfig.mqtt.user, server->arg(F("mqtt_username")).c_str());
    strcpy(globalConfig.mqtt.pass, server->arg(F("mqtt_password")).c_str());
    strcpy(globalConfig.mqtt.topic, topic.c_str());
    globalConfig.mqtt.interval = server->arg(F("interval")).toInt();
    Config::saveConfig();

    if (Mqtt::mqttClient.connected())
    {
        Mqtt::mqttClient.disconnect();
    }

    if (Mqtt::mqttConnect())
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置MQTT服务器成功，已连接。\",\"data\":{\"mqttconnected\":\"已连接\"}}"));
    }
    else
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置MQTT服务器成功，未连接。\",\"data\":{\"mqttconnected\":\"未连接\"}}"));
    }
}

#ifndef DISABLE_MQTT_DISCOVERY
void Http::handleDiscovery()
{
    if (!checkAuth())
    {
        return;
    }
    strcpy(globalConfig.mqtt.discovery_prefix, server->arg(F("discovery_prefix")).c_str());
    globalConfig.mqtt.discovery = !globalConfig.mqtt.discovery;
    Config::saveConfig();

    if (module)
    {
        module->mqttDiscovery(globalConfig.mqtt.discovery);
    }
    if (globalConfig.mqtt.discovery)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"已经打开MQTT自发现。\",\"data\":{\"discovery\":1}}"));
    }
    else
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"已经关闭MQTT自发现。\",\"data\":{\"discovery\":0}}"));
    }
}
#endif
#endif

void Http::handledhcp()
{
    if (!checkAuth())
    {
        return;
    }
    String ip = server->arg(F("static_ip"));
    String netmask = server->arg(F("static_netmask"));
    String gateway = server->arg(F("static_gateway"));
    if (!Wifi::isIp(ip))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"IP地址错误\"}"));
        return;
    }
    if (!Wifi::isIp(netmask))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"掩码地址错误\"}"));
        return;
    }
    if (!Wifi::isIp(gateway))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"网关地址错误\"}"));
        return;
    }

    IPAddress static_ip;
    IPAddress static_sn;
    IPAddress static_gw;
    static_ip.fromString(ip);
    static_sn.fromString(netmask);
    static_gw.fromString(gateway);

    if (!(static_ip.isV4() && static_sn.isV4() && (!static_gw.isSet() || static_gw.isV4())))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"IP地址或者网关错误\"}"));
        return;
    }

    if ((static_ip.v4() & static_sn.v4()) != (static_gw.v4() & static_sn.v4()))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"网段错误\"}"));
        return;
    }

    bool old = globalConfig.wifi.is_static;
    globalConfig.wifi.is_static = server->arg(F("dhcp")).equals(F("2"));
    strcpy(globalConfig.wifi.ip, ip.c_str());
    strcpy(globalConfig.wifi.sn, netmask.c_str());
    strcpy(globalConfig.wifi.gw, gateway.c_str());
    Config::saveConfig();

    if (old != globalConfig.wifi.is_static)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置DHCP信息成功，重启后生效\"}"));
    }
    else
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置DHCP信息成功\"}"));
    }
}

void Http::handleScanWifi()
{
    if (!checkAuth())
    {
        return;
    }
    int n = WiFi.scanNetworks();
    if (n == 0)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"\",\"data\":{\"list\":[]}}"));
        //server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"找不到网络，请重新试试。\"}"));
        return;
    }

    //sort networks
    int indices[n];
    for (int i = 0; i < n; i++)
    {
        indices[i] = i;
    }

    // RSSI排序
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
            {
                std::swap(indices[i], indices[j]);
            }
        }
    }

    // 删除重复项（必须对RSSI进行排序）
    String cssid;
    for (int i = 0; i < n; i++)
    {
        if (indices[i] == -1)
            continue;
        cssid = WiFi.SSID(indices[i]);
        for (int j = i + 1; j < n; j++)
        {
            if (cssid == WiFi.SSID(indices[j]))
            {
                indices[j] = -1; // set dup aps to index -1
            }
        }
    }

    int _minimumQuality = -1;
    int quality;
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"\",\"data\":{\"list\":["));
    for (int i = 0; i < n; i++)
    {
        if (indices[i] == -1)
            continue; // skip dups
        int RSSI = WiFi.RSSI(indices[i]);
        if (RSSI <= -100)
        {
            quality = 0;
        }
        else if (RSSI >= -50)
        {
            quality = 100;
        }
        else
        {
            quality = 2 * (RSSI + 100);
        }
        if (_minimumQuality == -1 || _minimumQuality < quality)
        {
            if (i != 0)
            {
                server->sendContent_P(PSTR(","));
            }
            snprintf_P(tmpData, sizeof(tmpData), PSTR("{\"name\":\"%s\",\"rssi\":%d,\"quality\":%d,\"type\":%d}"), WiFi.SSID(indices[i]).c_str(), RSSI, quality, indices[i]);
            server->sendContent_P(tmpData);
        }
    }

    server->sendContent_P(PSTR("]}}"));
}

void Http::handleWifi()
{
    if (!checkAuth())
    {
        return;
    }
    String wifi = server->arg(F("wifi_ssid"));
    if (wifi == "")
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"WiFi名称不能为空。\"}"));
        return;
    }
    String password = server->arg(F("wifi_password"));

    if (WiFi.getMode() == WIFI_STA)
    {
        strcpy(globalConfig.wifi.ssid, wifi.c_str());
        strcpy(globalConfig.wifi.pass, password.c_str());
        Config::saveConfig();
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置WiFi信息成功，重启模块（手动）使用新的Wifi信息连接。\"}"));
    }
    else
    {
        Wifi::tryConnect(wifi, password);
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"尝试将ESP连接到网络。 如果失败，请重新连接到AP再试一次。\"}"));
    }
}

void Http::handleOperate()
{
    if (!checkAuth())
    {
        return;
    }
    String d = server->arg(F("d")); // 1：重启模块 2：重置模块
    if (d == F("1"))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设备正在重启 . . .\"}"));
    }
    else if (d == F("2"))
    {
        Config::resetConfig();
        Config::saveConfig();
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"正在重置模块 . . . 设备将会重启。\"}"));
    }
    delay(200);

    Led::blinkLED(400, 4);
    ESP.restart();
}

void Http::handleOTA()
{
    if (!checkAuth())
    {
        return;
    }
    strcpy(globalConfig.http.ota_url, server->arg(F("ota_url")).c_str());
    server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"如果成功后设备会重启 . . . \"}"));
    Http::OTA(String(globalConfig.http.ota_url));
}

void Http::handleNotFound()
{
    if (captivePortal())
    {
        return;
    }
    server->sendHeader(PSTR("Cache-Control"), PSTR("no-cache, no-store, must-revalidate"));
    server->sendHeader(PSTR("Pragma"), PSTR("no-cache"));
    server->sendHeader(PSTR("Expires"), PSTR("-1"));
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);

    snprintf_P(tmpData, sizeof(tmpData), PSTR("File Not Found\n\nURI: %s\nMethod: %s\nArguments: %d\n"),
               server->uri().c_str(), server->method() == HTTP_GET ? PSTR("GET") : PSTR("POST"), server->args());
    server->send_P(404, PSTR("text/plain"), tmpData);
    for (uint8_t i = 0; i < server->args(); i++)
    {
        snprintf_P(tmpData, sizeof(tmpData), PSTR(" %s: %s\n"), server->argName(i).c_str(), server->arg(i).c_str());
        server->sendContent_P(tmpData);
    }
}

void Http::handleGetStatus()
{
    if (!checkAuth())
    {
        return;
    }

    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"\",\"data\":{"));

    snprintf_P(tmpData, sizeof(tmpData), PSTR("\"uptime\":\"%s\",\"free_mem\":%d"), Rtc::msToHumanString(millis()).c_str(), ESP.getFreeHeap() / 1024);
    server->sendContent_P(tmpData);

#ifndef DISABLE_MQTT
    if (Mqtt::mqttClient.connected())
    {
        server->sendContent_P(PSTR(",\"mqttconnected\":\"已连接\""));
    }
    else
    {
        server->sendContent_P(PSTR(",\"mqttconnected\":\"未连接\""));
    }

#ifndef DISABLE_MQTT_DISCOVERY
    snprintf_P(tmpData, sizeof(tmpData), PSTR(",\"discovery\":%d"), globalConfig.mqtt.discovery ? 1 : 0);
    server->sendContent_P(tmpData);
#endif
#endif

    if (Wifi::configPortalStart == 0 && WiFi.isConnected())
    {
        snprintf_P(tmpData, sizeof(tmpData), PSTR(",\"ip\":\"%s\""), WiFi.localIP().toString().c_str());
        server->sendContent_P(tmpData);
    }

    if (module)
    {
        String tmp = module->httpGetStatus(server);
        if (tmp.length() > 0)
        {
            server->sendContent_P(PSTR(","));
            server->sendContent(tmp);
        }
    }

#ifdef WEB_LOG_SIZE
    bool cflg = true;
    uint8_t counter = 0;
    if (server->hasArg(F("i")))
    {
        counter = server->arg(F("i")).toInt();
    }
    snprintf_P(tmpData, sizeof(tmpData), PSTR(",\"logindex\":%d,\"log\":\""), Debug::webLogIndex);
    server->sendContent_P(tmpData);
    if (counter != Debug::webLogIndex)
    {
        if (!counter)
        {
            counter = Debug::webLogIndex;
            cflg = false;
        }
        do
        {
            char *tmp;
            uint16_t len;
            Debug::GetLog(counter, &tmp, &len);
            if (len)
            {
                if (cflg)
                {
                    server->sendContent_P(PSTR("\\n"));
                }

                size_t j = 0;
                for (size_t i = 0; i < len - 1; i++)
                {
                    char each = tmp[i];
                    if (each == '\\' || each == '"')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = each;
                    }
                    else if (each == '\b')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 'b';
                    }
                    else if (each == '\f')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 'f';
                    }
                    else if (each == '\n')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 'n';
                    }
                    else if (each == '\r')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 'r';
                    }
                    else if (each == '\t')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 't';
                    }
                    else
                    {
                        tmpData[j++] = each;
                    }
                }
                tmpData[j++] = '\0';

                server->sendContent_P(tmpData);
                cflg = true;
            }
            counter++;
            if (!counter)
            {
                counter++;
            } // Skip log index 0 as it is not allowed
        } while (counter != Debug::webLogIndex);
    }
    server->sendContent_P(PSTR("\"}}"));
#else
    server->sendContent_P(PSTR("}}"));
#endif
}

void Http::handleUpdate()
{
    // handler for the /update form POST (once file upload finishes)
    server->on(
        F("/update"), HTTP_POST, [&]() {
        if (!checkAuth())
        {
            return;
        } 
        if (Update.hasError())
        {
            uint8_t _error = Update.getError();
            if(_error == UPDATE_ERROR_WRITE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Flash Write Failed"), _error);
            } else if(_error == UPDATE_ERROR_ERASE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Flash Erase Failed"), _error);
            } else if(_error == UPDATE_ERROR_READ){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Flash Read Failed"), _error);
            } else if(_error == UPDATE_ERROR_SPACE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Not Enough Space"), _error);
            } else if(_error == UPDATE_ERROR_SIZE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Bad Size Given"), _error);
            } else if(_error == UPDATE_ERROR_STREAM){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Stream Read Timeout"), _error);
            } else if(_error == UPDATE_ERROR_SIGN){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Signature verification failed"), _error);
            } else if(_error == UPDATE_ERROR_FLASH_CONFIG){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Flash config wrong real: %d IDE: %d"), _error,  ESP.getFlashChipRealSize(), ESP.getFlashChipSize());
            } else if(_error == UPDATE_ERROR_NEW_FLASH_CONFIG){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: new Flash config wrong real: %d"), _error, ESP.getFlashChipRealSize());
            } else if(_error == UPDATE_ERROR_MAGIC_BYTE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Magic byte is wrong, not 0xE9"), _error);
            } else if (_error == UPDATE_ERROR_BOOTSTRAP){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Invalid bootstrapping state, reset ESP8266 before updating"), _error);
            } else {
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: UNKNOWN"), _error);
            }
            Debug::AddLog(LOG_LEVEL_ERROR);
            char out[150] = {0};
            snprintf_P(out, sizeof(out), PSTR("{\"code\":0,\"msg\":\"%s\"}"), tmpData);
            server->send_P(200, PSTR("text/html"), out);
        }
        else
        {
            Config::saveConfig();
            server->client().setNoDelay(true);
            server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"升级成功，设备将自动重启，稍后刷新页面即可。\"}"));
            //server->send_P(200, PSTR("text/html"), PSTR("<meta charset='utf-8'/><meta http-equiv=\"refresh\" content=\"15;URL=/\">升级成功！正在重启 . . ."));
            delay(100);
            server->client().stop();
            ESP.restart();
        } }, [&]() {
                HTTPUpload &upload = server->upload();
                if (upload.status == UPLOAD_FILE_START)
                {
                    if (globalConfig.http.user[0] != 0 && globalConfig.http.pass[0] != 0 && server->client().localIP().toString() != "192.168.4.1" && !server->authenticate(globalConfig.http.user, globalConfig.http.pass))
                    {
                        Debug::AddInfo(PSTR("Unauthenticated Update"));
                        return;
                    }
                    WiFiUDP::stopAll();
                    Debug::AddInfo(PSTR("Update: %s"), upload.filename.c_str());
                    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                    if (!Update.begin(maxSketchSpace, U_FLASH))//start with max available size
                    { 
                    }
                }
                else if (upload.status == UPLOAD_FILE_WRITE && !Update.hasError())
                {
                    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                    {
                    }
                }
                else if (upload.status == UPLOAD_FILE_END && !Update.hasError())
                {
                    if (Update.end(true))
                    { 
                        Debug::AddInfo(PSTR("Update Success: %u   Rebooting..."), upload.totalSize);
                    }
                }
                else if (upload.status == UPLOAD_FILE_ABORTED)
                {
                    Update.end();
                    Debug::AddInfo(PSTR("Update was aborted"));
                }
                delay(0); });
}

void Http::begin()
{
    if (isBegin)
    {
        return;
    }
    isBegin = true;
    server = new ESP8266WebServer();

    server->on(F("/"), handleRoot);
#ifndef DISABLE_MQTT
    server->on(F("/mqtt"), handleMqtt);
#ifndef DISABLE_MQTT_DISCOVERY
    server->on(F("/discovery"), handleDiscovery);
#endif
#endif
    server->on(F("/dhcp"), handledhcp);
    server->on(F("/scan_wifi"), handleScanWifi);
    server->on(F("/wifi"), handleWifi);
    server->on(F("/operate"), handleOperate);
    server->on(F("/module_setting"), handleModuleSetting);
    server->on(F("/ota"), handleOTA);
    server->on(F("/get_status"), handleGetStatus);
    server->onNotFound(handleNotFound);
    handleUpdate();

    if (module)
    {
        module->httpAdd(server);
    }
    server->begin(globalConfig.http.port);
    Debug::AddInfo(PSTR("HTTP server started port: %d"), globalConfig.http.port);
}

void Http::stop()
{
    if (!isBegin)
    {
        return;
    }
    server->stop();
    Debug::AddInfo(PSTR("HTTP server stoped"));
}

void Http::loop()
{
    if (isBegin)
    {
        server->handleClient();
    }
}

bool Http::captivePortal()
{
    if (!Wifi::isIp(server->hostHeader()))
    {
        //Debug::AddInfo(PSTR("Request redirected to captive portal"));
        server->sendHeader(F("Location"), String(F("http://")) + server->client().localIP().toString(), true);
        server->send(302, F("text/plain"), ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
        server->client().stop();                // Stop is needed because we sent no content length
        return true;
    }
    return false;
}

void Http::handleModuleSetting()
{
    if (!checkAuth())
    {
        return;
    }

    int t = 0;
    if (server->arg(F("log_serial")).equals(F("1")))
    {
        t = t | 1;
    }
    if (server->arg(F("log_serial1")).equals(F("1")))
    {
        t = t | 8;
    }
#ifdef WEB_LOG_SIZE
    if (server->arg(F("log_web")).equals(F("1")))
    {
        t = t | 4;
    }
#endif

#ifdef USE_SYSLOG
    if (server->arg(F("log_syslog")).equals(F("1")))
    {
        t = t | 2;
        String log_syslog_host = server->arg(F("log_syslog_host"));
        String log_syslog_port = server->arg(F("log_syslog_port"));
        if (log_syslog_host.length() == 0)
        {
            server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"syslog服务器不能为空\"}"));
            return;
        }
        strcpy(globalConfig.debug.server, log_syslog_host.c_str());
        globalConfig.debug.port = log_syslog_port.toInt();
        WiFi.hostByName(globalConfig.debug.server, Debug::ip);
    }
#endif

    globalConfig.debug.type = t;
    if ((8 & globalConfig.debug.type) == 8)
    {
        Serial1.begin(115200);
    }

    String ntp = server->arg(F("ntp"));
    if (strcmp(globalConfig.wifi.ntp, ntp.c_str()) != 0)
    {
        strcpy(globalConfig.wifi.ntp, ntp.c_str());
        Rtc::init();
    }

    String httpUsername = server->arg(F("http_username"));
    String httpPassword = server->arg(F("http_password"));
    strcpy(globalConfig.http.user, httpUsername.c_str());
    strcpy(globalConfig.http.pass, httpPassword.c_str());

    String uid = server->arg(F("uid"));
    strcpy(globalConfig.uid, uid.c_str());
    Config::saveConfig();
    if (uid.length() == 0 || strcmp(globalConfig.uid, UID) != 0)
    {
#ifndef DISABLE_MQTT
#ifndef DISABLE_MQTT_DISCOVERY
        if (globalConfig.mqtt.discovery && module)
        {
            module->mqttDiscovery(false);
        }
#endif
#endif
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"修改了重要配置 . . . 正在重启中。\"}"));
        Led::blinkLED(400, 4);
        ESP.restart();
    }
    else
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"已经修改成功\"}"));
    }
}

bool Http::checkAuth()
{
    if (globalConfig.http.user[0] != 0 && globalConfig.http.pass[0] != 0 && server->client().localIP().toString() != F("192.168.4.1"))
    {
        if (!server->authenticate(globalConfig.http.user, globalConfig.http.pass))
        {
            server->requestAuthentication();
            return false;
        }
    }
    return true;
}

void Http::OTA(String url)
{
    if (url.indexOf(F("%04d")) != -1)
    {
        url.replace(F("%04d"), String(ESP.getChipId() & 0x1fff));
    }
    else if (url.indexOf(F("%d")) != -1)
    {
        url.replace(F("%d"), String(ESP.getChipId()));
    }
    url.replace(F("%hostname%"), UID);
    url.replace(F("%module%"), module ? module->getModuleName() : F(""));

    Config::saveConfig();
    Debug::AddInfo(PSTR("OTA Url: %s"), url.c_str());
    Led::blinkLED(200, 5);
    WiFiClient OTAclient;
    if (ESPhttpUpdate.update(OTAclient, url, (module ? module->getModuleVersion() : F(""))) == HTTP_UPDATE_FAILED)
    {
        Debug::AddError(PSTR("HTTP_UPDATE_FAILD Error (%d): %s"), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    }
}
