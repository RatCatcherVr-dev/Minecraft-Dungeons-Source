'use strict'

const path = require('path')


function fixPath (path) {
    // C: -> /c // or whatever drive letter?
    return path.replace(/\\/g, '/')
}

function isHyperMission (file) {
    const parts = path.parse(file)
    return /-hyper$/.test(parts.name)
}

Object.assign(module.exports, {
    fixPath,
    isHyperMission,
})