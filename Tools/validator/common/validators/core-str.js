'use strict'

const { and } = require('./core')

const optionalMarker = Symbol()

function any () {
    return (input, context) => {}
}

function nil (message = `must be null`) {
    return (input, context) => {
        if (input.type !== 'null') {
            context.raise(message, input)
        }
    }
}

function ofType (type) {
    return (message = `must be a ${type}`) =>
        (input, context) => {
            if (input.type !== type) {
                context.raise(message, input)
            }
        }
}

const number = ofType('number')
const string = ofType('string')
const boolean = ofType('boolean')
const object = ofType('object')
const array = ofType('array')

function values (values) {
    return (input, context) => {
        if (
            (input.type !== 'number' && input.type !== 'string' && input.type !== 'boolean') ||
            !values.includes(input.value())
        ) {
            context.raise(`value must be one of ${values.join(', ')}`, input)
        }
    }
}

function size (message, range = {}) {
    return and(
        array('must be an array'),
        (input, context) => {
            if (
                (range.min != null && input.size() < range.min) ||
                (range.max != null && input.size() > range.max)
            ) {
                context.raise(message, input)
            }
        }
    )
}

function every (validate) {
    return and(
        array('must be an array'),
        (input, context) => {
            const size = input.size()
            for (let index = 0; index < size; index++) {
                const element = input.get(index)

                if (element == null) {
                    context.raise(`missing element #${index} in array`, input)
                } else {
                    validate(element, context)
                }
            }
        }
    )
}

function tuple (validators, restValidator) {
    return and(
        array('must be a tuple'),
        (input, context) => {
            if (restValidator == null) {
                if (input.size() !== validators.length) {
                    context.raise(`tuple must have ${validators.length} elements`, input)
                    return
                }
            } else {
                if (input.size() < validators.length) {
                    context.raise(`tuple must have at least ${validators.length} elements`, input)
                }
            }

            validators.forEach((validator, index) => {
                const element = input.get(index)
                if (element == null) {
                    context.raise(`missing element #${index} in tuple`, input)
                } else {
                    validator(element, context)
                }
            })

            const size = input.size()
            for (let index = validators.length; index < size; index++) {
                const element = input.get(index)
                if (element == null) {
                    context.raise(`element #${index} in tuple is null or empty`, input)
                } else {
                    restValidator(element, context)
                }
            }
        }
    )
}

function stringifyKeys (keys) {
    return `[${keys.map((key) => `'${key}'`).join(', ')}]`
}

function shape (validators) {
    const keys = Object.keys(validators)

    return and(
        object(`must be an object with ${stringifyKeys(keys)}`),
        (input, context) => {
            keys.forEach((key) => {
                const validator = validators[key]

                if (input.has(key)) {
                    validator(input.get(key), context)
                } else {
                    if (!validator[optionalMarker]) {
                        if (validator.type == null) {
                            context.raise(`missing property '${key}'`, input)
                        } else {
                            context.raise(`missing property '${key}'`, input, validator.type)
                        }
                    }
                }
            })
        }
    )
}

function propsOnly (props) {
    const propSet = new Set(props)

    return and(
        object(`must be an object with ${stringifyKeys(props)}`),
        (input, context) => {
            input.forEach((value,  key) => {
                if (key[0] !== '#' && !propSet.has(key)) {
                    context.raise(
                        `unknown property '${key}'; object must not contain more properties than ${stringifyKeys(props)}`,
                        input,
                    )
                }
            })
        }
    )
}

function shapeOnly (validators) {
    return and(
        shape(validators),
        propsOnly(Object.keys(validators)),
    )
}

function optional (validate) {
    const instance = (input, context) => {
        // do nothing; this exists only to be decorated with optionalMarker
        validate(input, context)
    }

    instance[optionalMarker] = true

    return instance
}

function extractObject (validator) {
    return and(
        object(),
        (input, context) => {
            const object = {}

            input.forEach((value, key) => {
                object[key.value] = value
            })

            validator(object, context)
        }
    )
}

function extractArray (validator) {
    return and(
        array(),
        (input, context) => {
            const array = []

            const size = input.size()
            for (let index = 0; index < size; index++) {
                const element = input.get(index)

                if (element == null) {
                    context.raise(`missing element #${index} in array`, input)
                }

                array.push(element)
            }

            if (array.length === input.size()) {
                validator(array, context)
            }
        }
    )
}

function uniqueIds (message, getId) {
    return and(
        array('must be an array'),
        (input, context) => {
            const ids = new Set

            const size = input.size()
            for (let index = 0; index < size; index++) {
                const element = input.get(index)

                if (element == null) {
                    context.raise(`missing element #${index} in array`, input)
                    return
                } else {
                    const id = getId(element, context)

                    if (ids.has(id)) {
                        context.raise(message, element)
                        return
                    } else {
                        ids.add(id)
                    }
                }
            }
        }
    )
}

function propsXor (props) {
    return and(
        object(`must be an object`),
        (input, context) => {
            let found

            props.forEach((prop) => {
                if (input.has(prop)) {
                    if (found == null) {
                        found = prop
                    } else {
                        context.raise(`properties '${found}' and '${prop}' are mutually exclusive`, input)
                    }
                }
            })
        }
    )
}

function getId (entry, context) {
    if (!entry || entry.type !== 'object') {
        context.raise('must be an object with an id property', entry)
        return null
    }

    if (!entry.has('id')) {
        context.raise('must have an id property', entry)
        return null
    }

    return entry.get('id')
}

Object.assign(module.exports, {
    ast: {
        any,
        nil,
        number,
        string,
        boolean,
        array,
        object,
        values,
        size,
        every,
        tuple,
        shape,
        shapeOnly,
        propsOnly,
        optional,
        extractObject,
        extractArray,
        uniqueIds,
        propsXor,
        util: {
            getId,
        },
    }
})