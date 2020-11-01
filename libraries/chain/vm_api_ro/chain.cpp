/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */



static vector<account_name> get_active_producers() {
   const auto& ap = chain_api_get_controller().active_producers();
   vector<account_name> accounts; accounts.reserve( ap.producers.size() );

   for(const auto& producer : ap.producers )
      accounts.push_back(producer.producer_name);

   return accounts;
}

static uint32_t get_active_producers( uint64_t* producers, uint32_t size ) {

   auto active_producers = get_active_producers();

   size_t len = active_producers.size();
   if (len > size) {
      len = size;
   }

   memcpy( producers, active_producers.data(), len * 8 );
   return len;
}


