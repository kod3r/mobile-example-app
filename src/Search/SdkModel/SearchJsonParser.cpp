// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "SearchJsonParser.h"
#include "SearchResultModel.h"
#include "SearchVendorNames.h"
#include "YelpSearchJsonParser.h"

namespace ExampleApp
{
    namespace Search
    {
        namespace SdkModel
        {
            bool TryParseImageDetails(const Search::SdkModel::SearchResultModel& searchResultModel, std::string& out_imageUrl)
            {
                const std::string& vendor = searchResultModel.GetVendor();
                if(vendor == YelpVendorName)
                {
                    return Yelp::SdkModel::TryParseImageDetails(searchResultModel, out_imageUrl);
                }
                
                return false;
            }
        }
    }
}