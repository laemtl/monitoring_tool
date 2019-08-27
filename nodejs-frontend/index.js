const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');

const app = express();
app.use(bodyParser.json());
app.use(cors());

app.use(express.static(__dirname + '/dist'));

app.get('*', (req, res) => {
  res.sendFile(__dirname, '/dist/index.html');
});
 
const port = process.env.PORT || 3000;
app.listen(port, () => console.log(`Server started on port ${port}`));
