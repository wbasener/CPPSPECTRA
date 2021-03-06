// Provides the HSIDataReader class which can read binary ENVI hyperspectral
// image data. Use HSIDataOptions to set the data properties as needed, and use
// HSIDataReader to read the desired range of the data. Loaded data is stored
// in the HSIData struct.

#ifndef SRC_HSI_DATA_READER_H_
#define SRC_HSI_DATA_READER_H_
/*
#include <iostream>
#include <string>
#include <vector>
*/
namespace hsi {

// Interleave format: BSQ, BIP, or BIL. The data files are a stream of bytes,
// and the values in the data are stored in one of the interleave orderings.
enum HSIDataInterleaveFormat {
  // BSQ (band sequential) format is organized in order of bands(rows(cols)).
  // This format is optimal for reading slices of images. That is, when reading
  // an entire 2D image for a single spectral band.
  // For example, for a file with 2 bands, 2 rows, and 2 columns, the order
  // would be as follows:
  //   b0,r0,c0
  //   b0,r0,c1
  //   b0,r1,c0
  //   b0,r1,c1
  //   b1,r0,c0
  //   b1,r0,c1
  //   b1,r1,c0
  //   b1,r1,c1
  HSI_INTERLEAVE_BSQ,

  // BIL (band interleave by line) is organized in order of rows(bands(cols)).
  // This format is a comprimise, designed to give reasonable performance when
  // reading with either spatial-first or spectral-first priority.
  // Following the example above:
  //   r0 b0 c0
  //   r0 b0 c1
  //   r0 b1 c0
  //   r0 b1 c1
  //   r1 b0 c0
  //   r1 b0 11
  //   r1 b1 c0
  //   r1 b1 c1
  HSI_INTERLEAVE_BIL,

  // BIP (band interleave by pixel) is organized in order of rows(cols(bands)).
  // This format is optimal for reading entire spectra at any given pixel or
  // local sequence of pixels.
  // Following the example above:
  //   r0 c0 b0
  //   r0 c0 b1
  //   r0 c1 b0
  //   r0 c1 b1
  //   r1 c0 b0
  //   r1 c0 b1
  //   r1 c1 b0
  //   r1 c1 b1
  HSI_INTERLEAVE_BIP
};

// The precision/type of the data.
// TODO: Complex 2x32 (6) and 2x64 (9) are also possible HSI data types.
enum HSIDataType {
  HSI_DATA_TYPE_BYTE = 1,
  HSI_DATA_TYPE_INT16 = 2,
  HSI_DATA_TYPE_INT32 = 3,
  HSI_DATA_TYPE_FLOAT = 4,
  HSI_DATA_TYPE_DOUBLE = 5,
  HSI_DATA_TYPE_UNSIGNED_INT16 = 12,
  HSI_DATA_TYPE_UNSIGNED_INT32 = 13,
  HSI_DATA_TYPE_UNSIGNED_INT64 = 14,
  HSI_DATA_TYPE_UNSIGNED_LONG = 15
};

// Options that specify the location and format of the data. Needed to
// correctly parse the file.
struct HSIDataOptions {
  HSIDataOptions() {}

  explicit HSIDataOptions(const std::string& hsi_file_path)
      : hsi_file_path(hsi_file_path) {}

  // Attempts to read the header information from an HSI header file. Fatal
  // error if the read was unsuccessful and the information was not loaded.
  void ReadHeaderFromFile(const std::string& header_file_path);

  // Path to the binary hyperspectral data file.
  std::string hsi_file_path;

  // The format and type of the data.
  HSIDataInterleaveFormat interleave_format = HSI_INTERLEAVE_BSQ;
  HSIDataType data_type = HSI_DATA_TYPE_FLOAT;
  bool big_endian = false;

  // Offset of the header (if the header is attached to the data).
  int header_offset = 0;

