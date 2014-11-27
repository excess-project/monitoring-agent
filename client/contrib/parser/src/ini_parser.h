/**
 * @brief Parses a given file following the ini configuration format.
 *
 * The Parser wraps the access to the configuration file. The format of the
 * configuration follows the INI file format.
 *
 * @see http://en.wikipedia.org/wiki/INI_file
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */

#ifndef INI_PARSER_H_
#define INI_PARSER_H_

typedef struct {
    char* keys[256];
    char* values[256];
    int size;
} mfp_data;

/**
 * @brief Parses a given file.
 *
 * It should be noted that each time this function is called, the internal data
 * structure is cleared in advanced. This might currently result in some
 * inconsistencies while accessing the data in a concurrent setting.
 *
 * @return 1 if successful; 0 otherwise.
 */
int mfp_parse(const char* filename);

/**
 * @brief Returns a stored value for the given section and key.
 *
 * @return the value stored for the index <section, key>
 */
char* mfp_get_value(const char* section, const char* key);

/**
 * @brief Sets or overwrites the value for a given section and key.
 *
 */
void mfp_set_value(const char* section, const char* key, const char* value);

/**
 * @brief Returns the entire data stored for a given section.
 *
 */
void mfp_get_data(const char* section, mfp_data* data);

#endif
