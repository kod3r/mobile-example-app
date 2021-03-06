// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "PoiRingController.h"
#include "InteriorsFloorModel.h"
#include "InteriorsModel.h"
#include "InteriorId.h"
#include "TtyHandler.h"
#include "CameraHelpers.h"
#include "RenderCamera.h"
#include "IRayPicker.h"
#include "PoiRingView.h"
#include "RenderContext.h"
#include "MathFunc.h"
#include "MathsHelpers.h"
#include "IMyPinCreationInitiationViewModel.h"
#include "IMyPinCreationModel.h"
#include "EarthConstants.h"
#include "TerrainHeightProvider.h"
#include "TransformHelpers.h"
#include "VectorMath.h"
#include "InteriorController.h"

#include "InteriorHeightHelpers.h"
#include "ScreenProperties.h"

namespace ExampleApp
{
    namespace MyPinCreation
    {
        namespace PoiRing
        {
            namespace SdkModel
            {
                namespace
                {
                    
                    float CalculateAltitudeBasedSphereOuterScale(float altitude)
                    {
                        const float minAltitude = 10.f;
                        const float maxAltitude = 1500.f;
                        const float lowAltitudeScale = 0.05f;
                        const float highAltitudeScale = 1.0f;
                        float t = Eegeo::Math::Clamp01((altitude - minAltitude)/(maxAltitude-minAltitude));
                        return Eegeo::Math::Lerp(lowAltitudeScale, highAltitudeScale, t);
                    }
                    
                    float CalculateAltitudeBasedSphereScale(float altitude, float outerRingRadiusInMeters)
                    {
                        const float minAltitude = 10.f;
                        const float maxAltitude = 18000.f;
                        const float lowAltitudeSphereScale = outerRingRadiusInMeters - 0.5f;
                        const float highAltitudeSphereScale = outerRingRadiusInMeters - 100.f;
                        return lowAltitudeSphereScale + (((altitude - minAltitude)/maxAltitude) * (highAltitudeSphereScale - lowAltitudeSphereScale));
                    }

                    bool RingIsOnScreen(const Eegeo::Camera::RenderCamera& renderCamera, const Eegeo::dv3& position, float radius)
                    {
                        const Eegeo::Geometry::Frustum& frustum = renderCamera.GetFrustum();
                        const Eegeo::dv3 cameraRelativePosition = position - renderCamera.GetEcefLocation();

                        for (int i = 0; i < Eegeo::Geometry::Frustum::PLANES_COUNT; ++i)
                        {
                            const Eegeo::Geometry::Plane& p = frustum.planes[i];
                            double signedDist = p.a * cameraRelativePosition.GetX() + p.b * cameraRelativePosition.GetY() + p.c * cameraRelativePosition.GetZ() + p.d;

                            if (signedDist < -radius)
                            {
                                return false;
                            }
                        }

                        return true;
                    }
                }

                PoiRingController::PoiRingController(MyPinCreation::SdkModel::IMyPinCreationModel& myPinCreationModel,
                                                     PoiRingView& poiRingView,
                                                     Eegeo::Rendering::EnvironmentFlatteningService& environmentFlatteningService,
                                                     Eegeo::Resources::Terrain::Heights::TerrainHeightProvider& terrainHeightProvider,
                                                     Eegeo::Resources::Interiors::InteriorController& interiorController,
                                                     Eegeo::Rendering::ScreenProperties& screenProperties,
                                                     const bool interiorsAffectedByFlattening)
                    : m_myPinCreationModel(myPinCreationModel)
                    , m_poiRingView(poiRingView)
                    , m_scaleInterpolationParam(0.f)
                    , m_easeDurationInSeconds(1.2f)
                    , m_environmentFlatteningService(environmentFlatteningService)
                    , m_terrainHeightProvider(terrainHeightProvider)
                    , m_iconPosition(Eegeo::dv3::Zero())
                    , m_iconSize(0.0f)
                    , m_ringRadius(0.0f)
                    , m_interiorController(interiorController)
                    , m_screenProperties(screenProperties)
                    , m_interiorsAffectedByFlattening(interiorsAffectedByFlattening)
                {

                }

