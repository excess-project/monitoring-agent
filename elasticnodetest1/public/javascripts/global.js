// metrics data array 
var metricsData = [];

// DOM Ready =============================================================

$(document).ready(function() {

// Populate the table on initial page load
$.ajax({
  url: '/executions',
  success: function(data){
    $('#executions').dynatable({
      dataset: {
        records: data
      }
    });
  }
});//$.ajax


});//$(document)

// Functions =============================================================
function searchMetrics(idExe) {

	metricWindow = window.open( '',
		'metricWindow', 'menubar=no,location=no,status=no,directories=no,toolbar=no,scrollbars=yes,top=400,left=400,height=200,width=350'
	);

	var message = '';
	message="<font face='verdana, arial, helvetica, san-serif' size='2'>";
	message+="<form name='MetricPopup' action='/visualization' method='GET' target='_blank'>";
	message+="<input type='hidden' name='index' value='"+ idExe +"'> <br>";
	message+="From: <input type='text' name='from'> <br>";
	message+="To: <input type='text' name='to'> <br>";

  // jQuery AJAX call for JSON
  $.getJSON( '/executions/metrics/'+idExe, function( data ) {
		// Stick our metric data array into a metricsData variable in the global object
    metricsData = data;
		var i=0;    
		metricsData.forEach(function(value) {
			i+=1;
			message+="<input type='checkbox' name='metric"+ i +"' value='"+ value +"'>" + value +"<br>";
		});

		message+="<p><input type='submit' value='Visualization' onBlur='window.close();'> </p>";
		message+="</form>";
		message+="</font>";
		metricWindow.document.write(message);

   });//jQuery AJAX call for JSON
};

function exportMetrics(idExe) {

	metricWindow = window.open( '',
		'metricWindow', 'menubar=no,location=no,status=no,directories=no,toolbar=no,scrollbars=yes,top=400,left=400,height=350,width=600'
	);

	var message = '';
	message="<font face='verdana, arial, helvetica, san-serif' size='2'>";
  message+="<script type='text/javascript' src='/javascripts/global.js'></script>";
  message+="<form name='MetricPopup' >";
  // jQuery AJAX call for JSON
  $.getJSON( '/executions/'+idExe, function( data ) {
		// Stick our metric data array into a metricsData variable in the global object
    executionsData = data;
		message+="<textarea id='txt'cols=80 rows=10>"+JSON.stringify(executionsData)+"</textarea> <br>";		
		message+="<input type='button' value='Download CSV' onclick='JSON2CSV("+JSON.stringify(executionsData)+");' onBlur='window.close();' />";    
		message+="<input type='button' value='Download JSON' onclick='saveJSON("+JSON.stringify(executionsData)+");' onBlur='window.close();' /><br>";
	  message+="</form>";
		message+="</font>";
		metricWindow.document.write(message);
   });//jQuery AJAX call for JSON
};

function JSON2CSV(objArray) {
    var array = typeof objArray != 'object' ? JSON.parse(objArray) : objArray;
    var str = '';
    var line = '';
    var metric_type = '';  

    for (var i = 0; i < array.length; i++) {
			line = '';
      //include the header of each metric type
			if (metric_type != array[i]['type']){
				metric_type = array[i]['type']
		    for (var index in array[i]) {
	    		line += index + ',';
	    	}    
				line = line.slice(0, -1);
	    	str += line + '\r\n';   
			}
			line = '';

			for (var index in array[i]) {
				line += array[i][index] + ',';
			}
			line = line.slice(0, -1);
      str += line + '\r\n';
    }
    window.open("data:text/csv;charset=utf-8," + escape(str))
    return str;
    
};

function saveJSON(objArray){
	var array = typeof objArray != 'object' ? JSON.parse(objArray) : objArray;
	window.open("data:text/json;charset=utf-8," + JSON.stringify(array))
};
