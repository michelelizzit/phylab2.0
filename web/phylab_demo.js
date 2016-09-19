var nomeGrafico;
var updateFrequency = 10; /* value in milliseconds*/
var defaultAutoScale = false;
var lowCPU = false;
var defaultSmoothLines = 0;
var defaultScaleMinVal = 0;
var defaultScaleMaxVal = 100;
var defaultAnimation = 0;
var minAcquisitionPeriod = 3;
var sensorType = "hum";
var dbAvailable = true;
var unitMeasureTable = {
	temp: "°",
	hum: "%",
	volt: "V",
	dist: "cm",
	acc: "m/s²",
	lamp: "Ω"
};

var currentDataArr = [0, 0];

var defaultTargetRate = updateFrequency;
var animationUpdateTime = defaultAnimation;

var data;

var optionXAxisDefault = {
	display: false
};

var optionXAxisLamp = {
	display: true,
	position: "bottom",
	type: 'linear',
	ticks: {
		min: 0,
		max: 12
	}
};

var options = {
	scales: {
		xAxes: [optionXAxisDefault],
		yAxes: []
	},
	responsive: true,
	maintainAspectRatio: false,
	animation: {
		duration: 0
	},
	animationSteps: defaultAnimation,
	legend: {
		display: false
	}
};


var myLineChart;
function initChart() {
	sensorType = $("#sensorTypeSelect").val();
	nomeGrafico = $("#sensorTypeSelect option:selected" ).text();
	$(document).prop('title', ("PhyLab 2.0 - " + nomeGrafico));
	unitMeasure = unitMeasureTable[sensorType];
	document.getElementById("currentUnit").innerHTML = unitMeasure;
	lastTimestamp = 0;
	$("#animationInput").attr('max', defaultTargetRate - 1);
	$("#animationInput").val(defaultAnimation);
	$("#targetRate").val(defaultTargetRate);
	$("#smoothLines").val(defaultSmoothLines);
	$("#minValScale").val(defaultScaleMinVal);
	$("#maxValScale").val(defaultScaleMaxVal);
	$("#targetRate").attr('min', minAcquisitionPeriod);

	if (defaultAutoScale) $( "#autoScale" ).prop( "checked", true );
	else $( "#autoScale" ).prop( "checked", false );

	data = {
		labels: [],
		datasets: [
		{
			fill: true,
			lineTension: defaultSmoothLines,
			pointRadius: 1,
			borderWidth: 1,
			label: "Value",
			backgroundColor: "rgba(23, 101, 212, 0.1)",
			borderColor: "#5A8FDB",/*"rgba(152,62,59,1)"*/
            /*hoverBackgroundColor: "rgba(20,20,220,1)",
            hoverBorderColor: "rgba(220,20,220,1)",*/
            data: []
        }
        ]
    }
    refreshChart();
    setAutoScale();
    fadeLoadingDiv();
    //connectWebsocket();
}

function downloadData() {
	stopChart();
	date = new Date().toISOString().slice(0, 19);
	var tmpFileName = "PhyLab_data_" + nomeGrafico + "_" + date + ".csv";

	/*formatta alldata in una stringa csv*/
	var tmpAllDataString = "";
	tmpAllDataString += "Data type: ," + nomeGrafico + "\n"; /*prima riga*/
	tmpAllDataString += "Value,Time\n"; /*seconda riga*/
	for (tmpCnt = 0; tmpCnt < allData.length; tmpCnt++)
		tmpAllDataString += allData[tmpCnt][1] + "," + allData[tmpCnt][0] + "\n";

	/*rimuove a capo finale*/
	if (tmpAllDataString.length > 0)
		tmpAllDataString = tmpAllDataString.slice(0, tmpAllDataString.length - 1);

	downloadText(tmpFileName, tmpAllDataString);
}

function downloadChart() {
	date = new Date().toISOString().slice(0, 19);
	var tmpFileName = "PhyLab_chart_" + nomeGrafico + "_" + date + ".png";

	downloadImage(tmpFileName, myLineChart.toBase64Image());
}

function downloadText(fileName, fileContent) {
	/*crea un a*/
	var tmpDownloadA = document.createElement('a');
	tmpDownloadA.setAttribute('href', 'data:text/csv;charset=utf-8,' + encodeURIComponent(fileContent));
	tmpDownloadA.setAttribute('download', fileName);

	/*si nasconde*/
	tmpDownloadA.style.display = 'none';

	/*si auto clicca*/
	document.body.appendChild(tmpDownloadA);
	tmpDownloadA.click();
	document.body.removeChild(tmpDownloadA);
}

