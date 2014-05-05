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
});

// Populate the table with benchmarks info
$.ajax({
  url: '/executions/4BRb8WyWQqCdEFVdH46vgQ',
  success: function(data){
    $('#executions-detail').dynatable({
      dataset: {
        records: data
      }
    });
  }
});


});

// Functions =============================================================







