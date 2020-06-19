/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <appbase/application.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/history_plugin/history_plugin.hpp>


namespace eosio {
	using std::shared_ptr;
	typedef shared_ptr<class match_engine_plugin_impl> match_engine_ptr;
	typedef shared_ptr<const class match_engine_plugin_impl> match_engine_const_ptr;
	using namespace appbase;

	namespace match_engine_apis {
		class read_only {
		public:
			match_engine_const_ptr match_engine;

		public:
			read_only(match_engine_const_ptr &&match_engine)
					: match_engine(match_engine) {}




			struct get_orders_params {
				int pkey;
			};

			struct order {
				account_name proposer;
				double price;
				double amount;
				string from_sym;
				string to_sym;
				uint8_t trade_type;
			};



			chain_apis::read_only::get_table_rows_result get_orders(const get_orders_params &params);


		};
		class read_write {
		public:
			match_engine_ptr match_engine;
		public:
			read_write(match_engine_ptr &&match_engine)
			: match_engine(match_engine) {}

			struct trade_params{
				int pkey;
			};

			struct trade_log {
				fc::variant                 processed;
			};

			static void trade(const trade_params &params, chain::plugin_interface::next_function<eosio::chain_apis::read_write::push_transaction_results> next);
		};
	}

/**
 *  This is a template plugin, intended to serve as a starting point for making new plugins
 */
	class match_engine_plugin : public appbase::plugin<match_engine_plugin> {
	public:
		match_engine_plugin();

		virtual ~match_engine_plugin();

		APPBASE_PLUGIN_REQUIRES()

		virtual void set_program_options(options_description &, options_description &cfg) override;

		void plugin_initialize(const variables_map &options);

		void plugin_startup();

		void plugin_shutdown();

		match_engine_apis::read_only get_read_only_api() const { return match_engine_apis::read_only(match_engine_const_ptr(my)); }
		match_engine_apis::read_write get_read_write_api() { return match_engine_apis::read_write(match_engine_ptr(my)); }

	private:
		match_engine_ptr my;
	};

}

FC_REFLECT(eosio::match_engine_apis::read_write::trade_params, (pkey))
FC_REFLECT(eosio::match_engine_apis::read_only::get_orders_params, (pkey))
FC_REFLECT(eosio::match_engine_apis::read_only::order, (proposer)(price)(amount)(from_sym)(to_sym)(trade_type))
FC_REFLECT(eosio::match_engine_apis::read_write::trade_log, (processed))
