
/**
 * Module dependencies.
 //curl -i -X POST -H 'Content-Type: application/json' -d '{"Name": "MyFisrtexecution", "Description": "Testing ","other":"values","onemore":"please"}' http://localhost:3000/executions
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
app.set('view engine', 'jade');
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

//Searching for the values of a specific benchmark //NOT WORKING
app.get('/execution/$ID', function(req, res){
	  client.search({index:'$ID'},  function(err, result)
 		{
 			if (result.hits != undefined){
	  			var only_results = result.hits.hits;
	  			var es_result = [];
	  			var keys = Object.keys(only_results);

	  			keys.forEach(
	  				function(key)
	  					{
        				result.push(only_results[key]._source);
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
	client.index({index:'executions',type: 'TBD',body:the_json},function(err,es_reply)
  	{
  		  console.log(es_reply);
  		  res.send(es_reply);
  	});
});

//Adding a new time to an existing execution and respond the provided ID //NOT WORKING
app.post('/execution/$ID', function(req, res){
  var the_json = req.body;
	client.index({index:'$ID',type: 'TBD',body:the_json},function(err,es_reply)
  	{
  		  console.log(es_reply);
  		  res.send(es_reply);
  	});
});

http.createServer(app).listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});