function downloadImage(fileName, fileContent) {
	/*crea un a*/
	var tmpDownloadA = document.createElement('a');
	tmpDownloadA.setAttribute('href', fileContent);
	tmpDownloadA.setAttribute('download', fileName);

	/*si nasconde*/
	tmpDownloadA.style.display = 'none';

	/*si auto clicca*/
	document.body.appendChild(tmpDownloadA);
	tmpDownloadA.click();
	document.body.removeChild(tmpDownloadA);
}

function refreshChart() {
	if (typeof myLineChart !== "undefined") {
		myLineChart.destroy();
	}
	if ($(".chartjs-hidden-iframe").length) { //if exists remove old chart
		$(".chartjs-hidden-iframe").remove();
	}
	var ctx = document.getElementById("myChart");
	myLineChart = new Chart(ctx, {
		type: 'line',
		data: data,
		options: options
	});
}

var allData = [];
/*
window.WebSocket = window.WebSocket || window.MozWebSocket;
var websocket;*/

function sendDebugStr() {
	//websocket.send($('#debugInput').val());
	console.log("SENT: " + $('#debugInput').val());
	$('#debugInput').val('');
}

function clearData() {
	//websocket.send(sensorType + "D");
	resetChart();
	alert("Data archive cleared");
}

function getTemperature() {
	gotData = false;
	currentDataArr[0] = Math.random() * 10000;
	//console.info(cnt);
	//console.info("C: " + cnt + " T: " + currentTemp + " O: " + message.data);
	currentDataArr[0] = currentDataArr[0] / 10000;
	updateChart();
	updateTempDiv();
	gotData = true;
}

function resetChart() {
	/*stops the chart*/
	stopChart();
	/*empty dataset*/
	for (tmp = 0; tmp < valuesDisplayed; tmp++) {
		data.labels.splice(0, 1);
		data.datasets[0].data.splice(0, 1);
	}
	/*renders the empty chart*/
	myLineChart.update();
	/*the number of displayed values is now zero*/
	valuesDisplayed = 0;
	document.getElementById("currentNumVal").innerHTML = valuesDisplayed;

	/*reset cnt*/
	cnt = 0;
	/*reset alldata*/
	allData = [];

	/*reset timestamp*/
	lastTimestamp = 0;
}
var cnt = 0;


var getData = false;

var gotData = true;

function updateAll()  {
	if ((getData == true) && (!useDB)) {
		if (gotData == true) {
			getTemperature();
		}
	}
	else if ((getData == true) && (useDB)) {
		if (gotData == true) {
			updateWithDB();
		}
	}
}

var updateIntervalFunction = setInterval(updateAll, updateFrequency);

function setNewRate() {
	updateFrequency = parseFloat(document.getElementById("targetRate").value);
	/*animation time shall never be longer than chart refresh time*/
	$("#animationInput").attr('max', updateFrequency - 1);
	document.getElementById("currentTargetSampleRate").innerHTML = updateFrequency;
	clearInterval(updateIntervalFunction);
	updateIntervalFunction = setInterval(updateAll, updateFrequency);
	setAnimation();
}

function setNewSkip() {
	skipData = parseFloat(document.getElementById("skipData").value);
}

var valuesDisplayed = 0;
var sampleCnt = 0;
var fixAnimation = false;

