/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package siglib_wrap;

public final class SLRoundingMode_t {
  public final static SLRoundingMode_t SIGLIB_ROUND_UP = new SLRoundingMode_t("SIGLIB_ROUND_UP");
  public final static SLRoundingMode_t SIGLIB_ROUND_TO_NEAREST = new SLRoundingMode_t("SIGLIB_ROUND_TO_NEAREST");
  public final static SLRoundingMode_t SIGLIB_ROUND_DOWN = new SLRoundingMode_t("SIGLIB_ROUND_DOWN");
  public final static SLRoundingMode_t SIGLIB_ROUND_TO_ZERO = new SLRoundingMode_t("SIGLIB_ROUND_TO_ZERO");
  public final static SLRoundingMode_t SIGLIB_ROUND_AWAY_FROM_ZERO = new SLRoundingMode_t("SIGLIB_ROUND_AWAY_FROM_ZERO");

  public final int swigValue() {
    return swigValue;
  }

  public String toString() {
    return swigName;
  }

  public static SLRoundingMode_t swigToEnum(int swigValue) {
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (int i = 0; i < swigValues.length; i++)
      if (swigValues[i].swigValue == swigValue)
        return swigValues[i];
    throw new IllegalArgumentException("No enum " + SLRoundingMode_t.class + " with value " + swigValue);
  }

  private SLRoundingMode_t(String swigName) {
    this.swigName = swigName;
    this.swigValue = swigNext++;
  }

  private SLRoundingMode_t(String swigName, int swigValue) {
    this.swigName = swigName;
    this.swigValue = swigValue;
    swigNext = swigValue+1;
  }

  private SLRoundingMode_t(String swigName, SLRoundingMode_t swigEnum) {
    this.swigName = swigName;
    this.swigValue = swigEnum.swigValue;
    swigNext = this.swigValue+1;
  }

  private static SLRoundingMode_t[] swigValues = { SIGLIB_ROUND_UP, SIGLIB_ROUND_TO_NEAREST, SIGLIB_ROUND_DOWN, SIGLIB_ROUND_TO_ZERO, SIGLIB_ROUND_AWAY_FROM_ZERO };
  private static int swigNext = 0;
  private final int swigValue;
  private final String swigName;
}

