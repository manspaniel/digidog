import React from 'react'
import styled, { css } from 'styled-components'
import * as theme from './theme'

export default class SpriteEditor extends React.Component {
  
  tools = [
    {
      name: 'pencil',
      icon: 'P',
      label: 'Pencil',
      keyCode: 80,
      mousedown (info) {
        let tileRef = info.sprite.data[info.index]
        if (!tileRef) tileRef = info.sprite.data[info.index] = [0, false, false]
        let tile = info.tiles[tileRef[0]]
        let px = info.pixel.x - (info.tilePos.x * 8)
        let py = info.pixel.y - (info.tilePos.y * 8)
        if (tileRef[1]) {
          px = 7 - px
        }
        if (tileRef[2]) {
          py = 7 - py
        }
        tile[py * 8 + px] = info.button === 0 ? 1 : 0
        this.redraw()
        this.tileChanged()
        this.spriteChanged()
      }
    },
    {
      name: 'tile',
      icon: 'T',
      label: 'Place Tile',
      hoverTile: true,
      keyCode: 84,
      mousedown (info) {
        let tileRef = info.sprite.data[info.index]
        if (!tileRef) tileRef = info.sprite.data[info.index] = [0, false, false]
        if (info.rightClick) {
          // Right click eyedrops the current tile!
          if (this.props.onTileChange) {
            this.props.onTileChange(tileRef[0])
          }
        } else {
          // Normal left click draws the selected tile
          tileRef[0] = this.props.selectedTile
          tileRef[1] = false
          tileRef[1] = false
          this.redraw()
          this.spriteChanged()
        }
      }
    },
    {
      name: 'flipX',
      icon: 'X',
      label: 'Flip X',
      hoverTile: true,
      keyCode: 88,
      mousedown (info) {
        let tileRef = info.sprite.data[info.index]
        if (!tileRef) tileRef = info.sprite.data[info.index] = [0, false, false]
        tileRef[1] = !tileRef[1]
        this.redraw()
        this.spriteChanged()
      }
    },
    {
      name: 'flipY',
      icon: 'Y',
      label: 'Flip Y',
      hoverTile: true,
      keyCode: 89,
      mousedown (info) {
        let tileRef = info.sprite.data[info.index]
        if (!tileRef) tileRef = info.sprite.data[info.index] = [0, false, false]
        tileRef[2] = !tileRef[2]
        this.redraw()
        this.spriteChanged()
      }
    }
  ]
  
  constructor (props) {
    super(props)
    
    const toolSetting = global.localStorage.getItem('spriteToolName') || 'pencil'
    const gridSetting = global.localStorage.getItem('spriteGridMode')
    const scaleSetting = global.localStorage.getItem('spriteScale')
    
    let initialTool
    for (let tool of this.tools) {
      if (tool.name === toolSetting) {
        initialTool = tool
      }
    }
    
    this.state = {
      tool: initialTool,
      scale: scaleSetting === 'auto' ? 'auto' : parseInt(scaleSetting),
      grid: gridSetting === 'false' ? false : true
    }
  }
  
  componentDidMount () {
    
    this.handleKeyPress = (e) => {
      for (let tool of this.tools) {
        if (e.keyCode === tool.keyCode) {
          this.setTool(tool)
          break;
        }
      }
    }
    
    this.handleResize = () => {
      this.redraw()
    }
    
    this.handleMouseUp = () => {
      this.mouseDown = false
    }
    
    window.addEventListener('keydown', this.handleKeyPress)
    window.addEventListener('resize', this.handleResize)
    window.addEventListener('mouseup', this.handleMouseUp)
    
  }
  
  componentWillUnmount () {
    window.removeEventListener('keydown', this.handleKeyPress)
    window.removeEventListener('resize', this.handleResize)
    window.removeEventListener('mouseup', this.handleMouseUp)
  }
  
  resizeSprite (width, height) {
    const sprite = this.props.sprite
    const oldData = sprite.data
    const oldWidth = sprite.width
    const oldHeight = sprite.height
    
    sprite.data = []
    sprite.width = width
    sprite.height = height
    
    // Transpose the existing tile layout into the new resized version
    for (let y = 0; y < height; y++) {
      for (let x = 0; x < width; x++) {
        sprite.data.push(oldData[y * oldWidth + x] || [0, false, false])
      }
    }
    this.redraw()
  }
  
  setCanvas (el) {
    this.canvas = el
    this.redraw()
  }
  
  setContainer (el) {
    this.container = el
    this.redraw()
  }
  
