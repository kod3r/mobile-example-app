// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include "IStateMachineState.h"
#include "AppCamera.h"
#include "AppModes.h"
#include "Tours.h"
#include "ICallback.h"
#include "Interiors.h"
#include "GlobeCamera.h"
#include "InteriorsExplorer.h"
#include "MapMode.h"
#include "MyPinCreation.h"

namespace ExampleApp
{
    namespace AppModes
    {
        namespace States
        {
            namespace SdkModel
            {
                class TourState : public Helpers::IStateMachineState
                {
                private:
                    
                    AppCamera::SdkModel::IAppCameraController& m_cameraController;
                    int m_tourCameraHandle;
                    
                    Eegeo::Helpers::TCallback0<TourState> m_tourStartedCallback;
                    Tours::SdkModel::ITourService& m_tourService;
                    
                    AppModes::SdkModel::IAppModeModel& m_appModeModel;
                    
                    Eegeo::Resources::Interiors::InteriorSelectionModel& m_interiorSelectionModel;
                    
                    Eegeo::Camera::GlobeCamera::GpsGlobeCameraController& m_worldCameraController;
                    ExampleApp::InteriorsExplorer::SdkModel::InteriorsExplorerCameraController& m_interiorsCameraController;
                    
                    InteriorsExplorer::SdkModel::InteriorsExplorerModel& m_interiorsExplorerModel;
                    MapMode::SdkModel::IMapModeModel& m_mapModeModel;
                    bool m_previousMapModeState;
                    
                    MyPinCreation::SdkModel::IMyPinCreationModel& m_myPinCreationModel;
                    
                    void OnTourEnded();
                    
                public:
                    
                    TourState(AppCamera::SdkModel::IAppCameraController& cameraController,
                              int tourCameraHandle,
                              Tours::SdkModel::ITourService& tourService,
                              Eegeo::Resources::Interiors::InteriorSelectionModel& interiorSelectionModel,
                              AppModes::SdkModel::IAppModeModel& appModeModel,
                              Eegeo::Camera::GlobeCamera::GpsGlobeCameraController& worldCameraController,
                              InteriorsExplorer::SdkModel::InteriorsExplorerCameraController& interiorsCameraController,
                              InteriorsExplorer::SdkModel::InteriorsExplorerModel& interiorsExplorerModel,
                              MapMode::SdkModel::IMapModeModel& mapModeModel,
                              MyPinCreation::SdkModel::IMyPinCreationModel& myPinCreationModel);
                    
                    ~TourState();
                    
                    void Enter(int previousState);
                    
                    void Update(float dt);
                    
                    void Exit(int nextState);
                };
            }
        }
    }
}