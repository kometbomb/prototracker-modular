#include "SynthGrid.h"
#include "SDL.h"
#include "ISynth.h"
#include "Renderer.h"
#include "ModularSynth.h"
#include "SynthModule.h"
#include "EditorState.h"
#include "ModuleSelector.h"
#include "Color.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <algorithm>



SynthGrid::SynthGrid(EditorState& editorState, ISynth& synth)
	:Editor(editorState, true), mSynth(synth), mMode(IDLE), mSelectedModule(-1)
{
	mModuleSelector = new ModuleSelector(editorState);
	editorState.patternEditor.currentTrack.addListener(this);
	initNetwork();
}


void SynthGrid::drawWire(Renderer& renderer, int x1, int y1, int x2, int y2, const Color& color1, const Color& color2) const
{
	const int spacing = 8;
	int dx = x1 - x2, dy = y1 - y2;
	int d = std::max(4, static_cast<int>(sqrt(dx * dx + dy * dy) / spacing));
	
	if (d == 0)
		return;
	
	int px = x1, py = y1;
	
	for (int i = 1 ; i <= d ; ++i)
	{
		int x = (x2 - x1) * i / d + x1;
		int y = (y2 - y1) * i / d + y1 + sin((float)i/d * M_PI) * 16;
		
		renderer.renderLine(px, py, x, y + 1, Color(0,0,0));
		renderer.renderLine(px, py, x, y, i % 2 ? color1 : color2);
		
		px = x;
		py = y;
	}
}


void SynthGrid::drawAngledWire(Renderer& renderer, int x1, int y1, int x2, int y2, int y3, const Color& color1) const
{
	renderer.renderLine(x1, y1, x1, y3, color1);
	renderer.renderLine(x1, y3, x2, y3, color1);
	renderer.renderLine(x2, y3, x2, y2, color1);
}


void SynthGrid::startMove(int module)
{
	// Don't start moving from empty slot
	
	if (getModularSynth().getModule(module) == NULL)
		return;
	
	mMode = MOVING_MODULE;
	mFromModule = module;
}


void SynthGrid::endMove(int targetModule)
{
	if (mMode != MOVING_MODULE)
		return;
	
	mMode = IDLE;
	
	getModularSynth().swapModules(mFromModule, targetModule);
}
	

void SynthGrid::startConnect(int fromModule, int toModule, int fromOutput, int toInput)
{
	mMode = CONNECTING_MODULE;
	mFromModule = fromModule;
	mToModule = toModule;
	mFromOutput = fromOutput; 
	mToInput = toInput;
	
	const ModularSynth& modularSynth = getModularSynth();
	
	for (int i = 0 ; i < modularSynth.getNumConnections() ; ++i)
	{
		const SynthConnection& connection = modularSynth.getConnection(i);
		
		if (connection.fromModule == fromModule && connection.fromOutput == fromOutput)
		{
			mFromModule = -1;
			mFromOutput = -1;
			mToModule = connection.toModule;
			mToInput = connection.toInput;
			break;
		}
		else if (connection.toModule == toModule && connection.toInput == toInput)
		{
			mFromModule = connection.fromModule;
			mFromOutput = connection.fromOutput;
			mToModule = -1;
			mToInput = -1;
			break;
		}
	}
}


void SynthGrid::endConnect(int module, int connector)
{
	if (mMode != CONNECTING_MODULE)
		return;
	
	mMode = IDLE;
	
	if (mFromModule != -1)
	{
		mToModule = module;
		mToInput = connector;
	}
	else
	{
		mFromModule = module;
		mFromOutput = connector;
	}
}


SDL_Rect SynthGrid::getConnectorArea(int moduleIndex, int type, int connectorIndex, const SDL_Rect& parent) const
{
	const SynthModule* module = getModularSynth().getModule(moduleIndex);
	const int connectorSize = 5;
	
	SDL_Rect area = {0,0,connectorSize,connectorSize};
	
	if (module)
	{
		SDL_Rect moduleArea = getModuleArea(moduleIndex, parent, true);
		int c;
		
		if (type == 0)
			c = module->getNumInputs();
		else
			c = module->getNumOutputs();
		
		if (c > 0)
		{	
			area.x = moduleArea.x + moduleArea.w * (connectorIndex * 2 + 1) / (c * 2) - connectorSize / 2;
			area.y = moduleArea.y;
		
			if (type == 1)
				area.y += moduleArea.h - area.h - 2;
			else
				area.y += 2;
		}
	}
	
	return area;
}


