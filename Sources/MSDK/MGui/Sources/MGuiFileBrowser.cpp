/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiFileBrowser.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2013 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#include <MEngine.h>
#include <algorithm>
#include "../Includes/MGui.h"


static bool stringCompare(const string &left, const string &right)
{
	for(string::const_iterator lit = left.begin(), rit = right.begin(); lit != left.end() && rit != right.end(); ++lit, ++rit)
	{
		if( tolower( *lit ) < tolower( *rit ) )
			return true;
		else if( tolower( *lit ) > tolower( *rit ) )
			return false;
	}
	
	if(left.size() < right.size())
		return true;
	
	return false;
}

void MGuiFileBrowser::fileBrowserOkButtonEvents(MGuiButton * button, MGUI_EVENT_TYPE event)
{
	switch(event)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
		{
			MGuiFileBrowser * fileBrowser = (MGuiFileBrowser *)button->getCustomPointer();
			fileBrowser->close();
			fileBrowser->m_eventCallback(fileBrowser, MGUI_FILE_BROWSER_EVENT_OK);
            break;
		}
			
        default:
			break;
	}
}

void MGuiFileBrowser::fileBrowserCancelButtonEvents(MGuiButton * button, MGUI_EVENT_TYPE event)
{
	switch(event)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
		{
			MGuiFileBrowser * fileBrowser = (MGuiFileBrowser *)button->getCustomPointer();
			fileBrowser->close();
			fileBrowser->m_eventCallback(fileBrowser, MGUI_FILE_BROWSER_EVENT_CANCEL);
            break;
		}
			
        default:
			break;
	}
}

void MGuiFileBrowser::mainWinEvents(MGuiWindow * window, MGUI_EVENT_TYPE event)
{
	switch(event)
	{
        case MGUI_EVENT_MOUSE_BUTTON_UP:
		{
			MGuiFileBrowser * fileBrowser = (MGuiFileBrowser *)window->getCustomPointer();
			
			unsigned int b, bSize = window->getButtonsNumber();
			for(b=0; b<bSize; b++)
			{
				if(window->getButton(b)->isPressed())
				{
					fileBrowser->selectFile(b);
					break;
				}
			}
			
            break;
		}
			
        default:
			break;
	}
}

void MGuiFileBrowser::dirEditTextEvents(MGuiEditText * editText, MGUI_EVENT_TYPE event)
{
	switch(event)
	{
        case MGUI_EVENT_SEND_VARIABLE:
		{
			MGuiFileBrowser * fileBrowser = (MGuiFileBrowser *)editText->getCustomPointer();
			fileBrowser->updateMainWin();
            break;
		}
			
        default:
			break;
	}
}


