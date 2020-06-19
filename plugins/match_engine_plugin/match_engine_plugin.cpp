/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <iostream>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/chain/wast_to_wasm.hpp>
#include <fc/variant.hpp>
#include <fc/io/json.hpp>
#include <fc/exception/exception.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/signals2/connection.hpp>

#include <WAST/WAST.h>
#include <WASM/WASM.h>
#include <Runtime/Runtime.h>

#include <eosio/match_engine_plugin/match_engine_plugin.hpp>
#include <rdkafka.h>


const int PRODUCER_INIT_FAILED = -1;
const int PRODUCER_INIT_SUCCESS = 0;
const int PUSH_DATA_FAILED = -1;
const int PUSH_DATA_SUCCESS = 0;
std::string CONTRACT = "";
std::string WORKER = "frank1111111";
std::string WORKER_PRIVATE_KEY;
int16_t pair_id_start;
int16_t pair_id_end;
std::map<size_t, int> trx_cache;
int CLEAR_CACHE_INTERVAL;


#define CATCH_AND_CALL(NEXT)\
   catch ( const fc::exception& err ) {\
      NEXT(err.dynamic_copy_exception());\
   } catch ( const std::exception& e ) {\
      fc::exception fce( \
         FC_LOG_MESSAGE( warn, "rethrow ${what}: ", ("what",e.what())),\
         fc::std_exception_code,\
         BOOST_CORE_TYPEID(e).name(),\
         e.what() ) ;\
      NEXT(fce.dynamic_copy_exception());\
   } catch( ... ) {\
      fc::unhandled_exception e(\
         FC_LOG_MESSAGE(warn, "rethrow"),\
         std::current_exception());\
      NEXT(e.dynamic_copy_exception());\
   }

using namespace eosio::chain;
namespace eosio {
	using boost::signals2::scoped_connection;
	using namespace eosio;
	static appbase::abstract_plugin &_match_engine_plugin = app().register_plugin<match_engine_plugin>();
}


namespace eosio {

	class ProducerKafka {
	public:
		ProducerKafka() {};

		~ProducerKafka() {}

		int init_kafka(int partition, char *brokers, char *topic);

		int push_data_to_kafka(const string &data);

		void destroy();

	private:
		int partition_;

		//rd
		rd_kafka_t *handler_;
		rd_kafka_conf_t *conf_;

		//topic
		rd_kafka_topic_t *topic_;
		rd_kafka_topic_conf_t *topic_conf_;
	};


	int ProducerKafka::init_kafka(int partition, char *brokers, char *topic) {
		char tmp[16] = {0};
		char errstr[512] = {0};

		partition_ = partition;

		/* Kafka configuration */
		conf_ = rd_kafka_conf_new();

		//set logger :register log function

		/* Quick termination */
		snprintf(tmp, sizeof(tmp), "%i", SIGIO);
		rd_kafka_conf_set(conf_, "internal.termination.signal", tmp, NULL, 0);

		/*topic configuration*/
		topic_conf_ = rd_kafka_topic_conf_new();

		if (!(handler_ = rd_kafka_new(RD_KAFKA_PRODUCER, conf_, errstr, sizeof(errstr)))) {
			fprintf(stderr, "*****Failed to create new producer: %s*******\n", errstr);
			return PRODUCER_INIT_FAILED;
		}

		/* Add brokers */
		if (rd_kafka_brokers_add(handler_, brokers) == 0) {
			fprintf(stderr, "****** No valid brokers specified********\n");
			return PRODUCER_INIT_FAILED;
		}

		/* Create topic */
		topic_ = rd_kafka_topic_new(handler_, topic, topic_conf_);

		return PRODUCER_INIT_SUCCESS;
	}


	void ProducerKafka::destroy() {
		/* Destroy topic */
		rd_kafka_topic_destroy(topic_);

		/* Destroy the handle */
		rd_kafka_destroy(handler_);
	}

