(() => {
    'use strict'

    fetch(`${location.protocol}//${location.hostname}:8013`)
        .then((response) => response.json())
        .then((bundles) => {
            const list = document.getElementById('bundles')

            bundles.forEach((bundle) => {
                const item = document.createElement('li')
                list.appendChild(item)

                {
                    const result = bundle.match(/(^[^-]+)-(.+)$/)

                    const name = document.createElement('span')
                    item.appendChild(name)

                    name.classList.add('name')
                    name.textContent = result[1]

                    const timestamp = document.createElement('span')
                    item.appendChild(timestamp)

                    timestamp.classList.add('timestamp')
                    timestamp.textContent = result[2]
                }

                const unpack = document.createElement('button')
                item.appendChild(unpack)

                unpack.textContent = 'unpack'
                unpack.addEventListener('click', () => {
                    fetch(`${location.protocol}//${location.hostname}:8013/${bundle}`)
                })
            })
        })
})()