  redraw () {
    if (!this.canvas || !this.container) return
    
    this.canvas.width = 1
    this.canvas.height = 1
    const bounds = this.container.getBoundingClientRect()
    const sprite = this.props.sprite
    const tiles = this.props.tiles
    
    let canvasWidth, canvasHeight
    
    if (this.state.scale === 'auto') {
      canvasWidth = Math.floor(bounds.width - 10)
      canvasHeight = canvasWidth * (sprite.height / sprite.width)
      
      if (canvasHeight > bounds.height) {
        canvasHeight = Math.floor(bounds.height - 20)
        canvasWidth = canvasHeight * (sprite.width / sprite.height)
      }
    } else if (typeof this.state.scale === 'number') {
      canvasWidth = sprite.width * this.state.scale * 8
      canvasHeight = sprite.height * this.state.scale * 8
    }
    
    this.canvas.width = canvasWidth
    this.canvas.height = canvasHeight
    const scale = canvasWidth / (8 * sprite.width)
    
    const ctx = this.canvas.getContext('2d')
    ctx.fillRect(0, 0, canvasWidth, canvasHeight)
    
    // Draw each tile
    ctx.fillStyle = '#ffffff'
    for (let row = 0; row < sprite.height; row++) {
      for (let col = 0; col < sprite.width; col++) {
        const data = sprite.data[row * sprite.width + col]
        if (!data) continue
        const tileID = data[0]
        const flipX = data[1]
        const flipY = data[2]
        const tile = this.props.tiles[tileID]
        for (let y = 0; y < 8; y++) {
          for (let x = 0; x < 8; x++) {
            const refX = flipX ? 7 - x : x
            const refY = flipY ? 7 - y : y
            const pixelIndex = refY * 8 + refX
            if (tile[pixelIndex] === 1) {
              ctx.fillRect(
                col * 8 * scale + x * scale,
                row * 8 * scale + y * scale,
                scale,
                scale
              )
            }
          }
        }
      }
    }
    
    // Draw the grid
    if (this.state.grid) {
      ctx.strokeStyle = '#ff0000';
      for (let x = 0; x <= sprite.width; x++) {
        const pos = Math.floor(x * scale * 8 + (x === sprite.width ? -1 : 0)) + 0.5
        ctx.beginPath()
        ctx.moveTo(pos, 0)
        ctx.lineTo(pos, this.canvas.height)
        ctx.stroke()
      }
      for (let y = 0; y <= sprite.height; y++) {
        const pos = Math.floor(y * scale * 8 + (y === sprite.width ? -1 : 0)) + 0.5
        ctx.beginPath()
        ctx.moveTo(0, pos)
        ctx.lineTo(this.canvas.width, pos)
        ctx.stroke()
      }
    }
    
    // Hovering over a tile
    if (this.state.tool && this.state.tool.hoverTile && this.state.hoverTile) {
      ctx.strokeStyle = 'rgb(31, 222, 222)';
      ctx.lineWidth = 3
      ctx.strokeRect(this.state.hoverTile.x * 8 * scale, this.state.hoverTile.y * 8 * scale, 8 * scale, 8 * scale)
    } else if (this.state.tool && this.state.hoverPixel) {
      ctx.strokeStyle = 'rgb(31, 222, 222)';
      ctx.lineWidth = 3
      ctx.strokeRect(this.state.hoverPixel.x * scale, this.state.hoverPixel.y * scale, scale, scale)
    }
  }
  
  getPixelFromEvent (e) {
    const bounds = e.target.getBoundingClientRect()
    const sprite = this.props.sprite
    const px = Math.floor(((e.clientX - bounds.left) / bounds.width) * (sprite.width * 8))
    const py = Math.floor(((e.clientY - bounds.top) / bounds.height) * (sprite.height * 8))
    return { x: px, y: py }
  }
  
  updateMousePosition (e) {
    const pos = this.getPixelFromEvent(e)
    const tile = { x: Math.floor(pos.x / 8), y: Math.floor(pos.y / 8) }
    this.setState({
      hoverPixel: pos,
      hoverTile: tile
    })
    
    // Get the tile itself
    const index = tile.y * this.props.sprite.width + tile.x
    const tileRef = this.props.sprite.data[index]
    if (tileRef) {
      if (this.props.onTileHighlight) {
        this.props.onTileHighlight(tileRef[0])
      }
    }
  }
  
  handleMouseLeave () {
    if (this.props.onTileHighlight) {
      this.props.onTileHighlight(null)
    }
    this.setState({
      hoverPixel: undefined,
      hoverTile: undefined
    })
  }
  
  componentDidUpdate () {
    this.redraw()
  }
  
  handleMouseDown (e, dragging) {
    this.mouseDown = true
    if (!dragging) {
      this.mouseButton = e.button
    }
    const pos = this.getPixelFromEvent(e)
    const tilePos = { x: Math.floor(pos.x / 8), y: Math.floor(pos.y / 8) }
    const index = tilePos.y * this.props.sprite.width + tilePos.x
    if (this.state.tool.mousedown) {
      this.state.tool.mousedown.call(this, {
        button: dragging ? this.mouseButton : e.button,
        rightClick: (dragging ? this.mouseButton : e.button) !== 0,
        tilePos: tilePos,
        index: index,
        sprite: this.props.sprite,
        tiles: this.props.tiles,
        pixel: pos,
        dragging: !!dragging
      })
    }
  }
  
  handleMouseMove (e) {
    this.updateMousePosition(e)
    if (this.mouseDown) {
      this.handleMouseDown(e, true)
    }
  }
  
  spriteChanged () {
    // The sprite has changed in some way!
  }
  