MGuiFileBrowser::MGuiFileBrowser(MWindow * rootWindow, MFontRef * font)
{
	m_font = font;
	
	m_fileButtonsWidth = 150;
	m_fileButtonsHeight = 18;
	m_browserHeight = 18;
	m_margin = 3;
	
	m_headWinColor = MVector4(0.2f, 0.3f, 0.4f, 1.0f);
	m_mainWinColor = MVector4(0.15f, 0.15f, 0.15f, 1.0f);
	m_filesWinColor = MVector4(0.4f, 0.6f, 0.8f, 0.3f);
	m_headTextColor = MVector4(0.0f, 0.0f, 0.0f, 1.0f);
	m_browserTextColor = MVector4(1, 1, 1, 0.75f);
	m_buttonColor = MVector4(0.4f, 0.6f, 0.8f, 0.75f);
	m_highButtonColor = MVector4(0.8, 0.9f, 1.0f, 0.75f);
	m_pressButtonColor = MVector4(1.0, 1.0f, 0.5f, 1.0f);
	
	
	m_headWin = rootWindow->addNewWindow();
	m_headWin->setColor(m_headWinColor);
	
	m_dirWin = rootWindow->addNewWindow();
	m_dirWin->setColor(m_filesWinColor);
	m_dirWin->setScrollBarVisible(false);
	
	m_fileWin = rootWindow->addNewWindow();
	m_fileWin->setColor(m_filesWinColor);
	m_fileWin->setScrollBarVisible(false);
	
	m_mainWin = rootWindow->addNewWindow();
	m_mainWin->setColor(m_mainWinColor);
	m_mainWin->setEventCallback(mainWinEvents);
	m_mainWin->setMargin(MVector2(0.0f));
	m_mainWin->setCustomPointer(this);
	
	// texts and buttons
	{
		m_dirEditText = m_dirWin->addNewEditText();
		m_dirEditText->setFont(m_font);
		m_dirEditText->setColor(m_headTextColor);
		m_dirEditText->setEventCallback(dirEditTextEvents);
		m_dirEditText->enableVariable(&m_currentDirectory, M_VARIABLE_STRING);
		m_dirEditText->setSingleLine(true);
		m_dirEditText->setCustomPointer(this);
		
		m_fileEditText = m_fileWin->addNewEditText();
		m_fileEditText->setFont(m_font);
		m_fileEditText->setColor(m_headTextColor);
		m_fileEditText->enableVariable(&m_currentFile, M_VARIABLE_STRING);
		m_fileEditText->setSingleLine(true);
		
		m_okButton = m_headWin->addNewButton();
		m_okButton->setFont(m_font);
		m_okButton->setEventCallback(fileBrowserOkButtonEvents);
		m_okButton->setTextAlign(M_ALIGN_CENTER);
		m_okButton->setAutoScaleFromText(false);
		m_okButton->setTextColor(MVector4(0, 0, 0, 1));
		m_okButton->setText("ok");
		m_okButton->setNormalColor(m_buttonColor);
		m_okButton->setHighLightColor(m_highButtonColor);
		m_okButton->setPressedColor(m_pressButtonColor);
		m_okButton->setCustomPointer(this);
		
		m_cancelButton = m_headWin->addNewButton();
		m_cancelButton->setFont(m_font);
		m_cancelButton->setEventCallback(fileBrowserCancelButtonEvents);
		m_cancelButton->setTextAlign(M_ALIGN_CENTER);
		m_cancelButton->setAutoScaleFromText(false);
		m_cancelButton->setTextColor(MVector4(0, 0, 0, 1));
		m_cancelButton->setText("cancel");
		m_cancelButton->setNormalColor(m_buttonColor);
		m_cancelButton->setHighLightColor(m_highButtonColor);
		m_cancelButton->setPressedColor(m_pressButtonColor);
		m_cancelButton->setCustomPointer(this);
	}
	
	m_headWin->setVisible(false);
	m_dirWin->setVisible(false);
	m_fileWin->setVisible(false);
	m_mainWin->setVisible(false);
}

MGuiFileBrowser::~MGuiFileBrowser(void)
{
	MWindow * rootWindow = m_mainWin->getRootWindow();
	rootWindow->deleteWindow(m_headWin);
	rootWindow->deleteWindow(m_dirWin);
	rootWindow->deleteWindow(m_fileWin);
	rootWindow->deleteWindow(m_mainWin);
}

void MGuiFileBrowser::updateMainWin(void)
{
	m_mainWin->clear();
	
	m_files.clear();
	m_files.push_back(string(".."));
	
	if(readDirectory(m_currentDirectory.getData(), &m_files))
	{
		char filename[256];
		
		
		// prepare
		unsigned int f, fSize =  m_files.size();
		for(f=0; f<fSize; f++)
		{
			const char * name = m_files[f].c_str();
			
			if(f > 0)
			{
				getGlobalFilename(filename, m_currentDirectory.getData(), name);
				if(isDirectory(filename))
					m_files[f] = "/" + m_files[f];
			}
		}
		
		// sort
		sort(m_files.begin(), m_files.end(), stringCompare);
		
		
		// scan
		for(f=0; f<fSize; f++)
		{
			float y = m_browserHeight*f;
			const char * name = m_files[f].c_str();
			
			string textName = name;
			
			float grey = 0.24f;
			if((f%2) == 0)
				grey = 0.2f;
			
			MGuiButton * button = m_mainWin->addNewButton();
			button->setPosition(MVector2(0, y));
			button->setScale(MVector2(m_mainWin->getScale().x, m_browserHeight));
			button->setNormalColor(MVector3(grey*0.9f, grey*1.1f, grey*1.3f));
			button->setHighLightColor(MVector3(0.35f));
			button->setPressedColor(MVector3(0.35f));
			
			MGuiText * text = m_mainWin->addNewText();
			text->setFont(m_font);
			text->setText(textName.c_str());
			text->setPosition(MVector2(0, y));
			text->setColor(m_browserTextColor);
		}
	}
	
	m_mainWin->resizeScroll();
}

