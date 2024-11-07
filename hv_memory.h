#pragma once
#include "systemc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DIMENSION 20//each hypervector(each row) has 20 components
#define NUM_CLASS 5 // total number of class stored in the AM
#define is_binary false // flag to indicate the hv type = binary or bipolar
#define NUM_LEVELS 61
#define MIN_LEVEL -2
#define MAX_LEVEL 4


enum binary { ZERO = 0, ONE = 1 };
enum bipolar { MINUSONE = -1, ONE_BP = 1 };

// Define two types: one for binary and one for bipolar
typedef binary hv_bn[DIMENSION];
typedef bipolar hv_bp[DIMENSION];

SC_MODULE(HV_Memory) {

    sc_in<bool> train;
    sc_in<bool> test;

    const int entries; // represent the number of hv stored in the memory

    // Allocate memory space for both binary and bipolar types
    hv_bn* memory_binary; // pointer to an array of binary hvs
    hv_bp* memory_bipolar; // pointer to an array of bipolar hvs

    SC_CTOR(HV_Memory);
    HV_Memory(sc_module_name name, int entries) : sc_module(name), entries(entries)
    {
        memory_binary = nullptr;
        memory_bipolar = nullptr;

        if (is_binary) {
            memory_binary = (hv_bn*)malloc(entries * sizeof(hv_bn)); // Allocate memory for binary hvs
        }
        else {
            memory_bipolar = (hv_bp*)malloc(entries * sizeof(hv_bp)); // Allocate memory for bipolar hvs
        }
        
        SC_METHOD(process_signals);
        sensitive << train << test;
    }

    //destructor
    ~HV_Memory() {
        if (memory_binary) {
            free(memory_binary);  // Only free once
            
        }
        if (memory_bipolar) {
            free(memory_bipolar);
            
        }
    }


    void init_hv_memory();                    // Initialize HV memory for discrete items (IdM)
    void init_continuous_hv_memory();         // Initialize continuous HV memory (CiM)
    void init_associative_memory(hv_bn* item_memory_binary, hv_bp* item_memory_bipolar, hv_bn* continuous_memory_binary, hv_bp* continuous_memory_bipolar);
    void free_hv_memory();                    // Free allocated memory

    hv_bn* get_hv_vector_binary(int item_id); // Returns a pointer to the binary hypervector at the specified index.
    hv_bp* get_hv_vector_bipolar(int item_id);// Returns a pointer to the bipolar hypervector at the specified index.

    void bind_and_bundle(hv_bp& im_vector, hv_bp& cim_vector,  int am_id);
    void bind_and_bundle_test(hv_bp& im_vector, hv_bp& cim_vector, int am_id);

    void write_binary_IM(int item_id, hv_bn & hv);   // Write a binary hypervector to IdM
    void write_binary_CiM(int item_id, hv_bn & hv);   // Write a binary hypervector to CiM
    void write_bipolar_IM(int item_id, hv_bp & hv);  // Write a bipolar hypervector to IdM
    void write_bipolar_CiM(int item_id, hv_bp & hv);  // Write a bipolar hypervector to CiM
    void write_binary_AM(int item_id, hv_bn& result_binary); // Write a binary hypervector to AM
    void write_bipolar_AM(int item_id, hv_bp& result_bipolar); // Write a bipolar hypervector to AM

    void read_binary_IM(int item_id, hv_bn & hv);    // Read a binary hypervector from IdM
    void read_binary_CiM(int item_id, hv_bn & hv);    // Read a binary hypervector from CiM
    void read_bipolar_IM(int item_id, hv_bp & hv);   // Read a bipolar hypervector from IdM
    void read_bipolar_CiM(int item_id, hv_bp & hv);   // Read a bipolar hypervector from CiM
    void read_bipolar_AM(int item_id, hv_bp& am_vector); // Read a bipolar hypervector from AM

    void print_hv_memory(); // Print contents of the memory for debugging

    void generate_orthogonal_vectors(hv_bn & vector1, hv_bn & vector2);       // Generate orthogonal binary vectors
    void generate_orthogonal_vectors_bipolar(hv_bp & vector1, hv_bp & vector2);// Generate orthogonal bipolar vectors

    void interpolate_vectors(hv_bn & vec1, hv_bn & vec2, hv_bn & result, double ratio); // Interpolate binary vectors
    void interpolate_vectors_bipolar(hv_bp & vec1, hv_bp & vec2, hv_bp & result, double ratio); // Interpolate bipolar vectors


    int hamming_distance(hv_bp& hv1, hv_bp& hv2); //calculates the hamming distance between two bipolar hypervector
    void map_to_hv(float value, hv_bp& hypervector, bool is_feature);
    void map_emg_to_hv(const std::string& emg_file, const std::string& label_file);

    


    // In current implementation, system assumes that it works with bipolar hvs only
    void process_signals() {
        if (train.read()) {
            std::cout << "Training mode active" << std::endl;
          
            const std::string emg_file = "C:/Users/arifb/Downloads/systemc-3.0.0/systemc-3.0.0/HDC_Exp3/training/training_emg.csv";
            const std::string label_file = "C:/Users/arifb/Downloads/systemc-3.0.0/systemc-3.0.0/HDC_Exp3/training/training_labels.csv";
            // Step 1: Map EMG and label data to hypervectors
            map_emg_to_hv(emg_file, label_file);
            // Step 2: Bind and Bundle each mapped EMG and label hypervector, and update AM
            for (int i = 0; i < entries; i++) {
                hv_bp im_vector, cim_vector;
                read_bipolar_IM(i, im_vector);  // Read hypervector from IM
                read_bipolar_CiM(i, cim_vector); // Read hypervector from CiM

                // Bind and bundle the IM and CiM vectors to create an associative memory representation
                bind_and_bundle(im_vector, cim_vector, i);
                // Optionally, display the updated AM for verification
                print_hv_memory();
            }
            // Optionally, display the updated AM for verification
            //print_hv_memory();

        }

        if (test.read()) {
            std::cout << "Testing mode active" << std::endl;

            for (int i = 0; i < entries; i++) {

                hv_bp new_hv_test_IM, new_hv_test_CiM; // in the future this should be the real test data
                read_bipolar_IM(i, new_hv_test_IM);
                read_bipolar_CiM(i, new_hv_test_CiM);
                bind_and_bundle_test(new_hv_test_IM, new_hv_test_CiM, 1);  // Use new_hv_IM and new_hv_CiM in binding
                write_bipolar_IM(i, new_hv_test_IM);    // Store new_hv_IM in IM
                write_bipolar_CiM(i, new_hv_test_CiM);  // Store new_hv_CiM in CiM
                // write AM function has already been used in the bind_and_bundle function. 

                
            }
          
            print_hv_memory();
        }
    }
};
