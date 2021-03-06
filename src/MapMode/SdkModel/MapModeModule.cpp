// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "MapModeModule.h"
#include "MapModeModel.h"
#include "MapModeController.h"

namespace ExampleApp
{
    namespace MapMode
    {
        namespace SdkModel
        {
            MapModeModule::MapModeModule(Eegeo::Rendering::EnvironmentFlatteningService& environmentFlatteningService,
                                         WeatherMenu::SdkModel::IWeatherController& weatherController)
            : m_pMapModeModel(NULL)
            {
                m_pMapModeModel = Eegeo_NEW(MapModeModel)();
                m_pMapModeController = Eegeo_NEW(MapModeController)(*m_pMapModeModel, environmentFlatteningService, weatherController);
            }

            MapModeModule::~MapModeModule()
            {
                Eegeo_DELETE m_pMapModeController;
                Eegeo_DELETE m_pMapModeModel;
            }

            IMapModeModel& MapModeModule::GetMapModeModel()
            {
                return *m_pMapModeModel;
            }
        }
    }
}