	int ProducerKafka::push_data_to_kafka(const string &data) {
		char *buffer;
		buffer = (char *) data.c_str();
		size_t buf_len = data.size();
		int ret;

		if (NULL == buffer)
			return 0;

		ret = rd_kafka_produce(topic_, partition_, RD_KAFKA_MSG_F_COPY,
							   (void *) buffer, (size_t) buf_len, NULL, 0, NULL);

		if (ret == -1) {
			fprintf(stderr, "****Failed to produce to topic %s partition %i: %s**d***\n",
					rd_kafka_topic_name(topic_), partition_,
					rd_kafka_err2str(rd_kafka_errno2err(errno)));

			rd_kafka_poll(handler_, 0);
			return PUSH_DATA_FAILED;
		}

		fprintf(stderr, "***Sent %d bytes to topic:%s partition:%i*****\n",
				static_cast<int>(buf_len), rd_kafka_topic_name(topic_), partition_);

		rd_kafka_poll(handler_, 0);

		return PUSH_DATA_SUCCESS;
	}


	class match_engine_plugin_impl {
	public:
		bool bypass_filter = false;
		chain_plugin *chain_plug = nullptr;
		fc::optional<scoped_connection> applied_transaction_connection;
		fc::optional<scoped_connection> accepted_block_connection;
		ProducerKafka *producer;
		deque<bytes> kafka_cache;
		string kafka_server;
		string kafka_topic;


		match_engine_plugin_impl() {
		}

		void on_action_trace(const action_trace &at) {
			/*
			 * on log action trigger
			 * */
			bool notincache =
					std::find(kafka_cache.begin(), kafka_cache.end(), at.act.data) == kafka_cache.end();
			if (notincache) {
				controller &cc = app().get_plugin<chain_plugin>().chain();
				const auto& findex_acnt = cc.get_account(N(findexfindex));
				auto findex_abi_def = findex_acnt.get_abi();
				fc::microseconds abi_serializer_max_time = app().get_plugin<chain_plugin>().get_abi_serializer_max_time();
				abi_serializer dex_serializer{findex_abi_def, abi_serializer::create_yield_function( abi_serializer_max_time )};
				string trade_log;
				auto variant = dex_serializer.binary_to_variant(dex_serializer.get_action_type(at.act.name),
																at.act.data, abi_serializer::create_yield_function( abi_serializer_max_time ));
				auto v = variant.get_object().find("data")->value();

			    const auto deadline = fc::time_point::now() + fc::exception::format_time_limit;
				trade_log = fc::json::to_string(v, deadline);
				std::ostringstream replace_stream;

				replace_stream << ", \"head_block_num\"" << ":" << cc.head_block_state()->block_num << "}";
				string replace_str = replace_stream.str();
				trade_log.replace(trade_log.end() - 1, trade_log.end(), replace_str);
				ilog("trade log is ${log}", ("log", trade_log));

				if (PUSH_DATA_SUCCESS == this->producer->push_data_to_kafka(trade_log)) {
					// ilog("push data success");
				} else {
					// ilog("push data failed");
				}
				kafka_cache.push_back(at.act.data);
			}
			if (kafka_cache.size() > 50)
				kafka_cache.pop_front();
		}


		void on_applied_transaction(const transaction_trace_ptr &trace) {
			for (const auto &atrace : trace->action_traces) {
				if (atrace.act.account == N(findexfindex) && atrace.act.name == N(log)) {
					on_action_trace(atrace);
				}
			}
		}

		struct async_result_visitor : public fc::visitor<std::string> {
		  	template<typename T>
			std::string operator()(const T &v) const {
			    const auto deadline = fc::time_point::now() + fc::exception::format_time_limit;
				return fc::json::to_string(v, deadline);
			}
		};

		void accepted_block(const block_state_ptr &bsp) {
			controller &cc = app().get_plugin<chain_plugin>().chain();
			int head_block_num = cc.head_block_num();

			match_engine_apis::read_write::trade_params params;
			match_engine_apis::read_write::trade(params,
												 [](const fc::static_variant<fc::exception_ptr, match_engine_apis::read_write::trade_log> &result) {
													 if (result.contains<fc::exception_ptr>()) {
														 try {
															 result.get<fc::exception_ptr>()->dynamic_rethrow_exception();
														 } catch (...) {
															 ilog("call back error...");
														 }
													 } else {
														 result.visit(async_result_visitor());
													 }
												 });

		}

	};

	match_engine_plugin::match_engine_plugin() : my(new match_engine_plugin_impl()) {
		ilog("matching engine initialize...");
	}

