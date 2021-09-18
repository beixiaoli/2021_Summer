/*
 * default calculate functions
 * author: clarkyy
 * date:   20160909
 * http://www.allwinnertech.com
 */
#include "stdafx.h"
#include "CalculateFuncDefault.h"
#include "../hawkview_common/LinkedList.h"
#include "../MainFrm.h"
#include "../hawkview_handler/LapackHandler.h"
#include "./dlgs4calibration/DenoiseOpt.h"
#include "../hawkview_handler/calibrate_function.h"
#include <math.h>
#include <direct.h>
#include <io.h>

NAMESPACE_START

static void MSC_Block_Divide(IN int length, IN int block_num, IN int block_min, IN int divide_type, OUT int *divide_result)
{
	if(divide_type == 0) { // equal divide
		int dir_div = length/block_num;
		//if(dir_div%2 != 0) {
		//	dir_div = dir_div-1;
		//}
		for(int i = 0; i < block_num-1; i++) {
			divide_result[i] = dir_div;
		}
		divide_result[block_num-1] = length-dir_div*(block_num-1);
	} else {// other divide type

	}
}

static int MSC_Fixed_Process(IN_OUT int *stat_val, IN int fix_bit, IN int max_gain, IN int offset, IN int block_num_height, IN int block_num_width, OUT int *ovflag)
{
	int tbl_min, tbl_max, i, o_val;//, max_val;
	*ovflag = 0;
	tbl_min = *stat_val;
	tbl_max = *stat_val;
	for(i = 0; i < block_num_height * block_num_width; i++) {
		tbl_min = min(tbl_min, *(stat_val+i));
		tbl_max = max(tbl_max, *(stat_val+i));
	}
	if(tbl_max == 0) {
		DebugMsg("tbl_max = 0");
		return -1;
	}
	for(i = 0; i < block_num_height * block_num_width; i++) {
		o_val = (((tbl_max - offset * (*(stat_val+i))) * (1 << fix_bit)) + ((*(stat_val+i))/2)) / (*(stat_val+i));
		*(stat_val+i) = o_val;
	}
	#if 0
	max_val = max_gain * (1 << fix_bit) - 1;
	for(i = 0; i < block_num * block_num; i++) {
		if(*(stat_val+i) > max_val) {
			*(stat_val+i) = max_val;
			*ovflag = 1;
		}
	}
	#endif
	return 0;
}

////////////////////////////////////////////////////////
//�������ƽ������
//int n;
//double y[],yy[];
////////////////////////////////////////////////////////
void kspt(int n, double y[], double yy[])
{ 
	int i;
	if (n<5)
	{
		for (i=0; i<=n-1; i++)
			yy[i]=y[i];
	}
	else
	{
		yy[0]=69.0*y[0]+4.0*y[1]-6.0*y[2]+4.0*y[3]-y[4];
		yy[0]=yy[0]/70.0;
		yy[1]=2.0*y[0]+27.0*y[1]+12.0*y[2]-8.0*y[3];
		yy[1]=(yy[1]+2.0*y[4])/35.0;
		for (i=2; i<=n-3; i++)
		{
			yy[i]=-3.0*y[i-2]+12.0*y[i-1]+17.0*y[i];
			yy[i]=(yy[i]+12.0*y[i+1]-3.0*y[i+2])/35.0;
		}
		yy[n-2]=2.0*y[n-5]-8.0*y[n-4]+12.0*y[n-3];
		yy[n-2]=(yy[n-2]+27.0*y[n-2]+2.0*y[n-1])/35.0;
		yy[n-1]=-y[n-5]+4.0*y[n-4]-6.0*y[n-3];
		yy[n-1]=(yy[n-1]+4.0*y[n-2]+69.0*y[n-1])/70.0;
	}
}

// sum, count: 0-R, 1-GR, 2-GB, 3-B
void BayerPos2SumCount(IN BAYER_FORMAT format, IN int x, IN int y, IN int data, OUT double *sum, OUT long *count)
{
	if ((y & 1) && (x & 1))
	{
		if (BAYER_BGGR == format)      // R
		{
			sum[0] += data;
			count[0]++;
		}
		else if (BAYER_RGGB == format) // B
		{
			sum[3] += data;
			count[3]++;
		}
		else if (BAYER_GBRG == format) // GR
		{
			sum[1] += data;
			count[1]++;
		}
		else if (BAYER_GRBG == format) // GB
		{
			sum[2] += data;
			count[2]++;
		}
		else // error
		{
			// do nothing
		}

		return;
	}
	else if ((y & 1) && !(x & 1))
	{
		if (BAYER_BGGR == format)      // GR
		{
			sum[1] += data;
			count[1]++;
		}
		else if (BAYER_RGGB == format) // GB
		{
			sum[2] += data;
			count[2]++;
		}
		else if (BAYER_GBRG == format) // R
		{
			sum[0] += data;
			count[0]++;
		}
		else if (BAYER_GRBG == format) // B
		{
			sum[3] += data;
			count[3]++;
		}
		else // error
		{
			// do nothing
		}

		return;
	}
	else if (!(y & 1) && (x & 1))
	{
		if (BAYER_BGGR == format)      // GB
		{
			sum[2] += data;
			count[2]++;
		}
		else if (BAYER_RGGB == format) // GR
		{
			sum[1] += data;
			count[1]++;
		}
		else if (BAYER_GBRG == format) // B
		{
			sum[3] += data;
			count[3]++;
		}
		else if (BAYER_GRBG == format) // R
		{
			sum[0] += data;
			count[0]++;
		}
		else // error
		{
			// do nothing
		}

		return;
	}
	else if (!(y & 1) && !(x & 1))
	{
		if (BAYER_BGGR == format)      // B
		{
			sum[3] += data;
			count[3]++;
		}
		else if (BAYER_RGGB == format) // R
		{
			sum[0] += data;
			count[0]++;
		}
		else if (BAYER_GBRG == format) // GB
		{
			sum[2] += data;
			count[2]++;
		}
		else if (BAYER_GRBG == format) // GR
		{
			sum[1] += data;
			count[1]++;
		}
		else // error
		{
			// do nothing
		}

		return;
	}
	else // error
	{
		// do nothing
	}

	return;
}


int CalculateDefault4BLC(IN const void *data, IN const pSImageFormat format, OUT int *blcRet)
{
	if (!data || !format || !blcRet)
	{
		return -1;
	}

	switch (format->format)
	{
	case IMAGE_BMP:
	case IMAGE_JPEG:
		{
			SBmpImageInfo bmpInfo;
			if (GetBmpImageInfo((const unsigned char *)data, &bmpInfo))
			{
				SRGBFloat rgb;
				SRegion region = { 0, 0, bmpInfo.width, bmpInfo.height };

				CalculateAvgRGB(&bmpInfo, &region, &rgb);

				blcRet[0] = int(rgb.r * 16);
				blcRet[1] = int(rgb.g * 16);
				blcRet[2] = blcRet[1];
				blcRet[3] = int(rgb.b * 16);
			}
			return 0;
		}
		break;
	case IMAGE_YUV420_NV12:
	case IMAGE_YUV420_NV21:
	case IMAGE_YUV420_P:
	case IMAGE_YUV420_YV12:
	case IMAGE_YUV422:
	case IMAGE_YUV444:
		{
			int length = format->width * format->height;
			int *rgbData = new int[length + length + length];
			int *rgbPtr = rgbData;
			double sumR = 0, sumG = 0, sumB = 0;
			int i = 0, j = 0;

			ConvertYuv2Rgb((const unsigned char *)data, format, rgbData);

			for (i = 0; i < format->height; i++)
			{
				for (j = 0; j < format->width; j++)
				{
					sumB += *rgbPtr++;
					sumG += *rgbPtr++;
					sumR += *rgbPtr++;
				}
			}

			blcRet[0] = int(sumR * 16 / length);
			blcRet[1] = int(sumG * 16 / length);
			blcRet[2] = blcRet[1];
			blcRet[3] = int(sumB * 16 / length);

			SAFE_DELETE_ARRAY(rgbData);
			return 0;
		}
		break;
	case IMAGE_BAYER_BGGR8:
	case IMAGE_BAYER_GBRG8:
	case IMAGE_BAYER_GRBG8:
	case IMAGE_BAYER_RGGB8:
	case IMAGE_BAYER_BGGR10:
	case IMAGE_BAYER_GBRG10:
	case IMAGE_BAYER_GRBG10:
	case IMAGE_BAYER_RGGB10:
	case IMAGE_BAYER_BGGR12:
	case IMAGE_BAYER_GBRG12:
	case IMAGE_BAYER_GRBG12:
	case IMAGE_BAYER_RGGB12:
		{
			int length = format->width * format->height >> 2;  // R GR GB B 1/4
			double sumR = 0, sumGR = 0, sumGB = 0, sumB = 0;
			double raw00 = 0, raw01 = 0, raw10 = 0, raw11 = 0;
			const unsigned char *rawData = (const unsigned char *)data;
			int i = 0, j = 0;
			BAYER_FORMAT bayer_fmt = Convert2Bayer(format->format);

			if (format->bits == 10 || format->bits == 12 || format->bits == 16)
			{
				for (i = 0; i < format->height; i += 2)
				{
					for (j = 0; j < format->width; j += 2)
					{
						raw00 += (rawData[0] | (rawData[1] << 8));
						raw01 += (rawData[2] | (rawData[3] << 8));

						rawData += 4;
					}

					for (j = 0; j < format->width; j += 2)
					{
						raw10 += (rawData[0] | (rawData[1] << 8));
						raw11 += (rawData[2] | (rawData[3] << 8));

						rawData += 4;
					}
				}

				if (format->bits == 10)
				{
					raw00 *= 4;
					raw01 *= 4;
					raw10 *= 4;
					raw11 *= 4;
				}
				else if (format->bits == 16)
				{
					raw00 /= 16;
					raw01 /= 16;
					raw10 /= 16;
					raw11 /= 16;
				}
			}
			else  // regard as 8 bits
			{
				for (i = 0; i < format->height; i += 2)
				{
					for (j = 0; j < format->width; j += 2)
					{
						raw00 += *rawData++;
						raw01 += *rawData++;
					}

					for (j = 0; j < format->width; j += 2)
					{
						raw10 += *rawData++;
						raw11 += *rawData++;
					}
				}

				raw00 *= 16;
				raw01 *= 16;
				raw10 *= 16;
				raw11 *= 16;
			}

			if (BAYER_BGGR == bayer_fmt)
			{
				sumR  = raw11;
				sumGR = raw10;
				sumGB = raw01;
				sumB  = raw00;
			}
			else if (BAYER_RGGB == bayer_fmt)
			{
				sumR  = raw00;
				sumGR = raw01;
				sumGB = raw10;
				sumB  = raw11;
			}
			else if (BAYER_GBRG == bayer_fmt)
			{
				sumR  = raw10;
				sumGR = raw11;
				sumGB = raw00;
				sumB  = raw01;
			}
			else /*if (BAYER_RGRBG == bayer_fmt)*/
			{
				sumR  = raw01;
				sumGR = raw00;
				sumGB = raw11;
				sumB  = raw10;
			}

			blcRet[0] = int(sumR  / length);
			blcRet[1] = int(sumGR / length);
			blcRet[2] = int(sumGB / length);
			blcRet[3] = int(sumB  / length);

			return 0;
		}
		break;
	default:
		return -1;
	}
}

