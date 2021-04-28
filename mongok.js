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

const mongok_deleteOne = (your_db,coll,query) =>
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

const mongok_deleteOne = (your_db,coll,query) =>
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

const mongok_createCollection = (your_db,coll,your_text) =>
{
    return new Promise((resolve, reject) => {
       
      MongoClient.connect(url, function(err, db) {
        if (err) throw err;
        var dbo = db.db(your_db);
        dbo.createCollection(coll, function(err, res) {
          if (err) throw err;
          resolve(your_text)
          db.close();
        });
      });
    });
}

const mongok_dropCollection = (your_db,coll,your_text) =>
{
    return new Promise((resolve, reject) => {
       
      MongoClient.connect(url, function(err, db) {
        if (err) throw err;
        var dbo = db.db(your_db);
        dbo.collection(coll).drop(function(err, delOK) {
          if (err) throw err;
          if (delOK) resolve(your_text);
          db.close();
        });
      });
    });
}

const mongok_find_with_limit = (your_db,coll,query,limit) =>
{
    return new Promise((resolve, reject) => {
       
      MongoClient.connect(url, function(err, db) {
        if (err) throw err;
        var dbo = db.db(your_db);
        dbo.collection(coll).find(query).limit(limit).toArray(function(err, result) {
          if (err) throw err;
          resolve(result);
          db.close();
        });
      });
    });
}
module.exports = 
{
  mongok_findOne,
  mongok_updateOne,
  mongok_insertOne,
  mongok_deleteOne,
  mongok_createCollection,
  mongok_dropCollection,
  mongok_find_with_limit
}
