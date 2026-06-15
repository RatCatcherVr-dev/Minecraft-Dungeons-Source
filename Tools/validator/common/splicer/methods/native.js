'use strict'

Object.assign(module.exports, {
    methods: {
        has (obj, key) { return obj.hasOwnProperty(key) },
        get (obj, key) { return obj[key] },
        set (obj, key, value) { obj[key] = value },
        remove (obj, key) { delete obj[key] },
        forEach (obj, ...args) { if (obj != null) { obj.forEach(...args) } },
        isArray (obj) { return obj instanceof Array },
        isObject (obj) { return obj !== null && typeof obj === 'object' },
        value (obj) { return obj },
        lift (obj) { return obj },
        wrap (obj) { return obj },
    }
})