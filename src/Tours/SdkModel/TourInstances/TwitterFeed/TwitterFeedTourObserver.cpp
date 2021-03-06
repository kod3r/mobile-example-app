// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "TwitterFeedTourObserver.h"
#include "TourStateModel.h"
#include "TourModel.h"
#include "TwitterFeedTourStateMachineFactory.h"
#include "ITourService.h"
#include "ITourRepository.h"
#include "ITwitterFeedService.h"
#include <vector>
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

namespace ExampleApp
{
    namespace Tours
    {
        namespace SdkModel
        {
            namespace TourInstances
            {
                namespace TwitterFeed
                {
                    TwitterFeedTourObserver::TwitterFeedTourObserver(Camera::IToursCameraTransitionController& toursCameraTransitionController,
                                                                     Camera::IToursCameraController& toursCameraController,
                                                                     ITourService& tourService,
                                                                     WorldPins::SdkModel::IWorldPinsService& worldPinsService,
                                                                     Eegeo::Resources::Interiors::InteriorController& interiorController,
                                                                     InteriorsExplorer::SdkModel::InteriorVisibilityUpdater& interiorVisibilityUpdater,
                                                                     const Eegeo::Resources::Interiors::InteriorSelectionModel& interiorSelectionModel,
                                                                     ITourRepository& tourRepository,
                                                                     Social::TwitterFeed::ITwitterFeedService& twitterFeedService,
                                                                     const std::map<std::string, TweetStateData>& tweetStateDataMap,
                                                                     const std::map<std::string, int>& twitterTourIconOverrideMap,
                                                                     Metrics::IMetricsService& metricsService,
                                                                     ExampleAppMessaging::TMessageBus& messageBus)
                    : m_toursCameraTransitionController(toursCameraTransitionController)
                    , m_toursCameraController(toursCameraController)
                    , m_tourService(tourService)
                    , m_worldPinsService(worldPinsService)
                    , m_interiorController(interiorController)
                    , m_interiorVisibilityUpdater(interiorVisibilityUpdater)
                    , m_interiorSelectionModel(interiorSelectionModel)
                    , m_tourRepository(tourRepository)
                    , m_twitterFeedService(twitterFeedService)
                    , m_twitterFeedLoadedCallback(this, &TwitterFeedTourObserver::OnTwitterFeedLoaded)
                    , m_tweetStateDataMap(tweetStateDataMap)
                    , m_twitterTourIconOverrideMap(twitterTourIconOverrideMap)
                    , m_metricsService(metricsService)
                    , m_messageBus(messageBus)
                    {
                        m_twitterFeedService.AddFeedLoadedCallback(m_twitterFeedLoadedCallback);
                    }
                    
                    TwitterFeedTourObserver::~TwitterFeedTourObserver()
                    {
                        m_twitterFeedService.RemoveFeedLoadedCallback(m_twitterFeedLoadedCallback);
                    }
                    
