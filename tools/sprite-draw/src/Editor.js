import React, { Component } from 'react';
import styled from 'styled-components'
import Navigator from './Navigator'
import TileSetSelector from './TileSetSelector'
import SpriteEditor from './SpriteEditor'
import * as theme from './theme'

export default class App extends Component {
  
  state = {
    
  }
  
  promptSpriteRename (sprite) {
    const name = prompt('Enter a new name for the sprite')
    if (!name.match(/^[a-z_][a-z0-9_]*$/i)) {
      alert('Invalid name')
      return
    }
    if (this.props.data.sprites[name]) {
      alert('That name is already taken!')
      return
    }
    if (!sprite.name) {
      alert('The selected sprite doesnt have a name property...')
    }
    
    // Delete the old one
    delete(this.props.data.sprites[sprite.name])
    
    // Update the name and add it back to the sprite list
    sprite.name = name
    this.props.data.sprites[name] = sprite
    
    // Render
    this.setState({
      sprite: sprite
    })
  }
  
  promptSpriteDuplicate (sprite) {
    const name = prompt('Enter a new name for the sprite')
    if (!name.match(/^[a-z_][a-z0-9_]*$/i)) {
      alert('Invalid name')
      return
    }
    if (this.props.data.sprites[name]) {
      alert('That name is already taken!')
      return
    }
    
    // Copy the sprite and add it!
    const newSprite = JSON.parse(JSON.stringify(sprite))
    newSprite.name = name
    this.props.data.sprites[name] = newSprite
    
    // Render
    this.setState({
      sprite: newSprite
    })
  }
  
  promptSpriteDelete (sprite) {
    if(!confirm('Are you super certain you wanna get rid of the selected sprite?')) return
    
    if (!sprite.name) {
      alert('The selected sprite doesnt have a name property...')
    }
    
    // Delete the old one
    delete(this.props.data.sprites[sprite.name])
    
    // Render
    this.setState({
      sprite: null
    })
  }
  
  render () {
    return (
      <EditorWrapper>
        <Sidebar>
          <Heading>Navigator</Heading>
          <Navigator
            sprites={this.props.data.sprites}
            selected={this.state.sprite && this.state.sprite.name}
            highlightSpritesWithTile={this.state.selectedTile}
            onSelect={spriteName => {
              const sprite = this.props.data.sprites[spriteName]
              sprite.name = spriteName
              this.setState({
                sprite: sprite
              })
            }}
            onRename={(sprite) => {
              this.promptSpriteRename(sprite)
            }}
            onDuplicate={(sprite) => {
              this.promptSpriteDuplicate(sprite)
            }}
            onDelete={(sprite) => {
              this.promptSpriteDelete(sprite)
            }}
          />
        </Sidebar>
        <Section>
          <Heading>Sprite</Heading>
          { this.state.sprite
            ? <SpriteEditor
                key={this.state.sprite.name}
                sprite={this.state.sprite}
                tiles={this.props.data.tiles}
                selectedTile={this.state.selectedTile}
                totalTilesets={this.props.totalTilesets}
                onTileHighlight={tileIndex => {
                  this.setState({
                    highlightTile: tileIndex
                  })
                }}
                onTileChange={tileIndex => {
                  this.setState({
                    selectedTile: tileIndex
                  })
                }}
              />
            : <PickASprite>Pick a sprite</PickASprite>
          }
        </Section>
        <Section auto>
          <Heading>Tiles</Heading>
          <TileSetSelector
            tiles={this.props.data.tiles}
            highlightTile={this.state.highlightTile}
            selectedTile={this.state.selectedTile}
            onSelect={tileIndex => {
              this.setState({
                selectedTile: tileIndex
              })
            }}
          />
        </Section>
      </EditorWrapper>
    )
  }
  
}

const EditorWrapper = styled.div`
  display: flex;
  flex-direction: row;
  height: 100%;
`

const Sidebar = styled.div`
  width: 250px;
  height: 100%;
  display: flex;
  flex-direction: column;
`

const Section = styled.div`
  border-left: 2px solid ${theme.toolbarGrey};
  display: flex;
  flex-direction: column;
  flex: 1 1;
  
  ${props => props.auto && `
    width: auto;
    flex: 0 0 ;
  `}
`

const Heading = styled.div`
  background: ${theme.darkerGrey};
  padding: 5px;
  text-transform: uppercase;
`

const PickASprite = styled.div`
  padding: 5px;
  color: #777777;
`