int CalculateDefault4Gain(IN const void *data, IN const pSImageFormat format, IN const pSRegion blocks,
				   IN const int *blcRet, OUT int *gainRet)
{
	if (!data || !format || !blocks || !blcRet || !gainRet)
	{
		return -1;
	}

	SFitPoint fitPoints4R[6], fitPoints4B[6];
	double realBlc[4] = { 0 };
	double rk = 0, rb = 0, rr = 0;
	double bk = 0, bb = 0, br = 0;
	int i = 0, j = 0, k = 0;

	// reset
	gainRet[0] = 256;
	gainRet[1] = 256;
	gainRet[2] = 256;
	gainRet[3] = 256;

	realBlc[0] = blcRet[0] / 16.0;
	realBlc[1] = blcRet[1] / 16.0;
	realBlc[2] = blcRet[2] / 16.0;
	realBlc[3] = blcRet[3] / 16.0;

	switch (format->format)
	{
	case IMAGE_BMP:
	case IMAGE_JPEG:
		{
			SBmpImageInfo bmpInfo;
			if (!GetBmpImageInfo((const unsigned char *)data, &bmpInfo))
			{
				return -1;
			}

			SRGBFloat blockRgb;
			for (i = 0; i < 6; i++)
			{
				CalculateAvgRGB(&bmpInfo, &blocks[i], &blockRgb);

				fitPoints4R[i].x = blockRgb.r - realBlc[0];
				fitPoints4R[i].y = blockRgb.g - realBlc[1];

				fitPoints4B[i].x = blockRgb.b - realBlc[3];
				fitPoints4B[i].y = blockRgb.g - realBlc[2];
			}
		}
		break;
	case IMAGE_YUV420_NV12:
	case IMAGE_YUV420_NV21:
	case IMAGE_YUV420_P:
	case IMAGE_YUV420_YV12:
	case IMAGE_YUV422:
	case IMAGE_YUV444:
		{
			int length = format->width * format->height;
			int *rgbData = new int[length + length + length];
			int *rgbPtr = NULL;
			int offset = 0, blockCount = 0;
			SRGBFloat blockRgb;

			ConvertYuv2Rgb((const unsigned char *)data, format, rgbData);

			for (i = 0; i < 6; i++)
			{
				blockRgb.r = blockRgb.g = blockRgb.b = 0;
				blockCount = 0;
				rgbPtr = rgbData + blocks[i].top * format->width * 3 + blocks[i].left * 3;
				offset = (format->width - (blocks[i].right - blocks[i].left)) * 3;
				for (j = blocks[i].top; j < blocks[i].bottom; j++)
				{
					for (k = blocks[i].left; k < blocks[i].right; k++)
					{
						blockRgb.b += *rgbPtr++;
						blockRgb.g += *rgbPtr++;
						blockRgb.r += *rgbPtr++;
						blockCount++;
					}

					rgbPtr += offset;
				}

				blockRgb.b /= blockCount;
				blockRgb.g /= blockCount;
				blockRgb.r /= blockCount;

				fitPoints4R[i].x = blockRgb.r - realBlc[0];
				fitPoints4R[i].y = blockRgb.g - realBlc[1];

				fitPoints4B[i].x = blockRgb.b - realBlc[3];
				fitPoints4B[i].y = blockRgb.g - realBlc[2];
			}

			SAFE_DELETE_ARRAY(rgbData);
		}
		break;
	case IMAGE_BAYER_BGGR8:
	case IMAGE_BAYER_GBRG8:
	case IMAGE_BAYER_GRBG8:
	case IMAGE_BAYER_RGGB8:
	case IMAGE_BAYER_BGGR10:
	case IMAGE_BAYER_GBRG10:
	case IMAGE_BAYER_GRBG10:
	case IMAGE_BAYER_RGGB10:
	case IMAGE_BAYER_BGGR12:
	case IMAGE_BAYER_GBRG12:
	case IMAGE_BAYER_GRBG12:
	case IMAGE_BAYER_RGGB12:
		{
			const unsigned char *rawPtr = NULL;
			int offset = 0, blockCount = 0;
			double rawSum[4] = { 0.0 }; // for R/GR/GB/B
			long rawCount[4] = { 0 };   // for R/GR/GB/B
			BAYER_FORMAT bayer_fmt = Convert2Bayer(format->format);

			if (format->bits == 8)
			{
				for (i = 0; i < 6; i++)
				{
					memset(rawSum, 0, sizeof(rawSum));
					memset(rawCount, 0, sizeof(rawCount));
					rawPtr = (const unsigned char *)data + blocks[i].top * format->width + blocks[i].left;
					offset = format->width - (blocks[i].right - blocks[i].left);
					for (j = blocks[i].top; j < blocks[i].bottom; j++)
					{
						for (k = blocks[i].left; k < blocks[i].right; k++)
						{
							BayerPos2SumCount(bayer_fmt, k, j, *rawPtr++, rawSum, rawCount);
						}

						rawPtr += offset;
					}

					for (j = 0; j < 4; j++)
					{
						if (rawCount[j] > 0)
						{
							rawSum[j] /= rawCount[j];
						}
					}

					fitPoints4R[i].x = rawSum[0] - realBlc[0];
					fitPoints4R[i].y = (rawSum[1] + rawSum[2]) / 2 - realBlc[1];

					fitPoints4B[i].x = rawSum[3] - realBlc[3];
					fitPoints4B[i].y = (rawSum[1] + rawSum[2]) / 2 - realBlc[2];
				}
			}
			else
			{
				for (i = 0; i < 6; i++)
				{
					memset(rawSum, 0, sizeof(rawSum));
					memset(rawCount, 0, sizeof(rawCount));
					rawPtr = (const unsigned char *)data + (blocks[i].top * format->width + blocks[i].left) * 2;
					offset = (format->width - (blocks[i].right - blocks[i].left)) * 2;
					for (j = blocks[i].top; j < blocks[i].bottom; j++)
					{
						for (k = blocks[i].left; k < blocks[i].right; k++)
						{
							BayerPos2SumCount(bayer_fmt, k, j, ((rawPtr[1] << 8) + rawPtr[0]), rawSum, rawCount);
							rawPtr += 2;
						}

						rawPtr += offset;
					}

					for (j = 0; j < 4; j++)
					{
						if (rawCount[j] > 0)
						{
							rawSum[j] /= rawCount[j];
						}
					}

					fitPoints4R[i].x = rawSum[0] - realBlc[0];
					fitPoints4R[i].y = (rawSum[1] + rawSum[2]) / 2 - realBlc[1];

					fitPoints4B[i].x = rawSum[3] - realBlc[3];
					fitPoints4B[i].y = (rawSum[1] + rawSum[2]) / 2 - realBlc[2];
				}
			}
		}
		break;
	default:
		return -1;
	}


	LineFit(fitPoints4R, 6, &rk, &rb, &rr);
	LineFit(fitPoints4B, 6, &bk, &bb, &br);

	rk += rb / 256;
	bk += bb / 256;

	gainRet[0] = int(256 * rk);
	gainRet[1] = 256;
	gainRet[2] = 256;
	gainRet[3] = int(256 * bk);

	return 0;
}
//**********************V200
//!�������õ��Ĳ�ͬ��ʽ��ͼ��ת��Ϊͬһ�ָ�ʽ��MemFile�������ڴ���������ͼ��RGB���������Ǻ��洦��
int CalculateDefault4Shading(IN const void *data, IN const pSImageFormat format,
					 IN const pSLscParams lscParams, OUT int *lscTable)
{
	if (!data || !format || !lscParams || !lscTable)//�κ�һ��Ϊ0�򷵻ش����־
	{
		return -1;
	}

	CMemFile bmpMem;//�ļ�

	switch (format->format)//���ú������ݵ�ͼƬ��ʽ��Ӧ�����Ա
	{
	case IMAGE_BMP:
	case IMAGE_JPEG:
		if (!SaveBmp2MemFile((const unsigned char *)data, &bmpMem))//����ͼ��ʧ��
		{
			return -1;
		}
		break;
	case IMAGE_YUV420_NV12:
	case IMAGE_YUV420_NV21:
	case IMAGE_YUV420_P:
	case IMAGE_YUV420_YV12:
	case IMAGE_YUV422:
	case IMAGE_YUV444:
		if (!SaveYuv2MemFile((const unsigned char *)data, format, &bmpMem))
		{
			return -1;
		}
		break;
	case IMAGE_BAYER_BGGR8:
	case IMAGE_BAYER_GBRG8:
	case IMAGE_BAYER_GRBG8:
	case IMAGE_BAYER_RGGB8:
	case IMAGE_BAYER_BGGR10:
	case IMAGE_BAYER_GBRG10:
	case IMAGE_BAYER_GRBG10:
	case IMAGE_BAYER_RGGB10:
	case IMAGE_BAYER_BGGR12:
	case IMAGE_BAYER_GBRG12:
	case IMAGE_BAYER_GRBG12:
	case IMAGE_BAYER_RGGB12:
		if (!SaveRaw2MemFileColor((const unsigned char *)data, format, &bmpMem))
		{
			return -1;
		}
		break;
	default:
		return -1;
	}
//!
	SBmpImageInfo bmpInfo;
	if (!GetBmpImageInfo(&bmpMem, &bmpInfo))//�������ͼ���ȡ��Info,��ȡͼ���ļ���Ϣ��������bmp��ʽ��ͼ����Ϣ�������ȡʧ��
	{
		return -1;
	}
//@������λֵlscRs
	int x1 = abs(bmpInfo.width - lscParams->centerX), x2 = abs(lscParams->centerX);//�ڴ��д洢��ͼƬ�������账���ͼƬ���Ƚ��бȽϣ�
	if (x1 < x2) { x1 = x2; }//x1Ϊ����
	int y1 = abs(bmpInfo.height - lscParams->centerY);
	int y2 = abs(lscParams->centerY);
	if (y1 < y2) { y1 = y2; }

	int sqDistance = x1 * x1 + y1 * y1;//�Խ��߳��ȵ�ƽ��
	int i = 0, j = 0, m = 0, n = 0;
	int range = 0, validRange = 0, lscRs = -1;

	for (i = 0; i < 18; i++)
	{   
		range = sqDistance >> i;//����iλ���൱�ڳ���
		if (range < 256)
		{
			lscRs = i;
			range += 1; // [0, range)
			break;
		}
	}

	if (lscRs == -1 || lscRs >= 18)  // not find
	{
		return -1;
	}
//@
	validRange = ((lscParams->radius * lscParams->radius) >> lscRs) + 1;//���ݵ�ͼƬ��������lscRsλ�ټ�1
	validRange = CLIP(validRange, 0, range);//�����������ڵڶ�����������Сֵ���͵��������������ֵ��֮�䣬�������/С�����ֵ/��Сֵ��������ֵ/��Сֵ
//#
	// calculate center rgb as the max value�������������һ���RGB��ֵ����Ϊ������ͬ��Բ����RGBֵҪ������ʹ����RGBֵ���������������һ����ɱ�����ϵ
	int radius1 = int(sqrt(double(1 << lscRs)) - 2);
	if (radius1 < 20) radius1 = 20;
	/*int radius1 = 20;*/
	SRegion centerPos = { lscParams->centerX - radius1, lscParams->centerY - radius1,
		lscParams->centerX + radius1, lscParams->centerY + radius1 };//������һ��ķ�Χ
	SRGBFloat centerRGB = { 0.0, 0.0, 0.0 };
	CalculateAvgRGB(&bmpInfo, &centerPos, &centerRGB);//����ͼ��ģ��RGB��ƽ��ֵ������������������ĸ�����ƽ��ֵ
//#
	const unsigned char *bmpData = bmpInfo.bmpData + (bmpInfo.height - 1) * bmpInfo.widthStep;
	int bmpOffset = bmpInfo.widthStep + bmpInfo.width * 3;//widthStep��Ĳ�������
	double sq = 0, sq_rt = 0, angle = 0;
	int sq_rtInt = 0;int r = 0, g = 0, b = 0;
	int *lscTableRet = new int[256 * 3];//ret����־������
	int *pointCount = new int[256 * 3];//���������
	int *rc = lscTableRet, *gc = lscTableRet + 256, *bc = lscTableRet + 512;//��ͬ��ָ���ַ
	int *pn_r = pointCount, *pn_g = pointCount + 256, *pn_b = pointCount + 512;
	memset(lscTableRet, 0x00, sizeof(int) * 256 * 3);//��ֵ����Ϊ0x00
	memset(pointCount, 0x00, sizeof(int) * 256 * 3);
//$����ÿһ��Բ����RGB��ֵ
	double ept = 0.01;
	for (i = 0; i < bmpInfo.height; i++)
	{
		for (j = 0; j < bmpInfo.width; j++)
		{
			angle = atan2((double)lscParams->centerY - i, (double)j - lscParams->centerX);//���ط�λ��
			if (angle < lscParams->alpha) { angle = angle + PI + PI; }//alphaΪ��ʼ��0~2�У�betaΪ�սǣ�0~2��

			if (angle >= lscParams->alpha && angle <= lscParams->beta)
			{
				r = bmpData[2];
				g = bmpData[1];
				b = bmpData[0];

				sq = pow(i - lscParams->centerY, 2.0) + pow(j - lscParams->centerX, 2.0);//pow(x,y)����x��y����
				sq_rt = sq / (1 << lscRs); //ͳ��Բ��ept��Χ�ڵĵ㣬1��0001������2λ��0100������4����Ϊ1����lscRsλ
				sq_rtInt = (int)sq_rt;
				ept = 0.04 + 0.001 * sq_rt;//��Χ��Բ������С���ɣ�����Բ����ʱ�򲻻������ֵͳ�ƽ���������ͳ�Ƶ�Բ����Χ�д˾���ƣ���Խ����ȡ�����ػ��һЩ
				if (abs(sq_rt - sq_rtInt) < ept)
				{
					rc[sq_rtInt] += r;   //ͳ��ͬһ��Χ�ڵ�Rֵ
					pn_r[sq_rtInt]++;   //����
					gc[sq_rtInt] += g; 
					pn_g[sq_rtInt]++;
					bc[sq_rtInt] += b; 
					pn_b[sq_rtInt]++;
				}
			}

			bmpData += 3;
		}

		bmpData -= bmpOffset;
	}
//$
	double rc_temp[256] = {0}, gc_temp[256] = {0}, bc_temp[256] = {0};
	double rc_double[256] = {0}, bc_double[256] = {0}, gc_double[256] = {0};

	for (i = 0; i < validRange; i++)
	{
		if ((pn_r[i] < 20) || (pn_g[i] < 20) || (pn_b[i] < 20))   //ͳ������20���㣬���ϴ���ȥ//�����Ե��ʱ��ͳ�Ƶĵ��ر��٣����ܴ�Ҫ��ȥ
		{
			validRange = i;
			break;
		}

		if (pn_r[i]!=0)
		{
			rc_temp[i] = (double)rc[i] / pn_r[i];     //����Բ��ƽ������
			rc_temp[i] = 1024.0 * centerRGB.r / rc_temp[i] * (1-i/(255.0*radius1*radius1));    //�����������������ֵ����һ��//���������棬����Խ�ߣ�����ԽС�������������ԽС
		}

		if (pn_g[i]!=0)
		{
			gc_temp[i] = (double)gc[i] / pn_g[i];
			gc_temp[i] = 1024.0 * centerRGB.g / gc_temp[i] * (1-i/(255.0*radius1*radius1));
		}

		if (pn_b[i]!=0)
		{
			bc_temp[i] = (double)bc[i]/pn_b[i];
			bc_temp[i] = 1024.0 * centerRGB.b / bc_temp[i] * (1-i/(255.0*radius1*radius1));
		}
	}


	kspt(validRange, rc_temp, rc_double);    //�������ƽ������//��MSC�ɲο���ƽ������Ҫ��
	kspt(validRange, gc_temp, gc_double);
	kspt(validRange, bc_temp, bc_double);

	double slope = rc_double[validRange-1] - rc_double[validRange-11];
	if (slope < 0) { slope = -slope; }
	for (i = validRange; i < 256; i++)//^��ͳ�Ƶ�Բ���Ĵ���������棬�ñ���ȥ������
	{
		rc_double[i] = rc_double[i-1] + slope / 11;
		gc_double[i] = gc_double[i-1] + slope / 11;
		bc_double[i] = bc_double[i-1] + slope / 11;
	}//^

	int tryRatioTimes = 0, tmpRet = 0;
	int count4095[3] = { 1 }, maxCount4095[3] = { 0 };
//&�򵥵Ĳ���ϵ������һ������ط�Ҫ����������100%һ��������Ϊ��������Խ�࣬����Խ�࣬���Ը�һ���ٷֱ���������һ��
	double ratioR = lscParams->compensation[0] / 100.0;
	double ratioG = lscParams->compensation[1] / 100.0;
	double ratioB = lscParams->compensation[2] / 100.0;
//&
TRY_RATIO_LOOP:
	tryRatioTimes++;//*��ֹ����ģ���ǰ�����ֵΪ4095����4����1024Ϊһ����Ϊʲô��4095����Ϊ��U12��12bit
	for (i = 0; i < 256; i++)
	{
		tmpRet = int(rc_double[i] + rc_double[i] * (ratioR - 1) * i / range);
		rc[i] = min(tmpRet, 4095);

		tmpRet = int(gc_double[i] + gc_double[i] * (ratioG - 1) * i / range);
		gc[i] = min(tmpRet, 4095);

		tmpRet = int(bc_double[i] + bc_double[i] * (ratioB - 1) * i / range);
		bc[i] = min(tmpRet, 4095);
	}
//*
// 	for (i = range; i < 256; i++)
// 	{
// 		rc[i] = min(rc_double[i] * ratioR, 4095);
// 		gc[i] = min(gc_double[i] * ratioG, 4095);
// 		bc[i] = min(bc_double[i] * ratioB, 4095);
// 	}

	// check validation
	for (i = 1; i < validRange; i++)
	{
		if (rc[i - 1] == 4095 && rc[i] == 4095) 
		{ 
			count4095[0]++; 
		}
		else
		{
			if (count4095[0] > maxCount4095[0])
			{
				maxCount4095[0] = count4095[0];
			}

			count4095[0] = 1;
		}

		if (gc[i - 1] == 4095 && gc[i] == 4095)
		{ 
			count4095[1]++; 
		}
		else
		{
			if (count4095[1] > maxCount4095[1])
			{
				maxCount4095[1] = count4095[1];
			}

			count4095[1] = 1;
		}

		if (bc[i- 1] == 4095 && bc[i] == 4095) 
		{ 
			count4095[2]++; 
		}
		else
		{
			if (count4095[2] > maxCount4095[2])
			{
				maxCount4095[2] = count4095[2];
			}

			count4095[2] = 1;
		}
	}

	if (count4095[0] > maxCount4095[0])
	{
		maxCount4095[0] = count4095[0];
	}

	if (count4095[1] > maxCount4095[1])
	{
		maxCount4095[1] = count4095[1];
	}

	if (count4095[2] > maxCount4095[2])
	{
		maxCount4095[2] = count4095[2];
	}

	if (maxCount4095[0] > 1 || maxCount4095[1] > 1 || maxCount4095[2] > 1)
	{
		ratioR -= 0.05;
		ratioG -= 0.05;
		ratioB -= 0.05;
		if (ratioR < 0.3 || ratioG < 0.3 || ratioB < 0.3)
		{
			SAFE_DELETE_ARRAY(lscTableRet);//���õ�ַָ�룿
			SAFE_DELETE_ARRAY(pointCount);
			return -1;
		}

		goto TRY_RATIO_LOOP;
	}

// 	if (tryRatioTimes > 1)
// 	{
// 	}

	// ret
	memcpy(lscTable, lscTableRet, sizeof(int) * 256 * 3);

#if _DEBUG
	FILE *fpLsc = NULL;//���ã�����ÿһ���ѽ��������Բ���
	fopen_s(&fpLsc, "D:\\test\\lsc_result.csv", "wb");
	if (fpLsc)
	{
		fprintf(fpLsc, "r, g, b,\n");
		for (int ii = 0; ii < 256; ii++)
		{
			fprintf(fpLsc, "%d, %d, %d,\n", rc[ii], gc[ii], bc[ii]);
		}
		fclose(fpLsc);
		fpLsc = NULL;
	}
#endif

	SAFE_DELETE_ARRAY(lscTableRet);
	SAFE_DELETE_ARRAY(pointCount);

	return 0;
}

int CalculateDefault4GCACenter(IN vector<string>& files, OUT pSGcaParams gcaParams)
{
	int ret = 0;
	if(files.empty())
	{
		DebugMsg("func = %s, line = %d, data = NULL", __FUNCTION__, __LINE__);
		return -1;
	}
	if(!gcaParams)
	{
		DebugMsg("func = %s, line = %d, mscTable = NULL", __FUNCTION__, __LINE__);
		return -1;
	}
	IplImage* colorImg;
	colorImg = cvLoadImage(files.at(0).c_str(), CV_LOAD_IMAGE_COLOR);
	gcaParams->ct_height = colorImg->height / 2;
	gcaParams->ct_width = colorImg->width / 2;
	cvReleaseImage( &colorImg);
	return ret;
}

