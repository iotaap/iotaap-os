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

/* Scripting */
const char configPageScriptOpen[] PROGMEM = R"rawliteral(<script>)rawliteral";
const char configPageScriptClose[] PROGMEM = R"rawliteral(</script>)rawliteral";
const char configPagePassToggleScript[] PROGMEM = R"rawliteral(
function toggle(id) {
var x = document.getElementById(id);
if (x.type === "password") { x.type = "text"; }
else { x.type = "password";}}
function showDiv(ida, idb){
document.getElementById(ida).style.display = "block";
document.getElementById(idb).style.display = "block";}
function hideDiv(ida, idb){
document.getElementById(ida).style.display= "none";
document.getElementById(idb).style.display= "none";}
)rawliteral";

/******************/
/* Style elements */
/******************/
const char configPageStyleBody[] PROGMEM = R"rawliteral(
body {background: #00a99d;font-family: sans-serif;font-size: 14px;color: #777;}
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
.btn {background: #00a99d;color: #fff;cursor: pointer;}
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
h1 {color: green;}
)rawliteral";
const char configPageStyleDiv[] PROGMEM = R"rawliteral(
div {background: #fff;max-width: 500px;margin: 75px auto;
padding: 30px;border-radius: 5px;text-align: center;}
)rawliteral";


