#ifndef MAIN_H_
#define MAIN_H_

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/uri.hpp>

#include "./TST.hpp"
#include "ConfigParser.hpp"
#include "utill.hpp"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

typedef std::string ValueType;
int S2_LV = 20;
std::string TIME_RES = "hour";


/* Load data from MongoDB */
mongocxx::cursor load_data(const std::map<std::string, std::string> &config)
{
    std::stringstream conn_addr;
    conn_addr << "mongodb://" << config.at("database.id") << ":" << config.at("database.password") << "@" << config.at("database.ip") << ":" << config.at("database.port");
    std::string conn_addr_s = conn_addr.str();

    // Connect to MongoDB
    conn = mongocxx::client{mongocxx::uri{conn_addr_s}};

    auto db = conn[config.at("database.db")];
    auto collection = db[config.at("database.collection")]; // Connect to collection

    mongocxx::cursor cursor = collection.find({}); // Query to MongoDB

    return cursor;
}

/* Pre-loading Dataset then build TST */
std::vector<bsoncxx::document::value> build_index(TST::TST<ValueType>& trie, const std::map<std::string, std::string>& config){

    // Load All IoT Dataset
    mongocxx::cursor res = load_data(config);
    std::vector<bsoncxx::document::value> documents;
    std::vector<double> encoding_times;
    std::vector<double> insertion_times;

    for (auto&& doc : res)
        documents.push_back(bsoncxx::document::value(doc));

    for(auto& doc_val : documents){
        auto doc = doc_val.view();
        auto DATE_OBJ = doc["DATE"];
        auto LOCATION_OBJ = doc["location"];

        double LAT;
        double LNG;
        int64_t unix_timestamp_s;

        // Temporal Information Preprocessing
        auto DATE_DOC = DATE_OBJ.get_document().view();
        auto DATE_START = DATE_DOC["START"];
        if (DATE_START && DATE_START.type() == bsoncxx::type::k_date) {
            unix_timestamp_s = DATE_START.get_date().value.count() / 1000; // Timestamp's unit is second
        } else {
            std::cerr << "START field is missing or not a valid date." << std::endl;
            break;
        }

        auto [year, month, day, hour, minute, second] = ExtractTimestampComponents(unix_timestamp_s);
        
        // Spatial Information Preprocessing
        auto LOCATION_DOC = LOCATION_OBJ.get_document().view();
        auto COORDINATES = LOCATION_DOC["coordinates"];
        if (COORDINATES && COORDINATES.type() == bsoncxx::type::k_array) {
			auto COOR_Arr = COORDINATES.get_array().value;
			auto it = COOR_Arr.begin();

			if (it != COOR_Arr.end()) {
				LNG = it->get_double().value;
				++it;

				if (it != COOR_Arr.end()) {
					LAT = it->get_double().value;
				}
			}
		}

        // Extract IoT Data (In this case, we only use object ID)
        ValueType object_id = doc["_id"].get_oid().value.to_string();

        /* Encoding */
        auto start_encoding = std::chrono::system_clock::now();
        unsigned int encoded_temp = trie.time_encoder(year, month, day, hour);
        unsigned long long encoded_spat = trie.space_encoder(LAT, LNG);
        auto end_encoding = std::chrono::system_clock::now();
        encoding_times.push_back(std::chrono::duration<double, std::milli>(end_encoding - start_encoding).count());
        
        /* Node Insertion */
        trie.Insert(encoded_temp, encoded_spat, object_id);
    }

    return documents;
}

/* REST API Handling */
void display_json(const json::value& jvalue, const std::string& prefix) {
    std::cout << prefix << jvalue.serialize() << std::endl;
}

void handle_request(http_request request, std::function<void(const json::value&, json::value&)> action) {
    request
        .extract_json()
        .then([=](pplx::task<json::value> task) {
            json::value answer = json::value::object();

            try {
                auto const& jvalue = task.get();
                display_json(jvalue, "📦 Received JSON Payload: ");

                if (!jvalue.is_null()) {
                    action(jvalue, answer);
                }

                std::cout << "✅ Response sent successfully!" << std::endl << std::endl;
                request.reply(status_codes::OK, answer);
            } catch (const http_exception& e) {
                std::cerr << "❌ HTTP Exception: " << e.what() << std::endl;
                request.reply(status_codes::BadRequest, U("Invalid JSON"));
            }
        });
}

void handle_post(http_request request, TST::TST<ValueType>& tst) {
    handle_request(request, [&](const json::value& jvalue, json::value& answer) {
        json::value type = jvalue.at(U("type"));
		json::value coordinates = jvalue.at(U("coordinates"));
		json::value time = jvalue.at(U("time"));

        int diagram_type = type.as_integer(); // Diagram Type
        auto c = coordinates.as_array(); // Spatial Range
        double ca[c.size()];
		for (unsigned i = 0; i < c.size(); i++) {
			ca[i] = c[i].as_double();
		}

        auto tm = time.as_array(); // Temporal Range
		int tma[tm.size()];
		for (unsigned i = 0; i < tm.size(); i++) {
			tma[i] = tm[i].as_integer();
		}

        // Encoding
        unsigned long long timeWindow_start = tst.time_encoder(tma[0], tma[1], tma[2], tma[3]);
        unsigned long long timeWindow_end = tst.time_encoder(tma[4], tma[5], tma[6], tma[7]);
        std::vector<double> spatialWindow_leftBottom = {ca[0], ca[1]};
        std::vector<double> spatialWindow_rightUpper = {ca[2], ca[3]};

        // Perform Query
        std::vector<ValueType> results;
        auto s2Cells = tst.REC_S2_FINDER(spatialWindow_leftBottom, spatialWindow_rightUpper);
        tst.range_search(s2Cells, timeWindow_start, timeWindow_end, results);

        json::value json_array = json::value::array(results.size());
        for (size_t i = 0; i < results.size(); i++) {
			std::string _id = results[i];
			try {
				// Convert _id to JSON string and store in the array
				json_array[i] = json::value::string(U(_id));
			} catch (const std::exception& e) {
				std::cerr << "Error adding _id to JSON array at index " << i << ": " << e.what() << std::endl;
			}
		}

        // Attach the JSON array to the answer
		answer[U("_id")] = json_array;
        results.clear();

    });
}

#endif