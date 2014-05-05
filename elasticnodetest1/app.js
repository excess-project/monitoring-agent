
/**
 * Module dependencies.
 //curl -i -X POST -H 'Content-Type: application/json' -d '{"Name": "MyFisrtexecution", "Description": "Testing ","other":"values","onemore":"please"}' http://localhost:3000/executions
 //curl -i -X POST -H 'Content-Type: application/json' -d '{"Name": "MyFisrtexecution", "Description": "Testing ","other":"values","onemore":"please"}' http://localhost:3000/execution/ID
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

// all environments
app.set('port', process.env.PORT || 3000);
app.set('views', path.join(__dirname, 'views'));
//Change to html templates
app.engine('html', require('ejs').renderFile);
app.set('view engine', 'html');
//app.set('view engine', 'jade');
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
	  client.search({index:'executions'},  function(err, result)
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



//curl -i -X POST -H 'Content-Type: application/json' -d '{"Name":"Execution2","Description": "Testin filter timestamp","nodes":["node1","node2","node3"]}' http://localhost:3000/executions
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":"1396622230","metric_1":"value1","metric_2":"value2","metric_3":"value3","metric_4":"value4"}' http://localhost:3000/execution/o0Wuji4QRCaKVwDmbVzeNQ
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":"1396622231","metric_1":"value2","metric_2":"value2","metric_3":"value3","metric_4":"value5"}' http://localhost:3000/execution/o0Wuji4QRCaKVwDmbVzeNQ
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":"1396622232","metric_1":"value4","metric_2":"value24","metric_3":"value34","metric_44":"value54"}' http://localhost:3000/execution/o0Wuji4QRCaKVwDmbVzeNQ
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":"1396622233","metric_1":"value27","metric_2":"value27","metric_3":"value36","metric_4":"value56"}' http://localhost:3000/execution/o0Wuji4QRCaKVwDmbVzeNQ
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":"1396622234","metric_1":"value2","metric_2":"value2","metric_3":"value3","metric_4":"value5"}' http://localhost:3000/execution/o0Wuji4QRCaKVwDmbVzeNQ
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":"1396622240","metric_1":"value2","metric_2":"value2","metric_3":"value3","metric_4":"value5"}' http://localhost:3000/execution/o0Wuji4QRCaKVwDmbVzeNQ
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":"13966222405","metric_1":"value2","metric_2":"value2","metric_3":"value3","metric_4":"value5"}' http://localhost:3000/execution/o0Wuji4QRCaKVwDmbVzeNQ

//curl -i -X POST -H 'Content-Type: application/json' -d '{"Name":"Execution3","Description": "Testing filter timestamp","nodes":["node1","node2","node3"]}' http://localhost:3000/executions
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":1396873680,"metric_1":"value1","metric_2":"value2","metric_3":"value3","metric_4":"value4"}' http://localhost:3000/execution/B5fe7pnYQzqSItnqXB3N1A
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":1396873690,"metric_1":"value1","metric_2":"value2","metric_3":"value3","metric_4":"value4"}' http://localhost:3000/execution/B5fe7pnYQzqSItnqXB3N1A
//curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":13968736905,"metric_1":"value1","metric_2":"value2","metric_3":"value3","metric_4":"value4"}' http://localhost:3000/execution/B5fe7pnYQzqSItnqXB3N1A
//Search DB:
//curl -XGET 'http://localhost:9200/b5fe7pnyqzqsitnqxb3n1a/_search?pretty=true'
//curl -XGET 'http://localhost:9200/o0wuji4qrcakvwdmbvzenq/_search?pretty=true'
//curl -i -X POST -H 'Content-Type: application/json' -d '{'Timestamp':1,'metric_1':'value1'}' http://localhost:3000/executions/geFQ3bahSE-QTHblpFJayQ
//Browser
//http://localhost:3000/execution/o0Wuji4QRCaKVwDmbVzeNQ/1396622231/1396622234

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
  		  console.log("NODE.js Output starts here .................");
  		  console.log(es_reply);
  		  res.send(es_reply._id);
  	});
});

//Adding a new time to an existing execution and respond the provided ID //NOT WORKING
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
