// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "CategorySearchMenuOption.h"
#include "ISearchQueryPerformer.h"
#include "IMenuViewModel.h"
#include "CategorySearchSelectedMessage.h"

namespace ExampleApp
{
    namespace CategorySearch
    {
        namespace View
        {
            CategorySearchMenuOption::CategorySearchMenuOption(CategorySearchModel model,
                    Menu::View::IMenuViewModel& menuViewModel,
                    ExampleAppMessaging::TMessageBus& messageBus)
                : m_model(model)
                , m_menuViewModel(menuViewModel)
                , m_messageBus(messageBus)
            {
            }

            CategorySearchMenuOption::~CategorySearchMenuOption()
            {

            }

            void CategorySearchMenuOption::Select()
            {
                m_menuViewModel.Close();
                m_messageBus.Publish(CategorySearchSelectedMessage(m_model.SearchCategory()));
            }
        }
    }
}
