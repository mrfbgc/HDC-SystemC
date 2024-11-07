#include "systemc.h"
#include "hv_memory.h"
#include <iostream>
#include <iomanip>
#include "hdc_controller.h" 

// Initialize HV memory for discrete items (IM)
void HV_Memory::init_hv_memory() {

    //memory is represented as a 2D array, "entries" corresponds to row(number of hv) and "Dimension" corresponds to columns(size of each hv)
    if (is_binary) {
        for (int i = 0; i < entries; i++) {
            for (int j = 0; j < DIMENSION; j++) {
                memory_binary[i][j] = static_cast<binary>(rand() % 2); // rand() function generates a random integer value 0 or 1 
                                                                       // static_cast ensures that the result is stored as a binary type (from the header file, enum representing the values 0 and 1).
            }
        }
    }
    else {
        for (int i = 0; i < entries; i++) {
            for (int j = 0; j < DIMENSION; j++) {
                memory_bipolar[i][j] = static_cast<bipolar>((rand() % 2) * 2 - 1); // the logic after the rand function generates a random value -1 or 1
            }
        }
    }
}

// Generate orthogonal vectors for binary HVs
void HV_Memory::generate_orthogonal_vectors(hv_bn& vector1, hv_bn& vector2) {
    // The loop runs through each element of the hypervectors
    for (int i = 0; i < DIMENSION; i++) {
        vector1[i] = static_cast<binary>(rand() % 2);
        vector2[i] = static_cast<binary>(!vector1[i]); // !vector1[i] ensures that the random value that is generated is not same with the vector1, so it is orthogonal
    }
    // Orthogonality in binary vectors means that at each position i, the values in vector1 and vector2 must be different. This ensures that for every 1 in vector1, there is a 0 in vector2, and vice versa.
}

// Generate orthogonal vectors for bipolar HVs
void HV_Memory::generate_orthogonal_vectors_bipolar(hv_bp& vector1, hv_bp& vector2) {
    // The loop runs through each element of the hypervectors
    for (int i = 0; i < DIMENSION; i++) {
        vector1[i] = static_cast<bipolar>((rand() % 2) * 2 - 1);
        vector2[i] = static_cast<bipolar>(-vector1[i]); // with the -vector1[i] it helps to directly generate orthogonal values. 1 changed to -1 and -1 changed to 1
    }
}

// Interpolate between two binary ,
void HV_Memory::interpolate_vectors(hv_bn& vec1, hv_bn& vec2, hv_bn& result, double ratio) {
    /*The function generates a new binary vector, result, which is a mix of vec1 and vec2. 
    The proportion of elements taken from vec2 is controlled by the ratio parameter, where the number of elements replaced is proportional to the ratio value.*/
    int flip_count = static_cast<int>(DIMENSION * ratio); // ratio: The proportion of elements in vec2 that should replace elements in vec1.
                                                          // flip_count: This calculates how many elements of vec1 will be replaced by elements from vec2

    // memcpy(): This is a standard C function that copies memory from one location to another
    memcpy(result, vec1, DIMENSION * sizeof(binary)); // The memory of vec1 is copied into result, meaning that initially, result is an exact copy of vec1.at  this stage result vector is same as vec1.

    // This loop runs flip_count times, meaning it will replace that number of elements in result with elements from vec2.
    for (int i = 0; i < flip_count; i++) {
        int index = rand() % DIMENSION;
        result[index] = vec2[index];
    }
}

// Interpolate between two bipolar vectors
void HV_Memory::interpolate_vectors_bipolar(hv_bp& vec1, hv_bp& vec2, hv_bp& result, double ratio) {
    int flip_count = static_cast<int>(DIMENSION * ratio);
    memcpy(result, vec1, DIMENSION * sizeof(bipolar));
    for (int i = 0; i < flip_count; i++) {
        int index = rand() % DIMENSION;
        result[index] = vec2[index];
    }
}

