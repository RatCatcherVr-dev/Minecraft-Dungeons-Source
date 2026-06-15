'use strict'

const empty = {
    forEach () {},
    map () { return [] },
    filter () { return [] },
    some () { return false },
    pipe () {},
}

const objectProto = {
    type: 'object',
    has (key) { return this._value.hasOwnProperty(key) },
    get (key) { return this.has(key) ? this._value[key] : empty },
    set (key, value) {
        this._value[key] = value

        if (this.has(key)) {
            if (this._values != null) {
                this._values = null
            }

            if (this._entries != null) {
                this._entries = null
            }
        } else {
            if (this._keys != null) {
                this._keys.push(key)
            }

            if (this._values != null) {
                this._values.push(value)
            }

            if (this._entries != null) {
                this._entries.push([key, value])
            }
        }
    },
    remove (key) {
        if (!this.has(key)) {
            return
        }

        delete this._value[key]

        if (this._keys != null) {
            this._keys = null
        }

        if (this._values != null) {
            this._values = null
        }

        if (this._entries != null) {
            this._entries = null
        }
    },
    size () {
        if (this._keys != null) {
            return this._keys.length
        } else if (this._entries != null) {
            return this._entries.length
        } else if (this._values != null) {
            return this._values.length
        } else {
            return this.entries().length
        }
    },
    isEmpty () { return this.size() <= 0 },
    keys () {
        if (this._keys != null) {
            return this._keys
        } else {
            this._keys = Object.keys(this._value)
            // Object.freeze(this._keys)
            return this._keys
        }
    },
    entries () {
        if (this._entries != null) {
            return this._entries
        } else {
            this._entries = Object.entries(this._value)
            // Object.freeze(this._entries)
            return this._entries
        }
    },
    values () {
        if (this._values != null) {
            return this._values
        } else {
            this._values = Object.values(this._value)
            // Object.freeze(this._values)
            return this._values
        }
    },
    forEach (callback, thisArg) {
        this.entries().forEach(([key, value]) => { callback.call(thisArg, value, key, this._value) })
    },
    pipe (cont) { cont(this) },
}

const arrayProto = {
    type: 'array',
    get (index) { return index < this._value.length ? this._value[index] : empty },
    size () { return this._value.length },
    isEmpty () { return this._value.length <= 0 },
    forEach (...args) { this._value.forEach(...args) },
    map (...args) { return this._value.map(...args) },
    filter (...args) { return this._value.filter(...args) },
    some (...args) { return this._value.some(...args) },
    push (...args) { this._value.push(...args) },
    pipe (cont) { cont(this) },
}

function equalsPrimitive (that) {
    if (this === that) {
        return true
    }

    if (that === null) {
        return this.type === 'null'
    }

    if (typeof that === 'object') {
        return this._value === that._value
    } else {
        return this._value === that
    }
}

const numberProto = {
    type: 'number',
    value () { return this._value },
    equals: equalsPrimitive,
    pipe (cont) { cont(this) },
}

const stringProto = {
    type: 'string',
    value () { return this._value },
    equals: equalsPrimitive,
    pipe (cont) { cont(this) },
}

const booleanProto = {
    type: 'boolean',
    value () { return this._value },
    equals: equalsPrimitive,
    pipe (cont) { cont(this) },
}

const nullProto = {
    type: 'null',
    value () { return null },
    equals: equalsPrimitive,
    pipe (cont) { cont(this) },
}

Object.assign(module.exports, {
    empty,
    objectProto,
    arrayProto,
    numberProto,
    stringProto,
    booleanProto,
    nullProto,
})