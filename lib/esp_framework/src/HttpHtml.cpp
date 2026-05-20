#include "HttpHtml.h"
#include "Config.h"

const char HTTP_HTML_HEAD_START[] PROGMEM = 
    "<!DOCTYPE html><html lang='zh-cn'><head><meta charset='utf-8'/><meta name='viewport'content='width=device-width, initial-scale=1, user-scalable=no'/><title>";

const char HTTP_HTML_HEAD_END[] PROGMEM = 
    "</title><style type='text/css'>body{font-family:-apple-system,BlinkMacSystemFont,'Microsoft YaHei',sans-serif;font-size:16px;color:#333;line-height:1.75}#body{margin:0 auto;width:80%;max-width:600px}@media screen and (max-width:900px){#body{width:98%}}#nav{text-align:center}#tab>div{display:none}#nav button{background:#eee;border:1px solid #ddd;padding:.7em 1em;cursor:pointer;z-index:1;margin-left:-1px;outline:0}#nav .active{background:#fff}table.gridtable{color:#333;border-width:1px;border-color:#ddd;border-collapse:collapse;margin:auto;margin-top:15px;width:100%}table.gridtable th{border-width:1.5px;padding:8px;border-style:solid;border-color:#ddd;background-color:#f5f5f5}table.gridtable td{border-width:1px;padding:8px;border-style:solid;border-color:#ddd;background-color:#fff}input,select{border:1px solid #ccc;padding:7px 0;border-radius:3px;padding-left:5px;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075);box-shadow:inset 0 1px 1px rgba(0,0,0,.075);-webkit-transition:border-colo"
    "r ease-in-out .15s,-webkit-box-shadow ease-in-out .15s;-o-transition:border-color ease-in-out .15s,box-shadow ease-in-out .15s;transition:border-color ease-in-out .15s,box-shadow ease-in-out .15s}input:focus,select:focus{border-color:#66afe9;outline:0;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 8px rgba(102,175,233,.6);box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 8px rgba(102,175,233,.6)}#tab button{color:#fff;border-width:0;border-radius:3px;cursor:pointer;outline:0;font-size:17px;line-height:2.4rem;width:100%}#tab button[disabled]{cursor:not-allowed;filter:alpha(opacity=65);-webkit-box-shadow:none;box-shadow:none;opacity:.65}.btn-info{background-color:#5bc0de;border-color:#46b8da}.btn-info:hover{background-color:#31b0d5;border-color:#269abc}.btn-success{background-color:#5cb85c;border-color:#4cae4c}.btn-success:hover{background-color:#449d44;border-color:#398439}.btn-danger{background-color:#d9534f;border-color:#d43f3a}.btn-danger:hover{background-color:#c9302c;border-"
    "color:#ac2925}.alert{width:80%;padding:15px;border:1px solid transparent;border-radius:4px;position:fixed;top:10px;left:10%;z-index:999999;display:none}label.bui-radios-label input{position:absolute;opacity:0;visibility:hidden}label.bui-radios-label .bui-radios{display:inline-block;position:relative;width:13px;height:13px;background:#fff;border:1px solid #979797;border-radius:50%;vertical-align:-2px}label.bui-radios-label input:checked+.bui-radios:after{position:absolute;content:'';width:7px;height:7px;background-color:#fff;border-radius:50%;top:3px;left:3px}label.bui-radios-label input:checked+.bui-radios{background:#00b066;border:1px solid #00b066}label.bui-radios-label input:disabled+.bui-radios{background-color:#e8e8e8;border:solid 1px #979797}label.bui-radios-label input:disabled:checked+.bui-radios:after{background-color:#c1c1c1}label.bui-radios-label .bui-radios{-webkit-transition:background-color ease-out .3s;transition:background-color ease-out .3s}input[type='range']{width:80"
    "%;height:10px;border:0;background-color:#f0f0f0;border-radius:5px;position:relative;-webkit-appearance:none!important;outline:0}input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:20px;height:20px;border-radius:50%;background:#f40}.file{position:relative;display:inline-block;background:#d0eeff;border:1px solid #99d3f5;border-radius:4px;padding:4px 12px;overflow:hidden;color:#1e88c7;text-decoration:none;text-indent:0;line-height:20px}.file input{position:absolute;font-size:100px;right:0;top:0;opacity:0}.file:hover{background:#aadffd;border-color:#78c3f3;color:#004974;text-decoration:none}</style><script type='text/javascript'>var timer;var logIndex=0;var defIntervalTime=3000;var intervalTime=defIntervalTime;var lt;function id(d){return document.getElementById(d)}function tab(v){var divs=id('tab').childNodes;var btns=id('nav').getElementsByTagName('button');for(var i=0;i<divs.length;i++){divs[i].style.display=divs[i]==id('tab'+v)?'block':'none';btns[i].setAttribute('cla"
    "ss',(i+1==v?'active':''))}intervalTime=v==5?1000:defIntervalTime}function serialize(form){var field,s='';if(typeof form=='object'&&form.nodeName=='FORM'){for(var i=0;i<form.elements.length;i++){field=form.elements[i];if(field.name&&!field.disabled&&field.type!='file'&&field.type!='reset'&&field.type!='submit'&&field.type!='button'){if((field.type!='checkbox'&&field.type!='radio')||field.checked){s+=field.name+'='+encodeURIComponent(field.value)+'&'}}}}if(s.length>1){s=s.substring(0,s.length-1)}return s}function ajax(){var ajaxData={type:(arguments[0].type||'GET').toUpperCase(),url:arguments[0].url||'',data:arguments[0].data||null,success:arguments[0].success||function(){},error:arguments[0].error||function(){}};var xhr=new XMLHttpRequest();xhr.responseType='json';xhr.open(ajaxData.type,ajaxData.url);if(ajaxData.type=='POST'){xhr.setRequestHeader('Content-Type','application/x-www-form-urlencoded; charset=utf-8');xhr.send(ajaxData.data)}else{xhr.send()}xhr.onreadystatechange=function(){i"
    "f(xhr.readyState==4){if(xhr.status==200){ajaxData.success(xhr.response)}else{ajaxData.error()}if(ajaxData.url=='/get_status'){lt=setTimeout(get_status,intervalTime)}}}}function toast(msg,duration,isok){var m=id('alert');m.innerHTML=msg;m.style.cssText=isok?'color: #3c763d;background-color: #dff0d8;border-color: #d6e9c6;':'color: #a94442; background-color: #f2dede; border-color: #ebccd1;';m.style.display='block';clearTimeout(timer);timer=setTimeout(function(){var d=0.5;m.style.webkitTransition='-webkit-transform '+d+'s ease-in, opacity '+d+'s ease-in';m.style.opacity='0';setTimeout(function(){m.style.display='none'},d*1000)},duration)}function postupdate(the){var form=new FormData();form.append('file',document.getElementsByName('update')[0].files[0]);var xhr=new XMLHttpRequest();xhr.responseType='json';xhr.open('POST',the.getAttribute('action'),true);xhr.onreadystatechange=function(){if(xhr.readyState==4){if(xhr.status==200){if(xhr.response.msg){toast(xhr.response.msg,xhr.response.code?"
    "5000:9000,xhr.response.code)}if(xhr.response.code){setTimeout('location.reload();',7000)}}else{toast('<strong>Oh snap!</strong> 请求出错！',9000,false)}}};xhr.upload.onprogress=function(ev){if(ev.lengthComputable){toast('已上传'+Math.floor(100*ev.loaded/ev.total)+'%，请耐心等待，稍后将弹出升级情况。',16000,1)}};xhr.send(form)}function postform(the){ajaxPost(the.getAttribute('action'),serialize(the));return false}function getRadioValue(radioName){var radios=document.getElementsByName(radioName);for(var i=0;i<radios.length;i++){var radio=radios.item(i);if(radio.checked){return radio.value}}return undefined}function setRadioValue(radioName,value){var radios=document.getElementsByName(radioName);for(var i=0;i<radios.length;i++){var radio=radios.item(i);if(radio.value==value){radio.checked=true;return}}}function ajaxPost(url,data,callback){ajax({type:'POST',url:url,dataType:'json',data:data,success:function(data){if(typeof(callback)=='function'){if(callback(data)===true){return}}if(data.msg){toast(data.msg,data.cod"
    "e?5000:9000,data.code)}if(data.data){setData(data.data)}},error:function(){toast('<strong>Oh snap!</strong> 请求出错！',9000,false)}})}function get_status(){clearTimeout(lt);ajaxPost('/get_status','i='+logIndex)}window.addEventListener('load',get_status);function setData(data){for(var key in data){if(typeof(setDataSub)=='function'){var result=setDataSub(data,key);if(result){continue}}var v=data[key];if(key=='discovery'){id('discovery').innerHTML=v==1?'已启动':'未启动';id('discovery_btn').setAttribute('class',v==1?'btn-danger':'btn-info');id('discovery_btn').innerHTML=v==1?'关闭MQTT自动发现':'打开MQTT自动发现'}else if(key=='logindex'){logIndex=v}else if(key=='log'){if(v){id('log').value+=v;id('log').scrollTop=99999}}else if(key=='ip'){if(v&&v!=window.location.hostname){toast('连接WiFi成功，IP地址：'+v,5000,1);window.setTimeout('location.href=\\\\'http://'+v+'\\\\'',5000)}}else{if(id(key)){id(key).innerHTML=v}else{console.log(key)}}}}</script></head><body><div id='body'><div id='alert' class='alert'></div><h1 style='text-"
    "align:center'>";

