const path = require('path')
const express = require('express')

const config = require('./config')

const app = express()

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '../client', 'index.html'))
})

app.get('/samples/*', (req, res) => {
    res.sendFile(path.join(__dirname, '..', req.url))
})

app.get('/*', (req, res) => {
    res.sendFile(path.join(__dirname, '../client', req.url))
})

app.listen(config['app-port'], () => {
    console.log(`file server started on port ${config['app-port']}`)
    console.log(`open http://127.0.0.1:8005 in your favourite browser`)
})