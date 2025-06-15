#include "kalmanfilter.h"
#include <math.h>

// todo: fine tune process_noise_var and meas_noise_var.These are default (typically good):
float process_noise_covar = 0.0001;  // Process noise covariance
float meas_noise_covar = 0.05;    // Measurement noise covariance
float new_estimate = 0;
float old_estimate = 0;      
float error_covar = process_noise_covar;       // from MTE 546- typically good to set error covariance to process noise covariance
float kalman_gain = 0;       


// This function performs the Kalman filter algorithm
float kalman_filter(float lidar_reading) {
  // Prediction step
  error_covar = error_covar + process_noise_covar;

  // Measurement update step
  kalman_gain = error_covar / (error_covar + meas_noise_covar);
  new_estimate = old_estimate + kalman_gain * (lidar_reading - old_estimate);
  error_covar = (1 - kalman_gain) * error_covar + fabs(new_estimate - lidar_reading)*process_noise_covar;
  old_estimate = new_estimate;

  return new_estimate;  // Return the filtered estimate
}