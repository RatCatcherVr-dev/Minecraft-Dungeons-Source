'use strict'

const fs = require('fs')
const path = require('path')
const { unpack } = require('../server/unpack')

if (process.argv.length != 3) {
    console.error(`use: node unpack.js <bundle>`)
    process.exit(1)
}

try {
    const raw = fs.readFileSync(
        path.join(__dirname, '..', 'bundles/in', `${process.argv[2]}.json`),
        { encoding: 'utf8' },
    )

    unpack(JSON.parse(raw))
} catch (ex) {
    console.error(ex.message)
    process.exit(1)
}