int CalculateDefault4GCA(IN vector<string>& files,IN int PictureNumber, OUT pSGcaParams gcaParams)
{
	int ret = 0;
	struct GCA_PairPoint_pre{
		CvPoint2D32f *R_corners;
		CvPoint2D32f *G_corners;
		CvPoint2D32f *B_corners;
	};
	GCA_PairPoint_pre PairPoint_pre[11];

	if(files.empty())
	{
		DebugMsg("func = %s, line = %d, data = NULL\n", __FUNCTION__, __LINE__);
		return -1;
	}
	if(files.size() != PictureNumber)
	{
		DebugMsg("func = %s, line = %d, format = NULL\n", __FUNCTION__, __LINE__);
		return -1;
	}
	if(!gcaParams)
	{
		DebugMsg("func = %s, line = %d, mscTable = NULL\n", __FUNCTION__, __LINE__);
		return -1;
	}
	if(PictureNumber > 11)
	{
		return -1;
	}

	int i, j;
	int corner_total[11] = {0}, corner_total_AllPic = 0, G_pair_flag = 0, B_pair_flag = 0, pr_thrs = 5;
	char szPath[MAX_PATH * 2] = {0};
	//bool DBG_IMG = gcaParams->corner_pic; //�Ƿ񱣴����ͼƬ------>�����ɹ���ѡ���Ƿ񱣴�DBGͼƬ(ռ���ڴ�ϴ�)
	bool DBG_IMG = 1;
	if(!gcaParams->corner_pic)
	{
		DBG_IMG = FALSE;
	}
	GetModuleFileName(NULL, szPath, MAX_PATH * 2);
	(_tcsrchr(szPath, _T('\\')))[1] = 0;//ɾ���ļ�����ֻ���·��
	char szTxtFilePath[MAX_PATH * 2] = { 0 };

	FILE *test_file = NULL;
	sprintf(szTxtFilePath, "%s\\tmp\\gca\\test.txt", szPath);
	fopen_s(&test_file, szTxtFilePath, "wb");

	int file_i = 0;
	int Rcorner_count = 5000;
	int Gcorner_count = 5000;
	int Bcorner_count = 5000;

	/* opencv find corner */
	for (file_i = 0; file_i < PictureNumber; file_i++) {
		IplImage* image_gray;
		IplImage* colorImg;
		IplImage* pR_Plane;
		IplImage* pG_Plane;
		IplImage* pB_Plane;
		IplImage* pR_Channel;
		IplImage* pG_Channel;
		IplImage* pB_Channel;
		DebugMsg("\nfile addr: %s. PictureNumber: %d\n", files.at(file_i).c_str(), PictureNumber);
		colorImg = cvLoadImage(files.at(file_i).c_str(), CV_LOAD_IMAGE_COLOR);
		image_gray = cvCreateImage(cvGetSize(colorImg), 8, 1);
		cvCvtColor(colorImg, image_gray, CV_BGR2GRAY);
		pR_Plane = cvCreateImage(cvGetSize(colorImg), IPL_DEPTH_8U, 1);
		pG_Plane = cvCreateImage(cvGetSize(colorImg), IPL_DEPTH_8U, 1);
		pB_Plane = cvCreateImage(cvGetSize(colorImg), IPL_DEPTH_8U, 1);
		cvSplit(colorImg, pB_Plane, pG_Plane, pR_Plane, NULL); //��ȡR��G��B��ͨ��ͼ��
		pR_Channel = cvCreateImage(cvGetSize(colorImg), IPL_DEPTH_8U, 3);
		pG_Channel = cvCreateImage(cvGetSize(colorImg), IPL_DEPTH_8U, 3);
		pB_Channel = cvCreateImage(cvGetSize(colorImg), IPL_DEPTH_8U, 3);
		cvZero(pR_Channel);
		cvZero(pG_Channel);
		cvZero(pB_Channel);
		cvMerge(NULL, NULL, pR_Plane, NULL, pR_Channel);
		cvMerge(NULL, pG_Plane, NULL, NULL, pG_Channel);
		cvMerge(pB_Plane, NULL, NULL, NULL, pB_Channel);

		IplImage* eig = cvCreateImage( cvGetSize(colorImg), 32, 1 );
	    IplImage* temp = cvCreateImage( cvGetSize(colorImg), 32, 1 );
		double quality = (double)(gcaParams->quality)/100.0;//�ϸ�̶ȣ�1���ϸ� 0��ϸ� Ĭ��0.1
	    double min_distance = 5;//��̫����..

		//��෢�ֵĽǵ������������
		Rcorner_count = 5000;
		Gcorner_count = 5000;
		Bcorner_count = 5000;
		CvPoint2D32f *pre_Rcorners = new CvPoint2D32f[Rcorner_count];
		CvPoint2D32f *pre_Gcorners = new CvPoint2D32f[Gcorner_count];
		CvPoint2D32f *pre_Bcorners = new CvPoint2D32f[Bcorner_count];

		cvGoodFeaturesToTrack( pR_Plane, eig, temp, pre_Rcorners, &Rcorner_count, quality, min_distance, 0, 3, 0, 0.04 );
		cvGoodFeaturesToTrack( pG_Plane, eig, temp, pre_Gcorners, &Gcorner_count, quality, min_distance, 0, 3, 0, 0.04 );
		cvGoodFeaturesToTrack( pB_Plane, eig, temp, pre_Bcorners, &Bcorner_count, quality, min_distance, 0, 3, 0, 0.04 );


		cvFindCornerSubPix( pR_Plane, pre_Rcorners, Rcorner_count, cvSize(10,10), cvSize(-1,-1),
	        cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
		cvFindCornerSubPix( pG_Plane, pre_Gcorners, Gcorner_count, cvSize(10,10), cvSize(-1,-1),
	        cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
		cvFindCornerSubPix( pB_Plane, pre_Bcorners, Bcorner_count, cvSize(10,10), cvSize(-1,-1),
        	cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));

		//��ͼ�������ǵ�
		for (i = 0; i < Rcorner_count; i++)
	    {
	        cvLine(colorImg, cvPoint(cvRound(pre_Rcorners[i].x-5), cvRound(pre_Rcorners[i].y)),cvPoint(cvRound(pre_Rcorners[i].x+5), cvRound(pre_Rcorners[i].y)),CV_RGB(255,0,0), 1,8,0);
	        cvLine(colorImg, cvPoint(cvRound(pre_Rcorners[i].x), cvRound(pre_Rcorners[i].y-5)),cvPoint(cvRound(pre_Rcorners[i].x), cvRound(pre_Rcorners[i].y+5)),CV_RGB(255,0,0), 1,8,0);

	        cvLine(pR_Channel, cvPoint(cvRound(pre_Rcorners[i].x-5), cvRound(pre_Rcorners[i].y)),cvPoint(cvRound(pre_Rcorners[i].x+5), cvRound(pre_Rcorners[i].y)),CV_RGB(255,255,255), 1,8,0);
	        cvLine(pR_Channel, cvPoint(cvRound(pre_Rcorners[i].x), cvRound(pre_Rcorners[i].y-5)),cvPoint(cvRound(pre_Rcorners[i].x), cvRound(pre_Rcorners[i].y+5)),CV_RGB(255,255,255), 1,8,0);
		}
		for (i = 0; i < Gcorner_count; i++)
	    {
	        cvLine(colorImg, cvPoint(cvRound(pre_Gcorners[i].x-5), cvRound(pre_Gcorners[i].y)),cvPoint(cvRound(pre_Gcorners[i].x+5), cvRound(pre_Gcorners[i].y)),CV_RGB(0,255,0), 1,8,0);
	        cvLine(colorImg, cvPoint(cvRound(pre_Gcorners[i].x), cvRound(pre_Gcorners[i].y-5)),cvPoint(cvRound(pre_Gcorners[i].x), cvRound(pre_Gcorners[i].y+5)),CV_RGB(0,255,0), 1,8,0);

	        cvLine(pG_Channel, cvPoint(cvRound(pre_Gcorners[i].x-5), cvRound(pre_Gcorners[i].y)),cvPoint(cvRound(pre_Gcorners[i].x+5), cvRound(pre_Gcorners[i].y)),CV_RGB(255,255,255), 1,8,0);
	        cvLine(pG_Channel, cvPoint(cvRound(pre_Gcorners[i].x), cvRound(pre_Gcorners[i].y-5)),cvPoint(cvRound(pre_Gcorners[i].x), cvRound(pre_Gcorners[i].y+5)),CV_RGB(255,255,255), 1,8,0);
	    }
		for (i = 0; i < Bcorner_count; i++)
	    {
	        cvLine(colorImg, cvPoint(cvRound(pre_Bcorners[i].x-5), cvRound(pre_Bcorners[i].y)),cvPoint(cvRound(pre_Bcorners[i].x+5), cvRound(pre_Bcorners[i].y)),CV_RGB(0,0,255), 1,8,0);
	        cvLine(colorImg, cvPoint(cvRound(pre_Bcorners[i].x), cvRound(pre_Bcorners[i].y-5)),cvPoint(cvRound(pre_Bcorners[i].x), cvRound(pre_Bcorners[i].y+5)),CV_RGB(0,0,255), 1,8,0);

			cvLine(pB_Channel, cvPoint(cvRound(pre_Bcorners[i].x-5), cvRound(pre_Bcorners[i].y)),cvPoint(cvRound(pre_Bcorners[i].x+5), cvRound(pre_Bcorners[i].y)),CV_RGB(255,255,255), 1,8,0);
	        cvLine(pB_Channel, cvPoint(cvRound(pre_Bcorners[i].x), cvRound(pre_Bcorners[i].y-5)),cvPoint(cvRound(pre_Bcorners[i].x), cvRound(pre_Bcorners[i].y+5)),CV_RGB(255,255,255), 1,8,0);
	    }
		sprintf(szTxtFilePath, "%stmp\\gca\\dbg_pic", szPath);
		if (_access(szTxtFilePath, 0) == -1)
		{
			int ret = _mkdir(szTxtFilePath);
			if(ret == -1)
			{
				DebugMsg("func = %s, line = %d, create tmp\\gca\\dbg_pic folder failed\n", __FUNCTION__, __LINE__);
				return ret;
			}
		}
		if(DBG_IMG) {
			sprintf(szTxtFilePath, "%s\\tmp\\gca\\dbg_pic\\Corner_image_%d.bmp", szPath, file_i);
			cvSaveImage(szTxtFilePath, colorImg);
			sprintf(szTxtFilePath, "%s\\tmp\\gca\\dbg_pic\\pR_Channel_%d.bmp", szPath, file_i);
			cvSaveImage(szTxtFilePath, pR_Channel);
			sprintf(szTxtFilePath, "%s\\tmp\\gca\\dbg_pic\\pG_Channel_%d.bmp", szPath, file_i);
			cvSaveImage(szTxtFilePath, pG_Channel);
			sprintf(szTxtFilePath, "%s\\tmp\\gca\\dbg_pic\\pB_Channel_%d.bmp", szPath, file_i);
			cvSaveImage(szTxtFilePath, pB_Channel);
		}
		fprintf(test_file, "\nR=%d  G=%d  B=%d\n", Rcorner_count, Gcorner_count, Bcorner_count);
		//cvShowImage("color_img", colorImg);
		//cvShowImage("pR_Channel", pR_Channel);
		//cvShowImage("pG_Channel", pG_Channel);
		//cvShowImage("pB_Channel", pB_Channel);
	    cvWaitKey();

		/* delete repeat corner */
		for(i = 0; i < Rcorner_count; i++) {
			for(j = 0; j < i; j++) {
				if(fabs(pre_Rcorners[i].x - pre_Rcorners[j].x) < 5.0 &&
					fabs(pre_Rcorners[i].y - pre_Rcorners[j].y) < 5.0 &&
					pre_Rcorners[j].x != 0.0 && pre_Rcorners[j].y != 0.0) {
					// repeat R corner set to 0
					pre_Rcorners[i].x = (pre_Rcorners[i].x + pre_Rcorners[j].x) / 2.0;
					pre_Rcorners[i].y = (pre_Rcorners[i].y + pre_Rcorners[j].y) / 2.0;
					pre_Rcorners[j].x = 0;
					pre_Rcorners[j].y = 0;
				}
			}
		}
		for(i = 0; i < Gcorner_count; i++) {
			for(j = 0; j < i; j++) {
				if(fabs(pre_Gcorners[i].x - pre_Gcorners[j].x) < 5.0 &&
					fabs(pre_Gcorners[i].y - pre_Gcorners[j].y) < 5.0 &&
					pre_Gcorners[j].x != 0.0 && pre_Gcorners[j].y != 0.0) {
					// repeat G corner set to 0
					pre_Gcorners[i].x = (pre_Gcorners[i].x + pre_Gcorners[j].x) / 2.0;
					pre_Gcorners[i].y = (pre_Gcorners[i].y + pre_Gcorners[j].y) / 2.0;
					pre_Gcorners[j].x = 0;
					pre_Gcorners[j].y = 0;
				}
			}
		}
		for(i = 0; i < Bcorner_count; i++) {
			for(j = 0; j < i; j++) {
				if(fabs(pre_Bcorners[i].x - pre_Bcorners[j].x) < 5.0 &&
					fabs(pre_Bcorners[i].y - pre_Bcorners[j].y) < 5.0 &&
					pre_Bcorners[j].x != 0.0 && pre_Bcorners[j].y != 0.0) {
					// repeat B corner set to 0
					pre_Bcorners[i].x = (pre_Bcorners[i].x + pre_Bcorners[j].x) / 2.0;
					pre_Bcorners[i].y = (pre_Bcorners[i].y + pre_Bcorners[j].y) / 2.0;
					pre_Bcorners[j].x = 0;
					pre_Bcorners[j].y = 0;
				}
			}
		}

		/* delete bound corner */
		int bd_thrs = 5;
		for(i = 0; i < Rcorner_count; i++) {
			if(pre_Rcorners[i].x < bd_thrs || pre_Rcorners[i].y < bd_thrs ||
				pre_Rcorners[i].x > colorImg->width - bd_thrs ||
				pre_Rcorners[i].y > colorImg->height - bd_thrs) {
					pre_Rcorners[i].x = 0;
					pre_Rcorners[i].y = 0;
				}
		}
		for(i = 0; i < Gcorner_count; i++) {
			if(pre_Gcorners[i].x < bd_thrs || pre_Gcorners[i].y < bd_thrs ||
				pre_Gcorners[i].x > colorImg->width - bd_thrs ||
				pre_Gcorners[i].y > colorImg->height - bd_thrs) {
					pre_Gcorners[i].x = 0;
					pre_Gcorners[i].y = 0;
				}
		}
		for(i = 0; i < Bcorner_count; i++) {
			if(pre_Bcorners[i].x < bd_thrs || pre_Bcorners[i].y < bd_thrs ||
				pre_Bcorners[i].x > colorImg->width - bd_thrs ||
				pre_Bcorners[i].y > colorImg->height - bd_thrs) {
					pre_Bcorners[i].x = 0;
					pre_Bcorners[i].y = 0;
				}
		}

		/* corner pair process */
		//int pr_thrs = sqrt((long double)(bmpInfo.width * bmpInfo.width + bmpInfo.height * bmpInfo.height)) * 0.0107;
		pr_thrs = 5;
		G_pair_flag = 0;
		B_pair_flag = 0;
		for(i = 0; i < Rcorner_count; i++) {
			if(pre_Rcorners[i].x && pre_Rcorners[i].y) {
				for(j = 0; j < Gcorner_count; j++) {
					if(fabs(pre_Rcorners[i].x - pre_Gcorners[j].x) < pr_thrs && 
						fabs(pre_Rcorners[i].y - pre_Gcorners[j].y) < pr_thrs) {
						G_pair_flag = 1;
					}
				}
				for(j = 0; j < Bcorner_count; j++) {
					if(fabs(pre_Rcorners[i].x - pre_Bcorners[j].x) < pr_thrs && 
						fabs(pre_Rcorners[i].y - pre_Bcorners[j].y) < pr_thrs) {
						B_pair_flag = 1;
					}
				}
				if(G_pair_flag && B_pair_flag) {
					corner_total[file_i]++;
				} else {
					pre_Rcorners[i].x = 0;
					pre_Rcorners[i].y = 0;
				}
				//reset flag
				G_pair_flag = 0;
				B_pair_flag = 0;
			}
		}

		PairPoint_pre[file_i].R_corners = new CvPoint2D32f[corner_total[file_i]];
		PairPoint_pre[file_i].G_corners = new CvPoint2D32f[corner_total[file_i]];
		PairPoint_pre[file_i].B_corners = new CvPoint2D32f[corner_total[file_i]];
		//CvPoint2D32f *R_corners = new CvPoint2D32f[corner_total[file_i]];
		//CvPoint2D32f *G_corners = new CvPoint2D32f[corner_total[file_i]];
		//CvPoint2D32f *B_corners = new CvPoint2D32f[corner_total[file_i]];

		j = 0;
		for(i = 0; i < Rcorner_count; i++) {
			if(pre_Rcorners[i].x && pre_Rcorners[i].y) {
				PairPoint_pre[file_i].R_corners[j].x = pre_Rcorners[i].x;
				PairPoint_pre[file_i].R_corners[j].y = pre_Rcorners[i].y;
				j++;
				if(j > corner_total[file_i])
					return -1;
			}
		}
		for(i = 0; i < corner_total[file_i]; i++) {
			for(j = 0; j < Gcorner_count; j++) {
				if(fabs(PairPoint_pre[file_i].R_corners[i].x - pre_Gcorners[j].x) < pr_thrs && 
					fabs(PairPoint_pre[file_i].R_corners[i].y - pre_Gcorners[j].y) < pr_thrs) {
					PairPoint_pre[file_i].G_corners[i].x = pre_Gcorners[j].x;
					PairPoint_pre[file_i].G_corners[i].y = pre_Gcorners[j].y;
					break;
				}
			}
			for(j = 0; j < Bcorner_count; j++) {
				if(fabs(PairPoint_pre[file_i].R_corners[i].x - pre_Bcorners[j].x) < pr_thrs && 
					fabs(PairPoint_pre[file_i].R_corners[i].y - pre_Bcorners[j].y) < pr_thrs) {
					PairPoint_pre[file_i].B_corners[i].x = pre_Bcorners[j].x;
					PairPoint_pre[file_i].B_corners[i].y = pre_Bcorners[j].y;
					break;
				}
			}
		}

		corner_total_AllPic += corner_total[file_i];

		cvReleaseImage( &eig);
		cvReleaseImage( &temp);
		cvReleaseImage( &colorImg);
	    cvReleaseImage( &image_gray);
		cvReleaseImage( &pR_Plane);
		cvReleaseImage( &pG_Plane);
		cvReleaseImage( &pB_Plane);
		cvReleaseImage( &pR_Channel);
		cvReleaseImage( &pG_Channel);
		cvReleaseImage( &pB_Channel);
		SAFE_DELETE_ARRAY(pre_Rcorners);
		SAFE_DELETE_ARRAY(pre_Gcorners);
		SAFE_DELETE_ARRAY(pre_Bcorners);
	}

	CvPoint2D32f *R_corners_total = new CvPoint2D32f[corner_total_AllPic];
	CvPoint2D32f *G_corners_total = new CvPoint2D32f[corner_total_AllPic];
	CvPoint2D32f *B_corners_total = new CvPoint2D32f[corner_total_AllPic];

	j = 0;
	for (file_i = 0; file_i < PictureNumber; file_i++) {
		for (i = 0; i < corner_total[file_i]; i++) {
			R_corners_total[j] = PairPoint_pre[file_i].R_corners[i];
			G_corners_total[j] = PairPoint_pre[file_i].G_corners[i];
			B_corners_total[j] = PairPoint_pre[file_i].B_corners[i];
			j++;
		}
		SAFE_DELETE_ARRAY(PairPoint_pre[file_i].R_corners);
		SAFE_DELETE_ARRAY(PairPoint_pre[file_i].G_corners);
		SAFE_DELETE_ARRAY(PairPoint_pre[file_i].B_corners);
	}

	fprintf(test_file, "\ntotal corner=%d, pic_num=%d, ct_height=%d, ct_width=%d.\n", corner_total_AllPic, PictureNumber, gcaParams->ct_height, gcaParams->ct_width);
	for(i = 0; i < corner_total_AllPic; i++)
	{
		fprintf(test_file, "%.6f, %.6f   %.6f, %.6f   %.6f, %.6f\n", R_corners_total[i].x, R_corners_total[i].y,
			G_corners_total[i].x, G_corners_total[i].y, B_corners_total[i].x, B_corners_total[i].y);
	}

	/* calculate rg */
	int center_h = gcaParams->ct_height;//(colorImg->height-1)/2;//center_h��center_wΪͼ���ѧ���ģ��ϸ�����������Ҫ�궨�ģ���ͨ�����߽���궨����
	int center_w = gcaParams->ct_width;//(colorImg->width-1)/2;
	float *rg = new float[corner_total_AllPic];
	float *dh = new float[corner_total_AllPic];
	float *dw = new float[corner_total_AllPic];
	float *drc_h = new float[corner_total_AllPic];
	float *drc_w = new float[corner_total_AllPic];
	float *dbc_h = new float[corner_total_AllPic];
	float *dbc_w = new float[corner_total_AllPic];
	float *drc = new float[2*corner_total_AllPic];
	float *dbc = new float[2*corner_total_AllPic];
	float *dlt = new float[2*corner_total_AllPic];
	float *drg = new float[2*corner_total_AllPic];
	float *rdlt_h = new float[2*corner_total_AllPic];
	float *rdlt_w = new float[2*corner_total_AllPic];
	float *bdlt_h = new float[2*corner_total_AllPic];
	float *bdlt_w = new float[2*corner_total_AllPic];

	for(i = 0; i < corner_total_AllPic; i++) {
		R_corners_total[i].x--;
		R_corners_total[i].y--;
		G_corners_total[i].x--;
		G_corners_total[i].y--;
		B_corners_total[i].x--;
		B_corners_total[i].y--;
	}
	for(i = 0; i < corner_total_AllPic; i++) {
		rg[i] = sqrt((float)((G_corners_total[i].y - center_h) * (G_corners_total[i].y - center_h) + (G_corners_total[i].x - center_w) * (G_corners_total[i].x - center_w)));
		dh[i] = G_corners_total[i].y - center_h;
		dw[i] = G_corners_total[i].x - center_w;
		drc_h[i] = R_corners_total[i].y - G_corners_total[i].y;
		drc_w[i] = R_corners_total[i].x - G_corners_total[i].x;
		dbc_h[i] = B_corners_total[i].y - G_corners_total[i].y;
		dbc_w[i] = B_corners_total[i].x - G_corners_total[i].x;
	}
	//for(i = 0; i < corner_total_AllPic; i++)
	//{
	//	fprintf(test_file, "%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", rg[i], dh[i], dw[i], drc_h[i], drc_w[i], dbc_h[i], dbc_w[i]);
	//}
	for(i = 0; i < corner_total_AllPic; i++) {
		drc[i] = drc_h[i];
		dbc[i] = dbc_h[i];
		dlt[i] = dh[i];
		drg[i] = rg[i];
		rdlt_h[i] = 1;
		rdlt_w[i] = 0;
		bdlt_h[i] = 1;
		bdlt_w[i] = 0;

		drc[i + corner_total_AllPic] = drc_w[i];
		dbc[i + corner_total_AllPic] = dbc_w[i];
		dlt[i + corner_total_AllPic] = dw[i];
		drg[i + corner_total_AllPic] = rg[i];
		rdlt_h[i + corner_total_AllPic] = 0;
		rdlt_w[i + corner_total_AllPic] = 1;
		bdlt_h[i + corner_total_AllPic] = 0;
		bdlt_w[i + corner_total_AllPic] = 1;
	}
	for(i = 0; i < 2*corner_total_AllPic; i++)
	{
		fprintf(test_file, "%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", drc[i], dbc[i], dlt[i], drg[i], rdlt_h[i], rdlt_w[i], bdlt_h[i], bdlt_w[i]);
	}

	float *rx = new float[3];
	float *bx = new float[3];
	float *rm = new float[3*2*corner_total_AllPic];
	float *bm = new float[3*2*corner_total_AllPic];
	memset(rx, 0, 3*sizeof(float));
	memset(bx, 0, 3*sizeof(float));
	memset(rm, 0, 3*2*corner_total_AllPic*sizeof(float));
	memset(bm, 0, 3*2*corner_total_AllPic*sizeof(float));

	//rm��bmΪ(2*corner_total) x 3�ľ���
	for(i = 0; i < 2*corner_total_AllPic; i++) {
		rm[3*i] = dlt[i];
		rm[3*i + 1] = rdlt_h[i];
		rm[3*i + 2] = rdlt_w[i];
		bm[3*i] = dlt[i];
		bm[3*i + 1] = bdlt_h[i];
		bm[3*i + 2] = bdlt_w[i];
	}
	//rm��bmת����opencv mat�������ʽ
	CvMat *rm_mat = cvCreateMat(2*corner_total_AllPic, 3, CV_32FC1);
	CvMat *rm_mat_invert = cvCreateMat(3, 2*corner_total_AllPic, CV_32FC1);
	CvMat *bm_mat = cvCreateMat(2*corner_total_AllPic, 3, CV_32FC1);
	CvMat *bm_mat_invert = cvCreateMat(3, 2*corner_total_AllPic, CV_32FC1);
	for(i = 0; i < 2*corner_total_AllPic; i++) {
		for(j = 0; j < 3; j++) {
			cvmSet(rm_mat, i, j, rm[3*i + j]);
			cvmSet(bm_mat, i, j, bm[3*i + j]);
		}
	}
	//��rm��bm�������
	cvInvert(rm_mat, rm_mat_invert, CV_SVD);
	cvInvert(bm_mat, bm_mat_invert, CV_SVD);

	//rm��bm�������ֱ���drc��dbc�����ת�ã����յõ�һ��3*1�ľ���
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 2*corner_total_AllPic; j++) {
			rx[i] += cvmGet(rm_mat_invert, i, j) * drc[j];
			bx[i] += cvmGet(bm_mat_invert, i, j) * dbc[j];
		}
	}

	rx[0] = floor(rx[0] * 65536.0 + 0.5);
	rx[1] = floor(rx[1] * 256.0 + 0.5);
	rx[2] = floor(rx[2] * 256.0 + 0.5);
	bx[0] = floor(bx[0] * 65536.0 + 0.5);
	bx[1] = floor(bx[1] * 256.0 + 0.5);
	bx[2] = floor(bx[2] * 256.0 + 0.5);

	gcaParams->r_param0 = rx[0];
	gcaParams->r_param1 = rx[1];
	gcaParams->r_param2 = rx[2];
	gcaParams->b_param0 = bx[0];
	gcaParams->b_param1 = bx[1];
	gcaParams->b_param2 = bx[2];
	fprintf(test_file, "\n\n%.6f, %.6f   %.6f\n", rx[0], rx[1], rx[2]);
	fprintf(test_file, "%.6f, %.6f   %.6f\n", bx[0], bx[1], bx[2]);

	cvReleaseMat(&rm_mat);
	cvReleaseMat(&rm_mat_invert);
	cvReleaseMat(&bm_mat);
	cvReleaseMat(&bm_mat_invert);
	

	SAFE_DELETE_ARRAY(R_corners_total);
	SAFE_DELETE_ARRAY(G_corners_total);
	SAFE_DELETE_ARRAY(B_corners_total);
	SAFE_DELETE_ARRAY(rg);
	SAFE_DELETE_ARRAY(dh);
	SAFE_DELETE_ARRAY(dw);
	SAFE_DELETE_ARRAY(drc_h);
	SAFE_DELETE_ARRAY(drc_w);
	SAFE_DELETE_ARRAY(dbc_h);
	SAFE_DELETE_ARRAY(dbc_w);
	SAFE_DELETE_ARRAY(drc);
	SAFE_DELETE_ARRAY(dbc);
	SAFE_DELETE_ARRAY(dlt);
	SAFE_DELETE_ARRAY(drg);
	SAFE_DELETE_ARRAY(rdlt_h);
	SAFE_DELETE_ARRAY(rdlt_w);
	SAFE_DELETE_ARRAY(bdlt_h);
	SAFE_DELETE_ARRAY(bdlt_w);
	SAFE_DELETE_ARRAY(rx);
	SAFE_DELETE_ARRAY(bx);
	SAFE_DELETE_ARRAY(rm);
	SAFE_DELETE_ARRAY(bm);
	fclose(test_file);
	return ret;
}

