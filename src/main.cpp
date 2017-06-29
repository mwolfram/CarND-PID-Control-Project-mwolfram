#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

/**
 * Code snippet from: https://carnd.slack.com/conversation/p-pid/p1494276925735399
 */
void reset_simulator(uWS::WebSocket<uWS::SERVER>& ws)
{
    // reset
    std::string msg("42[\"reset\", {}]");
    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
}

int main()
{
  uWS::Hub h;

  // twiddle
  int i = 0;
  bool decreased_p = false;
  double p[] = {0.23, 0.0, 15.0};
  double dp[] = {0.1, 0.01, 0.5};
  double best_err = -1.0;
  double curr_err = 0.0;
  int time = 0;

  PID pid;
  pid.Init(p[0], p[1], p[2]);

  h.onMessage([&pid, &i, &decreased_p, &p, &dp, &best_err, &curr_err, &time](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {

    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());

          // update pid controller with current CTE
          pid.UpdateError(cte);

          // steering value is the opposite of the total error
          double raw_steer_value = -pid.TotalError();
          double steer_value = raw_steer_value;

          if (steer_value > 1.0) {
              steer_value = 1.0;
          }
          if (steer_value < -1.0) {
              steer_value = -1.0;
          }
          
          // DEBUG
          //std::cout << time << ": CTE: " << cte << " Steering Value: " << steer_value << " (raw: " << raw_steer_value << ")" << std::endl;
          curr_err += cte * cte;
          ++time;

          // reset condition
          if (false) {
          //if (fabs(cte) > 2.0) {
              curr_err = 1000000 - (double)time;
          }

          if (false) {
          //if (fabs(cte) > 2.0 || time > 2000) {
              std::cout << "reset " << curr_err << std::endl;
              if (best_err < 0.0) {
                  best_err = curr_err;
                  curr_err = 0.0;
              }

              if (decreased_p) {
                  decreased_p = false;
                  if (curr_err < best_err) {
                      best_err = curr_err;
                      dp[i] *= 1.1;
                  }
                  else {
                      p[i] += dp[i];
                      dp[i] *= 0.9;
                  }
              }
              else {
                  if (curr_err < best_err) {
                      best_err = curr_err;
                      dp[i] *= 1.1;
                  }
                  else {
                      p[i] -= 2 * dp[i];
                      decreased_p = true;
                  }
              }

              i = (i + 1) % 3;
              p[i] += dp[i];

              pid.Kp = p[0];
              pid.Ki = p[1];
              pid.Kd = p[2];
              std::cout << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
              time = 0;
              cte = 0;
              curr_err = 0;
              pid.Reset();
              reset_simulator(ws);
          }

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
