#pragma once
#include "systemc.h"
#include "hv_memory.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// Definition of hdc_controller module 
SC_MODULE(hdc_controller) {

    sc_in<bool> clk; // Clock signal
    sc_out<bool> ready; // Ready signal to indicate when hypervector data is available
    sc_in<bool> start; // Start signal to begin acquiring data

    // Outputs to interface with hv_memory
    sc_out<binary> hv_binary_out[DIMENSION]; // Binary hypervector output
    sc_out<bipolar> hv_bipolar_out[DIMENSION]; // Bipolar hypervector output

    // Internal variables to store the acquired data and hypervectors
    std::vector<std::vector<float>> training_data;
    binary binary_hv[DIMENSION];
    bipolar bipolar_hv[DIMENSION];

    // Constructor
    SC_CTOR(hdc_controller) {
        SC_THREAD(process);
        sensitive << clk.pos();
    }

    // Process thread for acquiring data and converting it to hypervectors
    void process() {
        while (true) {
            wait(); // Wait for clock

            if (start.read()) {
                acquire_data();
                convert_to_hypervector();
                ready.write(true); // Indicate that the hypervector is ready
            }
        }
    }

    // Function to acquire training and test data
    void acquire_data() {
        std::ifstream file("C:/Users/arifb/Downloads/systemc-3.0.0/systemc-3.0.0/HDC_Exp3/training/training_emg.csv"); // Replace with actual data files or logic to read .pkl
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::vector<float> data_row;
            float value;
            while (ss >> value) {
                data_row.push_back(value);
            }
            training_data.push_back(data_row);
        }
        file.close();
    }

    // Function to convert acquired data to hypervectors
    void convert_to_hypervector() {
        // Assume training_data is filled and has at least DIMENSION elements.
        for (int i = 0; i < DIMENSION; i++) {
            float data_value = training_data[0][i]; // Get data from the first row as an example

            // Map data_value to binary or bipolar hypervector components
            binary_hv[i] = (data_value > 0) ? ONE : ZERO; // Simple thresholding for binary HV
            bipolar_hv[i] = (data_value > 0) ? ONE_BP : MINUSONE; // Thresholding for bipolar HV
        }

        // Output the generated hypervectors
        for (int i = 0; i < DIMENSION; i++) {
            hv_binary_out[i].write(binary_hv[i]);
            hv_bipolar_out[i].write(bipolar_hv[i]);
        }
    }
};