//&&&&&&&&&&&&&&&&
int CalculateDefault4MSC(IN const void *data, IN const pSImageFormat format, IN const pSMscParams mscParams, IN int scope, IN int vcm, IN int temp, OUT int *mscTable)
{
	int ret = 0;
	//DebugMsg("func = %s, line = %d", __FUNCTION__, __LINE__);
	if(!data)
	{
		DebugMsg("func = %s, line = %d, data = NULL", __FUNCTION__, __LINE__);
		return -1;
	}
	if(!format)
	{
		DebugMsg("func = %s, line = %d, format = NULL", __FUNCTION__, __LINE__);
		return -1;
	}
	if(!mscParams)
	{
		DebugMsg("func = %s, line = %d, mscParams = NULL", __FUNCTION__, __LINE__);
		return -1;
	}
	if(!mscTable)
	{
		DebugMsg("func = %s, line = %d, mscTable = NULL", __FUNCTION__, __LINE__);
		return -1;
	}
	//DebugMsg("func = %s, line = %d", __FUNCTION__, __LINE__);
	CMemFile bmpMem;//�ļ�
	switch (format->format)//���ú������ݵ�ͼƬ��ʽ��Ӧ�����Ա
	{
	case IMAGE_BMP:
	case IMAGE_JPEG:
		if (!SaveBmp2MemFile((const unsigned char *)data, &bmpMem))//����ͼ��ʧ��
		{
			DebugMsg("SaveBmp2MemFile failed");
			return -1;
		}
		break;
	case IMAGE_YUV420_NV12:
	case IMAGE_YUV420_NV21:
	case IMAGE_YUV420_P:
	case IMAGE_YUV420_YV12:
	case IMAGE_YUV422:
	case IMAGE_YUV444:
		if (!SaveYuv2MemFile((const unsigned char *)data, format, &bmpMem))
		{
			DebugMsg("SaveYuv2MemFile failed");
			return -1;
		}
		//DebugMsg("func = %s, line = %d", __FUNCTION__, __LINE__);
		break;
	case IMAGE_BAYER_BGGR8:
	case IMAGE_BAYER_GBRG8:
	case IMAGE_BAYER_GRBG8:
	case IMAGE_BAYER_RGGB8:
	case IMAGE_BAYER_BGGR10:
	case IMAGE_BAYER_GBRG10:
	case IMAGE_BAYER_GRBG10:
	case IMAGE_BAYER_RGGB10:
	case IMAGE_BAYER_BGGR12:
	case IMAGE_BAYER_GBRG12:
	case IMAGE_BAYER_GRBG12:
	case IMAGE_BAYER_RGGB12:
		if (!SaveRaw2MemFileColor((const unsigned char *)data, format, &bmpMem))
		{
			DebugMsg("SaveRaw2MemFileColor failed");
			return -1;
		}
		break;
	default:
		DebugMsg("unknow picture format");
		return -1;
	}
	//DebugMsg("func = %s, line = %d", __FUNCTION__, __LINE__);
	//!
	SBmpImageInfo bmpInfo;
	if (!GetBmpImageInfo(&bmpMem, &bmpInfo))//�������ͼ���ȡ��Info,��ȡͼ���ļ���Ϣ��������bmp��ʽ��ͼ����Ϣ�������ȡʧ��
	{
		DebugMsg("GetBmpImageInfo failed!!!");
		return -1;
	}

	FILE *bayer_tbl = NULL, *height_div_file = NULL, *width_div_file = NULL, *rtable_file = NULL, *gtable_file = NULL, *btable_file = NULL;
	FILE *rgbtable_left_file = NULL, *rgbtable_right_file = NULL, *lgl_test_tbl = NULL;
	//BAYER_FORMAT bayer_format = BAYER_RGGB;
	int i, j, k, l;
	char szPath[MAX_PATH * 2] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH * 2);
	(_tcsrchr(szPath, _T('\\')))[1] = 0;//ɾ���ļ�����ֻ���·��
	char szFilePath[MAX_PATH * 2] = { 0 };

	sprintf(szFilePath, "%s\\tmp\\msc\\test.txt", szPath);
	fopen_s(&bayer_tbl, szFilePath, "wb");

	sprintf(szFilePath, "%s\\tmp\\msc\\height_div.txt", szPath);
	fopen_s(&height_div_file, szFilePath, "wb");

	sprintf(szFilePath, "%s\\tmp\\msc\\width_div.txt", szPath);
	fopen_s(&width_div_file, szFilePath, "wb");

	sprintf(szFilePath, "%s\\tmp\\msc\\rtable.txt", szPath);
	fopen_s(&rtable_file, szFilePath, "wb");

	sprintf(szFilePath, "%s\\tmp\\msc\\gtable.txt", szPath);
	fopen_s(&gtable_file, szFilePath, "wb");

	sprintf(szFilePath, "%s\\tmp\\msc\\btable.txt", szPath);
	fopen_s(&btable_file, szFilePath, "wb");

	sprintf(szFilePath, "%s\\tmp\\msc\\rgbtable_%d_%d_left.txt", szPath, vcm, temp);
	fopen_s(&rgbtable_left_file, szFilePath, "wb");

	sprintf(szFilePath, "%s\\tmp\\msc\\rgbtable_%d_%d_right.txt", szPath, vcm, temp);
	fopen_s(&rgbtable_right_file, szFilePath, "wb");

	sprintf(szFilePath, "%s\\tmp\\msc\\lgl_test_tbl.txt", szPath);	//for debug
	fopen_s(&lgl_test_tbl, szFilePath, "wb");

	//int *rawin = new int[bmpInfo.height * bmpInfo.width];
	int *rin = new int[bmpInfo.height * bmpInfo.width];
	int *gin = new int[bmpInfo.height * bmpInfo.width];
	int *bin = new int[bmpInfo.height * bmpInfo.width];
	/* get bayer image */
	//ConvertBmp2Bayer(&bmpInfo, bayer_format, rawin);
	ConvertBmp2RGB(&bmpInfo, rin, gin, bin);
	for(i = 0; i < bmpInfo.width*bmpInfo.height; i++) {
		rin[i] *= 16;
		gin[i] *= 16;
		bin[i] *= 16;
	}

	/* write rawin table */
	fprintf(bayer_tbl, "%d x %d\n\n", bmpInfo.width, bmpInfo.height);
	//for(i = 0; i < bmpInfo.width*bmpInfo.height; i++) {
	//	if(i%bmpInfo.width == 0)
	//		fprintf(bayer_tbl, "\n");
	//	fprintf(bayer_tbl, "%d, ", rawin[i]);
	//}

	int fix_type = mscParams->Msc_Mode; // input "fix_type"
	int block_num_height, block_num_width, block_min, divide_type, fix_bit, max_gain, offset, ovflag, rov, gov, bov;
	//ms_para_assign
	if(fix_type < 0 || fix_type > 7) {
		block_num_height = 0;
		block_num_width = 0;
		block_min = 0;
		//divide_type = 0;
		fix_bit = 0;
		max_gain = 0;
		offset = 0;
		return -1;
	}
	block_min = 8;
	//divide_type = 1;
	if(fix_type == 0) {
		block_num_height = 16;
		block_num_width = 31;
		fix_bit = 10;
		max_gain = 4;
		offset = 0;
	} else if(fix_type == 1){
		block_num_height = 16;
		block_num_width = 31;
		fix_bit = 9;
		max_gain = 8;
		offset = 0;
	} else if(fix_type == 2){
		block_num_height = 16;
		block_num_width = 31;
		fix_bit = 8;
		max_gain = 16;
		offset = 0;
	} else if(fix_type == 3){
		block_num_height = 16;
		block_num_width = 31;
		fix_bit = 7;
		max_gain = 32;
		offset = 0;
	} else if(fix_type == 4){
		block_num_height = 16;
		block_num_width = 31;
		fix_bit = 8;
		max_gain = 1;
		offset = 1;
	} else if(fix_type == 5){
		block_num_height = 16;
		block_num_width = 31;
		fix_bit = 7;
		max_gain = 2;
		offset = 1;
	} else if(fix_type == 6){
		block_num_height = 16;
		block_num_width = 31;
		fix_bit = 6;
		max_gain =4;
		offset = 1;
	} else if(fix_type == 7){
		block_num_height = 16;
		block_num_width = 31;
		fix_bit = 5;
		max_gain =8;
		offset = 1;
	}
	
	divide_type = mscParams->divide_type; // input "devide type "
	
	/* enter raw calibration */
	int *height_div = new int[block_num_height];
	int *width_div = new int[block_num_width];
	int *height_com = new int[block_num_height];
	int *width_com = new int[block_num_width];
	int *height_sum = new int[block_num_height + 1];
	int *width_sum = new int[block_num_width + 1];
	int *rtable = new int[block_num_height*block_num_width];
	int *gtable = new int[block_num_height*block_num_width];
	int *btable = new int[block_num_height*block_num_width];
	int max_val = max_gain * (1 << fix_bit) - 1;
	//double *comp_tbl = new double[block_num*block_num];
	/*
	double comp_ref[16*16] = {
		9.89949, 9.21954, 8.60233, 8.06226, 7.61577, 7.28011, 7.07107, 7.00000, 7.00000, 7.07107, 7.28011, 7.61577, 8.06226, 8.60233, 9.21954, 9.89950,
		9.21955, 8.48528, 7.81025, 7.21110, 6.70820, 6.32456, 6.08276, 6.00000,	6.00000, 6.08276, 6.32456, 6.70820, 7.21110, 7.81025, 8.48528, 9.21955,
		8.60233, 7.81025, 7.07107, 6.40312,	5.83096, 5.38517, 5.09902, 5.00000,	5.00000, 5.09902, 5.38517, 5.83096, 6.40312, 7.07107, 7.81025, 8.60233,
		8.06226, 7.21110, 6.40312, 5.65685,	5.00000, 4.47214, 4.12311, 4.00000,	4.00000, 4.12311, 4.47214, 5.00000, 5.65685, 6.40312, 7.21110, 8.06226,
		7.61577, 6.70820, 5.83095, 5.00000,	4.24264, 3.60555, 3.16228, 3.00000,	3.00000, 3.16228, 3.60555, 4.24264, 5.00000, 5.83095, 6.70820, 7.61577,
		7.28011, 6.32456, 5.38517, 4.47214,	3.60555, 2.82843, 2.23607, 2.00000,	2.00000, 2.23607, 2.82843, 3.60555, 4.47214, 5.38517, 6.32456, 7.28011,
		7.07107, 6.08276, 5.09902, 4.12311,	3.16228, 2.23607, 1.41421, 1.00000,	1.00000, 1.41421, 2.23607, 3.16228, 4.12311, 5.09902, 6.08276, 7.07107,
		7.00000, 6.00000, 5.00000, 4.00000, 3.00000, 2.00000, 1.00000, 0.00000, 0.00000, 1.00000, 2.00000, 3.00000, 4.00000, 5.00000, 6.00000, 7.00000,
		7.00000, 6.00000, 5.00000, 4.00000, 3.00000, 2.00000, 1.00000, 0.00000, 0.00000, 1.00000, 2.00000, 3.00000, 4.00000, 5.00000, 6.00000, 7.00000,
		7.07107, 6.08276, 5.09902, 4.12311, 3.16228, 2.23607, 1.41421, 1.00000, 1.00000, 1.41421, 2.23607, 3.16228, 4.12311, 5.09902, 6.08276, 7.07107,
		7.28011, 6.32456, 5.38516, 4.47214, 3.60555, 2.82843, 2.23607, 2.00000, 2.00000, 2.23607, 2.82843, 3.60555, 4.47214, 5.38516, 6.32456, 7.28011,
		7.61577, 6.70820, 5.83095, 5.00000, 4.24264, 3.60555, 3.16228, 3.00000, 3.00000, 3.16228, 3.60555, 4.24264, 5.00000, 5.83095, 6.70820, 7.61577,
		8.06226, 7.21110, 6.40312, 5.65685, 5.00000, 4.47214, 4.12311, 4.00000, 4.00000, 4.12311, 4.47214, 5.00000, 5.65685, 6.40312, 7.21110, 8.06226,
		8.60233, 7.81025, 7.07107, 6.40312, 5.83095, 5.38516, 5.09902, 5.00000, 5.00000, 5.09902, 5.38516, 5.83095, 6.40312, 7.07107, 7.81025, 8.60233,
		9.21954, 8.48528, 7.81025, 7.21110, 6.70820, 6.32456, 6.08276, 6.00000, 6.00000, 6.08276, 6.32456, 6.70820, 7.21110, 7.81025, 8.48528, 9.21954,
		9.89949, 9.21954, 8.60233, 8.06226, 7.61577, 7.28011, 7.07107, 7.00000, 7.00000, 7.07107, 7.28011, 7.61577, 8.06226, 8.60233, 9.21954, 9.89949,
	};*/
	double comp_a, comp_ratio;
	int compensation = mscParams->compensation; // input "compensation"
	int manual_mode = mscParams->manual_mode; //input "manual mode"��ѡ���Ƿ�ʹ���ֶ����÷ֿ飬0�Զ���1�ֶ�
	int manual_width_div[31] = {0};	//31
	int manual_height_div[16] = {0};

	int cell_size, block_height, block_width, width_div_total = 0, height_div_total = 0;
	unsigned long long rstat=0, gstat=0, bstat=0;
	compensation = CLIP(compensation, 0, 100);
	/*if(block_num_height == 0 || block_num_width == 0 || block_min%2 != 0 || bmpInfo.height%4 != 0 || bmpInfo.width%4 != 0 ||
		bmpInfo.height == 0 || bmpInfo.width == 0 || bmpInfo.height<block_num_height*block_min ||
		bmpInfo.width<block_num_width*block_min || block_num_height%2 != 0 || block_num_width%2 != 0) {
			DebugMsg("block_num_height = %d, block_num_width = %d, block_min = %d, bmpInfo.height = %d, bmpInfo.width = %d", 
				block_num_height, block_num_width, block_min, bmpInfo.height, bmpInfo.width);
			ret = -1;
			goto _error;
	}*/
	/* get height block divide */
	int width_left = 3264, width_right = 1008, height_left = 3120, height_right = 3120;
	if(!manual_mode) {//�Զ��ֿ�
		MSC_Block_Divide(bmpInfo.height, block_num_height, block_min, divide_type, &height_div[0]);
		MSC_Block_Divide(bmpInfo.width, block_num_width, block_min, divide_type, &width_div[0]);
		for(i = 0; i < block_num_width; i++) {
			fprintf(lgl_test_tbl, "manual0_width_div[%d] = %d \n ",i,width_div[i]); //for debug
		}
		for(i = 0; i < block_num_height; i++) {
			fprintf(lgl_test_tbl, "manual0_height_div[%d] = %d \n ",i,height_div[i]); //for debug
		}
	} else {//�ֶ��ֿ�
		//width
		int blw_mid_left = 0, blw_mid_right = 0;
		manual_width_div[0] = 64; 
		for(i = 1; i < block_num_width/4; i++){
			manual_width_div[i] = floor(((double)width_left/2-64)/7);
			blw_mid_left += manual_width_div[i];
		}
		manual_width_div[block_num_width/4] = width_left/2-64-blw_mid_left;
		for(i = block_num_width/4+1; i < block_num_width/2; i++){
			manual_width_div[i] = manual_width_div[2*(block_num_width/4)+1-i];
		}
		manual_width_div[block_num_width/2] = 64;
		for(i = block_num_width/2+1; i < block_num_width/2+block_num_width/4; i++){
			manual_width_div[i] = floor(((double)width_right/2-64)/7);
			blw_mid_right += manual_width_div[i];
		}
		manual_width_div[block_num_width/2+block_num_width/4] = width_right/2-64-blw_mid_right;
		for(i = block_num_width/2+block_num_width/4+1; i < block_num_width-1; i++){
			manual_width_div[i] = manual_width_div[2*(block_num_width/2+block_num_width/4)+1-i];
		}
		manual_width_div[block_num_width-1] = 64;
		//height
		for(i = 0; i < block_num_height; i++){
			manual_height_div[i] = (bmpInfo.height/2)/8;
		}
		
		for(i = 0; i < block_num_width; i++) {
			width_div[i] = manual_width_div[i];
			width_div_total += width_div[i];
			fprintf(lgl_test_tbl, "manual1_width_div[%d] = %d \n ",i,width_div[i]); //for debug
		}
		for(i = 0; i < block_num_height; i++) {
			height_div[i] = manual_height_div[i];
			height_div_total += height_div[i];
			fprintf(lgl_test_tbl, "manual1_height_div[%d] = %d \n ",i,height_div[i]); //for debug
		}
		if(width_div_total != (bmpInfo.width)) {
			//��ʾ: width divide error!
			ret = -2;
			goto _error;
		}
		if(height_div_total != (bmpInfo.height)) {
			//��ʾ: height divide error!
			ret = -3;
			goto _error;
		}
	}
	//DebugMsg("func = %s, line = %d", __FUNCTION__, __LINE__);
	/*
	double comp_ref[22*22]= {
				9.89949, 9.21954, 8.60233, 8.06226, 7.61577, 7.28011, 7.07107, 7.00000, 7.00000, 7.07107, 7.28011, 7.61577, 8.06226, 8.60233, 9.21954, 9.89950,
				9.21955, 8.48528, 7.81025, 7.21110, 6.70820, 6.32456, 6.08276, 6.00000, 6.00000, 6.08276, 6.32456, 6.70820, 7.21110, 7.81025, 8.48528, 9.21955,
				8.60233, 7.81025, 7.07107, 6.40312, 5.83096, 5.38517, 5.09902, 5.00000, 5.00000, 5.09902, 5.38517, 5.83096, 6.40312, 7.07107, 7.81025, 8.60233,
				8.06226, 7.21110, 6.40312, 5.65685, 5.00000, 4.47214, 4.12311, 4.00000, 4.00000, 4.12311, 4.47214, 5.00000, 5.65685, 6.40312, 7.21110, 8.06226,
				7.61577, 6.70820, 5.83095, 5.00000, 4.24264, 3.60555, 3.16228, 3.00000, 3.00000, 3.16228, 3.60555, 4.24264, 5.00000, 5.83095, 6.70820, 7.61577,
				7.28011, 6.32456, 5.38517, 4.47214, 3.60555, 2.82843, 2.23607, 2.00000, 2.00000, 2.23607, 2.82843, 3.60555, 4.47214, 5.38517, 6.32456, 7.28011,
				7.07107, 6.08276, 5.09902, 4.12311, 3.16228, 2.23607, 1.41421, 1.00000, 1.00000, 1.41421, 2.23607, 3.16228, 4.12311, 5.09902, 6.08276, 7.07107,
				7.00000, 6.00000, 5.00000, 4.00000, 3.00000, 2.00000, 1.00000, 0.00000, 0.00000, 1.00000, 2.00000, 3.00000, 4.00000, 5.00000, 6.00000, 7.00000,
				7.00000, 6.00000, 5.00000, 4.00000, 3.00000, 2.00000, 1.00000, 0.00000, 0.00000, 1.00000, 2.00000, 3.00000, 4.00000, 5.00000, 6.00000, 7.00000,
				7.07107, 6.08276, 5.09902, 4.12311, 3.16228, 2.23607, 1.41421, 1.00000, 1.00000, 1.41421, 2.23607, 3.16228, 4.12311, 5.09902, 6.08276, 7.07107,
				7.28011, 6.32456, 5.38516, 4.47214, 3.60555, 2.82843, 2.23607, 2.00000, 2.00000, 2.23607, 2.82843, 3.60555, 4.47214, 5.38516, 6.32456, 7.28011,
				7.61577, 6.70820, 5.83095, 5.00000, 4.24264, 3.60555, 3.16228, 3.00000, 3.00000, 3.16228, 3.60555, 4.24264, 5.00000, 5.83095, 6.70820, 7.61577,
				8.06226, 7.21110, 6.40312, 5.65685, 5.00000, 4.47214, 4.12311, 4.00000, 4.00000, 4.12311, 4.47214, 5.00000, 5.65685, 6.40312, 7.21110, 8.06226,
				8.60233, 7.81025, 7.07107, 6.40312, 5.83095, 5.38516, 5.09902, 5.00000, 5.00000, 5.09902, 5.38516, 5.83095, 6.40312, 7.07107, 7.81025, 8.60233,
				9.21954, 8.48528, 7.81025, 7.21110, 6.70820, 6.32456, 6.08276, 6.00000, 6.00000, 6.08276, 6.32456, 6.70820, 7.21110, 7.81025, 8.48528, 9.21954,
				9.89949, 9.21954, 8.60233, 8.06226, 7.61577, 7.28011, 7.07107, 7.00000, 7.00000, 7.07107, 7.28011, 7.61577, 8.06226, 8.60233, 9.21954, 9.89949,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				0.00000, 0.00000, 0.00000, 0.00000,
	};
	
	//if is auto_mode && block_num is 22, give the comp_ref new value
	if(!manual_mode && block_num == 22) {
		double a[22]={10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		int index = 0;
		for(i = 0; i < 22; i++) {
			for(j = 0; j < 22; j++) {
				comp_ref[index++]= sqrt(double(a[i] * a[i] + a[j] * a[j]));
			}
		}	
	}
	*/
	double comp_ref[31*16]= {
				16.55295, 15.65248, 14.76482, 13.89244, 13.03840, 12.20656, 11.40175, 10.63015, 9.89949, 9.21954, 8.60233, 8.06226, 7.61577, 7.28011, 7.07107, 7.00000, 7.07107, 7.28011, 7.61577, 8.06226, 8.60233, 9.21954, 9.89950, 10.63015, 11.40175, 12.20656, 13.03840, 13.89244, 14.76482, 15.65248, 16.55295,
				16.15549, 15.23155, 14.31782, 13.41641, 12.52996, 11.66190, 10.81665, 10.00000, 9.21955, 8.48528, 7.81025, 7.21110, 6.70820, 6.32456, 6.08276, 6.00000, 6.08276, 6.32456, 6.70820, 7.21110, 7.81025, 8.48528, 9.21955, 10.00000, 10.81665, 11.66190, 12.52996, 13.41641, 14.31782, 15.23155, 16.15549,
				15.81139, 14.86607, 13.92839, 13.00000, 12.08305, 11.18034, 10.29563, 9.43398, 8.60233, 7.81025, 7.07107, 6.40312, 5.83096, 5.38517, 5.09902, 5.00000, 5.09902, 5.38517, 5.83096, 6.40312, 7.07107, 7.81025, 8.60233, 9.43398, 10.29563, 11.18034, 12.08305, 13.00000, 13.92839, 14.86607, 15.81139,
				15.52417, 14.56022, 13.60147, 12.64911, 11.70470, 10.77033, 9.84886, 8.94427, 8.06226, 7.21110, 6.40312, 5.65685, 5.00000, 4.47214, 4.12311, 4.00000, 4.12311, 4.47214, 5.00000, 5.65685, 6.40312, 7.21110, 8.06226, 8.94427, 9.84886, 10.77033, 11.70470, 12.64911, 13.60147, 14.56022, 15.52417,
				15.29706, 14.31782, 13.34166, 12.36932, 11.40175, 10.44031, 9.48683, 8.54400, 7.61577, 6.70820, 5.83095, 5.00000, 4.24264, 3.60555, 3.16228, 3.00000, 3.16228, 3.60555, 4.24264, 5.00000, 5.83095, 6.70820, 7.61577, 8.54400, 9.48683, 10.44031, 11.40175, 12.36932, 13.34166, 14.31782, 15.29706,
				15.13275, 14.14214, 13.15295, 12.16553, 11.18034, 10.19804, 9.21954, 8.24621, 7.28011, 6.32456, 5.38517, 4.47214, 3.60555, 2.82843, 2.23607, 2.00000, 2.23607, 2.82843, 3.60555, 4.47214, 5.38517, 6.32456, 7.28011, 8.24621, 9.21954, 10.19804, 11.18034, 12.16553, 13.15295, 14.14214, 15.13275,
				15.03330, 14.03567, 13.03840, 12.04159, 11.04536, 10.04988, 9.05539, 8.06226, 7.07107, 6.08276, 5.09902, 4.12311, 3.16228, 2.23607, 1.41421, 1.00000, 1.41421, 2.23607, 3.16228, 4.12311, 5.09902, 6.08276, 7.07107, 8.06226, 9.05539, 10.04988, 11.04536, 12.04159, 13.03840, 14.03567, 15.03330,
				15.00000, 14.00000, 13.00000, 12.00000, 11.00000, 10.00000, 9.00000, 8.00000, 7.00000, 6.00000, 5.00000, 4.00000, 3.00000, 2.00000, 1.00000, 0.00000, 1.00000, 2.00000, 3.00000, 4.00000, 5.00000, 6.00000, 7.00000, 8.00000, 9.00000, 10.00000, 11.00000, 12.00000, 13.00000, 14.00000, 15.00000,
				15.00000, 14.00000, 13.00000, 12.00000, 11.00000, 10.00000, 9.00000, 8.00000, 7.00000, 6.00000, 5.00000, 4.00000, 3.00000, 2.00000, 1.00000, 0.00000, 1.00000, 2.00000, 3.00000, 4.00000, 5.00000, 6.00000, 7.00000, 8.00000, 9.00000, 10.00000, 11.00000, 12.00000, 13.00000, 14.00000, 15.00000,
				15.03330, 14.03567, 13.03840, 12.04159, 11.04536, 10.04988, 9.05539, 8.06226, 7.07107, 6.08276, 5.09902, 4.12311, 3.16228, 2.23607, 1.41421, 1.00000, 1.41421, 2.23607, 3.16228, 4.12311, 5.09902, 6.08276, 7.07107, 8.06226, 9.05539, 10.04988, 11.04536, 12.04159, 13.03840, 14.03567, 15.03330,
				15.13275, 14.14214, 13.15295, 12.16553, 11.18034, 10.19804, 9.21954, 8.24621, 7.28011, 6.32456, 5.38516, 4.47214, 3.60555, 2.82843, 2.23607, 2.00000, 2.23607, 2.82843, 3.60555, 4.47214, 5.38516, 6.32456, 7.28011, 8.24621, 9.21954, 10.19804, 11.18034, 12.16553, 13.15295, 14.14214, 15.13275,
				15.29706, 14.31782, 13.34166, 12.36932, 11.40175, 10.44031, 9.48683, 8.54400, 7.61577, 6.70820, 5.83095, 5.00000, 4.24264, 3.60555, 3.16228, 3.00000, 3.16228, 3.60555, 4.24264, 5.00000, 5.83095, 6.70820, 7.61577, 8.54400, 9.48683, 10.44031, 11.40175, 12.36932, 13.34166, 14.31782, 15.29706,
				15.52417, 14.56022, 13.60147, 12.64911, 11.70470, 10.77033, 9.84886, 8.94427, 8.06226, 7.21110, 6.40312, 5.65685, 5.00000, 4.47214, 4.12311, 4.00000, 4.12311, 4.47214, 5.00000, 5.65685, 6.40312, 7.21110, 8.06226, 8.94427, 9.84886, 10.77033, 11.70470, 12.64911, 13.60147, 14.56022, 15.52417,
				15.81139, 14.86607, 13.92839, 13.00000, 12.08305, 11.18034, 10.29563, 9.43398, 8.60233, 7.81025, 7.07107, 6.40312, 5.83095, 5.38516, 5.09902, 5.00000, 5.09902, 5.38516, 5.83095, 6.40312, 7.07107, 7.81025, 8.60233, 9.43398, 10.29563, 11.18034, 12.08305, 13.00000, 13.92839, 14.86607, 15.81139,
				16.15549, 15.23155, 14.31782, 13.41641, 12.52996, 11.66190, 10.81665, 10.00000, 9.21954, 8.48528, 7.81025, 7.21110, 6.70820, 6.32456, 6.08276, 6.00000, 6.08276, 6.32456, 6.70820, 7.21110, 7.81025, 8.48528, 9.21954, 10.00000, 10.81665, 11.66190, 12.52996, 13.41641, 14.31782, 15.23155, 16.15549,
				16.55295, 15.65248, 14.76482, 13.89244, 13.03840, 12.20656, 11.40175, 10.63015, 9.89949, 9.21954, 8.60233, 8.06226, 7.61577, 7.28011, 7.07107, 7.00000, 7.07107, 7.28011, 7.61577, 8.06226, 8.60233, 9.21954, 9.89949, 10.63015, 11.40175, 12.20656, 13.03840, 13.89244, 14.76482, 15.65248, 16.55295,
	};
	/*
	if(manual_mode){
		double width[11] = {0};
		double height[11] = {0};
		double distance_w = 0, distance_h = 0;

		for(i = 0; i < block_num/2-1; i++) {
			distance_w = (width_div[i] + width_div[block_num/2-1]) / 2;
			distance_h = (height_div[i] + height_div[block_num/2-1]) / 2;
			for(j = i+1; j < block_num/2-1; j++) {
				distance_w += width_div[j];
				distance_h += height_div[j];
			}
			width[i] = distance_w;
			height[i] = distance_h;
			//fprintf(lgl_test_tbl, "width[%d] = %lf; height[%d] = %lf \n ",i,distance1,i,distance2); //for debug
		}

		width[block_num/2-1] = 0;
		height[block_num/2-1] = 0;

		for(i = 0; i < block_num*block_num/2 ; i = i+block_num) {
			for(j = 0; j < block_num/2 ; j++) {
				comp_ref[i+j]
				=comp_ref[i+block_num-j-1]
				=comp_ref[block_num*(block_num-1)+j-i]
				=comp_ref[block_num*block_num-1-j-i]
				=sqrt( pow(width[j],2) + pow(height[i/16],2) );
			}
		}
	}
	*/
	//manual_mode�����¼��㲹�� 
	if(manual_mode){
		double width[31] = {0};
		double height[16] = {0};
		double distance_w = 0, distance_h = 0;
		//width
		for(i = 0; i < block_num_width/2; i++) {
			distance_w = (width_div[i] + width_div[block_num_width/2]) / 2;
			for(j = i+1; j < block_num_width/2; j++) {
				distance_w += width_div[j];
			}
			width[i] = distance_w;
			fprintf(lgl_test_tbl, "manual1_width[%d] = %lf \n ",i,width[i]); //for debug
		}
		for(i = block_num_width-1; i > block_num_width/2; i--) {
			distance_w = (width_div[i] + width_div[block_num_width/2]) / 2;
			for(j = i-1; j > block_num_width/2; j--) {
				distance_w += width_div[j];
			}
			width[i] = distance_w;
			fprintf(lgl_test_tbl, "manual1_width[%d] = %lf \n ",i,width[i]); //for debug
		}
		//height
		for(i = 0; i < block_num_height/2-1; i++) {
			distance_h = (height_div[i] + height_div[block_num_height/2-1]) / 2;
			for(j = i+1; j < block_num_height/2-1; j++) {
				distance_h += height_div[j];
			}
			height[i] = distance_h;
			fprintf(lgl_test_tbl, "manual1_height[%d] = %lf \n ",i,height[i]); //for debug
		}
		for(i = block_num_height-1; i >block_num_height/2-1 ; i--) {
			distance_h = (height_div[i] + height_div[block_num_height/2]) / 2;
			for(j = i-1; j > block_num_height/2-1; j--) {
				distance_h += height_div[j];
			}
			height[i] = distance_h;
			fprintf(lgl_test_tbl, "manual1_height[%d] = %lf \n ",i,height[i]); //for debug
		}

		width[block_num_width/2] = 0;
		height[block_num_height/2-1] = 0;

		for(i = 0; i < block_num_height*block_num_width/2 ; i = i+block_num_width) {
			for(j = 0; j < block_num_width/2 ; j++) {
				comp_ref[i+j]
				=comp_ref[block_num_width*(block_num_height-1)+j-i]
				=sqrt( pow(width[j],2) + pow(height[i],2) );
			}
			for(j = block_num_width-1; j > block_num_width/2 ; j--) {
				comp_ref[i+block_num_width-j-1]
				=comp_ref[block_num_width*block_num_height-1-j-i]
				=sqrt( pow(width[j],2) + pow(height[i],2) );
			}
		}
	}

	//for debug�� output the comp_ref
