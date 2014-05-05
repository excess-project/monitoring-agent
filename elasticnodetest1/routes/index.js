
/*
 * GET home page.
 */

exports.index = function(req, res){
  res.render('index', { title: 'EXCESS Monitoring Framework' });
};

exports.helloworld = function(req, res){
  res.render('helloworld', { title: 'Hello, World!' });
};
