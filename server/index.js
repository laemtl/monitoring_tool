const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');

const app = express();
app.use(bodyParser.json());
app.use(cors());

app.use(express.static(__dirname + '/../client/dist'));

require('./ws.js');

app.get('*', (req, res) => {
  res.sendFile(__dirname, '/../dist/index.html');
});

const dashboard = require('./routes/dashboard');
app.use('/dashboard', dashboard);
 
const port = process.env.PORT || 3000;
app.listen(port, () => console.log(`Server started on port ${port}`));
