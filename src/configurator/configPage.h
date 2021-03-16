#ifndef __IOTAAP_OS_CONFIG_PAGE_H__
#define __IOTAAP_OS_CONFIG_PAGE_H__

#include <Arduino.h>

/* Html open/close */
const char configPageHtmlOpen[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
)rawliteral";
const char configPageHtmlClose[] PROGMEM = R"rawliteral(
</html>
)rawliteral";

/* Head open/close */
const char configPageHeadOpen[] PROGMEM = R"rawliteral(
<head>
)rawliteral";
const char configPageHeadClose[] PROGMEM = R"rawliteral(
</head>
)rawliteral";

/* Style open/close */
const char configPageStyleOpen[] PROGMEM = R"rawliteral(
<style type="text/css">
)rawliteral";
const char configPageStyleClose[] PROGMEM = R"rawliteral(
</style>
)rawliteral";

/* Body open/close */
const char configPageHtmlBodyOpen[] PROGMEM = R"rawliteral(
<body>
)rawliteral";
const char configPageHtmlBodyClose[] PROGMEM = R"rawliteral(
</body>
)rawliteral";

/* Form open/close */
const char configPageHtmlFormOpen[] PROGMEM = R"rawliteral(
<form action="/submit-config" method="POST">
)rawliteral";
const char configPageHtmlFormClose[] PROGMEM = R"rawliteral(
</form>
)rawliteral";

/* Title */
const char configPageTitleAdd[] PROGMEM = R"rawliteral(
<title>IoTaaP OS</title>
)rawliteral";

/* Title */
const char configPagePassToggleScript[] PROGMEM = R"rawliteral(
<script>
function toggle(id) {
var x = document.getElementById(id);
if (x.type === "password") { x.type = "text"; }
else { x.type = "password";}}
</script>
)rawliteral";

/******************/
/* Style elements */
/******************/
const char configPageStyleBody[] PROGMEM = R"rawliteral(
body {background: #3498db;font-family: sans-serif;font-size: 14px;color: #777;}
)rawliteral";
const char configPageStyleForm[] PROGMEM = R"rawliteral(
form {background: #fff;max-width: 400px;margin: 75px auto;
padding: 30px;border-radius: 5px;text-align: center;}
)rawliteral";
const char configPageStyleInput[] PROGMEM = R"rawliteral(
input {width: 100%;height: 44px;
border-radius: 4px;background: #f1f1f1;border: 0;padding: 0 15px;
margin: 5px auto;font-size: 15px;box-sizing: border-box;}
)rawliteral";
const char configPageStyleTextArea[] PROGMEM = R"rawliteral(
textarea {width: 100%;height: 150px;padding: 12px 15px;
box-sizing: border-box;border: 0;border-radius: 4px;background-color: #f1f1f1;
font-size: 15px;resize: none;font-family: sans-serif;margin: 5px auto;}
)rawliteral";
const char configPageStyleBtn[] PROGMEM = R"rawliteral(
.btn {background: #3498db;color: #fff;cursor: pointer;}
)rawliteral";
const char configPageStyleContainer[] PROGMEM = R"rawliteral(
.container {display: grid;width: 100%;}
)rawliteral";
const char configPageStyleLabel[] PROGMEM = R"rawliteral(
label {font-weight: bold;display: inline;width: 1px;white-space: nowrap;}
)rawliteral";
const char configPageStyleSpan[] PROGMEM = R"rawliteral(
span {display: table-cell;padding: 0 0 0 5px;}
)rawliteral";
const char configPageStyleCheckboxCont[] PROGMEM = R"rawliteral(
.checkboxContainer {display: flex;flex-wrap: wrap;}
.checkboxContainer input {width: 25px;margin-right: 50px;margin-left: 25px;height: 25px;margin-top: 25px;}
.checkboxContainer span {width: auto;margin-top: 30px;margin-bottom: 20px;}
)rawliteral";
const char configPageStylePassShowHide[] PROGMEM = R"rawliteral(
.passwordShow {float: right;margin-left: 340px;margin-top: -58px;position: relative;z-index: 2;}
.showHide {margin-top: -20px;margin-left: 345px;font-size: 8px;}
input[type=checkbox] {transform: scale(0.4);}
)rawliteral";
const char configPageStyleRedH1[] PROGMEM = R"rawliteral(
h1 {color: red;}
)rawliteral";
const char configPageStyleGreenH1[] PROGMEM = R"rawliteral(
h1 {color: red;}
)rawliteral";
const char configPageStyleDiv[] PROGMEM = R"rawliteral(
div {background: #fff;max-width: 500px;margin: 75px auto;
padding: 30px;border-radius: 5px;text-align: center;}
)rawliteral";


