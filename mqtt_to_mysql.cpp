#include <iostream>
#include <thread>
#include <chrono>

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>

#include <mqtt/async_client.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class callback : public virtual mqtt::callback {
    sql::Connection *conn;
public:
    callback(sql::Connection *c) : conn(c) {}

    void message_arrived(mqtt::const_message_ptr msg) override {
        try {
            auto payload = msg->get_payload_str();
            auto data = json::parse(payload);

            float temperature = data["temperature"];
            float humidity = data["humidity"];
            float pressure = data["pressure"];

            std::cout << "Received data: T=" << temperature << ", H=" << humidity << ", P=" << pressure << std::endl;

            auto pstmt = conn->prepareStatement("INSERT INTO data (temperature, humidity, pressure) VALUES (?, ?, ?)");
            pstmt->setDouble(1, temperature);
            pstmt->setDouble(2, humidity);
            pstmt->setDouble(3, pressure);
            pstmt->execute();
            delete pstmt;
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void connection_lost(const std::string &cause) override {
        std::cerr << "Connection lost: " << cause << std::endl;
    }
};

int main() {
    try {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> con(driver->connect("tcp://linux_server_ip_address:3306", "linux_username", "linux_password"));
        con->setSchema("your_database_name");

        const std::string SERVER_ADDRESS{"tcp://linux_server_ip_address:1883"};
        const std::string CLIENT_ID{"mqtt_mysql_client"};
        const std::string TOPIC{"weather/data"};

        mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
        callback cb(con.get());
        client.set_callback(cb);

        mqtt::connect_options connOpts;
        client.connect(connOpts)->wait();
        client.subscribe(TOPIC, 1)->wait();

        std::cout << "Listening for MQTT messages..." << std::endl;

        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        client.disconnect()->wait();

    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
