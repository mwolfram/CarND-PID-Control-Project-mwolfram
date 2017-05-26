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

double PID::TotalError() {
    cout << "P_err " << p_error << ", D_err " << d_error << ", I_err " << i_error << endl;
    return Kp * p_error - Kd * d_error + Ki * i_error;
}

