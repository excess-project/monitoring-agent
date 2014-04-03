
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
app.get('/helloworld', routes.helloworld);
app.get('/users', user.list);

app.get('/ping', function(req, res){
//	res.send(client.info());
	 if (client.ping()) {
	 	var clusterdetails = client.info(true,true);
	 	var clusterdetailsjson = JSON.stringify(clusterdetails);
//	 	console.log(clusterdetails);
	 	console.log(clusterdetailsjson);
	 	// I do not understand why the above printout a string and the one bellow does not.
	 	res.send(clusterdetailsjson);
	 	
	 } else{
	 	res.send('ElasticSearch Cluster is not alive');
	 };
	 
	// res.send();
// This should be an elasticsearch search ;-)	 
/*  post.find({}, function(err, post){
    if(post){
      res.send(post);
    }else{
      res.send('No posts in database');
    }
  })*/
});

function json2array(json){
    var result = [];
    var keys = Object.keys(json);
    keys.forEach(function(key){
        result.push(json[key]);
    });
    return result;
}

app.get('/benchmark', function(req, res){
	  client.search({index:'testindex'},  function(err, result){
	  	//console.log(result.hits);
	  	var only_results = result.hits.hits;
	  	var result = [];
	  	var keys = Object.keys(only_results);
	  	keys.forEach(function(key){
        	result.push(only_results[key]._source);
        	console.log("Adding "+key+" number to result ");
        	console.log(JSON.stringify(result[key]));
    	});
	//  	console.log("Printing only_results :"+only_results);
	  	res.send(result);
	  });

	 
	 //res.send("testing"+result.hits);
// This should be an elasticsearch search ;-)	 
/*  post.find({}, function(err, post){
    if(post){
      res.send(post);
    }else{
      res.send('No posts in database');
    }
  })*/
});

app.post('/benchmark', function(req, res){
  var id = req.body.id;
  var content = req.body.content;
  var the_json = req.body;

  console.log(req.body);
  console.log(req.query);
  console.log(the_json);
  var es_reply = client.index({index:'testindex',type: 'testtype',body:the_json});
  console.log(es_reply);
  res.send('Contents : '+JSON.stringify(the_json));
//  res.send('ids');
  // MongoDB related Part
 /* var newPost = post({
    title: title,
    content: content
  });

  newPost.save();*/
//  res.send('Post created');
});

// Not Working (YET)
app.put('/benchmark', function(req, res){
  var content = req.body.content;
  var id = req.query.title;
  res.send('Content : '+content);
  res.send('id :'+id);
/* MongoDB related Part
  post.findOne({title: title}, function(err, post){
    if (post){
      post.content = content;
      post.save();
      res.send('Post edited');
    }else{
      res.send('Post doesnt exist');
    }
  })*/
})

http.createServer(app).listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});