//#if 0
	int abc = 0;
	for(int i = 0; i < block_num_width; i++)
	{
		for(int j = 0; j < block_num_height; j++){
			fprintf(lgl_test_tbl, "comp_ref[%d] = %lf \n ", abc, comp_ref[abc++]);
		}
		fprintf(lgl_test_tbl, "\n ");
	}
//#endif
	/* get r,g,b table */
	for(i = 0; i < block_num_height; i++) {
		height_com[i] = height_div[i];
		fprintf(lgl_test_tbl, "height_com[%d] = %d \n ",i,height_com[i]); //for debug
	}
	for(i = 0; i < block_num_width; i++) {
		width_com[i] = width_div[i];
		fprintf(lgl_test_tbl, "width_com[%d] = %d \n ",i,width_com[i]); //for debug
	}
	height_sum[0] = 0;
	width_sum[0] = 0;
	for(i = 0; i < block_num_height; i++) {
		height_sum[i+1] = height_sum[i] + height_com[i];
		fprintf(lgl_test_tbl, "height_sum[%d] = %d \n ",i,height_sum[i]); //for debug
	}
	for(i = 0; i < block_num_width; i++) {
		width_sum[i+1] = width_sum[i] + width_com[i];
		fprintf(lgl_test_tbl, "width_sum[%d] = %d \n ",i,width_sum[i]); //for debug
	}
	for(i = 0; i < block_num_height; i++) {
		for(j = 0; j < block_num_width; j++) {
			block_height = height_sum[i+1] - height_sum[i];
			block_width = width_sum[j+1] - width_sum[j];
			cell_size = block_height * block_width;
			rstat = 0;
			gstat = 0;
			bstat = 0;
// 			if(block_height%2 != 0 || block_width%2 != 0) {
// 				DebugMsg("block_height = %d, block_width = %d", block_height, block_width);
// 				ret = -1;
// 				goto _error;
// 			}
			for(k = 0; k < block_height; k++) {
				for(l = 0; l < block_width; l++) {
					rstat += rin[((height_sum[i]+k)*bmpInfo.width) + (width_sum[j]+l)];
					gstat += gin[((height_sum[i]+k)*bmpInfo.width) + (width_sum[j]+l)];
					bstat += bin[((height_sum[i]+k)*bmpInfo.width) + (width_sum[j]+l)];
				}
			}
			if(cell_size){
				rtable[i*block_num_width + j] = rstat / cell_size;
				gtable[i*block_num_width + j] = gstat / cell_size;
				btable[i*block_num_width + j] = bstat / cell_size;
			}else{
				rtable[i*block_num_width + j] = 0;
				gtable[i*block_num_width+ j] = 0;
				btable[i*block_num_width + j] = 0;
			}
		}
	}

	/* cal msc_blw_dlt_lut, msc_blh_dlt_lut 
	if(fix_type >= 4) {
		mscParams->msc_blw_dlt_lut[0] = min(max(4096/(width_div[0] + 1), 0), 4095);
		mscParams->msc_blw_dlt_lut[11] = min(max(4096/(width_div[10] + 1), 0), 4095);
		for(i = 1; i < 11; i++) {
			mscParams->msc_blw_dlt_lut[i] = min(max(8192/(width_div[i] + width_div[i-1] + 1), 0), 4095);
		}
		mscParams->msc_blh_dlt_lut[0] = min(max(4096/(height_div[0] + 1), 0), 4095);
		mscParams->msc_blh_dlt_lut[11] = min(max(4096/(height_div[10] + 1), 0), 4095);
		for(i = 1; i < 11; i++) {
			mscParams->msc_blh_dlt_lut[i] = min(max(8192/(height_div[i] + height_div[i-1] + 1), 0), 4095);
		}
	} else {
		mscParams->msc_blw_dlt_lut[0] = min(max(4096/(width_div[0] + 1), 0), 4095);
		mscParams->msc_blw_dlt_lut[8] = min(max(4096/(width_div[7] + 1), 0), 4095);
		for(i = 1; i < 8; i++) {
			mscParams->msc_blw_dlt_lut[i] = min(max(8192/(width_div[i] + width_div[i-1] + 1), 0), 4095);
		}
		mscParams->msc_blh_dlt_lut[0] = min(max(4096/(height_div[0] + 1), 0), 4095);
		mscParams->msc_blh_dlt_lut[8] = min(max(4096/(height_div[7] + 1), 0), 4095);
		for(i = 1; i < 8; i++) {
			mscParams->msc_blh_dlt_lut[i] = min(max(8192/(height_div[i] + height_div[i-1] + 1), 0), 4095);
		}
	}
	*/

	//for debug