	match_engine_plugin::~match_engine_plugin() {}

	void match_engine_plugin::set_program_options(options_description &, options_description &cfg) {

		cfg.add_options()
				("kafka-server", bpo::value<string>()->notifier([this](const string &v) {
					ilog("kafka server IP address: ${o}", ("o", my->kafka_server));
				}), "Use to push transaction log to kafka. format eg: 13.229.116.115:9092")
				("kafka-topic", bpo::value<string>()->notifier([this](const string &v) {
					ilog("kafka topic name: ${o}", ("o", my->kafka_topic));
				}), "Use to push transaction log to kafka topic. eg: trade-log")
				("findex-contract", bpo::value<string>()->composing(), "findex-contract name")
				("findex-worker", bpo::value<string>()->composing(), "findex-worker name")
				("findex-worker-privkey", bpo::value<string>()->composing(), "findex-worker privite name")
				("clear-cache-interval", bpo::value<int16_t>()->default_value(5),
				 "delete push transaction cache when large than the number")
				("pair-id-start", bpo::value<int16_t>()->default_value(1), "Pair start value")
				("pair-id-end", bpo::value<int16_t>()->default_value(10), "Pair end value");
	}

	void match_engine_plugin::plugin_initialize(const variables_map &options) {

		CONTRACT = options.at("findex-contract").as<string>();
		WORKER = options.at("findex-worker").as<string>();
		WORKER_PRIVATE_KEY = options.at("findex-worker-privkey").as<string>();
		pair_id_start = options.at("pair-id-start").as<int16_t>();
		pair_id_end = options.at("pair-id-end").as<int16_t>();
		CLEAR_CACHE_INTERVAL = options.at("clear-cache-interval").as<int16_t>();

		char *broker0 = (char *) options.at("kafka-server").as<string>().c_str();
		char *topic_name = (char *) options.at("kafka-topic").as<string>().c_str();

		my->producer = new ProducerKafka;
		if (PRODUCER_INIT_SUCCESS == my->producer->init_kafka(0, broker0, topic_name)) {
			ilog("producer init success");
		} else {
			ilog("producer init failed");
		}

		my->chain_plug = app().find_plugin<chain_plugin>();
		auto &chain = my->chain_plug->chain();


		my->applied_transaction_connection.emplace(
				chain.applied_transaction.connect( [&]( std::tuple<const transaction_trace_ptr&, const signed_transaction&> t ) {
					my->on_applied_transaction( std::get<0>(t) );
				} ));

		my->accepted_block_connection.emplace(
				chain.accepted_block_header.connect([&](const chain::block_state_ptr &bs) {
					my->accepted_block(bs);
				}));
	}


	void match_engine_plugin::plugin_startup() {
		// Make the magic happen
		ilog("magic happening...");
	}

	void match_engine_plugin::plugin_shutdown() {
		// OK, that's enough magic
	}


	namespace match_engine_apis {

		chain_apis::read_only::get_table_rows_result read_only::get_orders(const read_only::get_orders_params &params) {
			ilog("on get orders .... ");
			eosio::chain_apis::read_only::get_table_rows_params params_buy = {
					true, string_to_name((char *) CONTRACT.c_str()), CONTRACT, name("orders"), "", "", ""
			};

			eosio::chain_apis::read_only::get_table_rows_result orders;
			orders = app().get_plugin<chain_plugin>().get_read_only_api().get_table_rows(params_buy);
			return orders;
		}

		static void push_next_transaction(const std::shared_ptr<std::vector<signed_transaction>> &trxs, size_t index,
										  const std::function<void(const fc::exception_ptr &)> &next) {
			chain_plugin &cp = app().get_plugin<chain_plugin>();
			cp.accept_transaction(std::make_shared<packed_transaction>(trxs->at(index)),
								  [=](const fc::static_variant<fc::exception_ptr, transaction_trace_ptr> &result) {});
		}

		void
		push_transactions(std::vector<signed_transaction> &&trxs, const std::function<void(fc::exception_ptr)> &next) {
			auto trxs_copy = std::make_shared<std::decay_t<decltype(trxs)>>(std::move(trxs));
			push_next_transaction(trxs_copy, 0, next);
		}