/****************/
/* Div elements */
/****************/
const char configPageDivLogo[] PROGMEM = R"rawliteral(
<div><svg width="176pt" height="70pt" viewBox="0 0 78 25" version="1.1"><defs><image id="image5" width="179" height="57" xlink:href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAI8AAAA8CAYAAABfPelIAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAACeFJREFUeNrsXUtyKjcU7Ud5HmYZBq/AeAWGFRgmmWRgurKABysAVgAsIAUeZJKJeStwewXGK3BnmFF4K0h0qXPxsaJGaj52AN2qLmxorrqvjs79SC2SH//4rZZ8sJg2q0mUo5cLc3RMZ96Z13tzjP/6+dflAUHTMC99/NuM5j9uqeC1hk59NR08OARozPFo/pSjEc1+OswzN8dPwkDmEHfSBxOlhoWyPbinqTla9HZujkk0/fHLF+poYZ+R1dFjcwy3cWVGXwvAqRJoRNcsmv3EwGPFJVO4MpEFWGhRAjgCwi69NTx0PBXlfwAecjcjuDIR6fSmD0D43gPFNaWBF+XIwUNg6ICFFEC9IrcD4EhAXMdbM5wf2eYcwQNQ1AEKjV2adiDtAo45J43mPY9UvVDgcppgHpEHAIqFgZNG4JwJeEIqzBaAViyjVWLzOiXgjEOyqc+oakc5gNtCUVDqOu2AgLiFgFhEXNc9xUReV0V1n6o5N1aYT8RtCRM8m87tehhojrQ7QUY1payqFxA7PSfv60hRjhw8GcUzI7ihTQAa4DsJZWHppqwKjPWYvNWOnqLpTwA8yJwuwR4iHR+A4K5U8k3ujlxdFUBrA4BRTsFtgTUkBpkRgEYbvtenv+uoShe5Kq4TNeH6opxCwOzocJ75Tu3syVE4FEbJ7AAYwfErMU4zVppPM2BmaZMLGznS6j5lWxokNxy1H54UbUfgnAF44MJSYpUpsUmDgt4JWEkD5a9WnKNZ1XDXpR1Rjod5tCg4IVZRN3ZHQbLGLnpei5aXary0iMHxmYGHUvLcclXKJhz0akxUBYA6xE69aOLTlQvP50O4rQYKiFU7VTcgy81nwlQS89xQsJ3t01390v6qoMx/f5jMAr/ToutKMBheBPxGRx7wfWlP2k3M+YMS11rDQLvCNS+p3cUONqiSXo0xn3Bfonu5Z90Luu6lN9tyZF9/E2jUZV1a5wzATksrSJ7vETyaBWbmRpoBoBkRA7pEANjbZHCjR9p7BHi+BIKmn7ytg3KJdHRq9GUl73+AuLLoyRO5j0kZkO+quxKg2x7lrpvOyHWtGjtAPWeBdhYeQwhTPhBwcrjZsXXt0sHP5vz6nsCt0y8dyy5DtK9MJ9f1iA4LYgRzPAOUVbLFGH2zINv3ZZCBRXbRPQvRfRHQhrgonvN6ccRHmWEffmvvhUCD+l6AMXj14wLMkjnouQuDaUde7kj59pqn1bJbW6fFiNIZS3PO2ONKeLnLHPeUOxhvCmZuYPCETDyH6ta17e90h67nsRmgiI5Vvu0bPNJB4kaKmAIupkts2XS5BulQ0G9Ko+phx8vTmpaA5Vr0u8Bo3pMOuuY6mof5+tS54urarlhN3oMrVy8hduoGuCrV3fPoblu6B6Fuy3ZVIeA5REFwhJEy2vC5XkfPxyQIuodkkMYOgTx38MLTrk4F/ac+5hjxCoB5SJJgzkmpr/oeRuuT7nFJ3V9FRyh4FlYR0XdOnnygwNDagcNQFwQGyq0aVlm51fsHs4S0u6T6WKcgRuGlK2VWZq4ZFW4y8ejubaNbdISC53uJcz5jGqJhMUoZ0Q7fNnBu2eWLQOHrdLWt5YWsTDwG17Pw3BPrzrfUfVUpecMhDX3G0xK1DZmgT15CwGNG8T9yhLBziY6wr5+lSnWcZMt+uvHYaxvda2CWBU98jOaw4D8quSh5fgi1f8b2KUvbfZWQq0DmaAbYJpj5EKeF3NPVAcF4s4XudfU5lHl+KHFO/RPAk1m1lG1iFl+WlBVUhedW4Fy23SJRl9IKLfo5kgdvrLiD7pdKWcbZsDETn/OhNIz0OPelqA5jdGmU3m/Z/DfqiFZgu9WA65xb9Z4yJY0yso1uYcV5afBsQHW1pHvbt2jKWccUha8D62SMrOxck5XdKXCnvukOqhpXAwJqrb90UU/y3dMgKf90SqjuLumWea6lFzxYIRgCjPqOvnRX9plT+tvBPEzN1Xkw8jONol2fcG3TAHosqu7SRGvo4BqSOxVgjlxuRu7THA8lWYQlRPd6jZZOkIYEzHcFASYDrOHw571PAFBqbnQFHgTPr+Z/nVD9jmtv0GBYVXvL1DqK3KZpJ4WBVzuMmP/7aPcF8WDDAs0s2Tz7viomGj1NAlwXTJFRTHRjJQp6v92ASx/TdYXqXicOIW7LpkFXRtOwMoRa0RMVHwEgAHdJjKgToS0CzmqeaZf1NQ731UzerzBood0uAUeA2sZ1hujV6YyxZe8+Dra9VNevk7CiboLzQnQrCza5YHnhcVkdCijHMIIAo2ZNQdxShzSStz0Osz3i4j55W/jkM/jYjKAZOu+G7kEXZc0C2SZP3ua/QgP3JuKeluW+5bMnawpjaGeLGwDUM3onpJd3XBO78IKtjD7bZMssQLfaq9xiMAOSVxhe1iJf08Iw2XdnTJnVK76io0kD1mZc/H66UtkAHE5jh1b6yDPBHSu95IVPo2jiMwMP2GT9fBatCvzmiGk0oJZdMpaYdVew1X2bJ0Q5IfDQvoJVO40FiNbFOCsm4kXxM/K7I8cDgVFOlHlGlBn0HGtzeIuVEbGTHduklPE8niuAjnkjK9+1V6yTpxTDzFy7fOG9nNLRxJWRAHTvimdnCqBOQEbr/exAO/N3Qq/d1f6F5aoaBJxNdYghZVSLoowKC+PT5G2NrwCofY4ZGO+KJmmyPFSJkX2H1xmzPDpWPtMBq4Nbvn8vIYSlU/qhhwFqV5pf7Cd3AYYb85qjnzoouYiutmvFKHQ3NNOu0I5docDhuo4Gz4X0hhtPLQY6xyC6gZqJDLwrsTvAsgKSHR7Abk8W+8tz/22qH3VIp/ZDFWCR85Z4dfXLAPozqoFNkBS5GEmq83cKHHVbLU7JA/YV5AkyBcTGpw9ggGuKgW7PEDw1uv/7JGxVpisUEPlOts9Jp80Wf5ZQf0c4cE3a1m19GvPIBTR9mxKApdYTZAnNZAdsRyfnXyYHeKbrSGROboHrZEsZkAXsfYUdR4pkRjpvt9gw/Yri0CWOooF9j2udqlut4KaufXEIbeatDbVBYUqrHV8AhjpQOzmvDRBmVgKx5JgCNswS9xLflJhlVqBT5/F6NKhn1ndmBdfGc4D6d7vge5pATRBrLb6E3L1jF/hrfRjQsft7Gn/Z5nzrPD7gvPshEtrPcL3uJFaVz0N8E6P2b2alZX64JP6MwJmCx/rNLN1reR5Qy2AALeDf82jqMwAPJjx5iiJPSmxIWfBbXUOuD0Q5MfDQTHrHSi/TLX8mUlfv8cKiNK7vOSHwFIAmyE0FAKiWvO0boyLgmcTNvE8j2+rYLsYcl/voXIl1sLl3m+oVAqSv0fTHLxcEGpnXOMiPyAKIcxQRI3BORT5jvUn8sbbTkH8FGACAZfOVyIs0KQAAAABJRU5ErkJggg=="/></defs><g id="surface2"><use xlink:href="#image5" transform="matrix(0.435754,0,0,0.438596,0,0)"/></g></svg></div>
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
const char configDivInputText_c1[]  PROGMEM = R"rawliteral(:</label><span)rawliteral";
const char configDivInputText_c2[] PROGMEM = R"rawliteral(><input name=")rawliteral";
const char configDivInputText_d[] PROGMEM = R"rawliteral(" type="text" value=")rawliteral";
const char configDivInputText_e[] PROGMEM = R"rawliteral("></span></div>)rawliteral";

