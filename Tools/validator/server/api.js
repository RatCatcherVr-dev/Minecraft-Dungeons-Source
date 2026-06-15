const fs = require('fs')
const path = require('path')
const express = require('express')
const cors = require('cors')
const bodyParser = require('body-parser')
const eslint = require('eslint')

const config = require('./config')

const app = express()

app.use(cors())
app.use(bodyParser.raw({ type: 'application/json' }))

const linter = new eslint.Linter()
const lintRules = {
    'array-bracket-spacing': ['error', 'never'],
    'array-element-newline': ['error', {
        'multiline': true,
        'minItems': 3,
    }],
    'comma-dangle': ['error', {
        'arrays': 'never',
        'objects': 'never',
    }],
    'comma-spacing': ['error', {
        'before': false,
        'after': true,
    }],
    'indent': ['error', 4, {
        'ArrayExpression': 1,
        'ObjectExpression': 1,
    }],
    'key-spacing': ['error', {
        'beforeColon': false,
        'afterColon': true,
    }],
    'object-curly-newline': ['error', {
        'multiline': true,
        'minItems': 3,
    }],
    'quotes': ['error', 'double'],
    'quote-props': ['error', 'always'],
}

app.get('/', (req, res) => {
    fs.readdir(path.join(__dirname, '..', config['level-path']), (err, filesRaw) => {
        res.append('Content-Type', 'application/text')

        const filesNice = filesRaw.map((file) => file.match(/(.+)\.json$/)[1])
        res.end(JSON.stringify(filesNice))
    })
})

app.get('/:file', (req, res) => {
    fs.readFile(path.join(__dirname, '..', config['level-path'], `${req.params.file}.json`), (err, data) => {
        if (err) {
            res.status(500).send(`could not retrieve ${req.params.file}`)
            throw err
        }

        res.append('Content-Type', 'application/json')
        res.end(data)
    })
})

app.post('/:file', (req, res) => {
    const lintResult = linter.verifyAndFix(`(${String(req.body)})`, { rules: lintRules })

    fs.writeFile(
        path.join(__dirname, '..', config['level-path'], `${req.params.file}.json`),
        lintResult.output.slice(1, -1),
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

if (!fs.existsSync(path.join(__dirname, '..', config['level-path']))) {
    console.error(`please use a validpath for the 'level-path' field in config.json`)
    process.exit(1)
}

app.listen(config['api-port'], () => {
    console.log(`api server started on port ${config['api-port']}`)
    console.log(`open http://127.0.0.1:8005 in your favourite browser`)
})