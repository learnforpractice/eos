/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */


static uint32_t get_active_producers( uint64_t* producers, uint32_t size ) {

   auto active_producers = ctx().get_active_producers();

   size_t len = active_producers.size();
   if (len > size) {
      len = size;
   }

   memcpy( producers, active_producers.data(), len * 8 );
   return len;
}

