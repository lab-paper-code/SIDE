#include "main.hpp"

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cout << "Execute Index File : ./main <Execution Error: Miss Configuration File Path>" << std::endl;
        return 1;
    }

    /* Read Configuration File */
    // Identify Configuration Information
    ConfigParser configParser(argv[1]);
    std::map<std::string, std::string> config;

    try
    {
        // Store the configuration values in the separate map
        config["database.id"] = configParser.getValue("DATABASE", "ID");
        config["database.password"] = configParser.getValue("DATABASE", "PASSWD");
        config["database.ip"] = configParser.getValue("DATABASE", "IP");
        config["database.port"] = configParser.getValue("DATABASE", "PORT");
        config["database.db"] = configParser.getValue("DATABASE", "DB");
        config["database.collection"] = configParser.getValue("DATABASE", "COLLECTION");

        config["index.ip"] = configParser.getValue("INDEX_SERVER", "IP");
        config["index.port"] = configParser.getValue("INDEX_SERVER", "PORT");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Reading Configuration File Error: " << e.what() << std::endl;
        return 1;
    }

    /* Define TST */
    TST::TST<ValueType> tst(S2_LV, TIME_RES);
    build_index(tst, config); // Build Index


    /* REST Server */
	std::string index_url = "http://" + config["index.ip"] + ":" + config["index.port"];
	http_listener listener(U(index_url));

	listener.support(methods::POST, [&tst](http_request req) {
        handle_post(req, tst);
    });

	try {
		listener
			.open()
			.then([&listener]() { std::cout << "🚀 Index Server On" << std::endl << std::endl;})
			.wait();
		while (true);
	} catch (const std::exception &e) {
        std::cerr << "Server failed: " << e.what() << std::endl;
    }

    return 0;
}