const char *configDivInputPass_a = configDivInput_a;
const char *configDivInputPass_b = configDivInput_b;
const char configDivInputPass_c[] PROGMEM = R"rawliteral(:</label><span><input id=")rawliteral";
const char configDivInputPass_d[] PROGMEM = R"rawliteral(" name=")rawliteral";
const char configDivInputPass_e[] PROGMEM = R"rawliteral(" type="password" value=")rawliteral";
const char configDivInputPass_f[] PROGMEM = R"rawliteral("></span><span class="passwordShow"><input type="checkbox" onclick="toggle(')rawliteral";
const char configDivInputPass_g[] PROGMEM = R"rawliteral(')"></span><a class="showHide">Show/Hide</a></div>)rawliteral";


const char configDivInputBlocked_a[] PROGMEM = R"rawliteral(" id=")rawliteral";
const char configDivInputBlocked_b[] PROGMEM = R"rawliteral( id=")rawliteral";
const char configDivInputBlocked_c[] PROGMEM = R"rawliteral(1")rawliteral";

#define configDivInputBlocked_1( blocked)                           \
                                    configDivInputBlocked_a +       \
                                    String(blocked)
#define configDivInputBlocked_2( blocked)                           \
                                    configDivInputBlocked_b +       \
                                    String(blocked) +               \
                                    configDivInputBlocked_c

