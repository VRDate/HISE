/*  ===========================================================================
*
*   This file is part of HISE.
*   Copyright 2016 Christoph Hart
*
*   HISE is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   HISE is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with HISE.  If not, see <http://www.gnu.org/licenses/>.
*
*   Commercial licenses for using HISE in an closed source project are
*   available on request. Please visit the project's website to get more
*   information about commercial licensing:
*
*   http://www.hise.audio/
*
*   HISE is based on the JUCE library,
*   which must be separately licensed for cloused source applications:
*
*   http://www.juce.com
*
*   ===========================================================================
*/


HisePlayerExporter::HisePlayerExporter(ModulatorSynthChain* chainToExport, const File& file) :
	BaseExporter(chainToExport),
	ThreadWithQuasiModalProgressWindow("Exporting HISE Player library", true, true, chainToExport->getMainController()),
	libraryFile(file)
{
	getAlertWindow()->setLookAndFeel(&laf);
}

void HisePlayerExporter::run()
{
	ValueTree metadata("Info");

	ProjectHandler* handler = &GET_PROJECT_HANDLER(chainToExport);

	metadata.setProperty(Identifier("Name"), SettingWindows::getSettingValue((int)SettingWindows::ProjectSettingWindow::Attributes::Name, handler), nullptr);
	metadata.setProperty(Identifier("Version"), SettingWindows::getSettingValue((int)SettingWindows::ProjectSettingWindow::Attributes::Version, handler), nullptr);
	metadata.setProperty(Identifier("Company"), SettingWindows::getSettingValue((int)SettingWindows::UserSettingWindow::Attributes::Company, handler), nullptr);
	metadata.setProperty(Identifier("CompanyWebsite"), SettingWindows::getSettingValue((int)SettingWindows::UserSettingWindow::Attributes::CompanyURL, handler), nullptr);

	ValueTree library("LibraryData");

	if (threadShouldExit()) return;

	setStatusMessage("Exporting Preset");
	setProgress(0.0);
	library.addChild(exportPresetFile(), -1, nullptr);

	if (threadShouldExit()) return;

	setStatusMessage("Exporting Embedded Files");
	setProgress(1.0 / 7.0);
	library.addChild(exportEmbeddedFiles(true), -1, nullptr);
	
	if (threadShouldExit()) return;

	setStatusMessage("Exporting User Preset Files");
	setProgress(2.0 / 7.0);
	library.addChild(exportUserPresetFiles(), -1, nullptr);
	
	if (threadShouldExit()) return;

	setStatusMessage("Exporting Audio Files");
	setProgress(3.0 / 7.0);
	library.addChild(exportReferencedAudioFiles(), -1, nullptr);
	
	if (threadShouldExit()) return;

	setStatusMessage("Exporting Image Files");
	setProgress(4.0 / 7.0);
	library.addChild(exportReferencedImageFiles(), -1, nullptr);

	if (threadShouldExit()) return;

	setStatusMessage("Compressing Data");
	setProgress(5.0 / 7.0);

	ValueTree collection("HisePlayerLibrary");

	ScopedPointer<XmlElement> metaXml = metadata.createXml();

	collection.setProperty(Identifier("Info"), metaXml->createDocument("", true, false), nullptr);
	var compressedData = PresetHandler::writeValueTreeToMemoryBlock(library, true);
	
	collection.setProperty(Identifier("Data"), compressedData, nullptr);

	setStatusMessage("Writing File");
	setProgress(6.0 / 7.0);
	PresetHandler::writeValueTreeAsFile(collection, libraryFile.getFullPathName(), false);


	if (threadShouldExit()) return;
}