		chain::action generate_nonce_action() {
	    		return chain::action( {}, config::null_account_name, name("nonce"), fc::raw::pack(fc::time_point::now().time_since_epoch().count()));
		}

		void
		push_trade_transaction(string &pair_id, uint64_t buy_order_id, uint64_t sell_order_id, uint64_t deal_quantity,
							   chain::plugin_interface::next_function<eosio::chain_apis::read_write::push_transaction_results> next) {
			std::vector<signed_transaction> trxs;
			trxs.reserve(2);
			controller &cc = app().get_plugin<chain_plugin>().chain();
			const auto& findex_acnt = cc.get_account(N(findexfindex));
			auto findex_abi_def = findex_acnt.get_abi();

			auto abi_serializer_max_time = app().get_plugin<chain_plugin>().get_abi_serializer_max_time();
			abi_serializer dex_serializer{findex_abi_def, abi_serializer::create_yield_function( abi_serializer_max_time )};

			auto chainid = app().get_plugin<chain_plugin>().get_chain_id();

			auto admin_priv_key = private_key_type(WORKER_PRIVATE_KEY);
			signed_transaction trx;
			action act;

			act.account = string_to_name((char *) CONTRACT.c_str());
			act.name = N(executetrade);
			act.authorization = vector<permission_level>{{string_to_name(
					(char *) WORKER.c_str()), config::active_name}};
			string execute_trade = fc::format_string(
					"[{\"pair_id\": \"${p}\", \"buy_order_id\":\"${l}\", \"sell_order_id\":\"${k}\"}]",
					(fc::mutable_variant_object()("p", pair_id)("l", buy_order_id)("k", sell_order_id))
			);
			act.data = dex_serializer.variant_to_binary("executetrade", fc::json::from_string(execute_trade),
														abi_serializer::create_yield_function(abi_serializer_max_time));
			trx.actions.push_back(act);

			trx.expiration = cc.head_block_time() + fc::seconds(30);
			trx.max_net_usage_words = 5000;
			trx.context_free_actions.emplace_back(generate_nonce_action());
			trx.set_reference_block(cc.head_block_id());
			trx.sign(admin_priv_key, chainid);

			trxs.emplace_back(std::move(trx));

			std::hash<std::string> hasher;
			std::ostringstream u_str;
			u_str << string(act.data.begin(), act.data.end()) << deal_quantity;
			auto cache_key = hasher(u_str.str());
			auto cache_it = trx_cache.find(cache_key);
			if (cache_it == trx_cache.end()) {
				push_transactions(std::move(trxs), next);
				trx_cache.insert(std::make_pair(cache_key, cc.head_block_num()));
			} else if (cache_it != trx_cache.end() && cc.head_block_num() - cache_it->second > CLEAR_CACHE_INTERVAL) {
				push_transactions(std::move(trxs), next);
				trx_cache[cache_key] = cc.head_block_num();
			}

			/*
			 * clear transaction cache
			 * */
			for (auto it = trx_cache.begin(); it != trx_cache.end();) {
				it = (cc.head_block_num() - it->second > CLEAR_CACHE_INTERVAL) ? trx_cache.erase(it) : std::next(it);
			}

		}

		struct order_info {
			uint64_t price;
			long quantity;
			uint64_t id;
			time_point_sec place_time;
		};


		void limit_order_deal(string &pair_id, order_info &limit_buy_order, order_info &limit_sell_order,
							  chain::plugin_interface::next_function<eosio::chain_apis::read_write::push_transaction_results> next) {
			auto deal_quantity = limit_buy_order.quantity > limit_sell_order.quantity ? limit_sell_order.quantity
																					  : limit_buy_order.quantity;
			try {
				match_engine_apis::push_trade_transaction(pair_id, limit_buy_order.id,
														  limit_sell_order.id, deal_quantity,
														  next);
				limit_buy_order.quantity -= deal_quantity;
				limit_sell_order.quantity -= deal_quantity;
			} catch (...) {
				ilog("action limit order deal exception");
			}
		}