// 	for(int i = 0; i < 12; i++)
// 	{
// 		DebugMsg("mscParams->msc_blw_dlt_lut[%d] = %d", i, mscParams->msc_blw_dlt_lut[i]);
// 	}
// 	for(int i = 0; i < 12; i++)
// 	{
// 		DebugMsg("mscParams->msc_blh_dlt_lut[%d] = %d", i, mscParams->msc_blh_dlt_lut[i]);
// 	}

	/* fix process 
	if(MSC_Fixed_Process(&rtable[0], fix_bit, max_gain, offset, block_num_height, block_num_width, &rov) == -1) {
		DebugMsg("MSC_Fixed_Process, rtable Process failed!!!");
		ret = -1;
		goto _error;
	}
	if(MSC_Fixed_Process(&gtable[0], fix_bit, max_gain, offset, block_num_height, block_num_width, &gov) == -1) {
		DebugMsg("MSC_Fixed_Process, gtable Process failed!!!");
		ret = -1;
		goto _error;
	}
	if(MSC_Fixed_Process(&btable[0], fix_bit, max_gain, offset, block_num_height, block_num_width, &bov) == -1) {
		DebugMsg("MSC_Fixed_Process, btable Process failed!!!");
		ret = -1;
		goto _error;
	}*/
	if(scope) {
		//only color shading
		for(i = 0; i < block_num_height * block_num_width; i++) {
			rtable[i] = (((1 << fix_bit) * rtable[i]) + (gtable[i] / 2)) / gtable[i];
			btable[i] = (((1 << fix_bit) * btable[i]) + (gtable[i] / 2)) / gtable[i];
			gtable[i] = 1 << fix_bit;
		}
	}
	//DebugMsg("func = %s, line = %d", __FUNCTION__, __LINE__);
	/* cal Avg_Lum */

	//SRGBFloat centerRGB = { 0.0, 0.0, 0.0 };
	//SRegion centerPos;
	FILE *compensation_file = NULL;
	sprintf(szFilePath, "%s\\tmp\\msc\\compensation.txt", szPath);
	fopen_s(&compensation_file, szFilePath, "wb");
