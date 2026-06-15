'use strict'

function decorateLoc (ast, extras) {
    return (function recurse (ast) {
        if (ast.loc != null) {
            Object.assign(ast.loc, extras)
        }

        if (ast.type === 'ObjectExpression') {
            ast.properties.forEach((property) => {
                Object.assign(property.loc, extras)
                recurse(property.value)
            })
        } else if (ast.type === 'ArrayExpression') {
            ast.elements.forEach(recurse)
        }
    })(ast)
}

Object.assign(module.exports, {
    decorateLoc,
})