/****************/
/* Div elements */
/****************/
const char configPageDivLogo[] PROGMEM = R"rawliteral(
<div><svg width="78pt" height="25pt" viewBox="0 0 78 25" version="1.1"><defs><image id="image5" width="179" height="57" xlink:href="data:image/png;base64,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"/></defs><g id="surface2"><use xlink:href="#image5" transform="matrix(0.435754,0,0,0.438596,0,0)"/></g></svg></div>
)rawliteral";
const char configPageDivInvalidData[] PROGMEM = R"rawliteral(
<div><form action="/"><h1>Invalid data!</h1><input type=submit class=btn value="Go back"></form></div>
)rawliteral";
const char configPageDivNotFound[] PROGMEM = R"rawliteral(
<div><form action="/"><h1>404 - Not found!</h1><input type=submit class=btn value="Go back"></form></div>
)rawliteral";
const char configPageDivSuccess[] PROGMEM = R"rawliteral(
<div><h1>Success!</h1></div>
)rawliteral";


const char configDivInput_a[] PROGMEM = R"rawliteral(<div class="container"><label for=")rawliteral";
const char configDivInput_b[] PROGMEM = R"rawliteral(">)rawliteral";
const char configNewLiner[] PROGMEM = R"rawliteral(
)rawliteral";

const char *configDivInputText_a = configDivInput_a;
const char *configDivInputText_b = configDivInput_b;
const char configDivInputText_c[] PROGMEM = R"rawliteral(:</label><span><input name=")rawliteral";
const char configDivInputText_d[] PROGMEM = R"rawliteral(" type="text" value=")rawliteral";
const char configDivInputText_e[] PROGMEM = R"rawliteral("></span></div>)rawliteral";

const char *configDivInputPass_a = configDivInput_a;
const char *configDivInputPass_b = configDivInput_b;
const char configDivInputPass_c[] PROGMEM = R"rawliteral(:</label><span><input id=")rawliteral";
const char configDivInputPass_d[] PROGMEM = R"rawliteral(" name=")rawliteral";
const char configDivInputPass_e[] PROGMEM = R"rawliteral(" type="password" value=")rawliteral";
const char configDivInputPass_f[] PROGMEM = R"rawliteral("></span><span class="passwordShow"><input type="checkbox" onclick="toggle(')rawliteral";
const char configDivInputPass_g[] PROGMEM = R"rawliteral(')"></span><a class="showHide">Show/Hide</a></div>)rawliteral";



#define configDivInputText( desc, tag, val)             \
                                    configDivInputText_a +  \
                                    String(tag) +       \
                                    configDivInputText_b +  \
                                    String(desc) +      \
                                    configDivInputText_c +  \
                                    String(tag) +       \
                                    configDivInputText_d +  \
                                    String(val) +       \
                                    configDivInputText_e + \
                                    configNewLiner

#define configDivInputPass( desc, tag, val)             \
                                    configDivInputPass_a +  \
                                    String(tag) +       \
                                    configDivInputPass_b +  \
                                    String(desc) +      \
                                    configDivInputPass_c +  \
                                    String(tag) +       \
                                    configDivInputPass_d +  \
                                    String(tag) +       \
                                    configDivInputPass_e +  \
                                    String(val) +       \
                                    configDivInputPass_f +  \
                                    String(tag) +       \
                                    configDivInputPass_g + \
                                    configNewLiner


const char configDivRadioBool_a[] PROGMEM = R"rawliteral(<div class="checkboxContainer"><label for=")rawliteral";
const char configDivRadioBool_b[] PROGMEM = R"rawliteral(">)rawliteral";
const char configDivRadioBool_c[] PROGMEM = R"rawliteral(:</label><span class="radio-text">Disable</span><input type="radio" name=")rawliteral";
const char configDivRadioBool_d[] PROGMEM = R"rawliteral(" value="false" class="radio-button")rawliteral";
const char configDivRadioBool_e[] PROGMEM = R"rawliteral(><span class="radio-text">Enable</span><input type="radio" name=")rawliteral";
const char configDivRadioBool_f[] PROGMEM = R"rawliteral(" value="true" class="radio-button")rawliteral";
const char configDivRadioBool_g[] PROGMEM = R"rawliteral(></div>)rawliteral";
const char configDivRadioCh[] PROGMEM = R"rawliteral( checked="checked")rawliteral";

