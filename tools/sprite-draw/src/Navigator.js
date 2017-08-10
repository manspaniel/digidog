import React from 'react'
import styled, { css } from 'styled-components'
import * as theme from './theme'

const spriteContainsTile = (sprite, tileID) => {
  // if (typeof tileID !== 'number') return false
  if (tileID == 22 && sprite.name == 'sitting_tongue') debugger
  for (let k = 0; k < sprite.width * sprite.height; k++) {
    let tile = sprite.data[k]
    if (tile && parseInt(tile[0]) === parseInt(tileID)) {
      return true
    }
  }
  return false
}

export default class Navigator extends React.Component {
  
  state = {
    selected: null
  }
  
  create () {
    const name = prompt('Enter a name')
    if (name in this.props.sprites) {
      alert('That name is already taken')
    } else {
      this.props.sprites[name] = {
        width: 4,
        height: 4,
        data: {}
      }
    }
    this.forceUpdate();
  }
  
  selectItem (name) {
    let value
    if (this.props.onSelect) {
      this.props.onSelect(name)
    }
    this.setState({
      selected: name
    })
  }
  
  componentDidMount () {
    
    this.handleWindowClick = (e) => {
      if (this.state.showingMenuFor) {
        this.setState({
          showingMenuFor: null
        })
      }
    }
    window.addEventListener('click', this.handleWindowClick)
    
    this.handleKeyDown = (e) => {
      if (e.key === 'ArrowUp') {
        this.cycleSelection(-1)
        e.preventDefault()
      } else if(e.key === 'ArrowDown') {
        this.cycleSelection(1)
        e.preventDefault()
      }
    }
    window.addEventListener('keydown', this.handleKeyDown)
  }
  
  componentWillUnmount () {
    window.removeEventListener('click', this.handleWindowClick)
    window.removeEventListener('keydown', this.handleKeyDown)
  }
  
  cycleSelection (dir) {
    if (!this.props.selected) return
    const names = Object.keys(this.props.sprites).sort()
    let index = names.indexOf(this.props.selected)
    if (index === -1) return
    index += dir
    if (index < 0 || index > names.length - 1) return
    this.selectItem(names[index])
  }
  
  showContextMenu (spriteName) {
    this.setState({
      showingMenuFor: spriteName
    })
  }
  
  render () {
    
    const items = []
    
    const actions = {
      'Rename': this.props.onRename,
      'Duplicate': this.props.onDuplicate,
      'Delete': this.props.onDelete
    }
    
    for (let key of Object.keys(this.props.sprites).sort()) {
      const sprite = this.props.sprites[key]
      items.push((
        <ListItem
          key={key}
          temporaryHighlight={spriteContainsTile(sprite, this.props.highlightSpritesWithTile)}
          selected={this.props.selected === key}
          onClick={e => this.selectItem(key)}
          onContextMenu={e => {
            e.preventDefault()
            this.showContextMenu(key)
          }}
        >
          {key}
          { this.state.showingMenuFor === key && (
            <ContextMenu>
              { Object.keys(actions).map(label => (
                <MenuItem key={label} onClick={e => {
                  e.preventDefault()
                  e.stopPropagation()
                  this.setState({
                    showingMenuFor: null
                  })
                  actions[label](sprite)
                }}>{label}</MenuItem>
              ))}
            </ContextMenu>
          )}
        </ListItem>
      ))
    }
    
    return (
      <List>
        <ListInner>
          {items}
        </ListInner>
        <Buttons>
          <Button onClick={e => this.create()}>New Sprite</Button>
        </Buttons>
      </List>
    )
  }
  
}

const List = styled.div`
  height: 100%;
  width: 100%;
  display: flex;
  flex-direction: column;
  flex: 1 1;
`

const ListInner = styled.div`
  flex: 1 1;
  background: #ffffff;
  overflow: auto;
`

const ListItem = styled.div`
  display: block;
  padding: 5px;
  cursor: pointer;
  position: relative;
  
  ${props => props.selected && css`
    background: ${props.temporaryHighlight ? 'rgb(86, 201, 101)' : 'rgb(122, 15, 217)'} !important;
    color: white !important;
  `}
  
  ${props => props.temporaryHighlight && css`
    background: rgba(86, 201, 101, 0.2);
  `}
  
  &:hover {
    background: rgb(244, 242, 242);
  }
`

const ContextMenu = styled.div`
  position: absolute;
  right: 20px;
  top: 50%;
  background: white;
  border: 2px solid ${theme.toolbarGrey};
  box-shadow: rgba(0,0,0,0.3) 4px 4px 0px;
  z-index: 100;
`

const MenuItem = styled.div`
  padding: 5px 10px;
  border-bottom: 2px solid ${theme.toolbarGrey};
  color: black;
  
  &:hover {
    background: rgb(255, 136, 225);
    color: white;
  }
  
  &:last-child {
    border-bottom: 0px;
  }
`

const Buttons = styled.div`
  flex: 0 0;
  background: ${theme.toolbarGrey};
  padding: 5px;
  flex: none;
`

const Button = styled.button`
  ${theme.buttonStyle}
`