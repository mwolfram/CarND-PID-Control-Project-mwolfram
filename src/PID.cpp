#include <iostream>

#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;
}

void PID::UpdateError(double cte) {
    d_error = p_error - cte;
    p_error = cte;
    i_error += cte;
}

void PID::Reset() {
    d_error = 0.0;
    p_error = 0.0;
    i_error = 0.0;
}

double PID::TotalError() {
    return Kp * p_error - Kd * d_error + Ki * i_error;
}

