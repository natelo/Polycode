/*
 Copyright (C) 2012 by Ivan Safrin
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
 
#include "TextureBrowser.h"

AssetEntry::AssetEntry(String assetPath, String assetName, String extension) : UIElement() {

	this->assetPath = assetPath;

	if(assetName.length() > 20)
		assetName = assetName.substr(0,20)+"...";

	selectShape = new UIRect(120, 100);
	selectShape->visible = false;
	selectShape->setAnchorPoint(-1.0, -1.0, 0.0);
	addChild(selectShape);
	selectShape->processInputEvents = true;
	selectShape->setColor(0.0, 0.0, 0.0, 0.5);
    selectShape->loadTexture("browserIcons/large_selector.png");
    selectShape->setBlendingMode(Renderer::BLEND_MODE_NORMAL);

	imageShape = new UIRect(64,64);
	imageShape->setAnchorPoint(-1.0, -1.0, 0.0);
	addChild(imageShape);
	
	extension = extension.toLowerCase();
	
	if(extension == "png") {
		imageShape->loadTexture(assetPath);
	} else if(extension == "ogg" || extension == "wav") {
		imageShape->loadTexture("browserIcons/sound_icon.png");
	} else if(extension == "entity") {
		imageShape->loadTexture("browserIcons/entity_icon");
	} else if(extension == "sprite") {
		imageShape->loadTexture("browserIcons/sprite_icon.png");
	} else if(extension == "ttf" || extension == "otf") {
		imageShape->loadTexture("browserIcons/font_icon.png");
	} else if(extension == "vert" || extension == "frag") {
		imageShape->loadTexture("browserIcons/shader_icon.png");
	} else if(extension == "mesh") {
		imageShape->loadTexture("browserIcons/mesh_icon.png");
	}

	
	imageShape->setPosition(28, 10);
    imageShape->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
    
    String name = assetName;
    if(name.length() > 15) {
        name = name.substr(0, 15)+"...";
    }
	nameLabel = new UILabel(name, 11);
	addChild(nameLabel);
    nameLabel->setPosition((120.0-nameLabel->getWidth())/2.0, 80);
    
}

AssetEntry::~AssetEntry() {
	delete imageShape;
	delete nameLabel;
	delete selectShape;
}

AssetList::AssetList() : UIElement() {
	    
	reloadButton = new UIImageButton("browserIcons/reload_icon.png", 1.0, 20, 20);
	reloadButton->addEventListener(this, UIEvent::CLICK_EVENT);
	addChild(reloadButton);	
	reloadButton->setPosition(10, 5);		
	
}

AssetList::~AssetList() {

}

void AssetList::setExtensions(std::vector<String> extensions) {
	this->extensions = extensions;
	if(currentFolderPath != "") {
		showFolder(currentFolderPath);
	}
}

bool AssetList::hasExtension(String extension) {
	for(int i=0; i < extensions.size(); i++) {
		if(extensions[i] == extension.toLowerCase()) {
			return true;
		}
	}
	return false;
}

void AssetList::showFolder(String folderPath) {

	currentFolderPath = folderPath;

	for(int i=0; i < assetEntries.size(); i++) {
		removeChild(assetEntries[i]);
		delete assetEntries[i];
	}
	assetEntries.clear();
	
	currentEntry = NULL;
	
	vector<OSFileEntry> assets = OSBasics::parseFolder(folderPath, false);	
	
	Number xPos = 20;
	Number yPos = 30;
	
	for(int i=0; i < assets.size(); i++) {
		OSFileEntry entry = assets[i];
		if(entry.type != OSFileEntry::TYPE_FOLDER) {
			if(hasExtension(entry.extension)) {
				AssetEntry *newEntry = new AssetEntry(entry.fullPath, entry.name, entry.extension);
				newEntry->selectShape->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
				assetEntries.push_back(newEntry);
				newEntry->setPosition(xPos, yPos);
				xPos += 120;
				if(xPos > 500) {
					xPos = 20;
					yPos += 100;
				}
				addChild(newEntry);
			}
		}
	}
	
	setWidth(640);
	
	if(xPos == 20) {
		setHeight(yPos+20);
	} else {
		setHeight(yPos + 120);
	}

	
	rebuildTransformMatrix();	
}

void AssetList::handleEvent(Event *event) {
	if(event->getDispatcher() == reloadButton && event->getEventCode() == UIEvent::CLICK_EVENT) {
		showFolder(currentFolderPath);
	} else {
		for(int i=0; i < assetEntries.size(); i++) {
			if(event->getDispatcher() == assetEntries[i]->selectShape && event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
				assetEntries[i]->selectShape->visible = true;
				selectedPath = assetEntries[i]->assetPath;
				printf("%s\n", selectedPath.c_str());
				if(currentEntry) {
					currentEntry->selectShape->visible = false;
				}
				currentEntry = assetEntries[i];
			}
		}
	}
}

AssetBrowser::AssetBrowser() : UIWindow(L"Asset Browser", 850, 500) {
	defaultTemplateTree = NULL;
	
	Config *conf = CoreServices::getInstance()->getConfig();	
	String fontName = conf->getStringValue("Polycode", "uiDefaultFontName");
	
	closeOnEscape = true;	
	
	templateContainer = new UITreeContainer("boxIcon.png", L"Project", 200, 480-topPadding-padding-padding);	
	
	FolderUserData *data = new FolderUserData();
	data->type = 0;
	templateContainer->getRootNode()->setUserData(data);			

	addChild(templateContainer);		
	templateContainer->setPosition(padding,topPadding+padding);	
	templateContainer->getRootNode()->toggleCollapsed();
	
	templateContainer->getRootNode()->addEventListener(this, UITreeEvent::SELECTED_EVENT);
	
		
	assetList = new AssetList();
	
	listContainer = new UIScrollContainer(assetList, false, true, 640, 480-topPadding-padding-padding);
	listContainer->setPosition(220,topPadding+padding);		
	addChild(listContainer);

	cancelButton = new UIButton(L"Cancel", 100);
	cancelButton->addEventListener(this, UIEvent::CLICK_EVENT);
	addChild(cancelButton);
	cancelButton->setPosition(850-80-padding-100-10, 485);

	okButton = new UIButton(L"OK", 100);
	okButton->addEventListener(this, UIEvent::CLICK_EVENT);
	addChild(okButton);
	okButton->setPosition(850-80-padding, 485);	
	

			
	currentProject = NULL;
}

void AssetBrowser::setProject(PolycodeProject *project) {
	
	templateContainer->getRootNode()->clearTree();

	vector<OSFileEntry> templates = OSBasics::parseFolder(project->getRootFolder(), false);	
	templateContainer->getRootNode()->setLabelText(project->getProjectName());
	
	
	for(int i=0; i < templates.size(); i++) {
		OSFileEntry entry = templates[i];
		if(entry.type == OSFileEntry::TYPE_FOLDER) {
			UITree *newChild = templateContainer->getRootNode()->addTreeChild("folder.png", entry.name, NULL);			
			FolderUserData *data = new FolderUserData();
			data->type = 0;
			data->folderPath = entry.fullPath;
			newChild->setUserData(data);			
			newChild->toggleCollapsed();
			parseFolderIntoTree(newChild, entry);
		}
	}	
	
	currentProject = project;
}

AssetBrowser::~AssetBrowser() {
	
}

String AssetBrowser::getFileName() {
	return "";
}

String AssetBrowser::getTemplatePath() {
	return templatePath;
}

String AssetBrowser::getFullSelectedAssetPath() {
	return assetList->selectedPath;
}

String AssetBrowser::getSelectedAssetPath() {
	return assetList->selectedPath.replace(currentProject->getRootFolder()+"/", "");
}

void AssetBrowser::setExtensions(std::vector<String> extensions) {
	assetList->setExtensions(extensions);
}

void AssetBrowser::handleEvent(Event *event) {
	if(event->getEventType() == "UIEvent") {
		if(event->getEventCode() == UIEvent::CLICK_EVENT) {
			if(event->getDispatcher() == okButton) {
				dispatchEvent(new UIEvent(), UIEvent::OK_EVENT);						
			}
			
			if(event->getDispatcher() == cancelButton) {
				dispatchEvent(new UIEvent(), UIEvent::CLOSE_EVENT);				
			}									
		}
	}
	
	if(event->getEventType() == "UITreeEvent" && event->getEventCode() == UITreeEvent::SELECTED_EVENT) {
		if(event->getDispatcher() == templateContainer->getRootNode()) {
			UITreeEvent *treeEvent = (UITreeEvent*) event;
			FolderUserData *data = (FolderUserData *)treeEvent->selection->getUserData();
			assetList->showFolder(data->folderPath);
			listContainer->setContentSize(assetList->getWidth(), assetList->getHeight());
			listContainer->setScrollValue(0,0);
		}
	}
	
	UIWindow::handleEvent(event);	
}


void AssetBrowser::parseFolderIntoTree(UITree *tree, OSFileEntry folder) {
	vector<OSFileEntry> templates = OSBasics::parseFolder(folder.fullPath, false);
	for(int i=0; i < templates.size(); i++) {
		OSFileEntry entry = templates[i];	
		if(entry.type == OSFileEntry::TYPE_FOLDER) {
			UITree *newChild = tree->addTreeChild("folder.png", entry.nameWithoutExtension, NULL);
			FolderUserData *data = new FolderUserData();
			data->type = 1;
			data->folderPath = entry.fullPath;
			newChild->setUserData(data);
			parseFolderIntoTree(newChild, entry);
		}
	}	
}
