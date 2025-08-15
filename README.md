Tools to create a weather station:
- C/C++
- Arduino IDE
- Arduino R4 WiFi 
- BME280 sensor (measures temperature, humidity and pressure)
- Breadboard
- Jumper cables (male to male)
- Ubuntu server
- PuTTy for sanity (you cant copy and paste without this when playing around in the server)

To be installed in the ubuntu server
- Database(fx. MySQL)
- MQTT broker to handle data traffic
- MQTT client to receive weather data from the broker and write it to the database
- Grafana (to visualize data in graphs)

How I created my weather station:
- Start by setting up the Arduino R4 WiFi and have the BME280 connected to the Arduino.
- In Arduino IDE, create a script that measures the current temperature, humidity and pressure.
- Install ubuntu server (install also open ssh when prompted)
- After that I installed mysql(your database) and created a database called "weather" and created a table under that called "data".
- In the "data" table I created the model. "id", "temperature", "pressure", "humidity", "timestamp". Check below to see the specfics
- Installed MQTT broker and MQTT client
- Created a script in the server to receive incoming data from the Arduino
- Installed Grafana
- Visualized the data with Grafana

MYSQL "data" Table contains: 
    id INT AUTO_INCREMENT PRIMARY KEY,
    temperature FLOAT,
    humidity FLOAT,
    pressure FLOAT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP

Data with Grafana
<img width="915" height="857" alt="image" src="https://github.com/user-attachments/assets/fddf37fd-0b52-4963-80ae-3087d760fbfd" />

Ubuntu server - MQTT listening for incoming data
<img width="436" height="279" alt="image" src="https://github.com/user-attachments/assets/bd5cbfa6-b974-47e0-b9d1-85f30f909250" />

After all of that, I created a ASP.NET Core web API and a blazor server app to display the data in a different way.

Tools I used to make this work:
- C#
- ASP.NET Core Web API
- Blazor Server App

How I did it:
- I started by creating the web API to handle the data coming from the database and named it "WeatherAPI"
- Then I created the blazor server app and named it "WeatherDashboard"
- Created the service first
- Then modified index.razor to display the data

Data with Blazor Server App
<img width="3773" height="1109" alt="image" src="https://github.com/user-attachments/assets/3bd583f4-5afe-4dc3-840e-05282213af1e" />

