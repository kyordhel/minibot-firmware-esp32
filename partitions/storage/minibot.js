function do_stop(){
	const url = `${window.location.origin}/minibot?cmd=stop`;
	getData(url);
	log(`[Sent]: stop`);
	behselect = document.getElementById("behavior");
	behselect.selectedIndex = 0;
    behselect.dispatchEvent(new Event('change'));
}


function do_mv(){
	args = document.getElementById("mv").value;
	send_cmd("mv", args);
}


function do_pwm(){
	args = document.getElementById("pwm").value;
	send_cmd("pwm", args);
}


function do_behavior(){
	args = document.getElementById("behavior").value;
	send_cmd("behavior", args);
}


function send_cmd(cmd, args){
	const url = `${window.location.origin}/minibot?cmd=${cmd}&args=${args}`;
	getData(url);
	log(`[Sent]: ${cmd} ${args}`);
}


function clearSensors(){
	// document.getElementById("iminibot").classList.add("hidden");
	document.getElementById("batt").classList.add("hidden");
	document.getElementById("lightsrc").classList.add("hidden");
	for (var i = 0; i < 12; ++i)
		document.getElementById(`obssens${i}`).classList.add("hidden");
}


async function updateSensors(){
	data = await fetchSensorData();
	if( data == null ){
		clearSensors();
		return;
	}
	light = data.res.light
	dist  = data.res.distance
	light.sort(function(x, y){return x[0]-y[0]});
	dist.sort(function(x, y){return x[0]-y[0]});
	var x=0, y=0;
	for(var i = 0; i < light.length; ++i){
		if( (light[i][0]) < 0 || (light[i][1] < 0) ) continue;
		x+= light[i][1] * Math.cos(light[i][0]);
		y+= light[i][1] * Math.sin(light[i][0]);
	}
	var langle = Math.atan2(y, x);

	for(var j = 0; j < 12; ++j){
		imgobssens = document.getElementById(`obssens${j}`);
		if(imgobssens != null) imgobssens.classList.add("hidden");
	}
	for(var i = 0, j = 0; (i < dist.length) && (j < 12); ++i){
		imgobssens = document.getElementById(`obssens${j}`);
		if(imgobssens == null) continue;
		if((dist[i][0] < 0) || (dist[i][1] < 0) || (dist[i][1] > 0.2))
			continue;
		imgobssens.classList.remove("hidden");
		imgobssens.style.transform = `translate(-50%, -50%) rotate(${-dist[i][0]-1.5709}rad)`;
		++j;
	}

	imglightsrc = document.getElementById("lightsrc");
	imglightsrc.classList.remove("hidden");
	imglightsrc.style.transform = `translate(-50%, -50%) rotate(${-langle-1.5709}rad)`;
	battperc = document.getElementById("batt");
	battperc.innerHTML=`${Math.round(data.res.battery, 0)}%`;
	battperc.classList.remove("hidden")
}


async function fetchSensorData(){
	const url = `${window.location.origin}/minibot?cmd=sensors`;
	try{
		const signal = AbortSignal.timeout(1000);
		const response = await fetch(url, { signal });
		if(!response.ok) return null;
		const result = await response.json();
		return result;
	}catch(error){
		// return JSON.parse('{"req":"sensors","res":{		"light":[[0.0000, 1], [0.5236, 0], [1.0472, 0], [1.5708, 0], [2.0944, 0], [2.6180, 0], [3.1416, 0], [3.6652, 0], [4.1888, 0], [4.7124, 0], [5.2360, 0], [5.7596, 0]],"distance":[[0.0000, 5], [0.5236, -1], [1.0472, -1], [1.5708, -1], [2.0944, -1], [2.6180, -1], [3.1416, -1], [3.6652, -1], [4.1888, -1], [4.7124, -1], [5.2360, -1], [5.7596, -1]],"battery":50}}'
		// );
		return null;
	}
}


function log(what){
	elog = document.getElementById("log");
	elog.innerHTML=`${what}\n` + elog.innerHTML;
}


async function getData(url){
	try{
		const signal = AbortSignal.timeout(10000);
		const response = await fetch(url, { signal });
		if(!response.ok)
			throw new Error(`Response status: ${response.status}`);
		const result = await response.json();
		log(`[Minibot]: ${result.req} => ${result.res}`);
	}catch(error){
		log(`[Error]: ${error.message}`);
	}
}


window.onload = () =>{
	window.setInterval(updateSensors, 500);
};
