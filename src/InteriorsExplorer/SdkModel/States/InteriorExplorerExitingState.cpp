// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "InteriorExplorerExitingState.h"
#include "IAppCameraController.h"
#include "InteriorController.h"
#include "CameraFrustumStreamingVolume.h"
#include "InteriorVisibilityUpdater.h"
#include "InteriorsExplorerModel.h"

namespace ExampleApp
{
    namespace InteriorsExplorer
    {
        namespace SdkModel
        {
            namespace States
            {
                InteriorExplorerExitingState::InteriorExplorerExitingState(AppModes::States::SdkModel::InteriorExplorerState& parentState,
                                                                           Eegeo::Resources::Interiors::InteriorController& interiorController,
                                                                           Eegeo::Streaming::CameraFrustumStreamingVolume& cameraFrustumStreamingVolume,
                                                                           InteriorsExplorer::SdkModel::InteriorVisibilityUpdater& interiorVisibilityUpdater,
                                                                           InteriorsExplorerModel& interiorsExplorerModel)
                : m_parentState(parentState)
                , m_interiorController(interiorController)
                , m_cameraFrustumStreamingVolume(cameraFrustumStreamingVolume)
                , m_interiorVisibilityUpdater(interiorVisibilityUpdater)
                , m_interiorsExplorerModel(interiorsExplorerModel)
                {
                }
                
                InteriorExplorerExitingState::~InteriorExplorerExitingState()
                {
                }
                
                void InteriorExplorerExitingState::Enter(int previousState)
                {
                    m_cameraFrustumStreamingVolume.SetForceMaximumRefinement(true);
                    m_interiorVisibilityUpdater.SetInteriorShouldDisplay(false);
                }
                
                void InteriorExplorerExitingState::Update(float dt)
                {
                    if(m_interiorVisibilityUpdater.GetInteriorTransitionParam() <= 0.0f)
                    {
                        m_interiorsExplorerModel.ResumePreviousMapState();
                        m_interiorController.ClearSelectedInterior();
                        m_parentState.ReturnToWorldMode();
                    }
                }
                
                void InteriorExplorerExitingState::Exit(int nextState)
                {
                    m_cameraFrustumStreamingVolume.SetForceMaximumRefinement(false);
                }
            }
            
        }
    }
}