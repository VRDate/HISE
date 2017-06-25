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
*   which also must be licenced for commercial applications:
*
*   http://www.juce.com
*
*   ===========================================================================
*/



PopupIncludeEditor::PopupIncludeEditor(JavascriptProcessor *s, const File &fileToEdit) :
	sp(s),
	file(fileToEdit),
	callback(Identifier())
{
	Processor *p = dynamic_cast<Processor*>(sp);

	doc.setOwned(new CodeDocument());

	doc->replaceAllContent(file.loadFileAsString());

	const Identifier snippetId = Identifier("File_" + fileToEdit.getFileNameWithoutExtension());

	tokeniser = new JavascriptTokeniser();
	addAndMakeVisible(editor = new JavascriptCodeEditor(*doc, tokeniser, s, snippetId));

	addAndMakeVisible(resultLabel = new Label());

	resultLabel->setFont(GLOBAL_MONOSPACE_FONT());
	resultLabel->setColour(Label::ColourIds::backgroundColourId, Colours::darkgrey);
	resultLabel->setColour(Label::ColourIds::textColourId, Colours::white);
	resultLabel->setEditable(false, false, false);

	p->setEditorState(p->getEditorStateForIndex(JavascriptMidiProcessor::externalPopupShown), true);

	if (!file.existsAsFile()) editor->setEnabled(false);

	setSize(800, 800);
}

PopupIncludeEditor::PopupIncludeEditor(JavascriptProcessor* s, const Identifier &callback_) :
	sp(s),
	file(File()),
	callback(callback_)
{
	doc.setNonOwned(sp->getSnippet(callback_));

	tokeniser = new JavascriptTokeniser();
	addAndMakeVisible(editor = new JavascriptCodeEditor(*doc, tokeniser, s, callback));

	addAndMakeVisible(resultLabel = new Label());

	resultLabel->setFont(GLOBAL_MONOSPACE_FONT());
	resultLabel->setColour(Label::ColourIds::backgroundColourId, Colours::darkgrey);
	resultLabel->setColour(Label::ColourIds::textColourId, Colours::white);
	resultLabel->setEditable(false, false, false);

	setSize(800, 800);
}

PopupIncludeEditor::PopupIncludeEditor(JavascriptProcessor* s) :
	sp(s),
	file(File()),
	callback(Identifier())
{
	doc.setOwned(new CodeDocument());

	String allCode;
	sp->mergeCallbacksToScript(allCode);

	doc->replaceAllContent(allCode);
	doc->clearUndoHistory();
	doc->setSavePoint();

	static const Identifier empty("empty");

	tokeniser = new JavascriptTokeniser();
	addAndMakeVisible(editor = new JavascriptCodeEditor(*doc, tokeniser, s, empty));

	addAndMakeVisible(resultLabel = new Label());

	resultLabel->setFont(GLOBAL_MONOSPACE_FONT());
	resultLabel->setColour(Label::ColourIds::backgroundColourId, Colours::darkgrey);
	resultLabel->setColour(Label::ColourIds::textColourId, Colours::white);
	resultLabel->setEditable(false, false, false);

	setSize(800, 800);
}

PopupIncludeEditor::~PopupIncludeEditor()
{
	editor = nullptr;
	doc.clear();
	tokeniser = nullptr;

	Processor *p = dynamic_cast<Processor*>(sp);

	if (p != nullptr)
		p->setEditorState(p->getEditorStateForIndex(JavascriptMidiProcessor::externalPopupShown), false);

	sp = nullptr;
	p = nullptr;
}

void PopupIncludeEditor::timerCallback()
{
	resultLabel->setColour(Label::backgroundColourId, lastCompileOk ? Colours::green.withBrightness(0.1f) : Colours::red.withBrightness((0.1f)));
	stopTimer();
}

bool PopupIncludeEditor::keyPressed(const KeyPress& key)
{
	if (key.isKeyCode(KeyPress::F5Key))
	{
		if (file.existsAsFile())
		{
			String editorContent = doc->getAllContent();
			file.replaceWithText(editorContent);
		}

		if (isWholeScriptEditor())
		{
			sp->parseSnippetsFromString(doc->getAllContent());
			doc->setSavePoint();
			sp->setCompileScriptAsWhole(true);

		}

		sp->compileScript();
		sp->setCompileScriptAsWhole(false);

		lastCompileOk = sp->wasLastCompileOK();

		resultLabel->setColour(Label::backgroundColourId, Colours::white);
		resultLabel->setColour(Label::ColourIds::textColourId, Colours::white);
		resultLabel->setText(lastCompileOk ? "Compiled OK" : sp->getLastErrorMessage().getErrorMessage(), dontSendNotification);

		startTimer(200);

		return true;
	}

	return false;
}

void PopupIncludeEditor::resized()
{
	bool isInPanel = findParentComponentOfClass<FloatingTile>() != nullptr;

	if(isInPanel)
		editor->setBounds(0, 0, getWidth(), getHeight() - 18);
	else
		editor->setBounds(0, 5, getWidth(), getHeight() - 23);

	resultLabel->setBounds(0, getHeight() - 18, getWidth(), 18);
}

void PopupIncludeEditor::gotoChar(int character, int lineNumber/*=-1*/)
{
	CodeDocument::Position pos;

	pos = lineNumber != -1 ? CodeDocument::Position(*doc, lineNumber, character) :
		CodeDocument::Position(*doc, character);

	editor->scrollToLine(jmax<int>(0, pos.getLineNumber() - 1));
	editor->moveCaretTo(pos, false);
	editor->moveCaretToStartOfLine(false);
	editor->moveCaretToEndOfLine(true);
}
