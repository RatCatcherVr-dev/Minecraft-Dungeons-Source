'use strict'

const nativeToStr = require('../../converters/native-to-str')

Object.assign(module.exports, {
    methods: {
        has (obj, key) { return obj.has(key) },
        get (obj, key) { return obj.get(key) },
        set (obj, key, value) { obj.set(key, value) },
        remove (obj, key) { obj.remove(key) },
        forEach (obj, ...args) { obj.forEach(...args) },
        isArray (obj) { return obj.type === 'array' },
        isObject (obj) { return obj.type === 'object' },
        value (obj) { return obj.value() },
        lift (obj) { return nativeToStr.convert(obj) },
        wrap (obj) { return nativeToStr.wrap(obj) },
    }
})