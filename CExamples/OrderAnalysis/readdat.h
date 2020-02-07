// Header file to read vibration data from .dat file
// Columns are : Magnitude and Speed

SLArrayIndex_t read_vibration_data (double *bp,
    FILE *fp,
    SLData_t *pSpeed,
    const SLArrayIndex_t buffer_length)

{
    SLArrayIndex_t  i, sample_count;
    SLData_t        Speed;


    for (i = 0; ((i < buffer_length) && (fscanf (fp, "%le %le\n", bp, &Speed) != EOF)); i++) {
        if (i == 0) {                               // Return the speed
            *pSpeed = Speed;
        }

        bp++;
    }

    sample_count = i;

    for (; (i < buffer_length); i++) {              // Zero pad the array
        *bp++ = 0.0;
    }

    return (sample_count);
}       // End of read_vibration_data ()