const char HTTP_HTML_TAB2_WIFI[] PROGMEM = 
    "<div id='tab2'><form method='post' action='/wifi' onsubmit='postform(this);return false'><table class='gridtable'><thead><tr><th>WiFi名称</th><th>信号</th></tr></thead><tbody><tr id='clusss'><td>WiFi名称</td><td><input type='text' id='wifi_ssid' name='wifi_ssid'></td></tr><tr><td>WiFi密码</td><td><input type='text' name='wifi_password'></td></tr><tr><td colspan='2'><button type='submit' class='btn-info'>连接WiFi</button></td></tr><tr><td colspan='2'><button type='button' class='btn-danger' onclick='scanWifi()'>搜索WiFi</button></td></tr></tbody></table></form><script type='text/javascript'>function clickwifi(t){id('wifi_ssid').value=t.value}function scanWifi(){ajaxPost('scan_wifi','',function(data){if(data.code==1){if(data.data.list.length==0){scanWifi();return;}var trs=document.getElementsByClassName('addwifi');for(var i=trs.length-1;i>=0;i--){trs[i].remove()}for(var a in data.data.list){var w=data.data.list[a];var tr=document.createElement(\\\"tr\\\");var td=document.createElement(\\\"td\\\");tr.setAttr"
    "ibute('class','addwifi');td.innerHTML=\\\"<label class='bui-radios-label'><input type='radio' name='wifi' onclick='clickwifi(this)' value='\\\"+w.name+\\\"'/><i class='bui-radios'></i> \\\"+w.name+(w.type==7?' [开放]':'')+\\\"</label>\\\";tr.appendChild(td);td=document.createElement(\\\"td\\\");td.innerHTML=w.rssi+'dBm '+w.quality+'%';tr.appendChild(td);var oldEle=id('clusss');oldEle.parentNode.insertBefore(tr,oldEle)}}else{toast(data.msg,data.code?5000:9000,data.code)}})}</script>";

