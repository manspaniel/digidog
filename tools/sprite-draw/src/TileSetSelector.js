import React from 'react'
import styled, { css } from 'styled-components'
  
const scale = 3
const rows = 8
const cols = 8
const spacing = 3

export default class TileSetSelector extends React.Component {
  
  state = {}
  
  constructor (props) {
    super(props)
    
    for (let k = 0; k < rows * cols * props.totalTilesets; k++) {
      if (!props.tiles[k]) {
        const blank = []
        for (let f = 0; f < 64; f++) {
          blank.push(0)
        }
        props.tiles[k] = blank
      }
    }
    
    props.tiles.splice(props.totalTilesets * 64);
  }
  
  setCanvas (el) {
    this.canvas = el
    if (el) {
      this.redraw()
    }
  }
  
  redraw () {
    if (!this.canvas) return
    this.canvas.width = cols * 8 * scale + (cols + 1) * spacing
    this.canvas.height = rows * 8 * scale + (rows + 1) * spacing
    const ctx = this.canvas.getContext('2d')
    
    ctx.fillStyle = '#2044ff'
    ctx.fillRect(0, 0, this.canvas.width, this.canvas.height)
    
    let index = 0
    console.log('Starting at index', index)
    for (let row = 0; row < rows; row++) {
      for (let col = 0; col < cols; col++) {
        const x = spacing + col * (scale * 8 + spacing)
        const y = spacing + row * (scale * 8 + spacing)
        
        // Draw hovering
        let outlineColor
        if (this.props.selectedTile % 64 === index) {
          outlineColor = 'rgb(255, 222, 0)'
        } else if (this.state.hovering % 64 === index || this.props.highlightTile % 64 === index) {
          outlineColor = 'rgb(0, 255, 217)'
        }
        if (outlineColor) {
          ctx.fillStyle = outlineColor
          ctx.fillRect(x - spacing, y - spacing, scale * 8 + spacing * 2, scale * 8 + spacing * 2)
        }
        
        // Draw empty cell
        ctx.fillStyle = '#2b1559'
        ctx.fillRect(x, y, scale * 8, scale * 8)
        
        // Draw individual pixels
        const data = this.props.tiles[index + (this.props.tileset * rows * cols)]
        if (data) {
          let pixelIndex = 0
          for (let py = 0; py < 8; py++) {
            for (let px = 0; px < 8; px++) {
              if (data[pixelIndex]) {
                ctx.fillStyle = data[pixelIndex] === 1 ? '#ffffff' : '#ff00f5'
                ctx.fillRect(x + px * scale, y + py * scale, scale, scale)
              }
              pixelIndex++
            }
          }
        }
        
        index++
      }
    }
  }
  
  getTileFromEvent (e) {
    const bounds = e.target.getBoundingClientRect()
    if (e.clientX < bounds.left || e.clientX > bounds.left + bounds.width || e.clientY < bounds.top || e.clientY > bounds.top + bounds.height) return -1
    const localX = e.clientX - bounds.left
    const localY = e.clientY - bounds.top
    const col = Math.floor((localX - spacing) / (scale * 8 + spacing))
    const row = Math.floor((localY - spacing) / (scale * 8 + spacing))
    return row * cols + col + (this.props.tileset * rows * cols)
  }
  
  hovering (e) {
    if (e) {
      const index = this.getTileFromEvent(e)
      if (this.state.hovering !== index) {
        this.setState({
          hovering: index
        })
      }
    } else {
      this.setState({
        hovering: null
      })
    }
  }
  
  selecting (e) {
    const index = this.getTileFromEvent(e)
    this.setState({
      selected: index
    })
    if (this.props.onSelect) {
      this.props.onSelect(index)
    }
  }
  
  getInfoText (index) {
    if (index === -1) return 'N/A'
    return `#${index} (${index - Math.floor(index / cols) * rows}, ${Math.floor(index / rows)})`
  }
  
  startDrag (e) {
    const index = this.getTileFromEvent(e)
    this.dragging = true
    this.draggingTile = index
    this.redraw()
  }
  
  cancelDrag (e) {
    this.dragging = false
    this.redraw()
  }
  
  endDrag (e) {
    if (this.dragging) {
      // If you hold shift/cmd/ctrl, it'll swap rather than duplicate
      const index = this.getTileFromEvent(e)
      if (e.ctrlKey || e.shiftKey || e.metaKey) {
        // Move
        this.swapTileData(this.draggingTile, index)
      } else {
        // Copy
        this.copyTileData(this.draggingTile, index)
      }
      this.dragging = false
      this.draggingTile = undefined
      this.redraw()
    }
  }
  
  copyTileData (from, to) {
    this.props.tiles[to] = JSON.parse(JSON.stringify(this.props.tiles[from]))
  }
  
  swapTileData (fromIndex, toIndex) {
    const from = this.props.tiles[fromIndex]
    const to = this.props.tiles[toIndex]
    
    console.log('FROM TO', fromIndex, toIndex)
    
    this.props.tiles[fromIndex] = to
    this.props.tiles[toIndex] = from
    
    for (let spriteName in this.props.sprites) {
      const sprite = this.props.sprites[spriteName]
      if ((sprite.tileset || 0) === (this.props.tileset || 0)) {
        for (let k in sprite.data) {
          const item = sprite.data[k]
          if (item) {
            if (item[0] === fromIndex % 64) {
              item[0] = toIndex % 64
            } else if(item[0] === toIndex % 64) {
              item[0] = fromIndex % 64
            }
          }
        }
      }
    }
  }
  
  render () {
    
    let info = ':)'
    if (typeof this.state.hovering === 'number') {
      info = 'HOV: ' + this.getInfoText(this.state.hovering)
    } else if (typeof this.state.selected === 'number') {
      info = 'SEL: ' + this.getInfoText(this.state.selected)
    }
    
    return (
      <Wrapper>
        <InfoBar>{info}</InfoBar>
        <canvas
          ref={el => this.setCanvas(el)}
          onMouseDown={e => {
            e.preventDefault()
            this.startDrag(e)
          }}
          onMouseMove={e => {
            e.preventDefault()
            this.hovering(e)
          }}
          onMouseLeave={e => {
            this.hovering(null)
            this.cancelDrag(e)
          }}
          onMouseUp={e => {
            e.preventDefault()
            this.endDrag(e)
          }}
          onClick={e => this.selecting(e)}
        />
      </Wrapper>
    )
    
  }
  
}

const Wrapper = styled.div`
  padding: 10px;
`

const InfoBar = styled.div`
  color: #666666;
  padding-bottom: 5px;
`