#if 0
	int avg_lum = 0, max_lum = 0,max_x = 0, max_y = 0, comp_offset = 0, offset_x, offset_y, pos_offset;
	for(i = 0; i < block_num; i++) {
		for(j = 0; j < block_num; j++) {
			centerPos.left = width_sum[j];
			centerPos.right = width_sum[j+1];
			centerPos.top = height_sum[i];
			centerPos.bottom = height_sum[i+1];
			CalculateAvgRGB(&bmpInfo, &centerPos, &centerRGB);
			avg_lum = 0.299*centerRGB.r + 0.587*centerRGB.g + 0.114*centerRGB.b;
			if(avg_lum > max_lum) {
				max_lum = avg_lum;
				max_x = j;
				max_y = i;
			}
			fprintf(compensation_file, "%d, ", avg_lum);
		}
		fprintf(compensation_file, "\n");
	}

	//DebugMsg("func = %s, line = %d", __FUNCTION__, __LINE__);
	//compensation = min(max(compensation, 0), 100);
	//for(i = 0; i < block_num; i++) {
	//	comp_tbl[i] = compensation + (100 - compensation) * (i - 0) / (7 - 0);
	//}
	if(max_x <= 7)
		offset_x = 7 - max_x;
	else
		offset_x = max_x - 8;
	if(max_y <= 7)
		offset_y = 7 - max_y;
	else
		offset_y = max_y - 8;
	comp_offset = max(offset_x, offset_y);
	for(i = 0; i < block_num; i++) {
		for(j = 0; j < block_num; j++) {
			if(j <= 7)
				offset_x = 7 - j;
			else
				offset_x = j - 8;
			if(i <= 7)
				offset_y = 7 - i;
			else
				offset_y = i - 8;
			pos_offset = max(offset_x, offset_y);
			if(pos_offset <= comp_offset) {
				comp_tbl[i * block_num + j] = 100;
			} else {
				comp_tbl[i * block_num + j] = compensation + (100 - compensation) * (pos_offset - 7) / (comp_offset - 7);
			}
		}
	}
#endif
	comp_a = ((double)compensation - 100.0) / comp_ref[0];
	for(i = 0; i < block_num_height*block_num_width; i++) {
		if(i%block_num_width == 0) {
			fprintf(compensation_file, "\n");
		}
		comp_ratio = comp_a * comp_ref[i] + 100.0;
		rtable[i] = (long double)rtable[i] * comp_ratio / 100.0;
		gtable[i] = (long double)gtable[i] * comp_ratio / 100.0;
		btable[i] = (long double)btable[i] * comp_ratio / 100.0;
		fprintf(compensation_file, "%.2f, ", comp_ratio);
	}
	fclose(compensation_file);
	/* Limit */
	for(i = 0; i < block_num_height*block_num_width; i++) {
		if(rtable[i] > max_val) {
			rtable[i] = max_val;
			rov = 1;
		}
		if(gtable[i] > max_val) {
			gtable[i] = max_val;
			gov = 1;
		}
		if(btable[i] > max_val) {
			btable[i] = max_val;
			bov = 1;
		}
	}
	ovflag = rov + gov + bov;
	if(ovflag) {
		//over flow
	}
	/* write block_div table */
	for(i = 0; i < block_num_height; i++) {
		fprintf(height_div_file, "%d, ", height_div[i]);
	}
	for(i = 0; i < block_num_width; i++) {
		fprintf(width_div_file, "%d, ", width_div[i]);
	}
	/* write r,g,b table */
	for(i = 0; i < block_num_height*block_num_width; i++) {
		if(i%block_num_width == 0) {
			fprintf(rtable_file, "\n");
			fprintf(gtable_file, "\n");
			fprintf(btable_file, "\n");
		}
		fprintf(rtable_file, "%d, ", rtable[i]);
		fprintf(gtable_file, "%d, ", gtable[i]);
		fprintf(btable_file, "%d, ", btable[i]);
	}

	for(int i = 0; i < block_num_height; i++)
	{
		mscParams->msc_blh_lut[i] = height_div[i];
	}
	for(int i = 0; i < block_num_width; i++)
	{
		mscParams->msc_blw_lut[i] = width_div[i];
	}
	
	//save to msctable
 	int msc_num_width = 32;
 	int msc_num_height = 16;
	memset(mscTable, 0, 3 * msc_num_width * msc_num_height * sizeof(int));
	int m = 0, n = 0;
	for(int i = 0; i < msc_num_height; i++)//r
	{
		for(int j = 0; j < msc_num_width; j++)
		{
			mscTable[m++] = rtable[n++];
		}
	}

	n = 0;
	m = msc_num_width * msc_num_height;
	for(int i = 0; i < msc_num_height; i++)//g
	{
		for(int j = 0; j < msc_num_width; j++)
		{
			mscTable[m++] = gtable[n++];
		}
	}

	n = 0;
	m = 2 * msc_num_width * msc_num_height;
	for(int i = 0; i < msc_num_height; i++)//g
	{
		for(int j = 0; j < msc_num_width; j++)
		{
			mscTable[m++] = btable[n++];
		}
	}
// 	memcpy(mscTable, rtable, block_num * block_num * sizeof(int));
// 	memcpy(&mscTable[msc_num * msc_num], gtable, block_num * block_num * sizeof(int));
// 	memcpy(&mscTable[msc_num * msc_num * 2], btable, block_num * block_num * sizeof(int));
// 	for(int i = 0; i < 3 * msc_num * msc_num)
// 	{
// 		DebugMsg("mscTable[%d] = %d", i, mscTable[i]);
// 	}
	//DebugMsg("func = %s, line = %d", __FUNCTION__, __LINE__);

	//cut
	//int width_left = 3264, height_left = 3120, width_right = 1008, height_right = 3120;
	//left
	/*
	int *rtable_left = new int[height_left * width_left];
	int *gtable_left = new int[height_left * width_left];
	int *btable_left = new int[height_left * width_left];
	for(i = 0; i < height_left; i++) {
		for(j = 0; j < width_left; j++){
			rtable_left[i*width_left+j] = rtable[i*bmpInfo.width+j];
			gtable_left[i*width_left+j] = gtable[i*bmpInfo.width+j];
			btable_left[i*width_left+j] = btable[i*bmpInfo.width+j];
		}
	}
	//right
	int *rtable_right = new int[height_right * width_right];
	int *gtable_right = new int[height_right * width_right];
	int *btable_right = new int[height_right * width_right];
	for(i = 0; i < height_right; i++) {
		for(j = 0; j < width_right; j++){
			rtable_right[i*width_right+j] = rtable[i*bmpInfo.width+(j+width_left-64)];
			gtable_right[i*width_right+j] = gtable[i*bmpInfo.width+(j+width_left-64)];
			btable_right[i*width_right+j] = btable[i*bmpInfo.width+(j+width_left-64)];
		}
	}
	*/
	/* write r,g,b left&right table 
	fprintf(rgbtable_left_file, "rtable_%d_%d_left : \n", vcm, temp);
	fprintf(rgbtable_right_file, "rtable_%d_%d_right : \n", vcm, temp);
	for(i = 0; i < block_num_height*block_num_width; i++) {
		if(i%block_num_width == 0) {
			fprintf(rgbtable_left_file, "\n");
			fprintf(rgbtable_right_file, "\n");
		}
		fprintf(rgbtable_left_file, "%d, ", rtable_left[i]);
		fprintf(rgbtable_right_file, "%d, ", rtable_right[i]);
	}
	
	fprintf(rgbtable_left_file, "gtable_%d_%d_left : \n", vcm, temp);
	fprintf(rgbtable_right_file, "gtable_%d_%d_right : \n", vcm, temp);
	for(i = 0; i < block_num_height*block_num_width; i++) {
		if(i%block_num_width == 0) {
			fprintf(rgbtable_left_file, "\n");
			fprintf(rgbtable_right_file, "\n");
		}
		fprintf(rgbtable_left_file, "%d, ", gtable_left[i]);
		fprintf(rgbtable_right_file, "%d, ", gtable_right[i]);
	}
	
	fprintf(rgbtable_left_file, "btable_%d_%d_left : \n", vcm, temp);
	fprintf(rgbtable_right_file, "btable_%d_%d_right : \n", vcm, temp);
	for(i = 0; i < block_num_height*block_num_width; i++) {
		if(i%block_num_width == 0) {
			fprintf(rgbtable_left_file, "\n");
			fprintf(rgbtable_right_file, "\n");
		}
		fprintf(rgbtable_left_file, "%d, ", btable_left[i]);
		fprintf(rgbtable_right_file, "%d, ", btable_right[i]);
	}
	*/
_error:
	SAFE_DELETE_ARRAY(rtable);
	SAFE_DELETE_ARRAY(gtable);
	SAFE_DELETE_ARRAY(btable);
	/*SAFE_DELETE_ARRAY(rtable_left);
	SAFE_DELETE_ARRAY(gtable_left);
	SAFE_DELETE_ARRAY(btable_left);
	SAFE_DELETE_ARRAY(rtable_right);
	SAFE_DELETE_ARRAY(gtable_right);
	SAFE_DELETE_ARRAY(btable_right);*/
	SAFE_DELETE_ARRAY(height_div);
	SAFE_DELETE_ARRAY(width_div);
	SAFE_DELETE_ARRAY(height_com);
	SAFE_DELETE_ARRAY(width_com);
	SAFE_DELETE_ARRAY(height_sum);
	SAFE_DELETE_ARRAY(width_sum);
	//SAFE_DELETE_ARRAY(rawin);
	SAFE_DELETE_ARRAY(rin);
	SAFE_DELETE_ARRAY(gin);
	SAFE_DELETE_ARRAY(bin);
	//SAFE_DELETE_ARRAY(comp_tbl);
	/* close MSC table */
	if(bayer_tbl)
		fclose(bayer_tbl);
	if(height_div_file)
		fclose(height_div_file);
	if(width_div_file)
		fclose(width_div_file);
	if(rtable_file)
		fclose(rtable_file);
	if(gtable_file)
		fclose(gtable_file);
	if(btable_file)
		fclose(btable_file);
	if(rgbtable_left_file)
		fclose(rgbtable_left_file);
	if(rgbtable_right_file)
		fclose(rgbtable_right_file);
	if(lgl_test_tbl)
		fclose(lgl_test_tbl);
	bayer_tbl = NULL; 
	height_div_file = NULL;
	width_div_file = NULL;
	rtable_file = NULL;
	gtable_file = NULL;
	btable_file = NULL;
	rgbtable_left_file = NULL;
	rgbtable_right_file = NULL;
	return ret;
}
//&&&&&&&&&&&&&&&&
int CalculateDefault4WB(IN const void *data, IN const pSImageFormat format, OUT int *wbRet)
{
	if (!data || !format || !wbRet)
	{
		return -1;
	}


	wbRet[0] = 256;
	wbRet[1] = 256;
	wbRet[2] = 256;

	return 0;
}