void MGuiFileBrowser::resize(const MVector2 & position, const MVector2 & scale)
{
	m_headWin->setPosition(position);
	m_headWin->setScale(MVector2(scale.x, m_fileButtonsHeight*2 + m_margin*3));
	
	m_dirWin->setPosition(position + MVector2(m_margin));
	m_dirWin->setScale(MVector2(scale.x - m_fileButtonsWidth - m_margin*3, m_fileButtonsHeight));
	
	m_fileWin->setPosition(position + MVector2(m_margin) + MVector2(0, m_fileButtonsHeight + m_margin));
	m_fileWin->setScale(MVector2(scale.x - m_fileButtonsWidth - m_margin*3, m_fileButtonsHeight));
	
	m_mainWin->setPosition(position + MVector2(0, m_headWin->getScale().y));
	m_mainWin->setScale(MVector2(scale.x, scale.y - m_headWin->getScale().y));
	
	m_okButton->setXPosition(m_dirWin->getScale().x + m_margin*2);
	m_cancelButton->setXPosition(m_dirWin->getScale().x + m_margin*2);
	
	if(m_mainWin->isVisible())
		updateMainWin();
}

void MGuiFileBrowser::selectFile(unsigned int id)
{
	unsigned int fSize = m_files.size();
	if(id < fSize)
	{
		const char * name = m_files[id].c_str();
		
		if(id == 0) // go up in the hierarchy
		{
			char filename[256];
			getGlobalFilename(filename, m_currentDirectory.getData(), "../");
			
			m_currentDirectory.set(filename);
			updateMainWin();
		}
		else if(strlen(name) > 0)
		{
			bool isDir = (name[0] == '/');
			if(isDir)
				name = name+1;
			
			char filename[256];
			getGlobalFilename(filename, m_currentDirectory.getData(), name);
			
			if(isDir) // navigate
			{
				m_currentDirectory.set(filename);
				updateMainWin();
			}
			else
			{
				m_currentFile.set(name);
			}
		}
	}
}

void MGuiFileBrowser::open(const char * startDirectory, const char * startFile, const char * okName, void (* eventCallback)(MGuiFileBrowser * fileBrowser, MGUI_FILE_BROWSER_EVENT_TYPE event))
{
	bool forceRoot = true;
	
	m_eventCallback = eventCallback;
	
	m_headWin->setVisible(true);
	m_dirWin->setVisible(true);
	m_fileWin->setVisible(true);
	m_mainWin->setVisible(true);
	
	if(startDirectory)
	{
		if(isDirectory(startDirectory))
		{
			m_currentDirectory.set(startDirectory);
			forceRoot = false;
		}
	}
	
	if(startFile)
		m_currentFile.set(startFile);
	
	if(okName)
		m_okButton->setText(okName);
	
	
	if(forceRoot && m_currentDirectory.getData())
	{
		if(isDirectory(m_currentDirectory.getData()))
		{
			forceRoot = false;
		}
	}
	
	if(forceRoot)
	{
		#ifdef _WIN32
			m_currentDirectory.set("C:\\");
		#else
			m_currentDirectory.set("/");
		#endif
	}
	
	updateMainWin();
}

void MGuiFileBrowser::close(void)
{
	m_headWin->setVisible(false);
	m_dirWin->setVisible(false);
	m_fileWin->setVisible(false);
	m_mainWin->setVisible(false);
}