  // The size of the data. This is NOT the size of the chunk of data you want
  // to read, but rather of the entire data, even if you don't read everything.
  // These must all be non-zero.
  int num_data_rows = 0;
  int num_data_cols = 0;
  int num_data_bands = 0;
  std::string specNames = "NA";
  std::string wavelength = "NA";
  std::string bbl = "NA";
  std::string fwhm = "NA";
};

// Data range object is used for specifying the data range to read with the
// HSIDataReader.
struct HSIDataRange {
  // Attempts to read the data range information from config file. Fatal error
  // if the read fails and the information was not loaded.
  void ReadRangeFromFile(const std::string& range_config_file);
  int start_band = 0;
  int end_band = 0;
  int start_row = 0;
  int end_row = 0;
  int start_col = 0;
  int end_col = 0;
};

// This memory union occupies multiple bytes, but allows interpreting the data
// as an arbitrary type.
union HSIDataValue {
  HSIDataValue() : value_as_uint64(0) {}

  // The raw bytes.
  char bytes[8];

  // Interpret the memory as one of the following types:
  char value_as_byte;
  int16_t value_as_int16;
  int32_t value_as_int32;
  float value_as_float;
  double value_as_double;
  uint16_t value_as_uint16;
  uint32_t value_as_uint32;
  uint64_t value_as_uint64;
  unsigned long value_as_unsigned_long;
};

// This struct stores and provides access to hyperspectral data. All data is
// stored in a single vector, but can be indexed to access individual values.
struct HSIData {
  // The size of the data. This only counts the size of the data read in the
  // specified ranges (i.e. not necessarily the size of the entire data file).
  int num_rows = 0;
  int num_cols = 0;
  int num_bands = 0;

  HSIDataInterleaveFormat interleave_format = HSI_INTERLEAVE_BSQ;
  HSIDataType data_type = HSI_DATA_TYPE_FLOAT;

  int NumDataPoints() const {
    return num_rows * num_cols * num_bands;
  }

  // Return the index value at the given index into the hyperspectral cube.
  // This treats the image as a cube where rows and cols define the image Y
  // (height) and X (width) axes, respectively, and the third is the spectral
  // dimension.
  //
  // All dimensions are zero-indexed. Indices are relative to the data in
  // memory, and not absolute positions in the entire data file. For example,
  // if data was read with start_row = 10, then row index 0 in this HSIData
  // would correspond to row 10 in the original data file.
  HSIDataValue GetValue(const int row, const int col, const int band) const;
  HSIDataValue GetValueLib(const int row, const int col, const int band) const;

  // Returns the value as a double. If the data is not already stored as a
  // double, it will be cast to a double first.
  //
  // Note that very large unsigned 64-bit integers may not be cast correctly.
  double GetValueAsDouble(const int row, const int col, const int band) const;

  // Returns a vector containing the spectrum of the pixel at the given row
  // and col of the image.
  std::vector<HSIDataValue> GetSpectrum(const int row, const int col) const;

  // Returns the spectrum as above, but all values are cast to doubles.
  std::vector<double> GetSpectrumAsDoubles(const int row, const int col) const;

  // The raw data as bytes.
  std::vector<char> raw_data;
};

// The HSIDataReader is responsible for loading the data and storing it in
// memory.
class HSIDataReader {
 public:
  explicit HSIDataReader(const HSIDataOptions& data_options);

  // Read the data in the specified range. The range must be valid, within the
  // specified HSIDataOptions data size. Returns true on success.
  //
  // Ranges are 0-indexed and end is non-inclusive. For example,
  //   start_row = 2,
  //   end_row = 7
  // will return rows (2, 3, 4, 5, 6) where the first row in the data is row 0.
  void ReadData(const HSIDataRange& data_range);

  void SetData(const HSIData& hsi_data) {
    hsi_data_ = hsi_data;
  }

  // Writes the data currently stored in hsi_data_ in the order that it was
  // loaded in. Endian format is preserved from the original data. Returns true
  // on success.
  void WriteData(const std::string& save_file_path) const;

  // Returns the HSIData struct containing any data loaded in from ReadData().
  const HSIData& GetData() const {
    return hsi_data_;
  }

 private:
  // Contains options and information about the data file which is necessary
  // for the ReadData() method to correctly read in the HSI data.
  const HSIDataOptions data_options_;

  // This will be true if the machine is big endian. This is required for
  // reading in the data correctly, which may not match the byte order of the
  // machine it's being read on.
  bool machine_big_endian_;

  // The data struct will get filled in in the ReadData() method.
  HSIData hsi_data_;
};

}  // namespace hsi

#endif  // SRC_HSI_DATA_READER_H_
