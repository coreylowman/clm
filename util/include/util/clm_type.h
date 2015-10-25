#ifndef CLM_TYPE_H_
#define CLM_TYPE_H_

typedef enum {
    CLM_TYPE_INT,
    CLM_TYPE_MATRIX,
    CLM_TYPE_STRING,
    CLM_TYPE_FLOAT,
    CLM_TYPE_FUNCTION,
    CLM_TYPE_NONE
} ClmType;

const char *clm_type_to_string(ClmType type);

#endif