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

// Metrics link click
$('#executions table tbody').on('click', 'td a.linkmetrics', searchMetrics);


});//$(document)




// Functions =============================================================




function myRowWriter(rowIndex, record, columns, cellWriter) {
    var tr = '';

    // grab the record's attribute for each column
    for (var i = 0, len = columns.length; i < len; i++) {
      tr += cellWriter(columns[i], record);
    }

    return '<tr data-stuff=' + record.customData + '>' + tr + '</tr>';
  };

  function myRowReader(rowIndex, rowElement, record) {
    record.customData = $(rowElement).data('stuff');
  };

  $('#my-table').dynatable({
    writers: {
      _rowWriter: myRowWriter
    },
    readers: {
      _rowReader: myRowReader
    }
  });


function searchMetrics(event) {

  // Prevent Link from Firing
  event.preventDefault();

  // Retrieve username from link rel attribute
  var idExecution = $(this).attr('rel');

window.alert(idExecution);

};

function openWin() {

metricWindow = window.open( '',
'metricWindow', 'menubar=no,location=no,status=no,directories=no,toolbar=no,scrollbars=yes,top=400,left=400,height=200,width=200'
);



message="<font face='verdana, arial, helvetica, san-serif' size='2'>";
message+="<form name='MetricPopup' action='/visualization' method='GET' target='_blank'>";
message+="<input type='text' name='index' value='T08trkUaSrCiukSm7pUamw'> <br>";
message+="<input type='checkbox' name='metric' value='User_CPU'> User_CPU <br>";
message+="<input type='checkbox' name='metric2' value='Sys_CPU'> Sys_CPU <br>";
message+="<input type='checkbox' name='metric3' value='Memory'> Memory <br>";
message+="<p><input type='submit' value='Visualization' onBlur='window.close();'> </p>";
message+="</form>";
message+="</font>";




metricWindow.document.write(message);

return metric;
};





