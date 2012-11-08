<html>
<header>
<script type="text/javascript">
function createXMLHttpRequest() {
   try { return new XMLHttpRequest(); } catch(e) {}
   try { return new ActiveXObject("Msxml2.XMLHTTP"); } catch (e) {}
   alert("XMLHttpRequest not supported");
   return null;
}

function refreshStatus(url) {
	var xhReq = createXMLHttpRequest();
	uniq = new Date();
	uniq = "?" + uniq.getTime();
	xhReq.open("GET", url + uniq, true);
	xhReq.onreadystatechange = function() {
		if (xhReq.readyState != 4 || xhReq.status != 200)  { return; }
		json = eval("(" + xhReq.responseText + ")");
		document.getElementById("curr").innerHTML = json.curr;
		document.getElementById("total").innerHTML = json.total;
		document.getElementById("timestamp").innerHTML = json.framecount;
		refreshStatus(url);
	}
	xhReq.send(null);
	document.getElementById("status").onload = "";
}
</script>
</head>
<body id="status" onload="refreshStatus('live/ve14/info.json')">
<ul>
<!--
<li>Hostname: <span id="hostname">N/A</span></li>
<li>Shutter: <span id="shutter">N/A</span></li>
<li>Gain: <span id="gain">N/A</span></li>
-->
<li>Frame: <span id="curr">N/A</span> / <span id="total">N/A</span></li>
<li>Timestamp: <span id="timestamp">N/A</span></li>
</ul>
</body>
</html>
