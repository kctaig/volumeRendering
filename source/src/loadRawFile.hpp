#pragma once

#include <fstream>
#include <stdexcept>
#include "nlohmann/json.hpp"
#include <variant>

using json = nlohmann::json;

// Function to load metadata from JSON
json load_metadata(const string& meta_file_path) {
    std::ifstream meta_file(meta_file_path);
    if (!meta_file.is_open()) {
        throw std::runtime_error("Failed to open metadata file.");
    }

    json metadata;
    meta_file >> metadata;
    return metadata;
}

// Function to get the scalar type based on metadata
short3DVector load_raw_data(const string& raw_file_path, const vector<size_t> &d) {
    std::ifstream raw_file(raw_file_path, std::ios::binary);
    if (!raw_file.is_open()) {
        throw std::runtime_error("Failed to open raw data file.");
    }
    short3DVector data(d[0],vector<vector<int16_t>>(d[1],vector<int16_t>(d[2])));
    //raw_file.read(reinterpret_cast<char*>(data.data()), total_size * sizeof(int16_t));
    for (size_t i = 0; i < d[0]; i++) {
        for (size_t j = 0; j < d[1]; j++) {
            for (size_t k = 0; k < d[2]; k++) {
                raw_file.read(reinterpret_cast<char*>(&data[i][j][k]), sizeof(int16_t));
            }
        }
    }
    return data;
}

// Function to compare and print specific layers in the array
//void compare_layers(const vector<int16_t>& data, const vector<size_t>& dimensions, int layer1, int layer2) {
//    size_t layer_size = dimensions[0] * dimensions[1];
//    cout << "Layer " << layer1 << " data: ";
//    for (size_t i = 0; i < layer_size; ++i) {
//        cout << data[layer1 * layer_size + i] << " ";
//    }
//    cout << "\nLayer " << layer2 << " data: ";
//    for (size_t i = 0; i < layer_size; ++i) {
//        cout << data[layer2 * layer_size + i] << " ";
//    }
//    cout << endl;
//}

// function to load data with metadata
std::tuple<short3DVector, vector<size_t>, vector<float>>
load_raw_data_with_meta(const string& raw_file_path, const string& meta_file_path) {
    // Load JSON metadata
    json metadata = load_metadata(meta_file_path);
    vector<size_t> dimensions = metadata["dimensions"];  // Expected order: (X, Y, Z)
    vector<float> spacing = metadata["spacing"];         // Expected order: (X, Y, Z)
    string scalar_type = metadata["scalar_type"];

    short3DVector raw_data = load_raw_data(raw_file_path, dimensions);

    //compare_layers(raw_data, dimensions, 0, 299);

    return {raw_data, dimensions, spacing};
}