const char HTTP_HTML_TAB2_DHCP_START[] PROGMEM = 
    "<form method='post' action='/dhcp' onsubmit='postform(this);return false'><table class='gridtable'><thead><tr><th colspan='2'>WIFI高级设置</th></tr></thead><tbody><tr><td>DHCP</td><td><label class='bui-radios-label'><input type='radio' name='dhcp' value='1' onchange='dhcponchange(this)'/><i class='bui-radios'></i> DHCP</label>&nbsp;&nbsp;&nbsp;&nbsp;<label class='bui-radios-label'><input type='radio' name='dhcp' value='2' onchange='dhcponchange(this)'/><i class='bui-radios'></i> 静态IP</label></td></tr>";

const char HTTP_HTML_TAB2_DHCP_END[] PROGMEM = 
    "<tr><td colspan='2'><button type='submit' class='btn-info'>保存</button></td></tr></tbody></table></form><script type='text/javascript'>function dhcponchange(the){var v=getRadioValue('dhcp');var dom=document.getElementsByClassName('dhcp_hide');for(var i=0;i<dom.length;i++){dom[i].style.display=v==2?'':'none'}}</script>";

const char HTTP_HTML_TAB2_MQTT_START[] PROGMEM = 
    "<form method='post' action='/mqtt' onsubmit='postform(this);return false'><table class='gridtable'><thead><tr><th colspan='2'>MQTT设置</th></tr></thead><tbody>";

const char HTTP_HTML_TAB2_MQTT_RETAIN[] PROGMEM = 
    "<tr><td>保留(Retain)</td><td><label class='bui-radios-label'><input type='radio' name='retain' value='0'/><i class='bui-radios'></i> 关闭</label>&nbsp;&nbsp;&nbsp;&nbsp;<label class='bui-radios-label'><input type='radio' name='retain' value='1'/><i class='bui-radios'></i> 开启</label><br>除非你知道它是干嘛的。</td></tr>";

const char HTTP_HTML_TAB2_DISCOVERY_START[] PROGMEM = 
    "<form method='post' action='/discovery' onsubmit='postform(this);return false'><table class='gridtable'><thead><tr><th colspan='2'>MQTT自动发现</th></tr></thead><tbody>";

const char HTTP_HTML_TAB2_DISCOVERY_END[] PROGMEM = 
    "<tr><td colspan='2'><button type='submit' class='btn-info' id='discovery_btn'>打开MQTT自动发现</button></td></tr></tbody></table></form>";

const char HTTP_HTML_TAB2_END[] PROGMEM = 
    "</div>";

const char HTTP_HTML_TAB3_START[] PROGMEM = 
    "<div id='tab3'>";

