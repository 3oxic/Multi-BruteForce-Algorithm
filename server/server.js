var app = require('express')();

app.get('/', function (req, res) {
    res.send('Hello World!');
});


app.get('/hello2/:id', function (req, res) {
    var id = parseInt(req.params.id, 10);

    res.send('Hello World 2! ' + id);
    //http://example.com/api/users?id=4&token=sdfa3&geo=us
});

app.get('/checkUser/username=:username&password=:password', function (req,res) {

    // recover parameters
    console.log("Had a request");
    var username=req.params.username;
    var password=req.params.password;
    if(username === 'test' && password === 'HelloWorld!'){
        res.send('Correct Details');
    }
    else{
        res.send('Wrong details');
    }


});


app.get('/users/:id', function (req, res) {
    var id = parseInt(req.params.id, 10);

    res.send('Hello World 2! ' + id);
    //http://example.com/api/users?id=4&token=sdfa3&geo=us
});




var server = app.listen(3000, function () {
    var host = server.address().address;
    host = (host === '::' ? 'localhost' : host);
    var port = server.address().port;

    console.log('listening at http://%s:%s', host, port);
});