function updateChart() {
	var deletedData = false;
	if (valuesDisplayed == numMaxValues) {
		data.labels.splice(0, 1);
		data.datasets[0].data.splice(0, 1);
		valuesDisplayed--;
		deletedData = true;
	}
	else if (valuesDisplayed > numMaxValues) {
		data.labels.splice(0, 1);
		data.labels.splice(0, 1);
		data.datasets[0].data.splice(0, 1);
		data.datasets[0].data.splice(0, 1);
		valuesDisplayed--;
		valuesDisplayed--;
		deletedData = true;
	}
	if ((deletedData) && (fixAnimation)) {
		myLineChart.update(0, true);
	}

	valuesDisplayed++;
	var tmpTime = 0;

	if (!lowCPU) {/*if lowCPU is not set use accurate time esteem*/
		tmpTime = (new Date()).getTime() - dataStartTime;
	}
	else {
		tmpTime = (cnt * updateFrequency);
	}


	if (sensorType == "lamp") {
		var tmpValue1 = currentDataArr[0] * 10;
		var tmpValue2 = currentDataArr[1] * 1000;

		//Uncomment for random data
		tmpValue1 = (Math.random() > 0.5 ? -1 : 1) * Math.round(Math.random() * 10); tmpValue2 = tmpValue1;

		data.datasets[0].data.push({x: tmpValue1, y: tmpValue2});
		data.labels.push(tmpValue1 + "V " + tmpValue2 + "A");
		allData.push([ tmpValue1, tmpValue2] );
	}
	else {
		var tmpValue = currentDataArr[0] * 100;

		//Uncomment for random data
		tmpValue = Math.random() * 100;

		var tmpData = {x: tmpTime, y: tmpValue}
		data.datasets[0].data.push(tmpData);
		tmpTime = tmpTime / 1000;
		data.labels.push((tmpTime) + "s");
		allData.push([ tmpValue, tmpTime] );
	}

	cnt++;
	sampleCnt++;
	myLineChart.update(animationUpdateTime, true);
	document.getElementById("currentNumVal").innerHTML = valuesDisplayed;
	sampleTime = ((new Date()).getTime() / 100) - startTime;
	sampleTime *= 100;
	currentSampleRate = sampleTime / sampleCnt;
	document.getElementById("currentSampleRate").innerHTML = currentSampleRate.toFixed(2);

}
function addToChart(dataToAdd) {
	valuesDisplayed++;
	document.getElementById("currentNumVal").innerHTML = valuesDisplayed;

	dataToAdd[1] /= 100;
	dataToAdd[0] = parseFloat(dataToAdd[0].trim().substr(-13));
	if (dataToAdd[0] > 1) { //if valid time 
		var tmpDate = new Date(dataToAdd[0]);
		var timeStr = tmpDate.toISOString().replace("T", "\n").replace("Z", "");
		
		data.datasets[0].data.push(dataToAdd[1]);
		data.labels.push(timeStr);
		allData.push([ dataToAdd[1], dataToAdd[0] ] );

		lastTimestamp = dataToAdd[0];
	}
}
function updateTempDiv() {
	if (sensorType == "lamp") {
		document.getElementById("currentDataValDiv").innerHTML = "R" + ": " + (currentDataArr[0]/currentDataArr[1] * 10).toFixed(1) + " " + unitMeasure;
	}
	else {
		document.getElementById("currentDataValDiv").innerHTML = nomeGrafico + ": " + (currentDataArr[0] * 100).toFixed(2) + " " + unitMeasure;
	}
}
function startLog() {

}
function stopLog() {

}
function loadLog() {

}
var startTime = 0;
var dataStartTime = 0;
function startChart() {
	if (cnt == 0) /*se avvio e non ci sono valori resetta contatore temporale*/
		dataStartTime = (new Date()).getTime();
	
	sampleCnt = 0;
	getData = true;
	gotData = true;
	document.getElementById("currentTargetSampleRate").innerHTML = updateFrequency;
	startTime = (new Date()).getTime() / 100;
}
function stopChart() {
	getData = false;
	sampleCnt = 0;
	document.getElementById("currentTargetSampleRate").innerHTML = "0";
	document.getElementById("currentSampleRate").innerHTML = "0";
}

var numMaxValues = 100;

