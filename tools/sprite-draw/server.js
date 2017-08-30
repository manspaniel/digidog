const path = require('path')
const webpack = require('webpack')
const express = require('express')
const serveStatic = require('serve-static')
const config = require('./webpack.config')
const fs = require('fs')
const generateHeaders = require('./generateHeaders')

const app = express()
const compiler = webpack(config)

const dataFile = path.join(__dirname, process.argv[2] || 'data.json')
const headerFile = path.resolve(path.join(__dirname, '../../src/tiles.h'))

app.use(require('webpack-dev-middleware')(compiler, {
  publicPath: config.output.publicPath
}))

app.use(require('webpack-hot-middleware')(compiler))

app.use(serveStatic(path.join(__dirname, 'static')))

const defaultData = {
  tiles: [],
  sprites: {
    untitled: {
      width: 1,
      height: 1,
      data: []
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
    
    try {
    
      // Get the data
      const data = JSON.parse(bodyStr)
      
      if (req.query.action === 'persist') {
        
        // Save JSON file
        const output = JSON.stringify(data)
        fs.writeFileSync(dataFile, output)
        console.log('Persisted to file: ' + dataFile)
        
      } else if(req.query.action === 'publish') {
        
        // Save .h file, if one already exists in the parent project
        fs.writeFileSync(headerFile, generateHeaders(data))
        console.log('Regenerated header file: ' + headerFile)
        
      } else {
        throw new Error('Unknown action')
      }
      
      res.write(JSON.stringify({
        success: true
      }))
    
    } catch(err) {
      
      res.write(JSON.stringify({
        error: err.message
      }))
      
    }
    
    res.end()
  })
})

app.listen(3000, function(err) {
  if (err) {
    return console.error(err)
  }
  console.log('Listening at http://localhost:3000/')
})
