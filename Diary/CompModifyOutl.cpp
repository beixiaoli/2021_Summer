//ISO选取连续	
for (int i = 1; i < 13; i++) {
		if (ISOFlag & (1 << (i+1))) {
			for (int j = 0; j < 33; j++) {
				if((denoiseParams->curve_lp0[i*33+j] > denoiseParams->curve_lp0[(i-1)*33+j]) && (denoiseParams->curve_lp0[(i+1)*33+j] > denoiseParams->curve_lp0[i*33+j])) {
					denoiseParams->curve_lp0[i*33+j] = (float) (denoiseParams->curve_lp0[(i-1)*33+j] + denoiseParams->curve_lp0[(i+1)*33+j]) / 2;
				}
			}
		}
	}

//如果ISO选取不连续
for (int i = 0; i < 14; i++) {
	if (ISOFlag & (1 << i)) {
		ISOArray[k] = i;
		k++;
	}
}
	
for (int i = 1; i < (k-1); i++) {
	for (int j = 0; j < 33; j++) {
		if((denoiseParams->curve_lp0[ISOArray[i]*33+j] > denoiseParams->curve_lp0[ISOArray[i-1]*33+j]) && (denoiseParams->curve_lp0[ISOArray[i]*33+j] > denoiseParams->curve_lp0[ISOArray[i+1]*33+j])) {
			denoiseParams->curve_lp0[ISOArray[i]*33+j] = (float) (denoiseParams->curve_lp0[ISOArray[i-1]*33+j] + denoiseParams->curve_lp0[ISOArray[i+1]*33+j]) / 2;
		}
	}
}


