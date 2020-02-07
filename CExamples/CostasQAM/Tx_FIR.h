/* Low-pass FIR Filter - Fs = 48 KHz, Fc = 4KHz */

#define TX_PRE_FILTER_LENGTH 65

SLData_t    TxPreFilterState [TX_PRE_FILTER_LENGTH];        /* Second order */
SLArrayIndex_t  TxPreFilterIndex;

    /* Floating point coefficients */
SLData_t TxPreFilterCoeffs [] =
{
    -5.14992445549231330600e-04,
    -1.08012271232981946200e-03,
    -1.79871604163860479000e-03,
    -2.32712496934081624100e-03,
    -2.27214077079759347100e-03,
    -1.30028138268203029800e-03,
    6.63359121229396722400e-04,
    3.28142183105471947600e-03,
    5.78721942712145595600e-03,
    7.16816050990245513800e-03,
    6.53435426022742557600e-03,
    3.55287090179154082000e-03,
    -1.22558782932997881000e-03,
    -6.33823058555480281500e-03,
    -9.77387663124081214300e-03,
    -9.68291171420569696200e-03,
    -5.20963833333394372300e-03,
    2.87946461282460224400e-03,
    1.20941509493820396900e-02,
    1.88160683851618625000e-02,
    1.95093318193091044800e-02,
    1.21943756061523942600e-02,
    -2.34610046868170265800e-03,
    -2.01511548877609941700e-02,
    -3.47668196503180607100e-02,
    -3.89708989152726184600e-02,
    -2.70819708004762203800e-02,
    2.88234471082422349200e-03,
    4.79651803597203260600e-02,
    1.00474425870090006140e-01,
    1.49701785099277118200e-01,
    1.84713837562567456700e-01,
    1.97383932766100722600e-01,
    1.84713837562567456700e-01,
    1.49701785099277118200e-01,
    1.00474425870090006140e-01,
    4.79651803597203260600e-02,
    2.88234471082422349200e-03,
    -2.70819708004762203800e-02,
    -3.89708989152726184600e-02,
    -3.47668196503180607100e-02,
    -2.01511548877609941700e-02,
    -2.34610046868170265800e-03,
    1.21943756061523942600e-02,
    1.95093318193091044800e-02,
    1.88160683851618625000e-02,
    1.20941509493820396900e-02,
    2.87946461282460224400e-03,
    -5.20963833333394372300e-03,
    -9.68291171420569696200e-03,
    -9.77387663124081214300e-03,
    -6.33823058555480281500e-03,
    -1.22558782932997881000e-03,
    3.55287090179154082000e-03,
    6.53435426022742557600e-03,
    7.16816050990245513800e-03,
    5.78721942712145595600e-03,
    3.28142183105471947600e-03,
    6.63359121229396722400e-04,
    -1.30028138268203029800e-03,
    -2.27214077079759347100e-03,
    -2.32712496934081624100e-03,
    -1.79871604163860479000e-03,
    -1.08012271232981946200e-03,
    -5.14992445549231330600e-04
};
