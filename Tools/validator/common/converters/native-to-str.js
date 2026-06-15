'use strict'

const str = require('./str')

function makeObject (data, convert) {
    const object = {}
    Object.entries(data).forEach(([key, value]) => {
        if (key[0] !== '#') {
            object[key] = convert(value)
        }
    })
    // Object.freeze(object)

    const instance = Object.create(str.objectProto)
    instance._value = object

    return instance
}

function makeArray (data, convert) {
    const array = data.map((element) => convert(element))
    // Object.freeze(array)

    const instance = Object.create(str.arrayProto)
    instance._value = array

    return instance
}

function makeLiteral (data) {
    const type = typeof data
    const proto = type === 'number' ? str.numberProto
        : type === 'string' ? str.stringProto
        : type === 'boolean' ? str.booleanProto
        : str.nullProto

    const instance = Object.create(proto)
    instance._value = data

    return instance
}

function convert (data) {
    if (data instanceof Array) {
        return makeArray(data, convert)
    } else if (data != null && typeof data === 'object') {
        return makeObject(data, convert)
    } else {
        return makeLiteral(data)
    }
}

function wrap (data) {
    if (data instanceof Array) {
        const instance = Object.create(str.arrayProto)
        instance._value = data.slice()
        return instance
    } else if (data != null && typeof data === 'object') {
        const instance = Object.create(str.objectProto)
        instance._value = Object.assign({}, data)
        return instace
    } else {
        return makeLiteral(data)
    }
}

Object.assign(module.exports, {
    convert,
    wrap,
})