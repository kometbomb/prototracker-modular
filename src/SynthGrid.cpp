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



SynthGrid::SynthGrid(EditorState& editorState, ISynth& synth)
	:Editor(editorState, true), mSynth(synth), mMode(IDLE), mSelectedModule(-1)
{
	mModuleSelector = new ModuleSelector(editorState);
}


void SynthGrid::drawWire(Renderer& renderer, int x1, int y1, int x2, int y2, const Color& color) const
{
	const int spacing = 4;
	int dx = x1 - x2, dy = y1 - y2;
	int d = sqrt(dx * dx + dy * dy) / spacing;
	
	if (d == 0)
		return;
	
	int px = x1, py = y1;
	
	for (int i = 1 ; i <= d ; ++i)
	{
		int x = (x2 - x1) * i / d + x1;
		int y = (y2 - y1) * i / d + y1 + sin((float)i/d * M_PI) * d * 2;
		
		renderer.renderLine(px, py, x, y + 1, Color(0,0,0));
		renderer.renderLine(px, py, x, y, color);
		
		px = x;
		py = y;
	}
}


void SynthGrid::startMove(int module)
{
	const ModularSynth& modularSynth = static_cast<const ModularSynth&>(mSynth.getOscillator(0));
	
	// Don't start moving from empty slot
	
	if (modularSynth.getModule(module) == NULL)
		return;
	
	mMode = MOVING_MODULE;
	mFromModule = module;
}


void SynthGrid::endMove(int targetModule)
{
	if (mMode != MOVING_MODULE)
		return;
	
	mMode = IDLE;
	
	ModularSynth& modularSynth = static_cast<ModularSynth&>(mSynth.getOscillator(0));
	
	modularSynth.swapModules(mFromModule, targetModule);
}
	

void SynthGrid::startConnect(int fromModule, int toModule, int fromOutput, int toInput)
{
	mMode = CONNECTING_MODULE;
	mFromModule = fromModule;
	mToModule = toModule;
	mFromOutput = fromOutput; 
	mToInput = toInput;
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
	const ModularSynth& modularSynth = static_cast<const ModularSynth&>(mSynth.getOscillator(0));
	const SynthModule* module = modularSynth.getModule(moduleIndex);
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
		
		area.x = moduleArea.x + moduleArea.w / 2 + (connectorIndex - c / 2) * moduleArea.w / c;
		area.y = moduleArea.y;
		
		if (type == 1)
			area.y += moduleArea.h - area.h;
	}
	
	return area;
}


SDL_Rect SynthGrid::getModuleArea(int index, const SDL_Rect& parent) const
{
	SDL_Rect area = {0, 0, parent.w / gridWidth, parent.h / gridHeight};
	
	area.x = parent.x + area.w * (index % gridWidth);
	area.y = parent.y + area.h * (index / gridHeight);
	
	return area;
}


bool SynthGrid::pickModule(int x, int y, const SDL_Rect& area, int& moduleOut, bool includeEmpty)
{
	const ModularSynth& modularSynth = static_cast<const ModularSynth&>(mSynth.getOscillator(0));
	
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
	
	const ModularSynth& modularSynth = static_cast<const ModularSynth&>(mSynth.getOscillator(0));
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
	renderer.clearRect(area, Color(0,0,0));
	
	const ModularSynth& modularSynth = static_cast<const ModularSynth&>(mSynth.getOscillator(0));
	
	for (int index = 0 ; index < ModularSynth::maxModules ; ++index)
	{
		const SynthModule *module = modularSynth.getModule(index);
		
		if (module != NULL)
		{
			SDL_Rect moduleArea = getModuleArea(index, area);
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
		
	for (int i = 0 ; i < modularSynth.getNumConnections() ; ++i)
	{
		const SynthConnection& connection = modularSynth.getConnection(i);
		SDL_Rect fromModuleArea = getConnectorArea(connection.fromModule, 1, connection.fromOutput, area);
		SDL_Rect toModuleArea = getConnectorArea(connection.toModule, 0, connection.toInput, area);
		drawWire(renderer, fromModuleArea.x, fromModuleArea.y, toModuleArea.x, toModuleArea.y, Color(255,0,0));
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
		
		drawWire(renderer, mMouseX, mMouseY, moduleArea.x, moduleArea.y, Color(255, 0,0));
	}
}


bool SynthGrid::onEvent(SDL_Event& event)
{
	if (mModal != NULL)
	{
		setDirty(true);
		return mModal->onEvent(event);
	}
	
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		// Editor base class should probably do this when clicked
		setFocus(this);

		mMouseX = event.button.x;
		mMouseY = event.button.y;
		
		int moduleOut, connectorType, connector;
		
		switch (event.button.button)
		{
			case SDL_BUTTON_LEFT:
				
				// Left button starts module connection mode or module move mode
				// depending on if the user clicks the connector or just the module
				// area.
				
				if (pickConnector(event.button.x, event.button.y, mThisArea, moduleOut, connectorType, connector))
				{
					ModularSynth& modularSynth = static_cast<ModularSynth&>(mSynth.getOscillator(0));
					
					if (mMode == CONNECTING_MODULE)
					{
						if ((connectorType == 0 && mToModule == -1) || (connectorType == 1 && mToModule != -1))
						{
							endConnect(moduleOut, connector);
							modularSynth.connectModules(mFromModule, mToModule, mFromOutput, mToInput);
							mSelectedModule = -1;
						}
					}
					else if (mMode == IDLE)
					{
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
				else if (pickModule(event.button.x, event.button.y, mThisArea, moduleOut, true))
				{
					if (mMode == IDLE)
					{
						const ModularSynth& modularSynth = static_cast<const ModularSynth&>(mSynth.getOscillator(0));
						if (modularSynth.getModule(moduleOut) != NULL)
						{
							startMove(moduleOut);
						}
						else
						{
							showNewModuleDialog();
						}	

						mSelectedModule = moduleOut;						
					}
					else if (mMode == MOVING_MODULE)
					{
						endMove(moduleOut);
						mSelectedModule = -1;
					}
					
					
				}
				
				break;
				
			case SDL_BUTTON_RIGHT:
				if (mMode == CONNECTING_MODULE || mMode == MOVING_MODULE)
				{
					// Abort connecting/moving
					mMode = IDLE;
				}
				else if (pickModule(event.button.x, event.button.y, mThisArea, moduleOut, true))
				{
					ModularSynth& modularSynth = static_cast<ModularSynth&>(mSynth.getOscillator(0));
					modularSynth.removeModule(moduleOut);
				}					
					
				break;
		}
		
		return true;
	}
	
	if (event.type == SDL_MOUSEMOTION)
	{
		mMouseX = event.motion.x;
		mMouseY = event.motion.y;
		
		if (mMode == CONNECTING_MODULE)
			setDirty(true);
	}

	return false;
}


void SynthGrid::showNewModuleDialog()
{
	setModal(mModuleSelector);
	setFocus(mModuleSelector);
}


void SynthGrid::onFileSelectorEvent(const Editor& moduleSelector, bool accept)
{
	if (accept)
	{
		ModularSynth& modularSynth = static_cast<ModularSynth&>(mSynth.getOscillator(0));
		modularSynth.addModule(mSelectedModule, static_cast<const ModuleSelector&>(moduleSelector).getSelectedModuleId());
	}
		
	setModal(NULL);
}