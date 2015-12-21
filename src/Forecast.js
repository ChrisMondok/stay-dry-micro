function Forecast(data) {
	if(!data instanceof Array)
		throw new TypeError("malformed data");
	
	console.log(data.map(function(m) {return m.precipIntensity;}).join(', '));

	this.minutes = data;
}

Forecast.prototype.findWindows = function(tolerance, minLength) {
	var windows = [];

	var thisWindow = null;
	
	minLength = minLength || 0;

	this.minutes.forEach(function(minute) {
		if(minute.precipIntensity <= tolerance)
			expand(minute);
		else
			thisWindow = null;
	});

	return windows.filter(function(window) {
		return window.end - window.start >= minLength;
	});

	function expand(minute) {
		if(!thisWindow) {
			thisWindow = {start: minute.time, precipIntensity: minute.precipIntensity};
			windows.push(thisWindow);
		}
		thisWindow.end = minute.time;
		thisWindow.precipIntensity = Math.max(minute.precipIntensity, thisWindow.precipIntensity);
	}
};

Forecast.get = function(coords, callback) {
	if( !("latitude" in coords && "longitude" in coords))
		throw new TypeError("These coords don't look like coordinates to me!");

	var url = ("https://api.forecast.io/forecast/4f95e96d2cb2a3b847ce19f232b84a9b/LAT,LNG?exclude=currently,hourly,daily,alerts,flags")
		.replace(/LAT/,coords.latitude)
		.replace(/LNG/, coords.longitude);

	console.log("Getting forecast");
	var request = new XMLHttpRequest();
	request.open("GET", url);
	request.addEventListener('load', function() {
		var response = JSON.parse(this.responseText);
		console.log("GOT RESPONSE");
		callback(new Forecast(response.minutely.data));
	});
	request.send();
};