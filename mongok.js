const MongoClient = require('mongodb').MongoClient;
const url = "mongodb://localhost:27017/";

const mongok_findOne = async (your_db,coll,query) => {
    return new Promise((resolve, reject) => {
       
        MongoClient.connect(url,{ useNewUrlParser: true }, function(err, db) {
            if (err) throw err;
            var dbo = db.db(your_db);
            dbo.collection(coll).findOne(query, function(err, result) {
              if (err) throw err;
              if(result){
                  resolve(result)
              }
              db.close()
            });
          });
    });
}

const mongok_insertOne = async (your_db,coll,query) => {
    return new Promise((resolve, reject) => {
       
        MongoClient.connect(url,{ useNewUrlParser: true }, function(err, db) {
            if (err) throw err;
            var dbo = db.db(your_db);
            dbo.collection(coll).insertOne(query, function(err, result) {
              if (err) throw err;
              if(result){
                  resolve(result)
              }
              db.close()
            });
          });
    });
}
const mongok_updateOne = (your_db,coll,name,mode) =>
{
    return new Promise((resolve, reject) => {
       
        MongoClient.connect(url,{ useNewUrlParser: true }, function(err, db) {
            if (err) throw err;
            var dbo = db.db(your_db);
            mode = {$set: mode}
            dbo.collection(coll).updateOne(name, mode, function(err, result) {
              if (err) throw err;
              if(result){
                  resolve(result)
              }
              db.close()
            });
          });
    });
}

const mongok_updateOne = (your_db,coll,query) =>
{
    return new Promise((resolve, reject) => {
       
        MongoClient.connect(url,{ useNewUrlParser: true }, function(err, db) {
            if (err) throw err;
            var dbo = db.db(your_db);
            
            dbo.collection(coll).deleteOne(query, function(err, result) {
              if (err) throw err;
              if(result){
                  resolve(result)
              }
              db.close()
            });
          });
    });
}
module.exports = 
{
  mongok_findOne,
  mongok_updateOne,
  mongok_insertOne
}