function setMaxValue() {
	numMaxValues = parseFloat(document.getElementById("maxInput").value);
}
function setAutoScale() {
	if (document.getElementById('autoScale').checked) {
		enableAutoScale();
	}
	else {
		disableAutoScale();
	}
	refreshChart();
}
function setFixAnimation() {
	if (document.getElementById('fixAnimation').checked) {
		fixAnimation = true;
	}
	else {
		fixAnimation = false;
	}
}
function setAutoScaleMinMax() {
	if (!document.getElementById('autoScale').checked) {
		enableAutoScale();
		disableAutoScale();
	}
	refreshChart();
}
function enableAutoScale() {
	var tmpLen = options.scales.yAxes.length;
	options.scales.yAxes.splice(tmpLen - 1,1);
}
function disableAutoScale() {
	var maxValueScale = parseFloat(document.getElementById("maxValScale").value);
	var minValueScale = parseFloat(document.getElementById("minValScale").value);
	options.scales.yAxes.push({
		ticks: {
			max: maxValueScale,
			min: minValueScale,
			stepSize: 10
		}
	});
}
function setSmoothLines() {
	data.datasets[0].lineTension = parseFloat(document.getElementById("smoothLines").value);
	myLineChart.update(); //update chart, reloads variable "data"
}
function setAnimation() {
	animationUpdateTime = parseFloat(document.getElementById("animationInput").value);
	if (animationUpdateTime >= updateFrequency) {
		$("#animationInput").val(updateFrequency - 1);
		animationUpdateTime = updateFrequency - 1;
	}
}
function setSensorType() {
	resetChart();
	sensorType = $("#sensorTypeSelect").val();

	if (sensorType == "lamp") dbAvailable = false;
	else dbAvailable = true;
	updateDBavailable();

	if (sensorType == "lamp") {
		options.scales.xAxes[0] = optionXAxisLamp;
		data.datasets[0].fill = false;
	}
	else {
		options.scales.xAxes[0] = optionXAxisDefault;
		data.datasets[0].fill = true;
	}
	
	refreshChart();

	nomeGrafico = $("#sensorTypeSelect option:selected" ).text();
	$(document).prop('title', ("PhyLab 2.0 - " + nomeGrafico));
	unitMeasure = unitMeasureTable[sensorType];
	document.getElementById("currentUnit").innerHTML = unitMeasure;
	lastTimestamp = 0;
	updateTempDiv();
}
function fadeLoadingDiv() {
	$(".loadingDiv").fadeOut(2000, function() { $(this).remove(); });
}/*
function connectWebsocket() {
	$('.connectionIndicator').css('background-color', 'yellow');
	websocket = new WebSocket('ws://' + window.location.hostname + ':9999', 'phylab');
	websocket.onopen = function () {
		$('.connectionIndicator').css('background-color', 'green');
		if(!useDB) startChart();
	};
	websocket.onerror = function () {
		$('.connectionIndicator').css('background-color', 'red');
	};
	websocket.onclose = function () {
		$('.connectionIndicator').css('background-color', 'yellow');
		stopChart();
	};
	websocket.onmessage = function (message) {
		console.info(message.data);
		*//*$('#debug').append($('<p>', { text: message.data }));*//*
		if (message.data.charAt(0) == 'D') {
			currentDataArr[0] = parseFloat(message.data.substring(1, 6));
			//console.info(cnt);
			//console.info("C: " + cnt + " T: " + currentTemp + " O: " + message.data);
			currentDataArr[0] = currentDataArr[0] / 10000;
			updateChart();
			updateTempDiv();
			gotData = true;
		}
		else if (message.data.charAt(0) == 'E') {
			currentDataArr[1] = parseFloat(message.data.substring(1, 6));
			currentDataArr[1] = currentDataArr[1] / 10000;
		}
		else if (message.data.charAt(0) == 'F') {
			gotDBData(message.data);
		}
	};
}*/
function reconnectWebsocket() {		
	stopChart();
	//websocket.close();
	//connectWebsocket();
}
var useDB = false;
function changeUseDB() {
	useDB = $("#useDB").is(":checked");
	if (useDB) {
		stopChart();
		resetChart();
		$("#targetRate").prop('min', 250);
		$("#targetRate").val(1000);
		$("#DBoptions").show(500, "swing");
		setNewRate();
		updateWithDB();
		gotData = true;
	}
	else {
		$("#targetRate").prop('min', minAcquisitionPeriod);
		$("#targetRate").val(100);
		$("#DBoptions").hide(500, "swing");
		setNewRate();
		gotData = true;
	}
}
function zeroPad(val, resultSize) {
	var result = "0000000000000" + val;
	result = result.substr(result.length - resultSize);
	return result;
}
var lastTimestamp = 0;
function updateWithDB() {
	//websocket.send(sensorType + "T" + zeroPad(lastTimestamp, 13));
	gotData = true;
}
function gotDBData(data) {
	var numData = data.split(/\n/).length;
	var dataLines = data.split('\n');
	for (var cnt = 1; cnt < numData - 1; cnt++) {
		addToChart(dataLines[cnt].split('\t'));
	}
/*
	if (data.charAt(1) == "2")
		websocket.send(sensorType + "T" + zeroPad(lastTimestamp, 13));
	else
		myLineChart.update(0, true);*/
}
function updateDBavailable() {
	if (dbAvailable) {
		$("#useDBdiv").show(500, "swing");
	}
	else {
		if (useDB) {
			$("#useDB").removeAttribute("checked");
			changeUseDB();
		}
		$("#useDBdiv").hide(500, "swing");
	}
}
function changeShowLines() {
	if (document.getElementById('showLinesBox').checked) {
		options.showLines = true;
	}
	else {
		options.showLines = false;
	}
	refreshChart();
}