SDL_Rect SynthGrid::getModuleArea(int index, const SDL_Rect& parent, bool shrinkByMargin) const
{
	SDL_Rect area = {0, 0, parent.w / gridWidth, parent.h / gridHeight};
	
	area.x = parent.x + area.w * (index % gridWidth);
	area.y = parent.y + area.h * (index / gridWidth);
	
	if (shrinkByMargin)
	{
		const int margin = moduleMargin * parent.w / networkWidth;
		area.x += margin / 2;
		area.y += margin / 2;
		area.w -= margin;
		area.h -= margin;
	}
	
	return area;
}


bool SynthGrid::pickModule(int x, int y, const SDL_Rect& area, int& moduleOut, bool includeEmpty)
{
	const ModularSynth& modularSynth = getModularSynth();
	
	for (int index = 0 ; index < ModularSynth::maxModules ; ++index)
	{
		const SynthModule *module = modularSynth.getModule(index);
		
		if (includeEmpty || module != NULL)
		{
			SDL_Rect moduleArea = getModuleArea(index, area);
			SDL_Point point = {x,y};
			
			if (pointInRect(point, moduleArea))
			{
				moduleOut = index;
				return true;
			}
		}
	}
	
	return false;
}


bool SynthGrid::pickConnector(int x, int y, const SDL_Rect& area, int& moduleOut, int& connectorType, int& connector)
{
	if (!pickModule(x, y, area, moduleOut))
		return false;
	
	const ModularSynth& modularSynth = getModularSynth();
	const SynthModule *module = modularSynth.getModule(moduleOut);
	SDL_Point point = {x,y};
	
	for (connectorType = 0 ; connectorType <= 1 ; connectorType++)
	{
		int c;
		
		if (connectorType == 0)
			c = module->getNumInputs();
		else
			c = module->getNumOutputs();
		
		for (int i = 0 ; i < c ; ++i)
		{
			if (pointInRect(point, getConnectorArea(moduleOut, connectorType, i, area)))
			{
				connector = i;
				return true;
			}
		}
	}
	
	return false;
}


