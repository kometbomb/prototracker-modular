#include "SynthGrid.h"
#include "SDL.h"
#include "ISynth.h"
#include "IPlayer.h"
#include "Renderer.h"
#include "ModularSynth.h"
#include "SynthModule.h"
#include "EditorState.h"
#include "ModuleSelector.h"
#include "FileSelector.h"
#include "FileSection.h"
#include "Color.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>


SynthGrid::SynthGrid(EditorState& editorState, ISynth& synth, IPlayer& player)
	:Editor(editorState, true), mSynth(synth), mPlayer(player), mMode(IDLE), mSelectedModule(-1),
	mHoveredConnection(-1), mCopyBuffer(NULL)
{
	mModuleSelector = new ModuleSelector(editorState);
	mFileSelector = new FileSelector(editorState);
	editorState.patternEditor.currentTrack.addListener(this);
	setModularSynth(static_cast<ModularSynth&>(mSynth.getOscillator(0)));
	initNetwork();
}


SynthGrid::~SynthGrid()
{
	if (mCopyBuffer != NULL)
		delete mCopyBuffer;

	delete mModuleSelector;
	delete mFileSelector;
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

	ModularSynth& synth = getModularSynth();
	synth.lock();
	synth.swapModules(mFromModule, targetModule);
	synth.unlock();
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

	if (module)
	{
		SDL_Rect moduleArea = module->getModuleArea(getModuleArea(moduleIndex, parent, true));
		return module->getConnectorArea(moduleArea, type, connectorIndex);
	}
	else
	{
		SDL_Rect area = {};
		return area;
	}
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


void SynthGrid::drawConnection(Renderer& renderer, const SDL_Rect& area, int index, const Color& wireColor)
{
	const SynthConnection& connection = getModularSynth().getConnection(index);

	if (mConnectionPath[index].size() > 1)
	{

		const SDL_Point& node = mConnectionPath[index][0];
		SDL_Rect prevArea = { node.x, node.y, 16, 16 };
		for (int p = 0 ; p < mConnectionPath[index].size() ; ++p)
		{
			const SDL_Point& path = mConnectionPath[index][p];
			SDL_Rect nodeArea = { path.x, path.y, 16, 16 };
			renderer.renderLine(nodeArea.x, nodeArea.y, prevArea.x, prevArea.y, wireColor);
			prevArea = nodeArea;
		}
	}

	{
		const SDL_Point& node = mConnectionPath[index][0];
		SDL_Rect nodeArea = { node.x, node.y, 16, 16 };
		SDL_Rect fromModuleArea = getConnectorArea(connection.toModule, 0, connection.toInput, area);
		drawAngledWire(renderer, fromModuleArea.x + fromModuleArea.w / 2, fromModuleArea.y + fromModuleArea.h / 2, nodeArea.x, nodeArea.y, nodeArea.y, wireColor);
		renderer.renderRect(fromModuleArea, wireColor);
	}

	{
		const SDL_Point& node = mConnectionPath[index][(int)mConnectionPath[index].size() - 1];
		SDL_Rect nodeArea = { node.x, node.y, 16, 16 };
		SDL_Rect fromModuleArea = getConnectorArea(connection.fromModule, 1, connection.fromOutput, area);
		drawAngledWire(renderer, fromModuleArea.x + fromModuleArea.w / 2, fromModuleArea.y + fromModuleArea.h / 2, nodeArea.x, nodeArea.y, nodeArea.y, wireColor);
		renderer.renderRect(fromModuleArea, wireColor);
	}
}


void SynthGrid::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	if (mModal != NULL)
	{
		//mModal->onDraw(renderer, area);
		return;
	}

	renderer.renderBackground(area);
	
//	SDL_Rect focused = {area.x, area.y + 1, area.w, area.h - 2};
//	renderer.drawRect(focused, Color(255,64,64));

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
			SDL_Rect moduleArea = module->getModuleArea(getModuleArea(index, area, true));
			renderer.setClip(moduleArea);
			module->render(renderer, moduleArea, mSelectedModule == index);

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

	Color palette[9];
	palette[0] = Color(255,64,64);
	palette[1] = Color(64,255,64);
	palette[2] = Color(255,255,64);
	palette[3] = Color(64,64,255);
	palette[4] = Color(255,64,255);
	palette[5] = Color(128,128,128);
	palette[6] = Color(128,0,128);
	palette[7] = Color(255,128,64);
	palette[8] = Color(32,128,32);

	for (int i = 0 ; i < modularSynth.getNumConnections() ; ++i)
	{
		const Color wireColor = palette[i % 9];
		const SynthConnection& connection = modularSynth.getConnection(i);
		if (mConnectionPath.size() > i && mConnectionPath[i].size() > 1)
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
		drawConnection(renderer, area, i, palette[i % 9]);
	}

	if (mHoveredConnection != -1)
	{
		drawConnection(renderer, area, mHoveredConnection, Color(255, 255, 255));
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

		drawAngledWire(renderer, mMouseX, mMouseY, moduleArea.x + moduleArea.w / 2, moduleArea.y + moduleArea.h / 2, (mMouseY + moduleArea.y + moduleArea.h / 2) / 2, Color(255, 255, 255));
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
							modularSynth.lock();
							if (modularSynth.connectModules(mFromModule != -1 ? mFromModule : moduleOut, mToModule != -1 ? mToModule : moduleOut, mFromOutput != -1 ? mFromOutput : connector, mToInput != -1 ? mToInput : connector))
							{
								endConnect(moduleOut, connector);
								modularSynth.unlock();
								rebuildWires();
								mSelectedModule = -1;
								mHoveredConnection = -1;
							}
							else
							{
								modularSynth.unlock();
							}
						}
					}
					else if (mMode == IDLE || mMode == MOVING_MODULE)
					{
						// Abort moving if clicking on connectors

						if (connectorType == 0)
						{
							startConnect(-1, moduleOut, -1, connector);
							modularSynth.lock();
							modularSynth.detachConnection(moduleOut, 0, connector);
							modularSynth.unlock();
							mSelectedModule = moduleOut;
						}
						else
						{
							startConnect(moduleOut, -1, connector, -1);
							modularSynth.lock();
							modularSynth.detachConnection(moduleOut, 1, connector);
							modularSynth.unlock();
							mSelectedModule = moduleOut;
						}

						mHoveredConnection = -1;

						rebuildWires();
					}
				}
				else if (pickModule(event.button.x / SCALE, event.button.y / SCALE, mThisArea, moduleOut, true))
				{
					if (mMode == IDLE)
					{
						mSelectedModule = moduleOut;
						mHoveredConnection = -1;

						ModularSynth& modularSynth = getModularSynth();
						SynthModule *module = modularSynth.getModule(moduleOut);

						if (module != NULL)
						{
							if (event.button.clicks == 1)
							{
								startMove(moduleOut);
							}
							else if (event.button.clicks == 2)
							{
								module->onAction(*this);
							}
						}
						else
						{
							if (event.button.clicks == 2)
							{
								showNewModuleDialog();
							}
						}
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
				mHoveredConnection = -1;
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
			int dialSpeed = 1;
			SDL_Keymod modState = SDL_GetModState();

			if (modState & KMOD_ALT)
				dialSpeed = 5;

			if (modState & KMOD_SHIFT)
				dialSpeed *= 10;

			ModularSynth& synth = getModularSynth();
			synth.lock();
			synth.getModule(moduleOut)->onDial(event.wheel.y < 0 ? -dialSpeed : dialSpeed);
			synth.unlock();
			setDirty(true);
		}
	}
	else if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.mod & (KMOD_CTRL|KMOD_ALT))
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_s:
					displaySaveDialog();
					return true;

				case SDLK_o:
					displayLoadDialog();
					return true;
			}
		}
		else
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_RIGHT:
				{
					ModularSynth& modularSynth = getModularSynth();
					
					if (mSelectedModule + 1 <= ModularSynth::maxModules - 1 && modularSynth.getModule(mSelectedModule + 1) != NULL)
					{
						mSelectedModule += 1;
					}
					else if (modularSynth.getModule(mSelectedModule + 1) == NULL)
					{
						int index = mSelectedModule;
						
						while (index + 1 <= ModularSynth::maxModules - 1)
						{
							index += 1;
							if (modularSynth.getModule(index) != NULL)
							{
								mSelectedModule = index;
								break;
							}
						}
					}
					return true;
				}
					
				case SDLK_LEFT:
				{
					ModularSynth& modularSynth = getModularSynth();
					
					if (mSelectedModule - 1 >= 0 && modularSynth.getModule(mSelectedModule - 1) != NULL)
					{
						mSelectedModule -= 1;
					}
					else if (modularSynth.getModule(mSelectedModule - 1) == NULL)
					{
						int index = mSelectedModule;
						
						while (index - 1 >= 0)
						{
							index -= 1;
							if (modularSynth.getModule(index) != NULL)
							{
								mSelectedModule = index;
								break;
							}
						}
					}
					return true;
				}
					
				case SDLK_UP:
					if (event.key.keysym.mod & (KMOD_SHIFT))
					{
						ModularSynth& modularSynth = getModularSynth();
						int dialSpeed = 1;
//						SDL_Keymod modState = SDL_GetModState();
//						
//						if (modState & KMOD_ALT)
//						{
//							dialSpeed = 5;
//						}
						
						modularSynth.lock();
						modularSynth.getModule(mSelectedModule)->onDial(dialSpeed);
						modularSynth.unlock();
						setDirty(true);
					}
					else
					{
						ModularSynth& modularSynth = getModularSynth();
						
						if(!(mSelectedModule - 4 < 0) && modularSynth.getModule(mSelectedModule - 4) != NULL)
						{
							mSelectedModule -= 4;
						}
						else if (!(mSelectedModule - 4 < 0) && modularSynth.getModule(mSelectedModule - 4) == NULL)
						{
							int index = mSelectedModule;
								
							while (index - 4 >= 0)
							{
								index -= 4;
								if (modularSynth.getModule(index) != NULL)
								{
									mSelectedModule = index;
									break;
								}
							}
						}
					}
					return true;
					
				case SDLK_DOWN:
					if (event.key.keysym.mod & (KMOD_SHIFT))
					{
						ModularSynth& modularSynth = getModularSynth();
						int dialSpeed = 1;
//						SDL_Keymod modState = SDL_GetModState();
//						
//						if (modState & KMOD_ALT)
//						{
//							dialSpeed = 5;
//						}
							
						modularSynth.lock();
						modularSynth.getModule(mSelectedModule)->onDial(-dialSpeed);
						modularSynth.unlock();
						setDirty(true);
					}
					else
					{
						ModularSynth& modularSynth = getModularSynth();
						
						if (!(mSelectedModule + 4 >= ModularSynth::maxModules) && modularSynth.getModule(mSelectedModule + 4) != NULL)
						{
							mSelectedModule += 4;
						}
						else if(!(mSelectedModule + 4 >= ModularSynth::maxModules) && modularSynth.getModule(mSelectedModule + 4) == NULL)
						{
							int index = mSelectedModule;
							
							while (index + 4 <= ModularSynth::maxModules - 1)
							{
								index += 4;
								if (modularSynth.getModule(index) != NULL)
								{
									mSelectedModule = index;
									break;
								}
							}
						}
					}
					return true;
					
				case SDLK_BACKSPACE:
					gotoParentSynth();
					return true;

				case SDLK_F3:
					if (event.key.keysym.mod & (KMOD_SHIFT))
					{
						copySynth();
						for (int index = 0 ; index < ModularSynth::maxModules ; ++index)
						{
							ModularSynth& modularSynth = getModularSynth();
							modularSynth.lock();
							modularSynth.removeModule(index);
							modularSynth.unlock();
//							rebuildWires();
//							mMode = IDLE;
						}
					}
					else
					{
						copySynth();
					}
					return true;

				case SDLK_F4:
					pasteSynth();
					return true;

				case SDLK_DELETE:
					ModularSynth& modularSynth = getModularSynth();

					if (mSelectedModule != -1 && modularSynth.getModule(mSelectedModule) != NULL)
					{
						modularSynth.lock();
						modularSynth.removeModule(mSelectedModule);
						modularSynth.unlock();
						rebuildWires();
						mMode = IDLE;
					}

					return true;
			}
		}
	}
	else if (event.type == SDL_MOUSEMOTION)
	{
		mMouseX = event.motion.x / SCALE;
		mMouseY = event.motion.y / SCALE;
		int previousHoveredConnection = mHoveredConnection;
		mHoveredConnection = -1;

		if (mMode == CONNECTING_MODULE)
		{
			setDirty(true);
		}
		else
		{
			int moduleOut, connectorType, connector;
			if (pickConnector(event.button.x / SCALE, event.button.y / SCALE, mThisArea, moduleOut, connectorType, connector))
			{
				SynthModule *module = getModularSynth().getModule(moduleOut);
				showTooltip(getConnectorArea(moduleOut, connectorType, connector, mThisArea), connectorType == 1 ? module->getOutputName(connector) : module->getInputName(connector));

				if (connectorType == 0)
				{
					mHoveredConnection = findConnectionTo(moduleOut, connector);
				}
				else
				{
					mHoveredConnection = findConnectionFrom(moduleOut, connector);
				}

				if (mHoveredConnection != previousHoveredConnection)
					setDirty(true);
			}
		}
	}

	return false;
}


