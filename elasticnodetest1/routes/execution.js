/*
 * Just checking to see if the DB is up.
 */
exports.ping = function(client) {
  return function(req, res){
		client.info("",function(err,resp){
			if (err){
				console.log(err);
				res.send('No connection to ElasticSearch Cluster');
 			} else {
 				console.log(resp);
 				res.send(resp);
 			}
		});//client
	}
};

/*
 * Searching for the list of all benchmarks.
 */
exports.executions = function (client){
	return function(req, res){
		client.search({index:'executions',size:10000}, function(err, result)
		{
 			if (result.hits != undefined){
				var only_results = result.hits.hits;
        //  var all_hits = result.hits;
        //  var execution_ID = result.hits._id;
        //  console.log(execution_ID);
	  		var es_result = [];
	  		var keys = Object.keys(only_results);

var 	i = 0;
	  		keys.forEach(
	  			function(key)
	  			{
i++;
var exeID = only_results[key]._id;
temporary = {"id":exeID,"Name":only_results[key]._source.Name ,"Description":only_results[key]._source.Description,"Metrics":"<a href='#' onclick=searchMetrics('" + exeID + "') >Choose </a> |<a href='#' onclick=exportMetrics('" + exeID + "') > Export</a> "};

						//temporary = {"id":exeID,"Name":"<a href='/executions/details/"+exeID + "'>"+only_results[key]._source.Name + "</a>","Description":only_results[key]._source.Description,"Metrics":"<a href='#' class = 'linkmetrics' rel = '" + exeID + "'>Choose metrics</a>"};

						es_result.push(temporary);
            //es_result.push(only_results[key]);
           // console.log(temporary);

     			//	console.log("Adding "+key+" number to result ");
     			//	console.log(JSON.stringify(es_result[key]));
          //  console.log("The ID for this one is "+only_results[key]._id+" \n")
     			});
	  			res.send(es_result);		
	  			} else {
	  				res.send('No data in the DB');
	  			}	  
	  	})
		}
};

/*
 * Show more information about the execution.
 */
exports.details = function(client){
	return function(req, res){
  	client.get({
    	index:'executions',
      type: 'TBD', 
      id: req.params.ID
    },   
    function(err, result)
    {
      //console.log(result);
      if (result.found != false){          
      	res.send(result._source);    
      } else {
      	res.send('Requested resource was Not found');
      }    
    })
	} 
};

/*
 * Searching metrics of a specific execution.
 */
exports.metrics = function (client){
	return function(req, res){
		var id = req.params.ID.toLowerCase();
		client.indices.getMapping({
    	index:req.params.ID.toLowerCase(), 
    },   
    function(err, result)
    {
			if (result.found != false){				
				var metrics = result[id].mappings.TBD.properties
				var names = [];
				var metric_name = Object.keys(metrics);
				metric_name.forEach(function(metric) {
					if (metric != "Timestamp"){						
						names.push(metric);
					}	  
				});
      	//res.send(result);
				res.send(names);                    
       } else {
        res.send('No data in the DB');
       }
    })
	}
};



/*
 * Searching for the values of a specific benchmark.
 */
exports.values = function (client){
	return function(req, res){
    var from_time = req.params.from;
    var to_time = req.params.to;

		client.search({
    	index:req.params.ID.toLowerCase(), 
      size:10000,
			sort:["type", "Timestamp"],
      },   
      function(err, result)
    	{
      	if (result.hits != undefined){
          var only_results = result.hits.hits;
          var es_result = [];
          var keys = Object.keys(only_results);

          keys.forEach(
            function(key)
              {
                es_result.push(only_results[key]._source);
                //console.log("Adding "+key+" number to result ");
                //console.log(JSON.stringify(es_result[key]));
              });
          res.send(es_result);    
        } else {
          res.send('No data in the DB');
        }    
    })
	}
};

/*
 * Searching for the values of a specific benchmark, filter by range.
 */
exports.range = function (client){
	return function(req, res){
	  var from_time = req.params.from;
	  var to_time = req.params.to;

		client.search({
    	index:req.params.ID.toLowerCase(), 
      body: {
      	query: {
      	constant_score: {
        	filter: {
	        	range: {
	          	"Timestamp" : { "from" : from_time, "to" : to_time } 
	          }
	        }
        }
      }
      }},   
      function(err, result)
 			{
 			  if (result.hits != undefined){
	  			var only_results = result.hits.hits;
	  			var es_result = [];
	  			var keys = Object.keys(only_results);

	  			keys.forEach(
	  				function(key)
	  				{
        			es_result.push(only_results[key]._source);
        			//console.log("Adding "+key+" number to result ");
        			//console.log(JSON.stringify(es_result[key]));
        		});
	  			res.send(es_result);		
	  		} else {
	  			res.send('No data in the DB');
	  		}	  
	  })
	}
};

/*
 * Adding a new execution and respond the provided ID.
 */
exports.insert = function (client){
	return function(req, res){
  	var the_json = req.body;
  	//console.log("The request body is: ");
  	//console.log(the_json);
		client.index({index:'executions',type: 'TBD',body:the_json},function(err,es_reply)
  	{
  		//console.log(es_reply);
  		res.send(es_reply._id);
  	});
	}
};

/*
 * Adding a new time to an existing execution and respond the provided ID.
 */
exports.add = function (client){
	return function(req, res){
  	var the_json = req.body;
		client.index({index:req.params.ID.toLowerCase(),type: 'TBD',body:the_json},function(err,es_reply)
  	{
  		//console.log(es_reply);
  	  res.send(es_reply);
  	});
	}
};


