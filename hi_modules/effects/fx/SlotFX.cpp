/*
  ==============================================================================

    SlotFX.cpp
    Created: 28 Jun 2017 2:51:50pm
    Author:  Christoph

  ==============================================================================
*/

#include "SlotFX.h"

ProcessorEditorBody * SlotFX::createEditor(ProcessorEditor *parentEditor)
{
	return new SlotFXEditor(parentEditor);
}

bool SlotFX::setEffect(const String& typeName)
{
	int index = effectList.indexOf(typeName);

	if (index != -1)
	{
		ScopedPointer<FactoryType> f = new EffectProcessorChainFactoryType(128, this);

		f->setConstrainer(new Constrainer());

		currentIndex = index;

		if (wrappedEffect != nullptr)
			wrappedEffect->sendDeleteMessage();


		auto p = dynamic_cast<MasterEffectProcessor*>(f->createProcessor(f->getProcessorTypeIndex(typeName), typeName));

		if (p != nullptr && getSampleRate() > 0)
			p->prepareToPlay(getSampleRate(), getBlockSize());

		if(p != nullptr)
		{
			ScopedLock sl(swapLock);

			wrappedEffect = p;
		}

        
        for (int i = 0; i < p->getNumInternalChains(); i++)
        {
            dynamic_cast<ModulatorChain*>(p->getChildProcessor(i))->setColour(p->getColour());
        }
        
        if (JavascriptProcessor* sp = dynamic_cast<JavascriptProcessor*>(p))
        {
            sp->compileScript();
        }
        
		sendRebuildMessage(true);

		return true;
	}
	else
	{
		jassertfalse;
		return false;
	}
}

void SlotFX::createList()
{
	ScopedPointer<FactoryType> f = new EffectProcessorChainFactoryType(128, this);

	f->setConstrainer(new Constrainer());

	auto l = f->getAllowedTypes();

	for (int i = 0; i < l.size(); i++)
	{
		effectList.add(l[i].type.toString());
	}

	f = nullptr;
}