void SynthGrid::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);
	
	if (mModal != NULL)
	{
		//mModal->onDraw(renderer, area);
		return;
	}
	
	renderer.clearRect(area, Color(0,0,0));

	/*for (const auto& node : mNetwork)
	{
		SDL_Rect nodeArea = { node.x * area.w / (networkWidth - 1) + area.x, node.y * area.h / (networkHeight - 1) + area.y, 16, 16 };
		for (const auto& neighbor : node.connections)
		{
			const PathFinder::Node neighborNode = mNetwork[neighbor.node];
			SDL_Rect neighborNodeArea = { neighborNode.x * area.w / (networkWidth - 1) + area.x, neighborNode.y * area.h / (networkHeight - 1) + area.y, 16, 16 };
			renderer.renderLine(nodeArea.x, nodeArea.y, neighborNodeArea.x, neighborNodeArea.y, Color(64,64,64));
		}
	}*/
	
	const ModularSynth& modularSynth = getModularSynth();
	
	for (int index = 0 ; index < ModularSynth::maxModules ; ++index)
	{
		const SynthModule *module = modularSynth.getModule(index);
		
		if (module != NULL)
		{
			SDL_Rect moduleArea = getModuleArea(index, area, true);
			renderer.setClip(moduleArea);
			renderer.renderRect(moduleArea, mSelectedModule == index ? Color(96,96,96) : Color(64,64,64));
			
			SDL_Rect textArea = {moduleArea.x + 2, moduleArea.y + moduleArea.h / 2 - 4, 100, 100};
			renderer.renderText(textArea, Color(255,255,255), module->getName());
			
			for (int connectorType = 0 ; connectorType <= 1 ; connectorType++)
			{
				int c;
				
				if (connectorType == 0)
					c = module->getNumInputs();
				else
					c = module->getNumOutputs();
				
				for (int i = 0 ; i < c ; ++i)
				{
					renderer.drawRect(getConnectorArea(index, connectorType, i, area), Color(0,0,0));
				}
			}
		}
	}
	
	renderer.setClip(area);
	
	Color palette[6];
	palette[0] = Color(255,64,64);
	palette[1] = Color(64,255,64);
	palette[2] = Color(255,255,64);
	palette[3] = Color(64,64,255);
	palette[4] = Color(255,64,255);
	palette[5] = Color(128,128,128);
	
	for (int i = 0 ; i < modularSynth.getNumConnections() ; ++i)
	{
		
		const Color wireColor = palette[i % 6];
		const SynthConnection& connection = modularSynth.getConnection(i);
		if (mConnectionPath[i].size() > 1)
		{
			
			const SDL_Point& node = mConnectionPath[i][0];
			SDL_Rect prevArea = { node.x, node.y, 16, 16 };
			for (int p = 0 ; p < mConnectionPath[i].size() ; ++p)
			{
				const SDL_Point& path = mConnectionPath[i][p];
				SDL_Rect nodeArea = { path.x, path.y, 16, 16 };
				renderer.renderLine(nodeArea.x, nodeArea.y, prevArea.x, prevArea.y, wireColor);
				prevArea = nodeArea;
			}
		}
		
		{
			const SDL_Point& node = mConnectionPath[i][0];
			SDL_Rect nodeArea = { node.x, node.y, 16, 16 };
			SDL_Rect fromModuleArea = getConnectorArea(connection.toModule, 0, connection.toInput, area);
			drawAngledWire(renderer, fromModuleArea.x + fromModuleArea.w / 2, fromModuleArea.y + fromModuleArea.h / 2, nodeArea.x, nodeArea.y, nodeArea.y, wireColor);
			renderer.renderRect(fromModuleArea, wireColor);
		}
		
		{
			const SDL_Point& node = mConnectionPath[i][(int)mConnectionPath[i].size() - 1];
			SDL_Rect nodeArea = { node.x, node.y, 16, 16 };
			SDL_Rect fromModuleArea = getConnectorArea(connection.fromModule, 1, connection.fromOutput, area);
			drawAngledWire(renderer, fromModuleArea.x + fromModuleArea.w / 2, fromModuleArea.y + fromModuleArea.h / 2, nodeArea.x, nodeArea.y, nodeArea.y, wireColor);
			renderer.renderRect(fromModuleArea, wireColor);
		}
	}
	
	if (mMode == CONNECTING_MODULE)
	{
		SDL_Rect moduleArea;
		if (mFromModule != -1)
		{
			moduleArea = getConnectorArea(mFromModule, 1, mFromOutput, area);
		}
		else
		{
			moduleArea = getConnectorArea(mToModule, 0, mToInput, area);
		}
		
		drawWire(renderer, mMouseX, mMouseY, moduleArea.x + moduleArea.w / 2, moduleArea.y + moduleArea.h / 2, Color(255, 0,0), Color(0,255,0));
	}
	
	if (mMode == MOVING_MODULE)
	{
		int moduleOut;
		
		if (pickModule(mMouseX, mMouseY, area, moduleOut, true))
		{
			SDL_Rect moduleArea = getModuleArea(moduleOut, area);
			renderer.drawRect(moduleArea, Color(128,128,128));
		}
	}
}


