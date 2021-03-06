// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "CompassViewModule.h"
#include "CompassView.h"
#include "CompassController.h"
#include "WindowsAppThreadAssertionMacros.h"

namespace ExampleApp
{
    namespace Compass
    {
        namespace View
        {
            CompassViewModule::CompassViewModule(
                WindowsNativeState& nativeState,
                ICompassViewModel& viewModel,
                ExampleAppMessaging::TMessageBus& messageBus
            )
            {
                m_pView = Eegeo_NEW(CompassView)(nativeState);
                m_pController = Eegeo_NEW(CompassController)(
                                    *m_pView,
                                    viewModel,
                                    messageBus);
            }

            CompassViewModule::~CompassViewModule()
            {
                Eegeo_DELETE m_pController;
                Eegeo_DELETE m_pView;
            }

            void CompassViewModule::AnimateOffScreen()
            {
                m_pView->SetFullyOffScreen();
            }

            void CompassViewModule::AnimateOnScreen()
            {
                m_pView->SetFullyOnScreen();
            }
        }
    }
}
