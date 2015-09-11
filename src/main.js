Pebble.addEventListener('ready', function(e) {
	Pebble.addEventListener('appmessage', function(e) {
		console.log("Got message: ");
		console.log(JSON.stringify(e.payload));
		var timeWindow = deserializeTimewindow(e.payload[0], 0);
		console.log(JSON.stringify(timeWindow));
	});
	
	fetchAndSendData();
});
	

function fetchAndSendData() {
	getLocation(function(location) {
		console.log("Got coordinates "+location.coords.latitude+", "+location.coords.longitude);
		//getForecast(location.coords, function(forecast) {
		getForecast({latitude: 42.3587, longitude: -71.0567}, function(forecast) {
			var tolerance = 0.03;
			var oneMinute = 1000*60;
			var windows = forecast.findWindows(tolerance, oneMinute);
			console.log("Got "+windows.length+" windows");
			
			var swindows = [];
			for(var i = 0; i < 8; i++)
				swindows = swindows.concat(serializeTimeWindow(windows[i]));
			
			var sminutely = forecast.minutes.map(function(m) {
				return serializePrecipIntensity(m.precipIntensity);
			}).slice(0, 60);
			while(sminutely.length < 60)
				sminutely.push(sminutely[sminutely.length - 1]);
			
			var message = {
				KEY_WINDOWS: swindows,
				KEY_MINUTELY: sminutely
			};
			
			console.log("About to send this thing:\n", JSON.stringify(message));
			
			Pebble.sendAppMessage(message, function(e) {
				console.log("Sent successfully");
			}, function(e) {
				console.log("Failed to send");
				console.log(e.error.message);
			});
		});
	});
}

function deserializeTimewindow(array, start) {
	return {
		start: deserializeUint32(array, 0),
		end: deserializeUint32(array, 4),
		precipIntensity: array[8]/255
	};
}

function deserializeUint32(array, start) {
	var uint = 0;
	for(var i = 0; i < 4; i++)
		uint += (array[start + i]) << (8 * i);
	return uint;
}

function serializeTimeWindow(w) {
	var intensity = serializePrecipIntensity(w && w.precipIntensity);
	var output = []
		.concat(serializeUint32(w ? w.start : 0))
		.concat(serializeUint32(w ? w.end : 0))
		.concat(serializeUint32(intensity ));
	return output;
}

function serializePrecipIntensity(inPrHr) {
	return inPrHr ? Math.min(255, Math.floor(inPrHr / 0.4 * 255)) : 1;
}

function serializeUint32(number) {
	var output = [];
	for(var i = 0; i < 4; i++)
		output.push((number & (255 << 8*i)) >> 8*i);
	return output;
}

function getLocation(callback) {
	var locationOptions = {
		enableHighAccuracy: true,
		maximumAge: 10000,
		timeout: 10000
	};
	
	function errHandler(err) {
		console.log("Location error: "+err.code+": "+err.message);
	}
	
	navigator.geolocation.getCurrentPosition(callback, errHandler, locationOptions);
}
