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


 $('button').click(function(){
        var data = $('#txt').val();
        if(data == '')
            return;
        
        JSONToCSVConvertor(data, "test", true);
    });


});//$(document)


// Functions =============================================================
function searchMetrics(idExe) {

	metricWindow = window.open( '',
		'metricWindow', 'menubar=no,location=no,status=no,directories=no,toolbar=no,scrollbars=yes,top=400,left=400,height=200,width=300'
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

//return metric;

};

function exportMetrics(idExe) {

	metricWindow = window.open( '',
		'metricWindow', 'menubar=no,location=no,status=no,directories=no,toolbar=no,scrollbars=yes,top=400,left=400,height=300,width=600'
	);

	var message = '';
	message="<font face='verdana, arial, helvetica, san-serif' size='2'>";
//	message+="<form name='MetricPopup' onsubmit='JSONToCSVConvertor(executionsData, 'test', true)'>";
	message+="<input type='hidden' name='index' value='"+ idExe +"'> <br>";
	message+="From: <input type='text' name='from'> <br>";
	message+="To: <input type='text' name='to'> <br>";

  // jQuery AJAX call for JSON
  $.getJSON( '/executions/'+idExe, function( data ) {
		// Stick our metric data array into a metricsData variable in the global object
    executionsData = data;
		//window.alert(executionsData);

		message+="<textarea id='txt'cols=80 rows=10>"+JSON.stringify(executionsData)+"</textarea> <br>";
		message+="<p><button>Export CSV</button> </p>";
   
//		message+="</form>";
		message+="</font>";
		metricWindow.document.write(message);

   });//jQuery AJAX call for JSON

};

function JSONToCSVConvertor(JSONData, title, showLabel) {
    window.alert("aqui");


};