const char HTTP_HTML_TAB3_MODULE_START[] PROGMEM = 
    "<form method='post' action='/module_setting' onsubmit='postform(this);return false'><table class='gridtable'><thead><tr><th colspan='2'>模块设置</th></tr></thead><tbody>";

const char HTTP_HTML_TAB3_MODULE_END[] PROGMEM = 
    "<tr><td colspan='2'><button type='submit' class='btn-info'>设置</button></td></tr></tbody></table></form><div><button type='button' class='btn-danger' style='margin-top: 10px' onclick=\\\"javascript:if(confirm('确定要重启模块？')){ajaxPost('/operate', 'd=1');}\\\">重启模块</button><button type='button' class='btn-danger' style='margin-top: 10px' onclick=\\\"javascript:if(confirm('确定要重置模块？')){ajaxPost('/operate', 'd=2');}\\\">重置模块</button></div></div>";

const char HTTP_HTML_NAV_START[] PROGMEM =
    "</h1><div id='nav'><button onclick='tab(1)'class='active'>状态</button><button onclick='tab(2)'>联网</button><button onclick='tab(3)'>控制</button><button onclick='tab(4)'>关于</button>";

const char HTTP_HTML_NAV_END[] PROGMEM =
    "</div>";

const char HTTP_HTML_LOG_OUTPUT_START[] PROGMEM =
    "<tr><td>日志输出</td><td><label class='bui-radios-label'><input type='checkbox' name='log_serial' value='1'/><i class='bui-radios' style='border-radius:20%'></i> Serial</label>&nbsp;&nbsp;&nbsp;&nbsp;<label class='bui-radios-label'><input type='checkbox' name='log_serial1' value='1'/><i class='bui-radios' style='border-radius:20%'></i> Serial1</label>&nbsp;&nbsp;&nbsp;&nbsp;";

const char HTTP_HTML_LOG_OUTPUT_END[] PROGMEM =
    "</td></tr>";

const char HTTP_HTML_TAB4_START[] PROGMEM =
    "<div id='tab4'><table class='gridtable'><thead><tr><th colspan='2'>硬件参数</th></tr></thead><tbody>";

const char HTTP_HTML_TAB4_OTA[] PROGMEM =
    "<form method='POST' action='/update' enctype='multipart/form-data' onsubmit='postupdate(this);return false'>"
    "<tr><td colspan='2'><a class='file'><input type='file' name='update'>选择文件</a></td></tr>"
    "<tr><td colspan='2'><button type='submit' class='btn-info'>升级</button><br>"
    "</form>"
    "<tr><td colspan='2' style='text-align:center'>OTA更新</td></tr>"
    "<form method='POST' action='/ota' onsubmit='postform(this);return false'>"
    "<tr><td>OTA地址</td><td><input type='text' name='ota_url' value='" OTA_URL "' style='width:98%'></td></tr>"
    "<tr><td colspan='2'><button type='submit' class='btn-success' onclick=\"return confirm('确定要OTA更新？')\">OTA更新</button></td></tr>"
    "</form>"
    "</tbody></table>"
    "</div>";

const char HTTP_HTML_TAB5[] PROGMEM = 
    "<div id='tab5'><div style='display:inline-block;color:#000000;min-width:340px;position:absolute;left:1%;margin-top:20px;width:99%'><textarea readonly id='log' cols='340' wrap='off' style='resize:none;width:98%;height:600px;padding:5px;overflow:auto;background:#ffffff;color:#000000;'></textarea></div></div>";

const char HTTP_HTML_FOOTER[] PROGMEM = 
    "</div><div style='text-align:center;margin-top:20px'>开发者：<a href='https://github.com/qlwz' target='_blank' style='color:#333;text-decoration:none'>情留メ蚊子</a>&nbsp;&nbsp;&nbsp;<a href='https://bbs.iobroker.cn' target='_blank' style='color:#333;text-decoration:none'>来和大神一起玩智能家居</a></div><div></body></html>";

const char HTTP_HTML_SCRIPT_DISCOVERY_BTN[] PROGMEM = 
    "id('discovery_btn').setAttribute('class', 'btn-danger');id('discovery_btn').innerHTML='关闭MQTT自动发现';";

const char HTTP_HTML_SCRIPT_LOG_SERIAL[] PROGMEM = 
    "setRadioValue('log_serial', '1');";

const char HTTP_HTML_SCRIPT_LOG_SYSLOG[] PROGMEM = 
    "setRadioValue('log_syslog', '1');";

const char HTTP_HTML_SCRIPT_LOG_WEB[] PROGMEM = 
    "setRadioValue('log_web', '1');";

const char HTTP_HTML_SCRIPT_LOG_SERIAL1[] PROGMEM = 
    "setRadioValue('log_serial1', '1');";

const char HTTP_HTML_SCRIPT_END[] PROGMEM = 
    "</script>";

