var mongoose = require('mongoose')
var Schema = mongoose.Schema
    ,ObjectId = Schema.ObjectId;
var db_url = "mongodb://localhost:27017/your_database_name"
var db = mongoose.connect(db_url);

var postSchema = new Schema({
    id: ObjectId,
    title: String,
    content: String
})

var post = db.model('post', postSchema);
