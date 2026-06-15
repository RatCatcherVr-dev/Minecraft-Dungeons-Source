'use strict'

const fs = require('fs')
const path = require('path')

const nativeToStr = require('../converters/native-to-str').convert

const basePath = path.join(__dirname, '../../../..', 'Content/data/lovika/objectgroups')

function read (currentFile, parentFile) {
    try {
        return fs.readFileSync(
            path.join(basePath, currentFile),
            { encoding: 'utf8' }
        )
    } catch (ex) {
        console.error(
            parentFile != null
                ? `Could not open ${currentFile} referenced by ${parentFile}`
                : `Could not open ${currentFile}`
        )
        throw ex
    }
}

function parseNative (source, filePath) {
    try {
        return JSON.parse(source)
    } catch (ex) {
        console.error(`Failed to parse ${filePath}`)
        console.error(ex.message)
        throw ex
    }
}

function parseStr (source, filePath) {
    try {
        const data = JSON.parse(source)
        return nativeToStr(data)
    } catch (ex) {
        console.error(`Failed to parse ${filePath}`)
        console.error(ex.message)
        throw ex
    }
}

Object.assign(module.exports, {
    native (currentPath, parentPath) { return parseNative(read(currentPath, parentPath), currentPath) },
    str (currentPath, parentPath) { return parseStr(read(currentPath, parentPath), currentPath) },
})