bool SynthGrid::onEvent(SDL_Event& event)
{
	if (mModal != NULL)
	{
		return mModal->onEvent(event);
	}
	
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		// Editor base class should probably do this when clicked
		setFocus(this);

		mMouseX = event.button.x / SCALE;
		mMouseY = event.button.y / SCALE;
		
		int moduleOut, connectorType, connector;
		
		switch (event.button.button)
		{
			case SDL_BUTTON_LEFT:
				
				// Left button starts module connection mode or module move mode
				// depending on if the user clicks the connector or just the module
				// area.
				
				if (pickConnector(event.button.x / SCALE, event.button.y / SCALE, mThisArea, moduleOut, connectorType, connector))
				{
					ModularSynth& modularSynth = getModularSynth();
					
					if (mMode == CONNECTING_MODULE)
					{
						if ((connectorType == 0 && mToModule == -1) || (connectorType == 1 && mToModule != -1))
						{
							if (modularSynth.connectModules(mFromModule != -1 ? mFromModule : moduleOut, mToModule != -1 ? mToModule : moduleOut, mFromOutput != -1 ? mFromOutput : connector, mToInput != -1 ? mToInput : connector))
							{
								endConnect(moduleOut, connector);
								rebuildWires();
								mSelectedModule = -1;
							}
						}
					}
					else if (mMode == IDLE || mMode == MOVING_MODULE)
					{
						// Abort moving if clicking on connectors 
						
						if (connectorType == 0)
						{
							startConnect(-1, moduleOut, -1, connector);
							modularSynth.detachConnection(moduleOut, 0, connector);
							mSelectedModule = moduleOut;
						}
						else
						{
							startConnect(moduleOut, -1, connector, -1);
							modularSynth.detachConnection(moduleOut, 1, connector);
							mSelectedModule = moduleOut;
						}
						
						rebuildWires();
					}
				}
				else if (pickModule(event.button.x / SCALE, event.button.y / SCALE, mThisArea, moduleOut, true))
				{
					if (mMode == IDLE)
					{
						const ModularSynth& modularSynth = getModularSynth();
						if (modularSynth.getModule(moduleOut) != NULL)
						{
							startMove(moduleOut);
						}
						else
						{
							if (event.button.clicks == 2)
							{
								showNewModuleDialog();
							}
						}

						mSelectedModule = moduleOut;						
					}
				}
				
				break;
				
			case SDL_BUTTON_RIGHT:
				if (mMode == CONNECTING_MODULE || mMode == MOVING_MODULE)
				{
					// Abort connecting/moving
					mMode = IDLE;
				}
				break;
		}
		
		return true;
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		if (mMode == MOVING_MODULE)
		{
			int moduleOut;
			if (pickModule(event.button.x / SCALE, event.button.y / SCALE, mThisArea, moduleOut, true) && moduleOut != mFromModule)
			{
				endMove(moduleOut);
				rebuildWires();
				mSelectedModule = -1;
			}
			else
			{
				mMode = IDLE;
			}	
		}
	}		
	else if (event.type == SDL_MOUSEWHEEL)
	{
		int moduleOut;
		
		if (pickModule(mMouseX, mMouseY, mThisArea, moduleOut, false))
		{
			getModularSynth().getModule(moduleOut)->onDial(event.wheel.y < 0 ? -1 : 1);
			setDirty(true);
		}
	}
	else if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
			case SDLK_DELETE:
				ModularSynth& modularSynth = getModularSynth();
				
				if (mSelectedModule != -1 && modularSynth.getModule(mSelectedModule) != NULL)
				{
					modularSynth.removeModule(mSelectedModule);
					rebuildWires();
					mMode = IDLE;
				}
									
				break;
		}
	}
	else if (event.type == SDL_MOUSEMOTION)
	{
		mMouseX = event.motion.x / SCALE;
		mMouseY = event.motion.y / SCALE;
		
		if (mMode == CONNECTING_MODULE)
			setDirty(true);
	}

	return false;
}


void SynthGrid::showNewModuleDialog()
{
	mModuleSelector->populate(getModularSynth());
	setModal(mModuleSelector);
	setFocus(mModuleSelector);
}


void SynthGrid::onFileSelectorEvent(const Editor& moduleSelector, bool accept)
{
	if (accept)
	{
		getModularSynth().addModule(mSelectedModule, static_cast<const ModuleSelector&>(moduleSelector).getSelectedModuleId());
	}
		
	setModal(NULL);
}


ModularSynth& SynthGrid::getModularSynth()
{
	return static_cast<ModularSynth&>(mSynth.getOscillator(mEditorState.patternEditor.currentTrack));
}


const ModularSynth& SynthGrid::getModularSynth() const
{
	return static_cast<const ModularSynth&>(mSynth.getOscillator(0));
}