		void market_buy_limit_sell_deal(string &pair_id, order_info &market_buy_order, order_info &limit_sell_order,
										uint64_t price_precision, uint64_t quote_precision, uint64_t base_precision,
										chain::plugin_interface::next_function<eosio::chain_apis::read_write::push_transaction_results> next) {
			// buy market
			ilog("sell price ...: ${sell_price}", ("sell_price", limit_sell_order.price));
			ilog("sell quantity...: ${sell_quantity}",
				 ("sell_quantity", static_cast<uint64_t>(limit_sell_order.quantity)));

			auto deal_price = limit_sell_order.price;
			auto max_market_buy_quote =
					(market_buy_order.quantity) / (static_cast<double >(deal_price) / price_precision) /
					base_precision * quote_precision;
			auto deal_quantity_quote = max_market_buy_quote > limit_sell_order.quantity ? limit_sell_order.quantity
																						: max_market_buy_quote;
			auto deal_quantity_base = static_cast<double>(deal_price) / price_precision * deal_quantity_quote /
									  quote_precision * base_precision;

			try {
				match_engine_apis::push_trade_transaction(pair_id,
														  market_buy_order.id,
														  limit_sell_order.id,
														  limit_sell_order.quantity,
														  next);
				market_buy_order.quantity -= static_cast<uint64_t >(deal_quantity_base);
				limit_sell_order.quantity -= static_cast<uint64_t >(deal_quantity_quote);
			} catch (...) {
				ilog("action market buy order deal with limit sell order exception");
			}
		}

		void market_sell_limit_buy_deal(string &pair_id, order_info &limit_buy_order, order_info &market_sell_order,
										chain::plugin_interface::next_function<eosio::chain_apis::read_write::push_transaction_results> next) {
			auto deal_quantity =
					limit_buy_order.quantity > market_sell_order.quantity
					? market_sell_order.quantity
					: limit_buy_order.quantity;
			try {
				match_engine_apis::push_trade_transaction(pair_id,
														  limit_buy_order.id,
														  market_sell_order.id,
														  deal_quantity,
														  next);
				limit_buy_order.quantity -= deal_quantity;
				market_sell_order.quantity -= deal_quantity;
			} catch (...) {
				ilog("action limit order deal exception");
			}
		}