                void PoiRingController::Update(float dt, const Eegeo::Camera::RenderCamera& renderCamera, const Eegeo::dv3& cameraEcefInterestPoint)
                {
                    const bool showingInterior = m_interiorController.InteriorIsVisible();
                    float interiorDownScale = showingInterior ? 0.5f : 1.0f;
                    const float altitude = (float)(renderCamera.GetAltitude() - (m_myPinCreationModel.GetPosition().Length() - Eegeo::Space::EarthConstants::Radius));
                    const float outerRingRadiusInMeters = 120.f * interiorDownScale;
                    const float altitudeScale = CalculateAltitudeBasedSphereOuterScale(altitude);
                    const float transitionScale = CalculateTransitionScale(dt);
                    m_ringRadius = outerRingRadiusInMeters * altitudeScale * transitionScale;

                    if (m_scaleInterpolationParam < 0.01f && m_myPinCreationModel.GetCreationStage() != Details)
                    {
                        m_myPinCreationModel.SetLatLong(Eegeo::Space::LatLong::FromECEF(cameraEcefInterestPoint));
                    }

                    if(m_myPinCreationModel.NeedsTerrainHeight())
                    {
                        float terrainHeight;
                        if(m_terrainHeightProvider.TryGetHeight(m_myPinCreationModel.GetPosition(), 11, terrainHeight))
                        {
                            m_myPinCreationModel.SetTerrainHeight(terrainHeight);
                        }
                    }
                    
                    if(m_myPinCreationModel.GetCreationStage() == Ring)
                    {
                        m_myPinCreationModel.SetInterior(showingInterior);
                        if(showingInterior)
                        {
                            const Eegeo::Resources::Interiors::InteriorsModel *pModel = NULL;
                            bool success = m_interiorController.TryGetCurrentModel(pModel);
                            if(success)
                            {
                                const Eegeo::Resources::Interiors::InteriorId& buildingId = pModel->GetId();
                                m_myPinCreationModel.SetBuildingId(buildingId);
                            }

                            m_myPinCreationModel.SetFloor(m_interiorController.GetCurrentFloorIndex());
                            m_myPinCreationModel.SetTerrainHeight(pModel->GetTangentSpaceBounds().GetMin().y);
                            float floorHeightAboveSeaLevel = Helpers::InteriorHeightHelpers::GetFloorHeightAboveSeaLevel(*pModel, m_interiorController.GetCurrentFloorIndex());
                            const float floorHeightAboveTerrain = floorHeightAboveSeaLevel - m_myPinCreationModel.GetTerrainHeight();
                            m_myPinCreationModel.SetHeightAboveTerrain(floorHeightAboveTerrain);
                        }
                        else
                        {
                            m_myPinCreationModel.SetBuildingId(Eegeo::Resources::Interiors::InteriorId::NullId());
                            m_myPinCreationModel.SetFloor(0);
                            m_myPinCreationModel.SetHeightAboveTerrain(0.0f);
                        }
                    }
                    Eegeo::m44 sphereTransformMatrix;
                    sphereTransformMatrix.Scale(m_ringRadius);

                    Eegeo::dv3 scaledPoint;
                    if(m_interiorsAffectedByFlattening)
                    {
                        scaledPoint = Eegeo::Rendering::EnvironmentFlatteningService::GetScaledPointEcef(m_myPinCreationModel.GetPosition(), m_environmentFlatteningService.GetCurrentScale());
                    }
                    else
                    {
                        scaledPoint = Eegeo::Rendering::EnvironmentFlatteningService::GetScaledPointAboveGroundEcef(m_myPinCreationModel.GetPosition(),
                                                                                                                    m_myPinCreationModel.GetHeightAboveTerrain(),
                                                                                                                    m_environmentFlatteningService.GetCurrentScale());
                    }

                    const bool ringIsOnScreen = RingIsOnScreen(renderCamera, scaledPoint, m_ringRadius);
                    
                    if (m_myPinCreationModel.GetCreationStage() == Inactive && !ringIsOnScreen)
                    {
                        m_scaleInterpolationParam = 0.f;
                    }
                    
                    m_poiRingView.SetShouldRenderRing(m_scaleInterpolationParam > 0.f && ringIsOnScreen);
                    
                    Eegeo::dv3 cameraRelativePosition = scaledPoint - renderCamera.GetEcefLocation();
                    sphereTransformMatrix.SetRow(3, Eegeo::v4(cameraRelativePosition.ToSingle(), 1.f));

                    m_poiRingView.SetRingTransforms(sphereTransformMatrix);

					const float interiorInnerRingScale = showingInterior ? 1.05f : 1.0f;
                    float altitudeBasedScale = CalculateAltitudeBasedSphereScale(altitude, m_ringRadius);

                    m_poiRingView.SetInnerSphereScale(altitudeBasedScale * interiorInnerRingScale);

                    const float assetSize = 75.f;
                    const float iconScale = Eegeo::Helpers::TransformHelpers::ComputeModelScaleForConstantScreenSizeWithVerticalFoV(renderCamera, scaledPoint) / (m_screenProperties.GetScreenHeight()* 0.5f)*m_screenProperties.GetPixelScale() * assetSize;
                    
                    m_iconSize = Eegeo::Max(iconScale * transitionScale, 0.0f);
                    m_poiRingView.AddIconSprite(renderCamera, scaledPoint, m_iconSize);

                    m_iconPosition = scaledPoint + (Eegeo::dv3)renderCamera.GetModelMatrix().GetRow(1) * m_iconSize * 0.5f;
                }

                float PoiRingController::CalculateTransitionScale(float dt)
                {
                    float delta = m_myPinCreationModel.GetCreationStage() == Ring ? dt : -dt;
                    delta /= m_easeDurationInSeconds;
                    m_scaleInterpolationParam = Eegeo::Clamp(m_scaleInterpolationParam + delta, 0.f, 1.f);
                    return Eegeo::Helpers::MathsHelpers::PennerElasticEaseInOut(0.f, 1.f, m_scaleInterpolationParam);
                }

                void PoiRingController::GetIconPositionAndSize(Eegeo::dv3& out_positionEcef, float& out_sizeMeters) const
                {
                    out_positionEcef = m_iconPosition;
                    out_sizeMeters = m_iconSize;
                }
                
                void PoiRingController::GetSpherePositionAndRadius(Eegeo::dv3& out_sphereCenterEcef, float& out_sphereRadius) const
                {
                    out_sphereCenterEcef = m_myPinCreationModel.GetPosition();
                    out_sphereRadius = m_ringRadius;
                }
            }
        }
    }
}
