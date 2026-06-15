
'use strict'

function and (...validators) {
    return (input, context) => {
        for (let i = 0; i < validators.length; i++) {
            const validator = validators[i]

            const dummyContext = makeContext()
            validator(input, dummyContext)
            if (!dummyContext.isEmpty()) {
                dummyContext.forward(context)
                return
            }
        }
    }
}

function or (validators, message) {
    return (input, context) => {
        for (let i = 0; i < validators.length; i++) {
            const validator = validators[i]

            const dummyContext = makeContext()
            validator(input, dummyContext)
            if (dummyContext.isEmpty()) {
                return
            }
        }

        context.raise(message, input)
    }
}

function variants (getValueOrPairs, pairs) {
    if (arguments.length === 1) {
        return (input, context) => {
            for (const [condition, validator] of pairs) {
                if (condition(input)) {
                    validator(input, context)
                    return
                }
            }

            context.raise('could not satisfy any of the variants', input)
        }
    } else {
        return (input, context) => {
            const switchValue = getValueOrPairs(input)

            for (const [valueOrCondition, validator] of pairs) {
                if (typeof valueOrCondition === 'function') {
                    if (valueOrCondition(switchValue)) {
                        validator(input, context)
                        return
                    }
                } else {
                    if (valueOrCondition === switchValue) {
                        validator(input, context)
                        return
                    }
                }
            }

            context.raise('could not satisfy any of the variants', input)
        }
    }
}

function makeContext (issues = [], type = 'error') {
    function raise (message, extras, typeOverride) {
        issues.push({
            type: typeOverride || type,
            message,
            extras,
        })
    }

    function isEmpty () {
        return issues.length <= 0
    }

    function getIssues () {
        return issues
    }

    function forward (context) {
        issues.forEach((issue) => { context.raise(issue.message, issue.extras, issue.type) })
    }

    function branch (type) {
        return makeContext(issues, type)
    }

    return Object.freeze({
        raise,
        isEmpty,
        getIssues,
        forward,
        branch,
    })
}

function messageType (type, validator) {
    const instance = (input, context) => {
        validator(input, context.branch(type))
    }

    instance.type = type

    return instance
}

Object.assign(module.exports, {
    and,
    or,
    variants,
    messageType,
    makeContext,
})