		void read_write::trade
				(const read_write::trade_params &params,
				 chain::plugin_interface::next_function<eosio::chain_apis::read_write::push_transaction_results> next) {

			uint64_t price_precision = 1000000;
			uint64_t quote_precision = 10000;
			uint64_t base_precision = 10000;
			eosio::chain_apis::read_only::get_table_rows_result pairs;
			eosio::chain_apis::read_only::get_table_rows_result tokens;
			for (uint16_t int_pair_id = pair_id_start; int_pair_id <= pair_id_end; ++int_pair_id) {
				string pair_id = std::to_string(int_pair_id);
				eosio::chain_apis::read_only::get_table_rows_result buy_orders;
				eosio::chain_apis::read_only::get_table_rows_result sell_orders;
				try {
					eosio::chain_apis::read_only::get_table_rows_params params_buy = {
							true, string_to_name((char *) CONTRACT.c_str()), pair_id + "01", name("orders"), "", "", "", 10,
							"i128", "3"
					};

					eosio::chain_apis::read_only::get_table_rows_params params_sell = {
							true, string_to_name((char *) CONTRACT.c_str()), pair_id + "02", name("orders"), "", "", "", 10,
							"i128", "2"
					};

					buy_orders = app().get_plugin<chain_plugin>().get_read_only_api().get_table_rows(params_buy);
					sell_orders = app().get_plugin<chain_plugin>().get_read_only_api().get_table_rows(params_sell);
				} catch (...) {
					ilog("can not fetch order book.");
				}

				vector<order_info> buy_queue;
				vector<order_info> sell_queue;
				vector<order_info> market_buy_queue;
				vector<order_info> market_sell_queue;

				for (auto &buy_order_item: buy_orders.rows) {
					auto buy_price = buy_order_item.get_object().find("price")->value().as_uint64();
					auto buy_order_time = buy_order_item.get_object().find("place_time")->value().as_string();
					auto buy_order =
							order_info{.price = buy_price,
									.quantity = asset::from_string(buy_order_item.get_object().find(
											"quantity")->value().as_string()).get_amount(),
									.id = buy_order_item.get_object().find("id")->value().as_uint64(),
									.place_time = fc::time_point_sec::from_iso_string(buy_order_time)};
					if (buy_price == 0) {
						market_buy_queue.emplace_back(buy_order);
					} else {
						buy_queue.emplace_back(buy_order);
					}
				}
//				ilog("buy orders is size is: ${size}", ("size", buy_orders.rows.size()));

				for (auto &sell_order_item: sell_orders.rows) {
					auto sell_price = sell_order_item.get_object().find("price")->value().as_uint64();
					auto sell_order_time = sell_order_item.get_object().find("place_time")->value().as_string();
					auto sell_order =
							order_info{.price = sell_price,
									.quantity = asset::from_string(sell_order_item.get_object().find(
											"quantity")->value().as_string()).get_amount(),
									.id = sell_order_item.get_object().find("id")->value().as_uint64(),
									.place_time = fc::time_point_sec::from_iso_string(sell_order_time)};
					if (sell_price == 0) {
						market_sell_queue.emplace_back(sell_order);
					} else {
						sell_queue.emplace_back(sell_order);
					}
				}

				if (!market_buy_queue.empty() || !market_sell_queue.empty()) {
					// get price precision for this pair
					try {
						eosio::chain_apis::read_only::get_table_rows_params param_pair = {
								true, string_to_name((char *) CONTRACT.c_str()), CONTRACT, name("pairs"), "", "", "",
								int_pair_id, "", "1"
						};
						pairs = app().get_plugin<chain_plugin>().get_read_only_api().get_table_rows(param_pair);
						pairs.more = true;

						if (!pairs.rows.empty()) {
							auto pair_itr = std::find_if(pairs.rows.begin(), pairs.rows.end(), [&](auto &pair_obj) {
								return pair_obj.get_object().find("id")->value().as_uint64() == stoi(pair_id);
							});

							price_precision = pair_itr->get_object().find("price_precision")->value().as_uint64();
							uint64_t quote_id = pair_itr->get_object().find("quote_id")->value().as_uint64();
							uint64_t base_id = pair_itr->get_object().find("base_id")->value().as_uint64();

							auto find_quote = [&](auto &token_obj) {
								return token_obj.get_object().find("id")->value().as_uint64() == quote_id;
							};

							auto find_base = [&](auto &token_obj) {
								return token_obj.get_object().find("id")->value().as_uint64() == base_id;
							};

							eosio::chain_apis::read_only::get_table_rows_params param_tokens = {
									true, string_to_name((char *) CONTRACT.c_str()), CONTRACT, name("tokens"), "", "", "",
									static_cast<uint32_t >(std::max(quote_id, base_id)), "", "1"
							};
							tokens = app().get_plugin<chain_plugin>().get_read_only_api().get_table_rows(param_tokens);
							tokens.more = true;
							auto quote_itr = std::find_if(tokens.rows.begin(), tokens.rows.end(), find_quote);
							auto base_itr = std::find_if(tokens.rows.begin(), tokens.rows.end(), find_base);

							quote_precision = quote_itr->get_object().find("precision")->value().as_uint64();
							base_precision = base_itr->get_object().find("precision")->value().as_uint64();
						} else {
							continue;
						}

					} catch (...) { return; }
				}

				if (!buy_queue.empty() && !sell_queue.empty()) {
//				ilog("buy queue size ${buy}", ("buy", buy_queue.size()));
//				ilog("sell queue size ${sell}", ("sell", sell_queue.size()));
					try {
						for (auto &limit_buy_order: buy_queue) {
							uint64_t limit_buy_price = limit_buy_order.price;
							time_point_sec limit_buy_order_time = limit_buy_order.place_time;

							for (auto &limit_sell_order: sell_queue) {
								uint64_t limit_sell_price = limit_sell_order.price;
								time_point_sec limit_sell_order_time = limit_sell_order.place_time;

								// only limit order deal
								if (market_buy_queue.empty() && market_sell_queue.empty()
									&& limit_buy_order.quantity > 0 && limit_sell_order.quantity > 0) {
									if (limit_buy_price >= limit_sell_price) {
										limit_order_deal(pair_id, limit_buy_order, limit_sell_order, next);
									}
									break;
								}

								// market order exist
								if (!market_buy_queue.empty()) {
									for (auto &market_buy_order: market_buy_queue) {
										time_point_sec market_buy_order_time = market_buy_order.place_time;

										if (!market_sell_queue.empty()) {
											for (auto &market_sell_order: market_sell_queue) {
												time_point_sec market_sell_order_time = market_sell_order.place_time;

												if (market_buy_order_time <= market_sell_order_time) {
													// buy market order time is earlier
													if (limit_buy_order.quantity > 0 && limit_sell_order.quantity > 0 &&
														limit_buy_price >= limit_sell_price &&
														limit_buy_order_time <= market_buy_order_time) {
														ilog("coming limit order deal...0..");
														limit_order_deal(pair_id, limit_buy_order, limit_sell_order,
																		 next);
													} else if (market_buy_order.quantity > 0 &&
															   limit_sell_order.quantity > 0) {
														ilog("coming buy market...0..");
														market_buy_limit_sell_deal(pair_id, market_buy_order,
																				   limit_sell_order, price_precision,
																				   quote_precision, base_precision,
																				   next);
													}
												} else {
													// sell market order time is earlier
													if (limit_buy_order.quantity > 0 && limit_sell_order.quantity > 0 &&
														limit_buy_price >= limit_sell_price &&
														limit_sell_order_time <= market_sell_order_time) {
														ilog("coming limit order deal...1..");
														limit_order_deal(pair_id, limit_buy_order, limit_sell_order,
																		 next);
													} else if (limit_buy_order.quantity > 0 &&
															   market_sell_order.quantity > 0) {
														ilog("coming sell market deal...1..");
														market_sell_limit_buy_deal(pair_id, limit_buy_order,
																				   market_sell_order, next);
													}
												}
											}
										} else {
											if (limit_buy_order.quantity > 0 && limit_sell_order.quantity > 0 &&
												limit_buy_price >= limit_sell_price &&
												limit_buy_order_time <= market_buy_order_time) {
												ilog("coming limit order deal...2..");
												limit_order_deal(pair_id, limit_buy_order, limit_sell_order, next);
											} else if (market_buy_order.quantity > 0 && limit_sell_order.quantity > 0) {
												// market buy
												ilog("coming buy market...2...");
												market_buy_limit_sell_deal(pair_id, market_buy_order,
																		   limit_sell_order, price_precision,
																		   quote_precision, base_precision,
																		   next);
											}
										}

									}
								}

								if (!market_sell_queue.empty()) {
									for (auto &market_sell_order: market_sell_queue) {
										time_point_sec market_sell_order_time = market_sell_order.place_time;
										// sell market
										if (limit_buy_order.quantity > 0 && limit_sell_order.quantity > 0 &&
											limit_buy_price >= limit_sell_price &&
											limit_sell_order_time <= market_sell_order_time) {
											ilog("coming limit order deal...3..");
											limit_order_deal(pair_id, limit_buy_order, limit_sell_order, next);
										} else if (limit_buy_order.quantity > 0 && market_sell_order.quantity > 0) {
											ilog("coming sell market deal...3..");
											market_sell_limit_buy_deal(pair_id, limit_buy_order,
																	   market_sell_order, next);
										}
									}
								}
							}
						}
					} CATCH_AND_CALL(next)
				} else if (buy_queue.empty() && !market_buy_queue.empty() && !sell_queue.empty()) {
					// buy market
					try {
						for (auto &market_buy_order: market_buy_queue) {
							for (auto &limit_sell_order: sell_queue) {
								ilog("coming buy market...4...");
								market_buy_limit_sell_deal(pair_id, market_buy_order,
														   limit_sell_order, price_precision,
														   quote_precision, base_precision,
														   next);
							}
						}
					} CATCH_AND_CALL(next)

				} else if (sell_queue.empty() && !market_sell_queue.empty() && !buy_queue.empty()) {
					// sell market
					try {
						for (auto &limit_buy_order: buy_queue) {
							for (auto &market_sell_order: market_sell_queue) {
								ilog("coming sell order deal...5..");
								market_sell_limit_buy_deal(pair_id, limit_buy_order,
														   market_sell_order, next);

							}
						}
					} CATCH_AND_CALL(next)
				}
			}
		}

	}
}