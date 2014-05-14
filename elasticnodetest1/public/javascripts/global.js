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





