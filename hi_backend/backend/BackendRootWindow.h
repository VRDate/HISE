/*
  ==============================================================================

    BackendRootWindow.h
    Created: 15 May 2017 10:12:45pm
    Author:  Christoph

  ==============================================================================
*/

#ifndef BACKENDROOTWINDOW_H_INCLUDED
#define BACKENDROOTWINDOW_H_INCLUDED


class BackendProcessorEditor;

#define GET_BACKEND_ROOT_WINDOW(child) child->findParentComponentOfClass<ComponentWithBackendConnection>()->getBackendRootWindow()

#define GET_ROOT_FLOATING_TILE(child) GET_BACKEND_ROOT_WINDOW(child)->getRootFloatingTile()

// This is a simple counter that gets bumped everytime the layout is changed and shows a hint to reset the workspace
#define BACKEND_UI_VERSION 2

class BackendRootWindow : public AudioProcessorEditor,
						  public BackendCommandTarget,
						  public Timer,
						  public ComponentWithKeyboard,
						  public ModalBaseWindow,
						  public ComponentWithBackendConnection
{
public:

	BackendRootWindow(AudioProcessor *ownerProcessor, var editorState);

	~BackendRootWindow();

	bool isFullScreenMode() const;

	void paint(Graphics& g) override
	{
		g.fillAll(HiseColourScheme::getColour(HiseColourScheme::ColourIds::EditorBackgroundColourIdBright));

		//g.fillAll(Colour(0xFF333333));
	}

	void saveInterfaceData();

	void resized();

	void showSettingsWindow();

	void timerCallback() override;

	void toggleLayoutMode()
	{
		const bool shouldBeOn = !bpe->getRootFloatingTile()->isLayoutModeEnabled();

		getRootFloatingTile()->setLayoutModeEnabled(shouldBeOn);

		for (int i = 0; i < popoutWindows.size(); i++)
		{
			popoutWindows[i]->getRootFloatingTile()->setLayoutModeEnabled(shouldBeOn);
		}
	}

	BackendProcessor* getBackendProcessor() { return owner; }
	const BackendProcessor* getBackendProcessor() const { return owner; }

	BackendRootWindow* getBackendRootWindow() override { return this; }

	const BackendRootWindow* getBackendRootWindow() const override { return this; }

	BackendProcessorEditor* getMainPanel() { return mainEditor; }

	FloatingTileDocumentWindow* getLastPopup() { return popoutWindows.getLast(); }

	void resetInterface();

	CustomKeyboard* getKeyboard() const override
	{
		if (floatingRoot == nullptr)
			return nullptr;

		FloatingTile::Iterator<MidiKeyboardPanel> it(floatingRoot);

		while (auto kb = it.getNextPanel())
		{
			if (kb->isVisible())
				return kb->getKeyboard();
		}

		return nullptr;
	}

	const BackendProcessorEditor* getMainPanel() const { return mainEditor; }

	ModulatorSynthChain* getMainSynthChain() { return owner->getMainSynthChain(); }
	const ModulatorSynthChain* getMainSynthChain() const { return owner->getMainSynthChain(); }

	void loadNewContainer(ValueTree & v);

	void loadNewContainer(const File &f);
	
	FloatingTile* getRootFloatingTile() override { return floatingRoot; }

	MainController::ProcessorChangeHandler &getModuleListNofifier() { return getMainSynthChain()->getMainController()->getProcessorChangeHandler(); }

	void sendRootContainerRebuildMessage(bool synchronous)
	{
		getModuleListNofifier().sendProcessorChangeMessage(getMainSynthChain(), MainController::ProcessorChangeHandler::EventType::RebuildModuleList, synchronous);
	}

	int getCurrentWorkspace() const { return currentWorkspace; }

	void showWorkspace(int workspace);

	MainTopBar* getMainTopBar()
	{
		FloatingTile::Iterator<MainTopBar> iter(getRootFloatingTile());

		auto bar = iter.getNextPanel();

		jassert(bar != nullptr);

		return bar;
	}

	FloatingTileDocumentWindow* addFloatingWindow()
	{
		popoutWindows.add(new FloatingTileDocumentWindow(this));

		return popoutWindows.getLast();
	}

	void removeFloatingWindow(FloatingTileDocumentWindow* windowToRemove)
	{
		popoutWindows.removeObject(windowToRemove, true);
	}

private:

	OwnedArray<FloatingTileDocumentWindow> popoutWindows;

	int currentWorkspace = BackendCommandTarget::WorkspaceMain;
	
	Array<Component::SafePointer<FloatingTile>> workspaces;

	friend class BackendCommandTarget;

	PopupLookAndFeel plaf;

	BackendProcessor *owner;

	Component::SafePointer<BackendProcessorEditor> mainEditor;

	StringArray menuNames;

	ScopedPointer<MenuBarComponent> menuBar;

	ScopedPointer<ThreadWithQuasiModalProgressWindow::Overlay> progressOverlay;

	ScopedPointer<AudioDeviceDialog> currentDialog;

	ScopedPointer<ComponentBoundsConstrainer> constrainer;

	ScopedPointer<ResizableBorderComponent> yBorderDragger;
	ScopedPointer<ResizableBorderComponent> xBorderDragger;


	ScopedPointer<FloatingTile> floatingRoot;

	bool resetOnClose = false;

};

struct BackendPanelHelpers
{
	enum class Workspace
	{
		MainPanel = 0,
		ScriptingWorkspace,
		SamplerWorkspace,
		CustomWorkspace,
		numWorkspaces
	};

	template <class ContentType> static ContentType* toggleVisibilityForRightColumnPanel(FloatingTile* root, bool show)
	{
		auto rightColumn = getMainRightColumn(root);

		FloatingTile::Iterator<ContentType> iter(rightColumn->getParentShell());

		auto existingContent = iter.getNextPanel();

		if (existingContent != nullptr)
		{
			bool visible = existingContent->getParentShell()->getLayoutData().isVisible();

			if (visible != show)
			{
				existingContent->getParentShell()->getLayoutData().setVisible(show);
				rightColumn->refreshLayout();
				rightColumn->notifySiblingChange();
			}

			return existingContent;
		}

		return nullptr;
	}

	static VerticalTile* getMainTabComponent(FloatingTile* root);

	static HorizontalTile* getMainLeftColumn(FloatingTile* root);

	static HorizontalTile* getMainRightColumn(FloatingTile* root);

	static void showWorkspace(BackendRootWindow* root, Workspace workspaceToShow, NotificationType notifyCommandManager);

	struct ScriptingWorkspace
	{
		static FloatingTile* get(BackendRootWindow* rootWindow);

		static void setGlobalProcessor(BackendRootWindow* rootWindow, JavascriptProcessor* jsp);

		static void showEditor(BackendRootWindow* rootWindow, bool shouldBeVisible);
		static void showInterfaceDesigner(BackendRootWindow* rootWindow, bool shouldBeVisible);
	};

	struct SamplerWorkspace
	{
		static FloatingTile* get(BackendRootWindow* rootWindow);

		static void setGlobalProcessor(BackendRootWindow* rootWindow, ModulatorSampler* sampler);
	};

	static bool isMainWorkspaceActive(FloatingTile* root);

};


#endif  // BACKENDROOTWINDOW_H_INCLUDED
