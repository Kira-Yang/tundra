#include "StableHeaders.h"
#include "AssetModule.h"
#include "AssetInterface.h"
#include "AssetDefines.h"
#include "AssetEvents.h"
#include "AssetManager.h"
#include "AssetCache.h"
#include "RexAsset.h"

using namespace OpenSimProtocol;
using namespace RexTypes;

namespace Asset
{
    AssetManager::AssetManager(Foundation::Framework* framework) : 
        framework_(framework)
    {
        Foundation::EventManagerPtr event_manager = framework_->GetEventManager();
        
        event_category_ = event_manager->RegisterEventCategory("Asset");
        event_manager->RegisterEvent(event_category_, Events::ASSET_READY, "AssetReady");
        event_manager->RegisterEvent(event_category_, Events::ASSET_PROGRESS, "AssetProgress");
        event_manager->RegisterEvent(event_category_, Events::ASSET_CANCELED, "AssetCanceled");
        
        // Create asset cache
        cache_ = AssetCachePtr(new AssetCache(framework_));
    }
    
    AssetManager::~AssetManager()
    {
        cache_.reset();
        providers_.clear();
    }
    
    Foundation::AssetPtr AssetManager::GetAsset(const std::string& asset_id, Core::asset_type_t asset_type)
    {
        return GetFromCache(asset_id);
    }
  
    bool AssetManager::RequestAsset(const std::string& asset_id, Core::asset_type_t asset_type)
    {
        RexUUID asset_uuid(asset_id);

        if (GetFromCache(asset_id))
            return true;
        
        AssetProviderVector::iterator i = providers_.begin();
        while (i != providers_.end())
        {
            // See if a provider can handle request
            if ((*i)->RequestAsset(asset_id, asset_type))
                return true;
            
            ++i;
        }
        
        AssetModule::LogWarning("No asset provider would accept request for asset " + asset_id);
        return false;
    }
    
    Foundation::AssetPtr AssetManager::GetIncompleteAsset(const std::string& asset_id, Core::asset_type_t asset_type, Core::uint received)
    {
        if (!received)
            return Foundation::AssetPtr();
              
        // See if any provider has ongoing transfer for this asset
        AssetProviderVector::iterator i = providers_.begin();
        while (i != providers_.end())
        {
            if ((*i)->InProgress(asset_id))
                return (*i)->GetIncompleteAsset(asset_id, asset_type, received);

            ++i;
        }               
        
        // No transfer, either get complete asset or nothing      
        return GetAsset(asset_id, asset_type);
            
        // Not enough bytes
        return Foundation::AssetPtr();
    }
    
    bool AssetManager::QueryAssetStatus(const std::string& asset_id, Core::uint& size, Core::uint& received, Core::uint& received_continuous)
    {
        // See if any provider has ongoing transfer for this asset
        AssetProviderVector::iterator i = providers_.begin();
        while (i != providers_.end())
        {
            if ((*i)->InProgress(asset_id))
                return (*i)->QueryAssetStatus(asset_id, size, received, received_continuous);

            ++i;
        }          
        
        // If not ongoing, check cache
        Foundation::AssetPtr asset = GetFromCache(asset_id);
        if (asset)
        {
            size = asset->GetSize();
            received = asset->GetSize();
            received_continuous = asset->GetSize();
            return true;
        }
        
        return false;
    }
    
    void AssetManager::StoreAsset(Foundation::AssetPtr asset)
    {
        cache_->StoreAsset(asset);
    }
    
    bool AssetManager::RegisterAssetProvider(Foundation::AssetProviderPtr asset_provider)
    {
        if (!asset_provider)
        {
            AssetModule::LogWarning("Attempted to register asset provider with null pointer");
            return false;
        }
        
        AssetProviderVector::iterator i = providers_.begin();
        while (i != providers_.end())
        {
            if ((*i) == asset_provider)
            {
                AssetModule::LogWarning("Asset provider " + asset_provider->Name() + " already registered");
                return false;
            }            
            ++i;
        }        
        
        providers_.push_back(asset_provider);
        AssetModule::LogWarning("Asset provider " + asset_provider->Name()  + " registered");        
        return true;
    }
    
    bool AssetManager::UnregisterAssetProvider(Foundation::AssetProviderPtr asset_provider)
    {
        if (!asset_provider)
        {
            AssetModule::LogWarning("Attempted to unregister asset provider with null pointer");
            return false;
        }    
        
        AssetProviderVector::iterator i = providers_.begin();
        while (i != providers_.end())
        {
            if ((*i) == asset_provider)
            {
                providers_.erase(i);
                AssetModule::LogWarning("Asset provider " + asset_provider->Name()  + " unregistered");
                return true;
            }            
            ++i;
        }         
        
        AssetModule::LogWarning("Asset provider " + asset_provider->Name()  + " not found, could not unregister");        
        return false;
    }    
        
    void AssetManager::Update(Core::f64 frametime)
    {
        // Update all providers
        AssetProviderVector::iterator i = providers_.begin();
        while (i != providers_.end())
        {
            (*i)->Update(frametime);
            ++i;
        }       
    }
    
    Foundation::AssetPtr AssetManager::GetFromCache(const std::string& asset_id)
    {
        // First check memory cache
        Foundation::AssetPtr asset = cache_->GetAsset(asset_id, true, false);
        if (asset)
            return asset;

        // If transfer in progress in any of the providers, do not check disk cache again
        AssetProviderVector::iterator i = providers_.begin();
        while (i != providers_.end())
        {
            if ((*i)->InProgress(asset_id))
                return Foundation::AssetPtr();
            ++i;
        } 
            
        // Last check disk cache
        asset = cache_->GetAsset(asset_id, false, true);    
        return asset;    
    }
}