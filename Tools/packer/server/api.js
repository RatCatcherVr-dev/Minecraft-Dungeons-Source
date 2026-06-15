'use strict'

const path = require('path')

const fs = require('fs')
const express = require('express')
const cors = require('cors')
const bodyParser = require('body-parser')

const config = require('./config')

const { unpack } = require('./unpack')

const app = express()

app.use(cors())
app.use(bodyParser.raw({ type: 'application/json', limit: '10mb', extended: true }))

app.get('/', (req, res) => {
    fs.readdir(path.join(__dirname, '..', 'bundles/in'), (err, filesRaw) => {
        res.append('Content-Type', 'application/text')

        const filesNice = filesRaw.map((file) => file.match(/(.+)\.bundle$/)[1])
        res.end(JSON.stringify(filesNice))
    })
})

app.get('/:file', (req, res) => {
    fs.readFile(path.join(__dirname, '..', 'bundles/in', `${req.params.file}.bundle`), (err, data) => {
        if (err) {
            res.status(500).send(`could not retrieve ${req.params.file}`)
            return
        }

        const json = JSON.parse(data)

        res.append('Content-Type', 'application/json')
        res.append('Cache-Control', 'no-cache, no-store, must-revalidate')
        res.append('Expires', '0')

        unpack(json).then(() => {
            res.end(JSON.stringify({
                level: Object.keys(json.level),
                objectGroups: Object.keys(json.objectGroup),
            }))
        }, (filePath) => {
            res.status(500).end(JSON.stringify(filePath))
        })
    })
})

app.post('/:file', (req, res) => {
    fs.writeFile(
        path.join(__dirname, '..', 'bundles/in', `${req.params.file}.bundle`),
        req.body,
        (err) => {
            if (err) {
                console.error('save failed', req.params.file, new Date())
                console.error(err)
                res.status(500).end()
            } else {
                console.log('saved', req.params.file, new Date())
                res.status(202).end()
            }
        }
    )
})

app.listen(config['api-port'], () => {
    console.log(`api server started on port ${config['api-port']}`)
    console.log(`open http://127.0.0.1:${config['app-port']} in your favourite browser`)
})