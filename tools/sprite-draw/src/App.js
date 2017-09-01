import React, { Component } from 'react';
import fetch from 'isomorphic-fetch'
import styled from 'styled-components'
import Editor from './Editor'
import * as theme from './theme'

export default class App extends Component {
  
  state = {
    loaded: false,
    data: null
  }
  
  componentDidMount () {
    this.load()
  }
  
  async load () {
    const response = await fetch('/data')
    const data = await response.json();
    this.setState({
      loaded: true,
      data: data
    })
  }
  
  async save () {
    if (this.state.data) {
      const data = await fetch('/data?action=persist', {
        method: 'POST',
        body: JSON.stringify(this.state.data)
      })
      alert('Saved!');
    }
  }
  
  async publish () {
    if (this.state.data) {
      const data = await fetch('/data?action=publish', {
        method: 'POST',
        body: JSON.stringify(this.state.data)
      })
      alert('Published!');
    }
  }
  
  render() {
    if (!this.state.loaded) {
      return (
        <div>Loading</div>
      )
    } else {
      return (
        <AppFrame>
          <Toolbar>
            <Logo>Dog Editor</Logo>
            <ToolButton onClick={e => this.publish()}>Finalize</ToolButton>
            <ToolButton onClick={e => this.load()}>Reload</ToolButton>
            <ToolButton onClick={e => this.save()}>Save</ToolButton>
          </Toolbar>
          <Editor data={this.state.data} totalTilesets={3} />
        </AppFrame>
      )
    }
  }
  
}

const AppFrame = styled.div`
  position: fixed;
  top: 0px;
  left: 0px;
  right: 0px;
  bottom: 0px;
  font-family: Menlo, monospace;
  font-size: 14px;
  color: #333333;
  background: #ffffff;
  display: flex;
  flex-direction: column;
`

const Toolbar = styled.div`
  padding: 10px;
  background: ${theme.toolbarGrey};
  text-align: right;
  position: relative;
`

const Logo = styled.div`
  position: absolute;
  top: 10px;
  left: 10px;
  bottom: 10px;
  width: 100px;
  background: url('/logo.png') no-repeat left center;
  background-size: contain;
  line-height: 25px;
  padding-left: 20px;
`

const ToolButton = styled.button`
  ${theme.buttonStyle};
  margin-left: 10px;
`