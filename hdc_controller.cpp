#include "systemc.h"
#include "hv_memory.h"
#include <iostream>
#include <iomanip>


/*this method is responsible for writing a new binary hypervector (hv)
into the binary item memory (memory_binary) at a specific index (item_id) */
void HV_Memory::write_binary_IM(int item_id, hv_bn& hv) {
    if (item_id >= 0 && item_id < entries && is_binary) {
        memcpy(memory_binary[item_id], hv, DIMENSION * sizeof(binary));
        /*
        memcpy(): This is a function that copies data from one memory location to another.
        memory_binary[item_id]: This is the destination where the data will be copied, on this occassion, the binary hypervector stored at index item_id in the memory_binary array.
        hv: This is the source hypervector that will be copied into memory_binary[item_id].
        DIMENSION * sizeof(binary): This specifies the number of bytes to copy. The hypervector has a size of DIMENSION elements, and each element is of type binary (likely a 0 or 1). This ensures that the correct amount of data (in bytes) is copied.
        */
    }
}

// Write a binary hypervector to CiM, same functionality but only used for Continious item memory
void HV_Memory::write_binary_CiM(int item_id, hv_bn& hv) {
    if (item_id >= 0 && item_id < entries && is_binary) {
        memcpy(memory_binary[item_id], hv, DIMENSION * sizeof(binary));
    }
}

/*this method is responsible for writing a new bipolar hypervector (hv)
into the bipolar item memory(memory_bipolar) at a specific index(item_id) */
void HV_Memory::write_bipolar_IM(int item_id, hv_bp& hv) {
    if (item_id >= 0 && item_id < entries && !is_binary) {
        memcpy(memory_bipolar[item_id], hv, DIMENSION * sizeof(bipolar));
    }
}

// Write a bipolar hypervector to CiM, , same functionality but only used for Continious item memory
void HV_Memory::write_bipolar_CiM(int item_id, hv_bp& hv) {
    if (item_id >= 0 && item_id < entries && !is_binary) {
        memcpy(memory_bipolar[item_id], hv, DIMENSION * sizeof(bipolar));
    }
}

// Read a binary hypervector from IM
void HV_Memory::read_binary_IM(int item_id, hv_bn& hv) {
    if (item_id >= 0 && item_id < entries && is_binary) {
        memcpy(hv, memory_binary[item_id], DIMENSION * sizeof(binary));
    }
}

// Read a binary hypervector from CiM
void HV_Memory::read_binary_CiM(int item_id, hv_bn& hv) {
    if (item_id >= 0 && item_id < entries && is_binary) {
        memcpy(hv, memory_binary[item_id], DIMENSION * sizeof(binary));
    }
}

// Read a bipolar hypervector from IM
void HV_Memory::read_bipolar_IM(int item_id, hv_bp& hv) {
    if (item_id >= 0 && item_id < entries && !is_binary) {
        memcpy(hv, memory_bipolar[item_id], DIMENSION * sizeof(bipolar));
    }
}

// Read a bipolar hypervector from CiM
void HV_Memory::read_bipolar_CiM(int item_id, hv_bp& hv) {
    if (item_id >= 0 && item_id < entries && !is_binary) {
        memcpy(hv, memory_bipolar[item_id], DIMENSION * sizeof(bipolar));
    }
}

// Function to store binary hypervector into Associative Memory (AM)
void HV_Memory::write_binary_AM(int item_id, hv_bn& result_binary) {
    // Use the same approach as writing to IM for binary hypervectors
    for (int i = 0; i < DIMENSION; i++) {
        memory_binary[item_id][i] = result_binary[i];
    }
}

// Function to store bipolar hypervector into Associative Memory (AM)
void HV_Memory::write_bipolar_AM(int item_id, hv_bp& result_bipolar) {
    // Use the same approach as writing to IM for bipolar hypervectors
    for (int i = 0; i < DIMENSION; i++) {
        memory_bipolar[item_id][i] = result_bipolar[i];
    }
}

// Function to read a hypervector from AM
void HV_Memory::read_bipolar_AM(int item_id, hv_bp& am_vector) {
    for (int i = 0; i < DIMENSION; i++) {
        am_vector[i] = memory_bipolar[item_id][i];  // Read the hypervector stored in AM at position item_id
    }
}

// Debugging function for printing the values in the memories(IM,CiM and AM)
void HV_Memory::print_hv_memory() {
    if (is_binary) {
        std::cout << name() << " memory contains " << entries << " vectors of dimension " << DIMENSION << " (binary)" << std::endl;
        for (int i = 0; i < entries; i++) {
            for (int j = 0; j < DIMENSION; j++) {
                std::cout << memory_binary[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    else {
        std::cout << name() << " memory contains " << entries << " vectors of dimension " << DIMENSION << " (bipolar)" << std::endl;
        for (int i = 0; i < entries; i++) {
            for (int j = 0; j < DIMENSION; j++) {
                std::cout << memory_bipolar[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
}