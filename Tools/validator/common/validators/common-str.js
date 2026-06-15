'use strict'

const fs = require('fs')
const path = require('path')

const { and, or } = require('./core')
const { ast } = require('./core-str')

function locator () {
    return and(
        ast.string(),
        (input, context) => {
            if (!/^[^.]+\.[^.]+\.[^.]+$/.test(input.value())) {
                context.raise(`locator ${input.value()} does not follow the ___.___.___ format`, input)
            }
        }
    )
}

function localizationKey () {
    return and(
        ast.string(),
        (input, context) => {
            if (/[\sA-Z]/.test(input.value())) {
                context.raise(`localization key '${input.value()}' must be lowercase and not contain spaces`, input)
            }
        }
    )
}

function assetPath (options) {
    const basePath = path.join(__dirname, '../../../..', 'Content')
    const isResizable = options == null ? false : options.isResizable

    return and(
        ast.string(),
        (input, context) => {
            const pathCandidate = input.value()

            /*if (!pathCandidate.startsWith('/') && !pathCandidate.startsWith('Decor/')) {
                context.raise(`asset path '${pathCandidate}' must start with '/' or 'Decor/'`, input)
            } else */if (!isResizable) {
                const fullPath = pathCandidate.startsWith('/Game')
                    ? pathCandidate.slice('/Game'.length)
                    : pathCandidate

                if (!fs.existsSync(path.join(basePath, `${fullPath}.uasset`))) {
                    context.raise(`asset path '${pathCandidate}' must point to an existing asset`, input)
                }
            }
        }
    )
}

function weightedId () {
    return or([
        ast.string(),
        ast.shapeOnly({
            'id': ast.string(),
            'weight': ast.optional(ast.number()),
        }),
    ], 'must be an id or a { id, weight }')
}

function getStringOrProperty (stringOrObject, propertyName) {
    return stringOrObject.type === 'string'
        ? stringOrObject.value()
        : stringOrObject.get(propertyName).value()
}

function getStringOrId (stringOrObject) {
    return getStringOrProperty(stringOrObject, 'id')
}

Object.assign(module.exports, {
    locator,
    localizationKey,
    assetPath,
    weightedId,
    getStringOrProperty,
    getStringOrId,
})