// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include <string>

namespace ExampleApp
{
    namespace CategorySearch
    {
        namespace View
        {
            class CategorySearchModel
            {
                std::string m_name;
                std::string m_searchCategory;
                std::string m_icon;
                bool m_inMenu;

            public:
                CategorySearchModel(const std::string& name, const std::string& searchCategory, const std::string& icon, bool inMenu);

                const std::string& Name() const;

                const std::string& SearchCategory() const;

                const std::string& Icon() const;
                
                bool InMenu() const;
            };
        }
    }
}
