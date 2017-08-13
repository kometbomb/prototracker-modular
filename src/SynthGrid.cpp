#include "SynthGrid.h"
#include "SDL.h"
#include "ISynth.h"
#include "Renderer.h"
#include "ModularSynth.h"
#include "SynthModule.h"
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
	const int connectorSize = 8;
	
	SDL_Rect area = {0,0,connectorSize,connectorSize};
	
	if (module)
	{
		SDL_Rect moduleArea = getModuleArea(moduleIndex, parent);
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


SDL_Rect SynthGrid::getModuleArea(int index, const SDL_Rect& parent) const
{
	SDL_Rect area = {0, 0, parent.w / gridWidth, parent.h / gridHeight};
	
	area.x = parent.x + area.w * (index % gridWidth);
	area.y = parent.y + area.h * (index / gridWidth);
	
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
	
	const ModularSynth& modularSynth = getModularSynth();
	
	for (int index = 0 ; index < ModularSynth::maxModules ; ++index)
	{
		const SynthModule *module = modularSynth.getModule(index);
		
		if (module != NULL)
		{
			SDL_Rect moduleArea = getModuleArea(index, area);
			renderer.setClip(moduleArea);
			renderer.drawRect(moduleArea, mSelectedModule == index ? Color(255,0,0) : Color(255,255,255));
			
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
					renderer.drawRect(getConnectorArea(index, connectorType, i, area), Color(255,255,0));
				}
			}
		}
	}
	
	renderer.setClip(area);
		
	for (int i = 0 ; i < modularSynth.getNumConnections() ; ++i)
	{
		const SynthConnection& connection = modularSynth.getConnection(i);
		SDL_Rect fromModuleArea = getConnectorArea(connection.fromModule, 1, connection.fromOutput, area);
		SDL_Rect toModuleArea = getConnectorArea(connection.toModule, 0, connection.toInput, area);
		drawWire(renderer, fromModuleArea.x + fromModuleArea.w / 2, fromModuleArea.y + fromModuleArea.h / 2, toModuleArea.x + toModuleArea.w / 2, toModuleArea.y + toModuleArea.h / 2, Color(255,0,0), Color(0,255,0));
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
	return static_cast<ModularSynth&>(mSynth.getOscillator(0));
}


const ModularSynth& SynthGrid::getModularSynth() const
{
	return static_cast<const ModularSynth&>(mSynth.getOscillator(0));
}
