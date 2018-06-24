#ifndef _RANSAC_H_
#define _RANSAC_H_

#define ERROR_THRSHOLD 2

extern int predict_ransac(int testLevel, POINT* points, int dataNum, LINE& bestLine);
extern int least_square_linear_fit(POINT* fitPoint, const int num, LINE& fitLine);
#endif
