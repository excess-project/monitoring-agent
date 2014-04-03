/**

 * Module dependencies.
 */
var mongoose = require('mongoose');
var db = require('./database');
var post = mongoose.model('post');
var express = require('express')
  , routes = require('./routes')
  , user = require('./routes/user')
  , http = require('http')
  , path = require('path');

var app = express();

app.configure(function(){
  app.set('port', process.env.PORT || 3000);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'jade');
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
});

app.configure('development', function(){
  app.use(express.errorHandler());
});

app.post('/posts', function(req, res){
  var title = req.body.title;
  var content = req.body.content;
  var newPost = post({
    title: title,
    content: content
  });

  newPost.save();
  res.send('Post created');
});

app.get('/posts', function(req, res){
  post.find({}, function(err, post){
    if(post){
      res.send(post);
    }else{
      res.send('No posts in database');
    }
  })
});

app.put('/posts', function(req, res){
  var content = req.body.content;
  var title = req.query.title;
  post.findOne({title: title}, function(err, post){
    if (post){
      post.content = content;
      post.save();
      res.send('Post edited');
    }else{
      res.send('Post doesnt exist');
    }

  })
})

app.delete('/posts', function(req, res){
  var title = req.query.title;
  post.findOne({title: title}, function(err, post){
    if(post){
      post.remove()
      res.send('Post removed')
    }else{
      res.send('Post does not exist')
    }
  })

});

http.createServer(app).listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});