// Initialize continuous HV memory for signal intensities (CiM)
void HV_Memory::init_continuous_hv_memory() {

    /*The process involves generating two orthogonal vectors (representing minimum and maximum points), 
    and then interpolating between them to fill the memory with vectors that transition gradually from one extreme to the other.*/
    if (is_binary) {
        hv_bn min_vector, max_vector; // min_vector and max_vector are binary vectors that represent the minimum and maximum extremes.
        generate_orthogonal_vectors(min_vector, max_vector);

        // The for loop runs through each entry in the memory_binary array. entries refers to the number of hypervectors to be generated.
        for (int i = 0; i < entries; i++) {

            /*When i = 0, ratio = 0.0, meaning the first vector will be close to min_vector.
            When i = entries - 1, ratio = 1.0, meaning the last vector will be close to max_vector.
            For intermediate values of i, ratio smoothly transitions between 0 and 1.*/
            double ratio = static_cast<double>(i) / (entries - 1);

            // interpolate_vectors(): uses the ratio to combine min_vector and max_vector and store the result in memory_binary[i].
            interpolate_vectors(min_vector, max_vector, memory_binary[i], ratio);
        }
    }
    else {
        // same process has been done for the bipolar hvs
        hv_bp min_vector, max_vector;
        generate_orthogonal_vectors_bipolar(min_vector, max_vector);

        for (int i = 0; i < entries; i++) {
            double ratio = static_cast<double>(i) / (entries - 1);
            interpolate_vectors_bipolar(min_vector, max_vector, memory_bipolar[i], ratio);
        }
    }
}

/* The code assumes that memory_binary and memory_bipolar are pointers to dynamically allocated memory with using malloc
 and it frees this memory when it is no longer needed.*/
void HV_Memory::free_hv_memory() {

    /* for example; memory_binary: This is a pointer to the dynamically allocated memory that holds the binary hypervectors.
    if (memory_binary): This condition checks whether memory_binary has been allocated (i.e., it's not nullptr). 
    If memory_binary is non-null, it means that memory has been allocated, and it needs to be freed */

    if (memory_binary) free(memory_binary);
    if (memory_bipolar) free(memory_bipolar);
}

// Get the vector for a specific item in binary memory
hv_bn* HV_Memory::get_hv_vector_binary(int item_id) {
    //checking item_id is valid 
    if (item_id >= 0 && item_id < entries) {
        return &memory_binary[item_id]; //If item_id is valid, the method returns a pointer to the binary hypervector at the item_id index in memory_binary.
    }
    return nullptr; // else invalid pointer
}

// Get the vector for a specific item in bipolar memory
hv_bp* HV_Memory::get_hv_vector_bipolar(int item_id) {
    // //checking item_id is valid 
    if (item_id >= 0 && item_id < entries) {
        return &memory_bipolar[item_id]; //If item_id is valid, the method returns a pointer to the binary hypervector at the item_id index in memory_binary.
    }
    return nullptr; // else invalid pointer
}


void HV_Memory::bind_and_bundle(hv_bp& im_vector, hv_bp& cim_vector,  int am_id) {
    hv_bp result;
    int bundled_result[DIMENSION] = { 0 };  // Temporary variable to store the bundled sum (use int for summation)

    // Bind and sum (bundle) across features
    for (int feature = 0; feature < NUM_CLASS; feature++) {
        hv_bp bound_vector;

        // Binding: multiply IM and CiM vectors element-wise (for bipolar)
        for (int i = 0; i < DIMENSION; i++) {
            bound_vector[i] = static_cast<bipolar>(im_vector[i] * cim_vector[i]);  // Cast to bipolar

        }

        // Bundling: sum the bound vectors
        for (int i = 0; i < DIMENSION; i++) {
            bundled_result[i] += bound_vector[i];  // Summing into an int array
        }
    }


    // Apply thresholding to finalize the bundled result
    for (int i = 0; i < DIMENSION; i++) {
        result[i] = (bundled_result[i] > 0) ? ONE_BP : MINUSONE;  // Apply threshold to convert to bipolar
    }

    // Write the final result to AM
    write_bipolar_AM(am_id, result);  // Storing the final bundled vector into the AM
}

void HV_Memory::bind_and_bundle_test(hv_bp& im_vector, hv_bp& cim_vector, int am_id) {
    hv_bp result_test;
    int bundled_result_test[DIMENSION] = { 0 };  // Temporary variable to store the bundled sum (use int for summation)
    hv_bp bound_vector_test;
   
        // Binding: multiply IM and CiM vectors element-wise (for bipolar)
        for (int i = 0; i < DIMENSION; i++) {
            bound_vector_test[i] = static_cast<bipolar>(im_vector[i] * cim_vector[i]);  // Cast to bipolar

        }

        // Bundling: sum the bound vectors
        for (int i = 0; i < DIMENSION; i++) {
            bundled_result_test[i] += bound_vector_test[i];  // Summing into an int array
        }
        // Apply thresholding to finalize the bundled result
        for (int i = 0; i < DIMENSION; i++) {
            result_test[i] = (bundled_result_test[i] > 0) ? ONE_BP : MINUSONE;  // Apply threshold to convert to bipolar
        }
        write_bipolar_AM(am_id, result_test);

}

