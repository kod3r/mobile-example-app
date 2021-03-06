// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include "Tours.h"
#include "WorldPins.h"
#include "ICallback.h"
#include "Types.h"

namespace ExampleApp
{
    namespace Tours
    {
        namespace SdkModel
        {
            class TourRemovedObserver : private Eegeo::NonCopyable
            {
                ITourRepository& m_tourRepository;
                WorldPins::SdkModel::IWorldPinsService& m_worldPinsService;
                Eegeo::Helpers::TCallback1<TourRemovedObserver, TourModel> m_binding;
                
                void HandleTourRemoved(TourModel& tourModel);
                
            public:
                TourRemovedObserver(ITourRepository& tourRepository,
                                  WorldPins::SdkModel::IWorldPinsService& worldPinsService);
                
                ~TourRemovedObserver();
            };
        }
    }
}
