'use strict'

const fs = require('fs')
const { PNG } = require('pngjs')

const { extrude } = require('./art')

fs.createReadStream('./in.png')
    .pipe(new PNG())
    .on('parsed', function () {
        const string = extrude({
            data: this.data,
            width: this.width,
            height: this.height,
        })

        fs.writeFileSync('./out.obj', string)
    })