// Function to compute the Hamming distance between two bipolar hypervectors
int HV_Memory::hamming_distance(hv_bp& hv1, hv_bp& hv2) {
    int distance = 0;
    for (int i = 0; i < DIMENSION; i++) {
        if (hv1[i] != hv2[i]) {  // Compare each element
            distance++;
        }
    }
    return distance;
}

// Function to map a value to a hypervector based on initialized IM or CiM
void HV_Memory::map_to_hv(float value, hv_bp& hypervector, bool is_feature) {
    int index = static_cast<int>((value - MIN_LEVEL) / (MAX_LEVEL - MIN_LEVEL) * NUM_LEVELS); // Quantize value
    if (is_feature) {
        read_bipolar_IM(index, hypervector);  // Read hypervector from IM for feature ID
    }
    else {
        read_bipolar_CiM(index, hypervector); // Read hypervector from CiM for EMG values
    }
}

void HV_Memory::map_emg_to_hv(const std::string& emg_file, const std::string& label_file) {
    std::ifstream emg_input(emg_file);
    std::ifstream label_input(label_file);

    if (!emg_input.is_open() || !label_input.is_open()) {
        std::cerr << "Error: Could not open training files." << std::endl;
        return;
    }

    // Read EMG data
    std::string line;
    int row_index = 0;
    while (std::getline(emg_input, line)) {
        std::istringstream stream(line);
        std::string value;
        std::vector<float> emg_values;

        // Parse each value separated by a comma
        while (std::getline(stream, value, ',')) {
            emg_values.push_back(std::stof(value)); // Convert to float and store
        }

        //std::cout << "EMG data row " << row_index << " with " << emg_values.size() << " values." << std::endl;

        if (emg_values.size() != 32) {
            std::cerr << "Error: Incorrect EMG data format. Expected 32 values, got " << emg_values.size() << std::endl;
            continue;
        }

        // Map EMG values to hypervectors using CiM
        for (int i = 0; i < emg_values.size(); ++i) {
            hv_bp emg_hv;
            map_to_hv(emg_values[i], emg_hv, false); // Map EMG value to hypervector using CiM
            write_bipolar_CiM(row_index, emg_hv);    // Write hypervector to CiM
        }
        ++row_index;
    }

    row_index = 0;
    bool is_first_line = true;
    while (std::getline(label_input, line)) {
        // Skip the header line if it contains non-numeric text
        if (is_first_line) {
            is_first_line = false;
            if (line.find_first_not_of("0123456789") != std::string::npos) {
                std::cout << "Skipping header line: " << line << std::endl;
                continue;
            }
        }

        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        try {
            int label = std::stoi(line);
            hv_bp label_hv;

            //std::cout << "Mapping label " << label << " at row " << row_index << std::endl;
            map_to_hv(label, label_hv, true);            // Map label to hypervector using IM
            write_bipolar_IM(row_index, label_hv);       // Write hypervector to IM
            ++row_index;
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid label format at row " << row_index << ": " << line << std::endl;
            continue;
        }
    }
}


// SystemC main function with training and testing signal functionality
int sc_main(int argc, char* argv[]) {

    sc_signal<bool> train;
    sc_signal<bool> test;

    HV_Memory IM("IM", 32);  // 32 entries for IM
    HV_Memory CiM("CiM",20);  // 20 entries for CiM
    HV_Memory AM("AM", 5);     // 5 entries for AM


    // initialization step of the memories
    IM.init_hv_memory();
    CiM.init_continuous_hv_memory();
    AM.init_hv_memory();

    // train and test signals are connected to the HV_Memory Module here
    IM.train(train);
    IM.test(test);

    CiM.train(train);
    CiM.test(test);

    AM.train(train);
    AM.test(test);
    //connection ends

 
    //simulation stars
    sc_start(100, SC_SEC);
    train.write(true);
    test.write(false);
    sc_start(10, SC_SEC);
    //train.write(false);
    //test.write(true);
    //sc_start(10, SC_SEC);

    return 0;
}
