/* for image update */
g_img = new Array();

function createXMLHttpRequest() {
   try { return new XMLHttpRequest(); } catch(e) {}
   try { return new ActiveXObject("Msxml2.XMLHTTP"); } catch (e) {}
   alert("XMLHttpRequest not supported");
   return null;
}

function RefreshImg(name, url, refresh) {
	if(refresh == true) {
		obj = document.getElementById(name);
		obj.src = g_img[name].src;
	}

	uniq = new Date();
	uniq = "?" + uniq.getTime();
	g_img[name].src = url + uniq;
	delete uniq;
}

function InitialImg(name, url) {
	uniq = new Date();
	uniq = "?" + uniq.getTime();
	g_img[name] = new Image();
	g_img[name].onload = function() { RefreshImg(name, url, true); }
	g_img[name].onerror = function() { RefreshImg(name, url, false); }
	g_img[name].onabort = function() { RefreshImg(name, url, false); }
	g_img[name].src = url + uniq;
	document.getElementById(name).onload = "";
	if( document.getElementById(name + "td") ) {
		document.getElementById(name + "td").style.background = "#88f";
	}
}


/* for status update */

function refreshStatus(url, id) {
	var xhReq = createXMLHttpRequest();
	uniq = new Date();
	uniq = "?" + uniq.getTime();
	xhReq.open("GET", url + uniq, true);
	xhReq.onreadystatechange = function() {
		if(xhReq.readyState != 4 || xhReq.status != 200)  { return; }
		json = eval("(" + xhReq.responseText + ")");
		document.getElementById("desc").innerHTML = json.desc;
		document.getElementById("curr").innerHTML = json.curr;
		document.getElementById("total").innerHTML = json.total;
		document.getElementById("timestamp").innerHTML = json.framecount;
		document.getElementById("framedrop").innerHTML = json.framedrop;
		if(id>0 && id<=23) {
			document.getElementById("shutter").innerHTML = json.shutter[id-1];
			document.getElementById("gain").innerHTML = json.gain[id-1];
			document.getElementById("shutter_auto").innerHTML = json.shutter_auto[id-1] == 0 ? "off" : "on";
			document.getElementById("gain_auto").innerHTML = json.gain_auto[id-1] == 0 ? "off" : "on";
		} else {
			document.getElementById("shutter").innerHTML = json.shutter;
			document.getElementById("gain").innerHTML = json.gain;
		}
		refreshStatus(url, id);
	}
	xhReq.send(null);
}