void SynthGrid::showNewModuleDialog()
{
	mModuleSelector->setId(ModuleSelection);
	mModuleSelector->populate(getModularSynth());
	setModal(mModuleSelector);
}


void SynthGrid::onFileSelectorEvent(const Editor& selector, bool accept)
{
	if (accept)
	{
		int id = reinterpret_cast<const GenericSelector&>(selector).getId();
		switch (id)
		{
			case ModuleSelection: {
				ModularSynth& synth = getModularSynth();
				synth.lock();
				synth.addModule(mSelectedModule, static_cast<const ModuleSelector&>(selector).getSelectedModuleId());
				synth.unlock();
			} break;

			case FileSelectionLoad:
				if (loadSynth(reinterpret_cast<const FileSelector&>(selector).getSelectedPath()))
					showMessage(MessageInfo, "Synth layout loaded");
				break;

			case FileSelectionSave:
				if (saveSynth(reinterpret_cast<const FileSelector&>(selector).getSelectedPath()))
					showMessage(MessageInfo, "Synth saved");
				else
					showMessage(MessageError, "Synth layout was not saved");
				break;
		}
	}

	setModal(NULL);
}


ModularSynth& SynthGrid::getModularSynth()
{
	return *mCurrentModularSynth;
}


const ModularSynth& SynthGrid::getModularSynth() const
{
	return *mCurrentModularSynth;
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

	return (x * gridResolution + gridResolution * (connectorIndex * 2 + 1) / std::max(1, c * 2)) + (y * gridResolution + yOffset) * networkWidth;
}