int CalculateDefault4CCM(IN const void *data, IN const pSRegion blocks, IN const int *weight,
				 IN const double *colorTableRef, IN const int saturation, IN const int brightness, 
				 OUT int *ccmRet)
{
	if (!data || !blocks || !weight || !colorTableRef || !ccmRet)
	{
		return -1;
	}

	SBmpImageInfo bmpInfo;
	if (!GetBmpImageInfo((const unsigned char *)data, &bmpInfo))
	{
		return -1;
	}

	SRGBFloat blockRgb[24];
	int i = 0, j = 0;
	double RGain = 0.0, BGain = 0.0, TGain = 0.0;  // for wb correction

	// calculate average of 24 blocks
	for (i = 0; i < 24; i++)
	{
		CalculateAvgRGB(&bmpInfo, &blocks[i], &blockRgb[i]);
	}

	j = 0;
	for (i = 19; i < 23; i++)
	{
		if (!IS_DOUBLE_ZERO(blockRgb[i].g))
		{
			RGain += blockRgb[i].r / blockRgb[i].g;
			BGain += blockRgb[i].b / blockRgb[i].g;
			j++;
		}
	}

	if (j > 0)
	{
		RGain /= j;
		BGain /= j;
	}
	else
	{
		RGain = BGain = 1.0;
	}

	// wb correction
	for (i = 0; i < 24; i++)
	{
		blockRgb[i].r /= RGain;
		blockRgb[i].b /= BGain;
	}

	if(brightness == 0)
	{
		TGain = (colorTableRef[19 * 3 + 1] + colorTableRef[20 * 3 + 1] + colorTableRef[21 * 3 + 1] + colorTableRef[22 * 3 + 1])
			/ (blockRgb[19].g + blockRgb[20].g + blockRgb[21].g + blockRgb[22].g);
		for (i = 0; i < 24; i++)
		{
			blockRgb[i].r *= TGain;
			blockRgb[i].g *= TGain;
			blockRgb[i].b *= TGain;
			DebugMsg("blockRgb[i].r = %lf, blockRgb[i].g = %lf, blockRgb[i].b = %lf\n", 
				blockRgb[i].r, blockRgb[i].g, blockRgb[i].b);
		}
	}
	else if (brightness == 1)
	{
		for (i = 0; i < 24; i++)
		{
			DebugMsg("A : blockRgb[%d].r = %lf, blockRgb[%d].g = %lf, blockRgb[%d].b = %lf\n", 
				i, blockRgb[i].r, i, blockRgb[i].g, i, blockRgb[i].b);
			blockRgb[i].r = pow(blockRgb[i].r/ 255.0, 2.0) * 255;
			blockRgb[i].g = pow(blockRgb[i].g/ 255.0, 2.0) * 255;
			blockRgb[i].b = pow(blockRgb[i].b/ 255.0, 2.0) * 255;
		}
		for (i = 0; i < 24; i++)
		{
			DebugMsg("B : colorTableRef[%d].r = %lf, colorTableRef[%d].g = %lf, colorTableRef[%d].b = %lf\n", 
				i, colorTableRef[i * 3 + 0], i, colorTableRef[i * 3 + 1], i, colorTableRef[i * 3 + 2]);
			TGain = (pow((colorTableRef[i * 3 + 0] * 0.299 + colorTableRef[i * 3 + 1] * 0.587 + colorTableRef[i * 3 + 2] * 0.114) / 16.0 / 255.0, 2.0) * 255)
				/ (blockRgb[i].r * 0.299 + blockRgb[i].g * 0.587 + blockRgb[i].b * 0.114);
			DebugMsg("C : blockRgb[%d].r = %lf, blockRgb[%d].g = %lf, blockRgb[%d].b = %lf, TGain = %lf\n", 
				i, blockRgb[i].r, i, blockRgb[i].g, i, blockRgb[i].b, TGain);
			blockRgb[i].r = pow(blockRgb[i].r * TGain / 255.0, 0.5) * 255;
			blockRgb[i].g = pow(blockRgb[i].g * TGain / 255.0, 0.5) * 255;
			blockRgb[i].b = pow(blockRgb[i].b * TGain / 255.0, 0.5) * 255;
			DebugMsg("D : blockRgb[%d].r = %lf, blockRgb[%d].g = %lf, blockRgb[%d].b = %lf\n", 
				i, blockRgb[i].r, i, blockRgb[i].g, i, blockRgb[i].b);
		}
	}
	// fix r/g/b
	/*RGain = (colorTableRef[19 * 3 + 1] + colorTableRef[20 * 3 + 1] + colorTableRef[21 * 3 + 1] + colorTableRef[22 * 3 + 1])
		/ (blockRgb[19].g + blockRgb[20].g + blockRgb[21].g + blockRgb[22].g);
    for (i = 0; i < 24; i++)
	{
		blockRgb[i].r *= RGain;
		blockRgb[i].g *= RGain;
		blockRgb[i].b *= RGain;
	}*/

	/*
	 * Calculation color correction matrix
	 *             a1  a2  a3
	 * where CCM = a4  a5  a6.
	 *             a7  a8  a9
     * As result, 
	 *      CCM*[r,g,b]' = [R,G,B]'                                                          (1)
	 * where [r,g,b]' is the input and [R,G,B]' is the output
	 *      Target(r,g,b) = Sum{ki*[(Ri-R'i)^2 + (Gi-G'i)^2 + (Bi-B'i)^2]}, i=1,2,3,...,N    (2)
	 * where Ri,Gi,Bi is the output and R'i,G'i,B'i is the ideal color value, ki is weight.
	 * The smaller the Target(r,g,b) is, the better.
	 * From (1) and (2), we get
	 *      Target(r,g,b) = Sum[ki*(a1*ri+a2*gi+a3*bi-R'i)^2 + ki*(a4*ri+a5*gi+a6*bi-G'i)^2 
	 *                        + ki*(a7*ri+a8*gi+a9*bi-B'i)^2], i=1,2,3,...,N                 (3)
	 * Take a derivative of a1,a2,a3,...,a9, then we get 
     *	    Sum[2*ki*ri*(a1*ri+a2*gi+a3*bi-R'i)] = 0
	 *	    Sum[2*ki*gi*(a1*ri+a2*gi+a3*bi-R'i)] = 0
	 *	    Sum[2*ki*bi*(a1*ri+a2*gi+a3*bi-R'i)] = 0
	 *	    Sum[2*ki*ri*(a4*ri+a5*gi+a6*bi-G'i)] = 0
	 *	    Sum[2*ki*gi*(a4*ri+a5*gi+a6*bi-G'i)] = 0
	 *	    Sum[2*ki*bi*(a4*ri+a5*gi+a6*bi-G'i)] = 0
	 *	    Sum[2*ki*ri*(a7*ri+a8*gi+a9*bi-B'i)] = 0
	 *	    Sum[2*ki*gi*(a7*ri+a8*gi+a9*bi-B'i)] = 0
	 *	    Sum[2*ki*bi*(a7*ri+a8*gi+a9*bi-B'i)] = 0, i=1,2,3,...,N
	 * Simplify it, we get
	 *          a1 a4 a7
	 *      A * a2 a5 a8 = B                     (4)
	 *          a3 a6 a9
	 * where 
	 *          Sum(ki*ri*ri) Sum(ki*ri*gi) Sum(ki*ri*bi)  
	 *      A = Sum(ki*gi*ri) Sum(ki*gi*gi) Sum(ki*gi*bi), 
	 *          Sum(ki*bi*ri) Sum(ki*bi*gi) Sum(ki*bi*bi) 
     *	 
	 *          Sum(ki*ri*R'i) Sum(ki*ri*G'i) Sum(ki*ri*B'i)
     *      B = Sum(ki*gi*R'i) Sum(ki*gi*G'i) Sum(ki*gi*B'i), i=1,2,3,...,N
     *          Sum(ki*bi*R'i) Sum(ki*bi*G'i) Sum(ki*bi*B'i)
	 * Finally, we get 
	 *       a1 a4 a7
	 *       a2 a5 a8 = A' * B                    (5)
	 *       a3 a6 a9
	 * where A' is the inverse of A.
	 *
	 * Parameters introduction as follows,
	 *      t2 = A in the former and t2 = A' in latter,
	 *      t1 = B,
	 * coff is the result matrix without saturation correction, some like CCM
	 *     coff[0]  coff[3]  coff[6]     a1 a2 a3
	 *     coff[1]  coff[4]  coff[7]  =  a4 a5 a6  = CCM
	 *     coff[2]  coff[5]  coff[8]     a7 a8 a9
	 *
	 */
	double t1[9], t2[9], temp[9], coff[9];
	memset(t1, 0, 9*sizeof(double));
	memset(t2, 0, 9*sizeof(double));
	memset(temp, 0, 9*sizeof(double));
	memset(coff, 0, 9*sizeof(double));

	for (i = 0; i < 72-18; i += 3) // 0~17 block
	{ 
		t1[0] += blockRgb[i/3].r * colorTableRef[i]   * weight[i/3];
		t1[1] += blockRgb[i/3].r * colorTableRef[i+1] * weight[i/3];
		t1[2] += blockRgb[i/3].r * colorTableRef[i+2] * weight[i/3];
		t1[3] += blockRgb[i/3].g * colorTableRef[i]   * weight[i/3];
		t1[4] += blockRgb[i/3].g * colorTableRef[i+1] * weight[i/3];
		t1[5] += blockRgb[i/3].g * colorTableRef[i+2] * weight[i/3];
		t1[6] += blockRgb[i/3].b * colorTableRef[i]   * weight[i/3];
		t1[7] += blockRgb[i/3].b * colorTableRef[i+1] * weight[i/3];
		t1[8] += blockRgb[i/3].b * colorTableRef[i+2] * weight[i/3];

		t2[0] += blockRgb[i/3].r * blockRgb[i/3].r * weight[i/3];
		t2[1] += blockRgb[i/3].r * blockRgb[i/3].g * weight[i/3];
		t2[2] += blockRgb[i/3].r * blockRgb[i/3].b * weight[i/3];
		t2[3] += blockRgb[i/3].g * blockRgb[i/3].r * weight[i/3];
		t2[4] += blockRgb[i/3].g * blockRgb[i/3].g * weight[i/3];
		t2[5] += blockRgb[i/3].g * blockRgb[i/3].b * weight[i/3];
		t2[6] += blockRgb[i/3].b * blockRgb[i/3].r * weight[i/3];
		t2[7] += blockRgb[i/3].b * blockRgb[i/3].g * weight[i/3];
		t2[8] += blockRgb[i/3].b * blockRgb[i/3].b * weight[i/3];
	}
#if 1
	TRACE(_T("Matrix A:\n"));
	for (i = 0; i < 18; i++) {
		TRACE(_T("    %6.2f, %6.2f, %6.2f,\n"),
			blockRgb[i].r, blockRgb[i].g, blockRgb[i].b);
	}
	TRACE(_T("Matrix B:\n"));
	for (i = 0; i < 18; i++) {
		TRACE(_T("    %6.2f, %6.2f, %6.2f,\n"),
			colorTableRef[i*3], colorTableRef[i*3+1], colorTableRef[i*3+2]);
	}
#endif
#if 1
	TRACE(_T("Matrix A:\n"
		"    %6.2f, %6.2f, %6.2f\n"
		"    %6.2f, %6.2f, %6.2f\n"
		"    %6.2f, %6.2f, %6.2f\n"),
		t2[0], t2[1], t2[2],
		t2[3], t2[4], t2[5],
		t2[6], t2[7], t2[8]
	);
	TRACE(_T("Matrix B:\n"
		"    %6.2f, %6.2f, %6.2f\n"
		"    %6.2f, %6.2f, %6.2f\n"
		"    %6.2f, %6.2f, %6.2f\n"),
		t1[0], t1[1], t1[2],
		t1[3], t1[4], t1[5],
		t1[6], t1[7], t1[8]
	);
#endif

	double div = 1.0 / (t2[0]*t2[4]*t2[8]+t2[1]*t2[5]*t2[6]+t2[2]*t2[3]*t2[7]-t2[2]*t2[4]*t2[6]-t2[1]*t2[3]*t2[8]-t2[0]*t2[5]*t2[7]);
	temp[0] = t2[4] * t2[8] - t2[5] * t2[7];
	temp[1] = t2[3] * t2[8] - t2[5] * t2[6];
	temp[2] = t2[3] * t2[7] - t2[4] * t2[6];
	temp[3] = t2[1] * t2[8] - t2[2] * t2[7];
	temp[4] = t2[0] * t2[8] - t2[2] * t2[6];
	temp[5] = t2[0] * t2[7] - t2[1] * t2[6];
	temp[6] = t2[1] * t2[5] - t2[2] * t2[4];
	temp[7] = t2[0] * t2[5] - t2[2] * t2[3];
	temp[8] = t2[0] * t2[4] - t2[1] * t2[3];

	// A' = t2
	t2[0] =  div * temp[0];
	t2[1] = -div * temp[3];
	t2[2] =  div * temp[6];
	t2[3] = -div * temp[1];
	t2[4] =  div * temp[4];
	t2[5] = -div * temp[7];
	t2[6] =  div * temp[2];
	t2[7] = -div * temp[5];
	t2[8] =  div * temp[8];

	/*
	 *   coff   =   t2    *   t1
	 *   0 1 2     0 1 2     0 1 2
	 *   3 4 5  =  3 4 5  *  3 4 5
	 *   6 7 8     6 7 8     6 7 8
	 */
	for (i = 0; i < 3; i++)
	{
		coff[0] += t2[i] * t1[i*3];
		coff[1] += t2[i] * t1[i*3+1];
		coff[2] += t2[i] * t1[i*3+2];
		coff[3] += t2[i+3] * t1[i*3];
		coff[4] += t2[i+3] * t1[i*3+1];
		coff[5] += t2[i+3] * t1[i*3+2];
		coff[6] += t2[i+6] * t1[i*3];
		coff[7] += t2[i+6] * t1[i*3+1];
		coff[8] += t2[i+6] * t1[i*3+2];
	}
#if 1
	TRACE(_T("Matrix X:\n"
		"    %6.2f, %6.2f, %6.2f\n"
		"    %6.2f, %6.2f, %6.2f\n"
		"    %6.2f, %6.2f, %6.2f\n"),
		coff[0], coff[1], coff[2],
		coff[3], coff[4], coff[5],
		coff[6], coff[7], coff[8]
	);
#endif

	// row normalize to 1
	double col1_sum,col2_sum,col3_sum;
	col1_sum = coff[0] + coff[3] + coff[6];
	col2_sum = coff[1] + coff[4] + coff[7];
	col3_sum = coff[2] + coff[5] + coff[8];
	coff[0] = coff[0] / col1_sum;
	coff[3] = coff[3] / col1_sum;
	coff[6] = coff[6] / col1_sum;
	coff[1] = coff[1] / col2_sum;
	coff[4] = coff[4] / col2_sum;
	coff[7] = coff[7] / col2_sum;
	coff[2] = coff[2] / col3_sum;
	coff[5] = coff[5] / col3_sum;
	coff[8] = coff[8] / col3_sum;

	int ma_best[9];
	if(brightness == 0)
	{
	ma_best[0] = int(coff[0] * 256);
	ma_best[1] = int(coff[3] * 256);
	ma_best[2] = int(coff[6] * 256);
	ma_best[3] = int(coff[1] * 256);
	ma_best[4] = int(coff[4] * 256);
	ma_best[5] = int(coff[7] * 256);
	ma_best[6] = int(coff[2] * 256);
	ma_best[7] = int(coff[5] * 256);
	ma_best[8] = int(coff[8] * 256);
	}
	else if (brightness == 1)
	{
		//double sa = 5.0/40.0;
		double sa = saturation/40.0;

		if (sa < 0)
			sa = 6*sa;

		double slop0 = 1/(1-sa);
		double slop1 = sa/(3*(1-sa));
		DebugMsg("\nsa = %lf, slop0 = %lf, slop1 = %lf\n", 
			sa, slop0, slop1);
		double sa_coff[9];
		for (i=0;i<9;i++)
		{
			if (i==0 || i==4 || i==8)
				sa_coff[i] = slop0-slop1;
			else
				sa_coff[i] = -slop1;

			DebugMsg("sa_coff[%d] = %lf\n", 
				i, sa_coff[i]);
		}

		double ma_coff[9];
		memset(ma_coff, 0, 9*sizeof(double));

		for (i=0;i<3;i++)
		{
			ma_coff[0] += sa_coff[i]*coff[i];
			ma_coff[1] += sa_coff[i]*coff[i+3];
			ma_coff[2] += sa_coff[i]*coff[i+6];
			ma_coff[3] += sa_coff[i+3]*coff[i];
			ma_coff[4] += sa_coff[i+3]*coff[i+3];
			ma_coff[5] += sa_coff[i+3]*coff[i+6];
			ma_coff[6] += sa_coff[i+6]*coff[i];
			ma_coff[7] += sa_coff[i+6]*coff[i+3];
			ma_coff[8] += sa_coff[i+6]*coff[i+6];
		}

		for (i=0;i<9;i++)
		{  
			ma_best[i] = ma_coff[i]*256;
		}
	}


	// check validation
    if (ma_best[1] + ma_best[2] > 0)
	{
		ma_best[1] = ma_best[2] = 0;
	}
    else if (ma_best[1] > 0)
    {
		ma_best[2] += ma_best[1];
		ma_best[1] = 0;        
    }
    else if (ma_best[2] > 0)
    {
        ma_best[1] += ma_best[2];
        ma_best[2] = 0;
    }
	ma_best[0] = 256 - ma_best[1] - ma_best[2];
    
	if (ma_best[3] + ma_best[5] > 0)
	{
		ma_best[3] = ma_best[5] = 0;
	}
    else if (ma_best[3] > 0)
    {
        ma_best[5] += ma_best[3];
        ma_best[3] = 0;
    }
    else if (ma_best[5] > 0)
    {
        ma_best[3] += ma_best[5];
        ma_best[5] = 0;
    }
	ma_best[4] = 256 - ma_best[3] - ma_best[5];
    
	if (ma_best[6] + ma_best[7] > 0)
	{
		ma_best[6] = ma_best[7] = 0;
	}
    else if (ma_best[6] > 0)
    {
        ma_best[7] += ma_best[6];
        ma_best[6] = 0;
    }
    else if (ma_best[7] > 0)
    {
        ma_best[6] += ma_best[7];
        ma_best[7] = 0;
    }
	ma_best[8] = 256 - ma_best[6] - ma_best[7];

	memcpy(ccmRet, ma_best, sizeof(ma_best));  // ret

	return 0;
}

int CalculateDefault4IqTest(IN const void *data, IN int testItem, IN const pSRegion blocks, OUT double *iqRet)
{
	if (!data || !iqRet || testItem < 1 || testItem > 2)
	{
		return -1;
	}

	if (testItem == 2 && !blocks)
	{
		return -1;
	}

	SBmpImageInfo bmpInfo;
	if (!GetBmpImageInfo((const unsigned char *)data, &bmpInfo))
	{
		return -1;
	}

	if (testItem == 1)
	{
		return TestLensShading(&bmpInfo, iqRet) == TRUE ? 0 : -1;
	}
	else
	{
		SBlockParams params = { 1, blocks, 24 };
		return TestColorReproduction(&bmpInfo, &params, iqRet) == TRUE ? 0 : -1;
	}

	return -1;
}

int CalculateDefault4Denoise(IN const void *files, IN const pSImageFormat format, IN const pSRegion blocks,
							 OUT void *noiseRet)
{
#ifndef CALC_DENOISE_DISABLE
	return CalculateDenoiseRet((CSingleLinkedList<CString> *)files, format, blocks, (sNoiseRet *)noiseRet);
#else
	return 0;
#endif
}

NAMESPACE_END
