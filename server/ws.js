const { spawn } = require('child_process');

const WebSocketServer = require('ws').Server,
  wss = new WebSocketServer({port: 40510})

wss.on('connection', function (ws) {
    ws.on('message', function (message) {
        console.log('received: %s', message)
    })
        
    var child = spawn('/home/lae/Dropbox/McGill/internship/dashboard/server/dummy/dummy');
    child.stdout.on('data', function(data) {
        ws.send(`${data}`);
        console.log('stdout: ' + data);
        //Here is where the output goes
    });
    child.stderr.on('data', function(data) {
        console.log('stderr: ' + data);
        //Here is where the error output goes
    });
    child.on('close', function(code) {
        console.log('closing code: ' + code);
        //Here you can get the exit code of the script
    });
});