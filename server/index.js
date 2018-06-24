//modules
const express = require('express');
const bp = require('body-parser');
const Members = require('./members')

/*
  App

*/

const app = express();
const port = 3000

app.set('port', port);
app.use(bp.json())

const members = new Members()
members.add({
	name: "Nish",
	sport: "Tennis"
})
members.add({
	name: "Campbell",
	sport: "Football"
})



const router = express.Router()

app.get('/api/values', (req, res) => {
	return res.send(members.getAll())
	// return res.sendStatus(404);
	// return res.status(404).send({
	// 	"message": "Server down. In Maintenance"
	// })
})

app.get('/api/values/:id', (req, res) => {
	const id = parseInt(req.params.id,10)
	return res.json(members.get(id))
})


app.post('/api/values', (req, res) => {
	const m = members.add(req.body)
	return res.json(m)
})

app.put('/api/values/:id', (req, res) => {
	const m = members.update(parseInt(req.params.id, 10), req.body)
	return res.json(m)
})


app.delete('/api/values/:id', (req, res) => {
	return res.send({
		"delete": members.delete(parseInt(req.params.id, 10))
	})
})
// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handler
app.use(function(err, req, res, next) {
  res.status(err.status || 500).send({
    error: err.msg || err.message || err
  });
});

app.listen(port, ()=>{
  console.log(`Server running on localhost:${port}`);
});