  tileChanged () {
    // A tile has changed in some way
    if (this.props.onTileEdit) {
      this.props.onTileEdit()
    }
  }
  
  setTool (tool) {
    global.localStorage.setItem('spriteToolName', tool.name)
    this.setState({ tool: tool })
  }
  
  cycleScale () {
    let next = this.state.scale
    if (this.state.scale === 'auto') {
      next = 1
    } else if (this.state.scale >= 32) {
      next = 'auto'
    } else {
      next = this.state.scale * 2
    }
    global.localStorage.setItem('spriteScale', next)
    this.setState({ scale: next })
  }
  
  cycleGridMode () {
    let next = !this.state.grid
    global.localStorage.setItem('spriteGridMode', next)
    this.setState({ grid: next })
  }
  
  cycleTileSets () {
    const sprite = this.props.sprite
    sprite.tileSet = ((sprite.tileSet + 1) || 1) % this.props.totalTilesets
    this.forceUpdate()
    if (this.onTileSetChange) {
      this.onTileSetChange()
    }
  }
  
  render () {
    const sprite = this.props.sprite
    
    let settingsPanel = <Settings>
      <SettingsItem>
        {sprite.width}x{sprite.height}
        <Button onClick={e => this.setState({ resizing: true })}>Resize</Button>
      </SettingsItem>
      <SettingsItem>
        <Button onClick={e => this.cycleTileSets()}>Tileset {sprite.tileSet || 0}</Button>
      </SettingsItem>
      <SettingsItem>
        <Button onClick={e => this.cycleScale()}>Scale: {this.state.scale}</Button>
        <Button onClick={e => this.cycleGridMode()}>Grid: {this.state.grid === true ? 'on' : 'off'}</Button>
      </SettingsItem>
    </Settings>
    
    if (this.state.resizing) {
      settingsPanel = <Settings>
        <SettingsItem>
          Width <NumberInput defaultValue={sprite.width} onChange={e => this.setState({ nextWidth: e.target.value })} />
          Height <NumberInput defaultValue={sprite.height} onChange={e => this.setState({ nextHeight: e.target.value })} />
          <Button
            onClick={e => this.resizeSprite(this.state.nextWidth || sprite.width, this.state.nextHeight || sprite.height)}
          >Resize</Button>
          
          <Button onClick={e => this.setState({ resizing: false })}>Cancel</Button>
        </SettingsItem>
      </Settings>
    }
    
    return (
      <Wrapper>
        { settingsPanel }
        <EditPanel>
          <ToolBoxWrapper>
            <ToolBox>
              { this.tools.map(tool => (
                <ToolButton
                  key={tool.name}
                  title={tool.label}
                  active={tool === this.state.tool}
                  onClick={e => this.setTool(tool)}
                >{tool.icon}</ToolButton>
              ))}
            </ToolBox>
          </ToolBoxWrapper>
          <CanvasWrapper innerRef={el => this.setContainer(el)}>
            <canvas
              onMouseMove={e => this.handleMouseMove(e)}
              onMouseDown={e => this.handleMouseDown(e)}
              onMouseLeave={e => this.handleMouseLeave(e)}
              onContextMenu={e => e.preventDefault()}
              ref={el => this.setCanvas(el)}
            />
          </CanvasWrapper>
        </EditPanel>
      </Wrapper>
    )
  }
  
}

const Wrapper = styled.div`
  display: flex;
  flex-direction: column;
  position: relative;
  height: 100%;
  width: 100%;
`

const Settings = styled.div`
  border-bottom: 2px solid #e1e1e1;
  line-height: 20px;
  display: flex;
  
  button {
    margin-left: 10px;
    
    &:last-child {
      margin-right: 0px;
    }
    
    &:first-child {
      margin-left: 0px;
    }
  }
`

const SettingsItem = styled.div`
  padding: 10px;
  border-right: 2px solid #e1e1e1;
  
  &:last-child {
    border-right: 0px;
  }
`

const NumberInput = styled.input`
  display: inline-block;
  margin-right: 10px;
  width: 30px;
  font-family: inherit;
  line-height: inherit;
  font-size: inherit;
  padding: 0px 5px;
  appearance: none;
  border: 0px;
  background: rgb(238, 238, 238);
  border-bottom: 2px solid rgb(172, 172, 172);
  outline: none;
  box-sizing: border-box;
  vertical-align: middle;
`

const Button = styled.button`
  ${theme.buttonStyle};
  line-height: inherit;
  vertical-align: middle;
`

const EditPanel = styled.div`
  display: flex;
  flex: 1 1;
`

const ToolBoxWrapper = styled.div`
  flex: 0 0;
  padding: 10px;
`

const ToolBox = styled.div`
  display: flex;
  flex-direction: column;
`

const ToolButton = styled.div`
  ${theme.buttonStyle};
  flex: 0 0;
  width: auto;
  padding: 2px 10px;
  
  ${props => props.active && css`
    background: rgb(25, 58, 194);
  `}
`

const CanvasWrapper = styled.div`
  flex: 1 1;
  padding-right: 10px;
  padding-top: 10px;
`