                    void TwitterFeedTourObserver::OnTwitterFeedLoaded(const std::string& userId,
                                                                      Social::TwitterFeed::TweetRepository& tweetRepository)
                    {
                        std::vector<Social::TwitterFeed::TweetModel*> tweets;
                        
                        if(tweetRepository.GetItemCount() == 0)
                        {
                            Eegeo_TTY("No tweets found for %s", userId.c_str());
                            return;
                        }
                        
                        for(int i = 0; i < tweetRepository.GetItemCount(); ++i)
                        {
                            tweets.push_back(tweetRepository.GetItemAtIndex(i));
                        }
                        
                        std::map<std::string, TweetStateData>::const_iterator tweetStateIt = m_tweetStateDataMap.find(userId);
                        
                        Eegeo_ASSERT(tweetStateIt != m_tweetStateDataMap.end(), "Couldn't find tweet state data for account: %s", userId.c_str());
                        
                        TweetStateData tweetStateData = (*tweetStateIt).second;
                        
                        Eegeo::Space::LatLong tourLocation = Eegeo::Space::LatLong::FromECEF(tweetStateData.ecefTarget);
                        
                        rapidjson::Document jsonDocument;
                        jsonDocument.SetObject();
                        
                        jsonDocument.AddMember("screenName", userId.c_str(), jsonDocument.GetAllocator());
                        jsonDocument.AddMember("imageUrl", tweetRepository.GetItemAtIndex(0)->GetBaseProfileImageUrl().c_str(), jsonDocument.GetAllocator());
                        
                        rapidjson::StringBuffer buffer;
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        jsonDocument.Accept(writer);
                        
                        std::string tourName = tweetRepository.GetItemAtIndex(0)->GetBaseUserName();
                        std::string twitterBaseUserName = tweetRepository.GetItemAtIndex(0)->GetBaseUserName();
                        const std::string twitterBaseProfileImage = tweetRepository.GetItemAtIndex(0)->GetBaseProfileImageThumbUrl();
                        
                        std::vector<TourStateModel> states;
                        for(int i = 0; i < tweets.size(); ++i)
                        {
                            states.push_back(TourStateModel(tweets[i]->GetUserName() + " (@"+tweets[i]->GetUserScreenName()+")",
                                                            tweets[i]->GetText(),
                                                            "",
                                                            "tours",
                                                            true,
                                                            tweets[i]));
                        }
                        
                        
                        const bool isInterior = tweetStateData.isInterior;
                        ExampleApp::WorldPins::SdkModel::WorldPinInteriorData worldPinInteriorData = tweetStateData.interiorData;
                        
                        const int twitterIcon = 13;
                        int iconIndex = twitterIcon;
                        
                        if(m_twitterTourIconOverrideMap.find(userId) != m_twitterTourIconOverrideMap.end())
                        {
                            iconIndex = m_twitterTourIconOverrideMap[userId];
                        }
                        
                        ExampleApp::Tours::SdkModel::TourModel tourModel(tourName,
                                                                         "@"+userId,
                                                                         iconIndex,
                                                                         tourLocation,
                                                                         tweetStateData.visibleOnMap,
                                                                         isInterior,
                                                                         worldPinInteriorData,
                                                                         false,
                                                                         Helpers::ColorHelpers::Color::FromRGB(30, 123, 195),
                                                                         Helpers::ColorHelpers::Color::FromRGB(255, 255, 255),
                                                                         Helpers::ColorHelpers::Color::FromRGB(30, 123, 195),
                                                                         Helpers::ColorHelpers::Color::FromRGB(30, 123, 195),
                                                                         states,
                                                                         true,
                                                                         twitterBaseUserName,
                                                                         twitterBaseProfileImage);
                        
                        TwitterFeedTourStateMachineFactory factory(m_toursCameraTransitionController,
                                                                   m_toursCameraController,
                                                                   m_worldPinsService,
                                                                   m_interiorController,
                                                                   m_interiorVisibilityUpdater,
                                                                   m_interiorSelectionModel,
                                                                   userId,
                                                                   m_tweetStateDataMap,
                                                                   m_metricsService,
                                                                   m_messageBus);
                        
                        m_tourService.AddTour(tourModel, *factory.CreateTourStateMachine(tourModel));
                        
                        // Assumes tours never removed
                        m_setOfTourTwitterBaseUserNames.insert(twitterBaseUserName);
                        
                        UpadateTweetLinksOut();
                    }
                    
                    void TwitterFeedTourObserver::UpadateTweetLinksOut()
                    {
                        int tourCount = static_cast<int>(m_tourRepository.GetItemCount());
                        
                        for(int i = 0; i < tourCount; ++i)
                        {
                            TourModel tourModel = m_tourRepository.GetItemAtIndex(i);
                            
                            if(!tourModel.UsesTwitter())
                            {
                                continue;
                            }
                            
                            const std::vector<TourStateModel>& states = tourModel.States();
                            
                            for(int stateIndex = 0; stateIndex < states.size(); ++stateIndex)
                            {
                                const TourStateModel& tourStateModel = states[stateIndex];
                                if(!tourStateModel.IsTweet())
                                {
                                    continue;
                                }
                                const std::string& userName = tourStateModel.Tweet()->GetUserName();
                                if(userName != tourModel.TwitterBaseUserName() &&
                                   m_setOfTourTwitterBaseUserNames.find(userName) != m_setOfTourTwitterBaseUserNames.end())
                                {
                                    tourStateModel.Tweet()->SetDoesLinkOut(true);
                                }
                            }
                        }

                    }
                    
                }
            }
        }
    }
}
