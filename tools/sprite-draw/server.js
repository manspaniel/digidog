const path = require('path')
const webpack = require('webpack')
const express = require('express')
const serveStatic = require('serve-static')
const config = require('./webpack.config')
const fs = require('fs')

const app = express()
const compiler = webpack(config)

const dataFile = path.join(__dirname, 'data.json')

app.use(require('webpack-dev-middleware')(compiler, {
  publicPath: config.output.publicPath
}))

// app.use(require('webpack-hot-middleware')(compiler))

app.use(serveStatic(path.join(__dirname, 'static')))

// app.get('/', function(req, res) {
//   res.sendFile(path.join(__dirname, 'index.html'))
// })

const defaultData = {
  tiles: [],
  sprites: {
    test: {
      width: 4,
      height: 4
    }
  }
}

app.get('/data', function(req, res) {
  fs.readFile(dataFile, (err, buf) => {
    if (err) {
      res.end(JSON.stringify(defaultData))
    } else {
      try {
        res.end(JSON.stringify(JSON.parse(buf.toString())))
      } catch (err) {
        res.end(JSON.stringify(defaultData))
      }
    }
  })
})

app.post('/data', function(req, res) {
  let bodyStr = '';
  req.on('data', function(chunk) {
    bodyStr += chunk.toString()
  })
  req.on('end', function() {
    const output = JSON.stringify(JSON.parse(bodyStr))
    fs.writeFile(dataFile, output, (err) => {
      res.end()
    })
  })
})

app.listen(3000, function(err) {
  if (err) {
    return console.error(err)
  }
  console.log('Listening at http://localhost:3000/')
})
