//incluse needed nodejs package
const fs = require('fs');
const raw = fs.readFileSync('../test_data.csv', 'utf8');

//split data from csv file
const data  = raw.split(/\r?\n/);

console.log('result', data);


var http = require('http');
//Load the http module.


var server = http.createServer(function (req, resp) {
    resp.writeHead(200, { 'content-type': 'application/json' });
    resp.end(JSON.stringify(data));
});
// Create a webserver with a request listener callback.  This will write the response header with the content type as json, and end the response by sending the MyData array in JSON format.

server.listen(8080);
// Tells the webserver to listen on port 8080(obviously this may be whatever port you want.)