/**
 * Module dependencies.
 */

var express = require('express');
var routes = require('./routes');
var user = require('./routes/user');
var http = require('http');
var path = require('path');
var app = express();
var elasticsearch = require('elasticsearch');
var client = new elasticsearch.Client({
  host: 'localhost:9200',
  log: 'trace'
});

//to call funtions here
var tools = require('./routes/tools');

// all environments
app.set('port', process.env.PORT || 3000);
app.set('views', path.join(__dirname, 'views'));
//Change to html templates
app.engine('html', require('ejs').renderFile);
app.set('view engine', 'html');
app.use(express.favicon());
app.use(express.logger('dev'));
app.use(express.json());
app.use(express.urlencoded());
app.use(express.methodOverride());
app.use(express.cookieParser('your secret here'));
app.use(express.session());
app.use(app.router);
app.use(express.static(path.join(__dirname, 'public')));

// development only
//if ('development' == app.get('env')) {
  app.use(express.errorHandler());
//}


app.get('/', routes.index);

//Just checking to see if the DB is up
app.get('/ping', function(req, res){
 		client.info("",function(err,resp){
 			if (err){
 				console.log(err);
 				res.send('No connection to ElasticSearch Cluster');
 			} else {
 				console.log(resp);
 				res.send(resp);
 			}
 		}); 		
});

//Searching for the list of all benchmarks
app.get('/executions', function(req, res){
	  client.search({index:'executions',size:10000},  function(err, result)
 		{
 			if (result.hits != undefined){
          
	  			var only_results = result.hits.hits;
        //  var all_hits = result.hits;
        //  var execution_ID = result.hits._id;
        //  console.log(execution_ID);
	  			var es_result = [];
	  			var keys = Object.keys(only_results);

	  			keys.forEach(
	  				function(key)
	  					{
//tools.metrics = (client);
//var msg = "hola testing";
//var metrics = app.get = ('/metrics/'+msg,tools.metrics(client));
//var metrics = this.get = ('/executions/metrics/'+only_results[key]._id).res.get();
//var metrics = res.get = ('/executions/metrics/'+only_results[key]._id);
//console.log("Metrics call "+metrics);


                //temporary = {"id":only_results[key]._id,"Name":"<a href='/executions/details/"+only_results[key]._id+"'>"+only_results[key]._source.Name+"</a>","Description":only_results[key]._source.Description,"Metrics":"<a href='/executions/metrics/"+only_results[key]._id+"'>see metrics</a>"};
temporary = {"id":only_results[key]._id,"Name":"<a href='/executions/details/"+only_results[key]._id+"'>"+only_results[key]._source.Name+"</a>","Description":only_results[key]._source.Description,"Metrics":"<a href='/visualization/?index="+only_results[key]._id+"&metric=User_CPU&metric2=Sys_CPU&metric3=Memory'>see metrics</a>"};
        				es_result.push(temporary);
                //es_result.push(only_results[key]);
                console.log(temporary);

        				console.log("Adding "+key+" number to result ");
        				console.log(JSON.stringify(es_result[key]));
                console.log("The ID for this one is "+only_results[key]._id+" \n")
        			});
	  			res.send(es_result);		
	  		} else {
	  			res.send('No data in the DB');
	  		}
	  
	  })
});

//Show more information about the execution
app.get('/executions/details/:ID', function(req, res){
      client.get({
            index:'executions',
             type: 'TBD', 
              id: req.params.ID
              },   
            function(err, result)

    {
      console.log(result);
      if (result.found != false){          
          res.send(result._source);    
        } else {
          res.send('Requested resource was Not found');
        }
    
    })
});

/*
//Searching metrics of a specific benchmark //not working
app.get('/executions/metrics/:ID', function(req, res){
	    client.indices.getMapping({
            index:req.params.ID.toLowerCase(), 
          },   
          function(err, result)
    {

      if (result.found != false){
	  //var only_results = result.mappings;
	  console.log("Result "+only_results);
          res.send(result);          
       } else {
          res.send('No data in the DB');
        }
    })
});
*/

//Searching metrics of a specific benchmark 
app.get('/executions/metrics/:ID', function(req, res){
          client.search({
            index:req.params.ID.toLowerCase(), 
            size:10000,
            sort:"Timestamp",
          },   
          function(err, result){

	     if (result.hits != undefined){
	        var only_results = result.hits.hits;
                var es_result = [];
                var keys = Object.keys(only_results);
	        var keys_name = [];

                keys.forEach(
                function(key){
                   es_result.push(only_results[key]._source);
                   //console.log("Key "+JSON.stringify(es_result[key]));
		   for (var key_name in es_result[key]) {                  
		      if (key_name != "Timestamp"){						
                         keys_name.push(key_name);
 	              }	
                   }
 	           keys_name = keys_name.unique();		   
                });
                //console.log("Keys_name "+JSON.stringify(keys_name));
		//metrics = {"Metrics":keys_name};
		//res.send (metrics);
                res.send(keys_name);        
             } else {
                res.send('No data in the DB');
             }
    
          })

});

//Searching for the values of a specific benchmark 
app.get('/executions/:ID/:from/:to', function(req, res){
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
        				console.log("Adding "+key+" number to result ");
        				console.log(JSON.stringify(es_result[key]));
        			});
	  			res.send(es_result);		
	  		} else {
	  			res.send('No data in the DB');
	  		}
	  
	  })
});

//Searching for the values of a specific benchmark
app.get('/executions/:ID', function(req, res){
    var from_time = req.params.from;
    var to_time = req.params.to;

          client.search({
            index:req.params.ID.toLowerCase(), 
            size:10000,
            sort:"Timestamp",
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
                console.log("Adding "+key+" number to result ");
                console.log(JSON.stringify(es_result[key]));
              });
          res.send(es_result);    
        } else {
          res.send('No data in the DB');
        }
    
    })
});


//Adding a new execution and respond the provided ID
app.post('/executions', function(req, res){
  var the_json = req.body;
  console.log("The request body is: ");
  console.log(the_json);
	client.index({index:'executions',type: 'TBD',body:the_json},function(err,es_reply)
  	{
  		  //console.log("NODE.js Output starts here .................");
  		  //console.log(es_reply);
  		  res.send(es_reply._id);
  	});
});

//Adding a new time to an existing execution and respond the provided ID 
app.post('/executions/:ID', function(req, res){
  var the_json = req.body;
	client.index({index:req.params.ID.toLowerCase(),type: 'TBD',body:the_json},function(err,es_reply)
  	{
  		  console.log(es_reply);
  		  res.send(es_reply);
  	});
});

http.createServer(app).listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});

//Function to delete duplicate elements in an array
Array.prototype.unique=function(a){
  return function(){return this.filter(a)}}(function(a,b,c){return c.indexOf(a,b+1)<0
});

