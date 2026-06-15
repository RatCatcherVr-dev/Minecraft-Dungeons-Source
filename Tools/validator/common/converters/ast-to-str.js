'use strict'

const str = require('./str')

function makeObject (node, parent, key, convert) {
    const object = {}

    const instance = Object.create(str.objectProto)

    node.properties.forEach((property) => {
        const key = property.key.type === 'Identifier'
            ? property.key.name
            : property.key.value

        if (key[0] !== '#') {
            object[key] = convert(property.value, instance, key)
        }
    })

    instance._value = object
    instance.loc = node.loc
    instance.parent = parent
    instance.key = key

    return instance
}

function makeArray (node, parent, key, convert) {
    const instance = Object.create(str.arrayProto)

    const array = node.elements.map((element, index) => convert(element, instance, index))

    instance._value = array
    instance.loc = node.loc
    instance.parent = parent
    instance.key = key

    return instance
}

function makeLiteral (node, parent, key) {
    const type = typeof node.value
    const proto = type === 'number' ? str.numberProto
        : type === 'string' ? str.stringProto
        : type === 'boolean' ? str.booleanProto
        : str.nullProto

    const instance = Object.create(proto)
    instance._value = node.value
    instance.loc = node.loc
    instance.parent = parent
    instance.key = key

    return instance
}

const convert = (() => {
    const makers = new Map([
        ['ObjectExpression', makeObject],
        ['ArrayExpression', makeArray],
        ['Literal', makeLiteral],
    ])

    return (node, parent, key) => {
        if (makers.has(node.type)) {
            return makers.get(node.type)(node, parent, key, convert)
        } else {
            // throw unexpected node ?
        }
    }
})()

Object.assign(module.exports, {
    convert,
})