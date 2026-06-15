'use strict'

const fs = require('fs')
const path = require('path')

const esprima = require('esprima')

const astToStr = require('../converters/ast-to-str').convert
const { decorateLoc } = require('../decorate')

const basePath = path.join(__dirname, '../../../..', 'Content/data/lovika/levels')

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
        return Function(`return (\n${source}\n)`)()
    } catch (ex) {
        console.error(`Failed to parse ${filePath}`)
        console.error(ex.message)
        throw ex
    }
}

function parseStr (source, filePath) {
    try {
        const program = esprima.parse(`(${source})`, { loc: true })
        const ast = program.body[0].expression
        decorateLoc(ast, { filePath })
        return astToStr(ast)
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