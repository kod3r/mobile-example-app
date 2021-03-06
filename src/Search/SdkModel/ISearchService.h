// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include <vector>
#include "Search.h"
#include "Space.h"
#include "ICallback.h"
#include "IdentitySearchCallbackData.h"
#include "SearchResultModel.h"

namespace ExampleApp
{
    namespace Search
    {
        namespace SdkModel
        {
            class ISearchService
            {
            public:
                virtual ~ISearchService() { }
                
                virtual bool CanHandleCategory(const std::string& category) const = 0;

                virtual void CancelInFlightQueries() = 0;

                // Implementation should call PerFormedQueryCallbacks and ReceivedQueryResultsCallback even if no search is made or falure occurs 
                virtual void PerformLocationQuerySearch(const SearchQuery& query) = 0;
                
                // Implementation should search for an update for the given SearchResultModel
                virtual void PerformIdentitySearch(const SearchResultModel& outdatedSearchResult,
                                                   Eegeo::Helpers::ICallback1<const IdentitySearchCallbackData&>& callback) = 0;

                virtual void InsertOnPerformedQueryCallback(Eegeo::Helpers::ICallback1<const SearchQuery&>& callback) = 0;

                virtual void RemoveOnPerformedQueryCallback(Eegeo::Helpers::ICallback1<const SearchQuery&>& callback) = 0;

                virtual void InsertOnReceivedQueryResultsCallback(Eegeo::Helpers::ICallback2<const SearchQuery&, const std::vector<SearchResultModel>& >& callback) = 0;

                virtual void RemoveOnReceivedQueryResultsCallback(Eegeo::Helpers::ICallback2<const SearchQuery&, const std::vector<SearchResultModel>& >& callback) = 0;
            };
        }
    }
}