void SynthGrid::initNetwork()
{
	// Init network graph for pathfinding
	// Also store module X/Y location on network

	mNetwork.resize(networkWidth * networkHeight);
	
	/* The network looks like this (M = module)
	
	0 1 2 3 4 ...
	
	1 |-------|
	  |\|/|\|/|
	2 |-M-|-M-| ... etc.
	  |/|\|/|\|
	3 ---------
	
	:
	*/
	
	for (int y = 0 ; y < networkHeight ; ++y)
	{
		for (int x = 0 ; x < networkWidth ; ++x)
		{
			int networkLocation = x + y * networkWidth;
			
			mNetwork[networkLocation].x = x;
			mNetwork[networkLocation].y = y;
			
			// Link to neighbors
			
			for (int dy = -1 ; dy <= 1 ; ++dy)
			{
				for (int dx = -1 ; dx <= 1 ; ++dx)
				{
					// Skip connection to itself
					if (dx == 0 && dy == 0)
						continue;
					
					if (dx != 0 && dy != 0)
						continue;
					
					if (dx != 0 && dy != 0 && ((x + y) & 1))
						continue;
					
					// Skip vertical connectino for every other column
					
					//if (!(x & 1) && dx == 0)
					//	continue;
					
					//if (!(x & 1) && && dx == 0)
					//	continue;
					
					// Skip horizontal connection for every other row
					
					//if ((y & 1) && dy == 0)
					//	continue;
					
					// Skip connections to outside the network
					if (x + dx >= 0 && x + dx < networkWidth &&
						y + dy >= 0 && y + dy < networkHeight) 
					{
						int neighborLocation = (x + dx)  + (y + dy) * networkWidth;
						mNetwork[networkLocation].connections.push_back(PathFinder::Node::Connection(neighborLocation));
					}
				}
			}
		}
	}
	
	for (int y = 0 ; y < gridHeight ; ++y)
	{
		for (int x = 0 ; x < gridWidth ; ++x)
		{
			int networkLocation = (x * gridResolution + gridResolution / 2) + (y * gridResolution + 1) * networkWidth;
			mModuleLocation[x + y * gridWidth][0] = networkLocation - networkWidth;
			mModuleLocation[x + y * gridWidth][1] = networkLocation + (networkWidth * (gridResolution - 1));
		}
	}
}


int SynthGrid::getConnectorNode(int moduleIndex, int type, int connectorIndex) const
{
	int x = moduleIndex % gridWidth;
	int y = moduleIndex / gridWidth;
	int c, yOffset;
	
	const SynthModule *module = getModularSynth().getModule(moduleIndex);
	
	if (type == 0)
	{
		c = module->getNumInputs();
		yOffset = 1;
	}
	else
	{
		c = module->getNumOutputs();
		yOffset = gridResolution - 1;
	}
				
	// moduleArea.x + moduleArea.w * (connectorIndex * 2 + 1) / (c * 2) - connectorSize / 2;
	
	return (x * gridResolution + gridResolution * (connectorIndex * 2 + 1) / (c * 2)) + (y * gridResolution + yOffset) * networkWidth;
}


void SynthGrid::rebuildWires()
{
	setDirty(true);
	
	// Reset network cost
	
	for (auto& node : mNetwork)
	{
		node.resetCosts();
	}
	
	// Make cost of the module node high
	
	for (int y = 0 ; y < gridHeight ; ++y)
	{
		for (int x = 0 ; x < gridWidth ; ++x)
		{
			if (getModularSynth().getModule(x + y * gridWidth))
			{
				for (int dy = moduleMargin ; dy <= gridResolution - moduleMargin ; ++dy)
				{
					for (int dx = moduleMargin ; dx <= gridResolution - moduleMargin ; ++dx)
					{
						int networkLocation = (x * gridResolution + dx) + (y * gridResolution + dy) * networkWidth;
						mNetwork[networkLocation].cost = 9999;
					}
				}
			}
		}
	}
	
	// Update connection paths
	
	mConnectionPath.clear();
	mConnectionPath.resize(getModularSynth().getNumConnections());
	
	PathFinder pathFinder;
	
	for (int i = 0 ; i < getModularSynth().getNumConnections() ; ++i)
	{
		pathFinder.setNetwork(mNetwork);
		const SynthConnection& connection = getModularSynth().getConnection(i);
		std::vector<int> path = pathFinder.findPath(getConnectorNode(connection.fromModule, 1, connection.fromOutput), getConnectorNode(connection.toModule, 0, connection.toInput));
		
		// Add some cost to nodes used by the connection
		
		int prevNode = -1;
		mConnectionPath[i].clear();
		
		for (auto node : path)
		{
			mNetwork[node].cost += 1;
			if (prevNode != -1)
			{
				PathFinder::Node::Connection& prevConnection = mNetwork[prevNode].findConnectionTo(node);
				prevConnection.timesUsed += 1;
				PathFinder::Node::Connection& nodeConnection = mNetwork[node].findConnectionTo(prevNode);
				nodeConnection.timesUsed += 1;
			}
			prevNode = node;
			
			SDL_Point point = { mNetwork[node].x * mThisArea.w / (networkWidth - 1) + mThisArea.x, mNetwork[node].y * mThisArea.h / (networkHeight - 1) + mThisArea.y};
			mConnectionPath[i].push_back(point);
		}
	}
}


void SynthGrid::onLoaded()
{
	rebuildWires();
}


void SynthGrid::onListenableChange(Listenable *listenable)
{
	rebuildWires();
}