#define configDivInputTextBlocked( desc, tag, blocked, val)             \
                                    configDivInputText_a +  \
                                    String(tag) +       \
                                    (blocked ? configDivInputBlocked_1(blocked) : "") + \
                                    configDivInputText_b +  \
                                    String(desc) +      \
                                    configDivInputText_c1 +  \
                                    (blocked ? configDivInputBlocked_2(blocked) : "") + \
                                    configDivInputText_c2 +  \
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

const char configDivRadioBoolBlock_a_hide[] PROGMEM = R"rawliteral( onclick="hideDiv(')rawliteral";
const char configDivRadioBoolBlock_a_show[] PROGMEM = R"rawliteral( onclick="showDiv(')rawliteral";
const char configDivRadioBoolBlock_b[] PROGMEM = R"rawliteral(',')rawliteral";
const char configDivRadioBoolBlock_c[] PROGMEM = R"rawliteral(1')")rawliteral";

#define configDivRadioBoolBlock_hide( blocking)                          \
                                    configDivRadioBoolBlock_a_hide +     \
                                    String(blocking) +                   \
                                    configDivRadioBoolBlock_b +          \
                                    String(blocking) +                   \
                                    configDivRadioBoolBlock_c
#define configDivRadioBoolBlock_show( blocking)                          \
                                    configDivRadioBoolBlock_a_show +     \
                                    String(blocking) +                   \
                                    configDivRadioBoolBlock_b +          \
                                    String(blocking) +                   \
                                    configDivRadioBoolBlock_c


#define configDivInputRadioBoolBlocking( desc, tag, blocking, val)  \
                                    configDivRadioBool_a +          \
                                    String(tag) +                   \
                                    configDivRadioBool_b +          \
                                    String(desc) +                  \
                                    configDivRadioBool_c +          \
                                    String(tag) +                   \
                                    configDivRadioBool_d +          \
                                    (blocking ? configDivRadioBoolBlock_hide(blocking) : "") + \
                                    (val ? "" : configDivRadioCh) + \
                                    configDivRadioBool_e +          \
                                    String(tag) +                   \
                                    configDivRadioBool_f +          \
                                    (blocking ? configDivRadioBoolBlock_show(blocking) : "") + \
                                    (val ? configDivRadioCh : "") + \
                                    configDivRadioBool_g + \
                                    configNewLiner



const char *configDivTextArea_a = configDivInput_a;
const char *configDivTextArea_b = configDivInput_b;
const char configDivTextArea_c1[] PROGMEM = R"rawliteral(:</label><span)rawliteral";
const char configDivTextArea_c2[] PROGMEM = R"rawliteral(><textarea name=)rawliteral";
const char configDivTextArea_d[] PROGMEM = R"rawliteral(>)rawliteral";
const char configDivTextArea_e[] PROGMEM = R"rawliteral(</textarea></span></div>)rawliteral";

#define configDivInputTextAreaBlocked( desc, tag, blocked, val)             \
                                    configDivTextArea_a +  \
                                    String(tag) +       \
                                    (blocked ? configDivInputBlocked_1(blocked) : "") + \
                                    configDivTextArea_b +  \
                                    String(desc) +      \
                                    configDivTextArea_c1 +  \
                                    (blocked ? configDivInputBlocked_2(blocked) : "") + \
                                    configDivTextArea_c2 +  \
                                    String(tag) +       \
                                    configDivTextArea_d +  \
                                    String(val) +       \
                                    configDivTextArea_e + \
                                    configNewLiner

/* Submit button */
const char configPageSubmitButton[] PROGMEM = R"rawliteral(<input type=submit class=btn value=Confirm>)rawliteral";


/* Page generator */
#define configPageHtmlStart     configPageHtmlOpen + \
                                configPageHeadOpen + \
                                configPageTitleAdd + \
                                configPageScriptOpen + \
                                configPagePassToggleScript + \
                                configPageScriptClose + \
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

#define configPageBodyEnd       configPageSubmitButton + \
                                configPageHtmlFormClose + \
                                configPageHtmlBodyClose

#define configPageHtmlEnd       configPageHtmlClose


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