void SynthGrid::rebuildWires()
{
	setDirty(true);

	// Unlock the player while the wires are rerouted...
	// does not have any effect other than visual so we
	// don't need the lock while it does its thing (which
	// could take >500 ms.

	// Also, from init we don't want to unlock this since
	// It's not locked yet (the below .lock()) would cause
	// a freeze.

	mPlayer.unlock();

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
			const SynthModule *module = getModularSynth().getModule(x + y * gridWidth);

			if (module != NULL)
			{
				// Figure out the margin in pathfinder grid resolution
				SDL_Rect cellArea = getModuleArea(x + y * gridWidth, mThisArea, true);
				SDL_Rect moduleArea = module->getModuleArea(cellArea);

				// Margin is relative to the module rect and the surrounding grid rect
				int moduleMarginX = moduleMargin + (moduleArea.x - cellArea.x) * gridResolution / cellArea.w;
				int moduleMarginY = moduleMargin + (moduleArea.y - cellArea.y) * gridResolution / cellArea.h;

				for (int dy = moduleMarginY ; dy <= gridResolution - moduleMarginY ; ++dy)
				{
					for (int dx = moduleMarginX ; dx <= gridResolution - moduleMarginX ; ++dx)
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

		// Skip unloaded modules
		if (getModularSynth().getModule(connection.fromModule) != NULL &&
			getModularSynth().getModule(connection.toModule) != NULL)
		{
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

	// Lock it again since there could be something related
	// to the audio happening later.

	mPlayer.lock();
}


void SynthGrid::onLoaded()
{
	rebuildWires();
	notify();
}


void SynthGrid::onListenableChange(Listenable *listenable)
{
	mHoveredConnection = -1;
	mSelectedModule = -1;
	mParentSynth = std::stack<ModularSynth*>();
	setModularSynth(static_cast<ModularSynth&>(mSynth.getOscillator(mEditorState.patternEditor.currentTrack)));
}


void SynthGrid::setModularSynth(ModularSynth& synth, bool rememberParent)
{
	if (rememberParent)
	{
		mParentSynth.push(mCurrentModularSynth);
	}

	mCurrentModularSynth = &synth;
	mSelectedModule = -1;
	mHoveredConnection = -1;
	mMode = IDLE;
	refreshView();
}


void SynthGrid::copySynth()
{
	if (mCopyBuffer != NULL)
		delete mCopyBuffer;

	mCopyBuffer = getModularSynth().clone();

	showMessageV(MessageInfo, "Synth layout copied");
}


void SynthGrid::pasteSynth()
{
	ModularSynth& synth = getModularSynth();
	synth.lock();
	synth.copy(*mCopyBuffer);
	synth.unlock();

	rebuildWires();
	
	refreshView();
	
	showMessageV(MessageInfo, "Synth layout pasted");

	notify();
}


int SynthGrid::findConnectionFrom(int fromModule, int fromOutput) const
{
	const ModularSynth& synth = getModularSynth();

	for (int i = 0 ; i < synth.getNumConnections() ; ++i)
	{
		const SynthConnection& connection = synth.getConnection(i);

		if (connection.fromModule == fromModule && connection.fromOutput == fromOutput)
			return i;
	}

	return -1;
}


int SynthGrid::findConnectionTo(int toModule, int toInput) const
{
	const ModularSynth& synth = getModularSynth();

	for (int i = 0 ; i < synth.getNumConnections() ; ++i)
	{
		const SynthConnection& connection = synth.getConnection(i);

		if (connection.toModule == toModule && connection.toInput == toInput)
			return i;
	}

	return -1;
}


void SynthGrid::gotoParentSynth()
{
	if (!mParentSynth.empty())
	{
		ModularSynth* prev = mParentSynth.top();
		mParentSynth.pop();
		setModularSynth(*prev);
	}
}


void SynthGrid::displayLoadDialog()
{
	mFileSelector->setId(FileSelectionLoad);
	mFileSelector->setTitle("Load synth");
	mFileSelector->setFilter(".synth");
	mFileSelector->setOverwriteCheck(false);
	mFileSelector->populate();
	setModal(mFileSelector);
}


void SynthGrid::displaySaveDialog()
{
	mFileSelector->setId(FileSelectionSave);
	mFileSelector->setTitle("Save synth");
	mFileSelector->setFilter(".synth");
	mFileSelector->setOverwriteCheck(true);
	mFileSelector->populate();
	setModal(mFileSelector);
}


bool SynthGrid::saveSynth(const char *path)
{
	FileSection *section = FileSection::createSection("SNTH");
	getModularSynth().writeSynth(*section);

	FILE *f = fopen(path, "wb");
	fwrite(section->getPackedData(), section->getPackedSize(), 1, f);
	fclose(f);

	delete section;

	return true;
}


bool SynthGrid::loadSynth(const char *path)
{
	FILE *f = fopen(path, "rb");

	if (f)
	{
		fseek(f, 0, SEEK_END);
		int fileSize = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *data = new char[fileSize];

		fread(data, fileSize, 1, f);
		fclose(f);

		FileSection *section = FileSection::openSection(data, fileSize);
		int offset = 0;

		if (!section || strcmp(section->getName(), "SNTH") != 0 ||
			!getModularSynth().readSynth(*section, offset))
		{
			showMessage(MessageError, "Failed to read synth layout");

			delete section;
			delete[] data;
			return false;
		}

		delete section;
		delete[] data;

		refreshView();

		return true;
	}

	return false;
}


void SynthGrid::refreshView()
{
	mCurrentModularSynth->onShow();
	rebuildWires();
	invalidateAll();
	notify();
}