#define configDivInputRadioBool( desc, tag, val)                    \
                                    configDivRadioBool_a +          \
                                    String(tag) +                   \
                                    configDivRadioBool_b +          \
                                    String(desc) +                  \
                                    configDivRadioBool_c +          \
                                    String(tag) +                   \
                                    configDivRadioBool_d +          \
                                    (val ? "" : configDivRadioCh) + \
                                    configDivRadioBool_e +          \
                                    String(tag) +                   \
                                    configDivRadioBool_f +          \
                                    (val ? configDivRadioCh : "") + \
                                    configDivRadioBool_g + \
                                    configNewLiner


const char *configDivTextArea_a = configDivInput_a;
const char *configDivTextArea_b = configDivInput_b;
const char configDivTextArea_c[] PROGMEM = R"rawliteral(:</label><span><textarea name=)rawliteral";
const char configDivTextArea_d[] PROGMEM = R"rawliteral(>)rawliteral";
const char configDivTextArea_e[] PROGMEM = R"rawliteral(</textarea></span></div>)rawliteral";

#define configDivInputTextArea( desc, tag, val)             \
                                    configDivTextArea_a +   \
                                    String(tag) +           \
                                    configDivTextArea_b +   \
                                    String(desc) +          \
                                    configDivTextArea_c +   \
                                    String(tag) +           \
                                    configDivTextArea_d +   \
                                    String(val) +           \
                                    configDivTextArea_e + \
                                    configNewLiner

/* Submit button */
const char configPageSubmitButton[] PROGMEM = R"rawliteral(<input type=submit class=btn value=Confirm>)rawliteral";


/* Page generator */
#define configPageHtmlStart     configPageHtmlOpen + \
                                configPageHeadOpen + \
                                configPageTitleAdd + \
                                configPagePassToggleScript + \
                                configPageStyleOpen + \
                                configPageStyleBody + \
                                configPageStyleForm + \
                                configPageStyleInput + \
                                configPageStyleTextArea + \
                                configPageStyleBtn + \
                                configPageStyleContainer + \
                                configPageStyleLabel + \
                                configPageStyleSpan + \
                                configPageStyleCheckboxCont + \
                                configPageStylePassShowHide + \
                                configPageStyleClose + \
                                configPageHeadClose + \
                                configPageHtmlBodyOpen + \
                                configPageHtmlFormOpen + \
                                configPageDivLogo

#define configPageHtmlEnd       configPageSubmitButton + \
                                configPageHtmlFormClose + \
                                configPageHtmlBodyClose + \
                                configPageHtmlClose


#define errorPageHtml           configPageHtmlOpen + \
                                configPageHeadOpen + \
                                configPageTitleAdd + \
                                configPageStyleOpen + \
                                configPageStyleBody + \
                                configPageStyleForm + \
                                configPageStyleInput + \
                                configPageStyleBtn + \
                                configPageStyleRedH1 + \
                                configPageStyleClose + \
                                configPageHeadClose + \
                                configPageHtmlBodyOpen + \
                                configPageDivInvalidData + \
                                configPageHtmlBodyClose + \
                                configPageHtmlClose

#define notFoundPageHtml        configPageHtmlOpen + \
                                configPageHeadOpen + \
                                configPageTitleAdd + \
                                configPageStyleOpen + \
                                configPageStyleBody + \
                                configPageStyleForm + \
                                configPageStyleInput + \
                                configPageStyleBtn + \
                                configPageStyleRedH1 + \
                                configPageStyleClose + \
                                configPageHeadClose + \
                                configPageHtmlBodyOpen + \
                                configPageDivNotFound + \
                                configPageHtmlBodyClose + \
                                configPageHtmlClose

#define successPageHtml         configPageHtmlOpen + \
                                configPageHeadOpen + \
                                configPageTitleAdd + \
                                configPageStyleOpen + \
                                configPageStyleBody + \
                                configPageStyleDiv + \
                                configPageStyleGreenH1 + \
                                configPageStyleClose + \
                                configPageHeadClose + \
                                configPageHtmlBodyOpen + \
                                configPageDivSuccess + \
                                configPageHtmlBodyClose + \
                                configPageHtmlClose

#endif
