const spacer = '  ';

const commentLine = (text) => {
  return '// ' + text + '\n'
}

const variableDeclaration = (name) => {
  return `const uint8_t ${name} [] PROGMEM = `
}

const formatArrayValues = (arr, cols = 8, header = 0) => {
  let str = '{\n' + spacer
  for (let x = 0; x < arr.length; x++) {
    if ((x - header) % cols === 0 && x !== 0) str += '\n' + spacer
    str += arr[x] + (x === arr.length - 1 ? '' : ', ')
  }
  str += '\n};'
  return str
}

const hexify = (n) => {
  const hex = Number(n).toString(16)
  return hex.length == 1 ? '0x0' + hex : '0x' + hex
}

const generateTileCode = (data) => {
  const arr = []
  for (let index in data.tiles.slice(0, 64 * 3)) {
    let tile = data.tiles[index]
    for (let x = 0; x < 8; x++) {
      let color = 0
      let alpha = 0
      for (let y = 0; y < 8; y++) {
        let pixelVal = tile[y * 8 + x]
        if (pixelVal === 1) {
          color = color | (1 << (y))
        }
        if (pixelVal !== 3) {
          alpha = alpha | (1 << (y))
        }
      }
      arr.push(hexify(color))
      arr.push(hexify(alpha))
    }
  }
  return commentLine('The tile bitmap data') + variableDeclaration('TILESET') + formatArrayValues(arr)
}

const generateSpriteCode = (name, sprite) => {
  const arr = []
  const comment = 'Sprite: ' + name + ' / ' + sprite.width + 'x' + sprite.height + ' tiles / ' + sprite.width * 8 + 'x' + sprite.height * 8 + ' pixels / tileset #' + (sprite.tileset || 0)
  
  // Add the header bytes first
  arr.push(hexify(sprite.tileset || 0))
  arr.push(hexify(sprite.width))
  arr.push(hexify(sprite.height))
  
  for (let k = 0; k < sprite.width * sprite.height; k++) {
    const item = sprite.data[k]
    if (!item) {
      arr.push('0x00')
      continue
    }
    let n = item[0]
    if (item[1]) {
      // Flip x
      n = n | 0x80
    }
    if (item[2]) {
      // Flip y
      n = n | 0x40
    }
    arr.push(hexify(n))
  }
  
  return commentLine(comment) + variableDeclaration(name) + formatArrayValues(arr, sprite.width, 3)
}

module.exports = function generateHeaders (data) {
  const sections = []
  
  sections.push('#ifndef TILESET_H\n#define TILESET_H')
  
  sections.push(generateTileCode(data))
  
  for (let key in data.sprites) {
    sections.push(generateSpriteCode(key, data.sprites[key]))
  }
  
  sections.push('#endif')
  
  return sections.join('\n\n')
  
}