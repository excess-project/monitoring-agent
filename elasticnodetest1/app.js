/**
 * Module dependencies.
 */

var express = require('express');
var routes = require('./routes');
var user = require('./routes/user');
var execution = require('./routes/execution');
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

//get actions
app.get('/', routes.index);
app.get('/ping', execution.ping(client));
app.get('/executions', execution.executions(client));
app.get('/executions/details/:ID', execution.details(client));
app.get('/executions/metrics/:ID', execution.metrics(client));
app.get('/executions/:ID', execution.values(client));
app.get('/executions/:ID/:from/:to', execution.range(client));

//post actions
app.post('/executions', execution.insert(client));
app.post('/executions/:ID', execution.add(client));


http.createServer(app).listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});




