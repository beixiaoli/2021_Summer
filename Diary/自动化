//CalcDenoiseCurve
//Set BDNF Curve
	GroupId = HW_ISP_CFG_TUNING_TABLES;
	CfgIds = HW_ISP_CFG_TUNING_BDNF;
	for (int i = 0; i < 33; i++)
	{
		tuning_bdnf_table_cfg->thres[i] = MAX(curve_hw_lp0[i], 0);
		if(ISO_idx == 0) {
			denoiseParams->curve_lp0[ISO_idx*33+i] = MAX(curve_hw_lp0[i], 1);
		}
		else {
			denoiseParams->curve_lp0[ISO_idx*33+i] = MAX(curve_hw_lp0[i], 0);
		}
	}

BOOL CImageShowCtrlCalibration::CalcDenoiseMul(IN CWnd *owner, IN const int ISOFlag, IN sDenoiseParams *denoiseParams, IN const void *capFmt)
{
	CString errInfo(_T("Unknown error."));
	if (!owner) {
		return FALSE;
	}
	if (!denoiseParams || !capFmt) {
		errInfo.SetString(_T("Invalid params"));
		owner->SendMessage(WM_CALC_FUNC_DONE, (WPARAM)FALSE, (LPARAM)&errInfo);
		return FALSE;
	}
	int ret = 0, times = 0;
	HW_U8 GroupId;
	HW_U32 CfgIds;
	int block_cnt = 0;
	int cap_cnt = 50;
	CIsp522CommHandler *isp522Handler;
	CIspCommHandler *ispHandler;
	int ispSel = g_HawkviewMainFrame->GetHawkviewConfigs()->m_IspSel;
	capture_format cap_fmt = *((capture_format*)capFmt);
	isp522_capture_format cap_fmt_isp522 = *((isp522_capture_format*)capFmt);
	struct isp500_test_enable_cfg *test_enable_cfg_isp500 = NULL;
	struct isp520_test_enable_cfg *test_enable_cfg_isp520 = NULL;
	struct isp521_test_enable_cfg *test_enable_cfg_isp521 = NULL;
	struct isp522_test_enable_cfg *test_enable_cfg_isp522 = NULL;
	struct isp500_dynamic_denoise_cfg *dynamic_denoise_cfg_isp500 = NULL;
	struct isp520_dynamic_denoise_cfg *dynamic_denoise_cfg_isp520 = NULL;
	struct isp521_dynamic_denoise_cfg *dynamic_denoise_cfg_isp521 = NULL;
	struct isp522_dynamic_denoise_cfg *dynamic_denoise_cfg_isp522 = NULL;
	struct isp_tuning_bdnf_table_cfg *tuning_bdnf_table_cfg = NULL;


	float *curve_rate = NULL;
	float *black_gain_mul = NULL;
	float *white_gain_mul = NULL;
	float black_curve_rate = 0;
	float white_curve_rate = 0;

	tuning_bdnf_table_cfg = (isp_tuning_bdnf_table_cfg *)malloc(sizeof(struct isp_tuning_bdnf_table_cfg));
	switch(g_IspVersion)
	{
		case 500:
			ispHandler = g_HawkviewMainFrame->GetIspCommHandler();
			cap_fmt.fps = g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputFps;
			cap_fmt.wdr = g_HawkviewMainFrame->GetHawkviewConfigs()->m_Wdrmode;
			cap_fmt.buffer = new unsigned char[g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputWidth * g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputHeight * 2];
			test_enable_cfg_isp500 = (isp500_test_enable_cfg *)malloc(sizeof(struct isp500_test_enable_cfg));
			dynamic_denoise_cfg_isp500 = (isp500_dynamic_denoise_cfg *)malloc(sizeof(struct isp500_dynamic_denoise_cfg));
			break;
		case 520:
			ispHandler = g_HawkviewMainFrame->GetIspCommHandler();
			cap_fmt.fps = g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputFps;
			cap_fmt.wdr = g_HawkviewMainFrame->GetHawkviewConfigs()->m_Wdrmode;
			cap_fmt.buffer = new unsigned char[g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputWidth * g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputHeight * 2];
			test_enable_cfg_isp520 = (isp520_test_enable_cfg *)malloc(sizeof(struct isp520_test_enable_cfg));
			dynamic_denoise_cfg_isp520 = (isp520_dynamic_denoise_cfg *)malloc(sizeof(struct isp520_dynamic_denoise_cfg));
			break;
		case 521:
			ispHandler = g_HawkviewMainFrame->GetIspCommHandler();
			cap_fmt.fps = g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputFps;
			cap_fmt.wdr = g_HawkviewMainFrame->GetHawkviewConfigs()->m_Wdrmode;
			cap_fmt.buffer = new unsigned char[g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputWidth * g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputHeight * 2];
			test_enable_cfg_isp521 = (isp521_test_enable_cfg *)malloc(sizeof(struct isp521_test_enable_cfg));
			dynamic_denoise_cfg_isp521 = (isp521_dynamic_denoise_cfg *)malloc(sizeof(struct isp521_dynamic_denoise_cfg));
			break;
		case 522:
			isp522Handler = g_HawkviewMainFrame->GetIsp522CommHandler();	
			cap_fmt_isp522.fps = g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputFps;
			cap_fmt_isp522.wdr = g_HawkviewMainFrame->GetHawkviewConfigs()->m_Wdrmode;
			cap_fmt_isp522.buffer = new unsigned char[g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputWidth * g_HawkviewMainFrame->GetHawkviewConfigs()->m_InputHeight * 2];
			test_enable_cfg_isp522 = (isp522_test_enable_cfg *)malloc(sizeof(struct isp522_test_enable_cfg));
			dynamic_denoise_cfg_isp522 = (isp522_dynamic_denoise_cfg *)malloc(sizeof(struct isp522_dynamic_denoise_cfg));
			break;
		default:
			errInfo.SetString(_T("Not support isp%d"), g_IspVersion);
			owner->SendMessage(WM_CALC_FUNC_DONE, (WPARAM)FALSE, (LPARAM)&errInfo);
			return FALSE;
			break;
	}

	
	curve_rate = new float[14*33];
	black_gain_mul = new float[14];
	white_gain_mul = new float[14];
	
	black_gain_mul[0] = 1;
	white_gain_mul[0] = 1;
	
	for (int i = 0; i < 14; i++) {
		if (ISOFlag & (1 << i)) {
			for (int j = 0; j < 33; j++) {
				curve_rate[i*33+j] = (float) denoiseParams->curve_lp0[i*33+j] / denoiseParams->curve_lp0[j]; 
			}
		}
	}

	for (int i = 1; i < 14; i++) {
		if (ISOFlag & (1 << i)) {
			black_curve_rate = 0;
			white_curve_rate = 0;
			for (int j = 0; j < 10; j++) {
				black_curve_rate += curve_rate[i*33+j];
			}
			black_gain_mul[i] = black_curve_rate / 10;
			for (int k = 23; k < 33; k++) {
				white_curve_rate += curve_rate[i*33+k]; 
			}
			white_gain_mul[i] = white_curve_rate / 10;
		}
	}

	for (int i = 0; i < 14; i++) {
		if (ISOFlag & (1 << i)) {
			//Get & Set denoise parameter
				GroupId = HW_ISP_CFG_DYNAMIC;
				CfgIds = HW_ISP_CFG_DYNAMIC_DENOISE;
				Sleep(100);
				switch(g_IspVersion)
				{
					case 500:
						ret = ispHandler->GetIspCfg(ispSel, GroupId, CfgIds, (void*)dynamic_denoise_cfg_isp500);
						if (ret < sizeof(struct isp500_dynamic_denoise_cfg)) {
							errInfo.Format(_T("Failed to get enable_cfg(%d/%d)", ret, sizeof(struct isp500_dynamic_denoise_cfg)));
							goto Calc_Denoise_FAILED;
						}
						isp500_convert_tuning_cfg_to_local(GroupId, CfgIds, (unsigned char *)dynamic_denoise_cfg_isp500);
						Sleep(60);
						
						isp500_convert_tuning_cfg_to_network(GroupId, CfgIds, (unsigned char *)dynamic_denoise_cfg_isp500);
						ret = ispHandler->SetIspCfg(ispSel, GroupId, CfgIds, (void*)dynamic_denoise_cfg_isp500, sizeof(struct isp500_dynamic_denoise_cfg));
						if (ret) {
							errInfo.Format(_T("Failed to set enable_cfg(0x01)"));
							goto Calc_Denoise_FAILED;
						}
						ret = ispHandler->UpdateIspCfg(ispSel);
						if (ret) {
							errInfo.Format(_T("Failed to set enable_cfg(0x02)"));
							goto Calc_Denoise_FAILED;
						}
						break;
					case 520:
						ret = ispHandler->GetIspCfg(ispSel, GroupId, CfgIds, (void*)dynamic_denoise_cfg_isp520);
						if (ret < sizeof(struct isp520_dynamic_denoise_cfg)) {
							errInfo.Format(_T("Failed to get enable_cfg(%d/%d)", ret, sizeof(struct isp520_dynamic_denoise_cfg)));
							goto Calc_Denoise_FAILED;
						}
						isp520_convert_tuning_cfg_to_local(GroupId, CfgIds, (unsigned char *)dynamic_denoise_cfg_isp520);
						Sleep(60);
						dynamic_denoise_cfg_isp520->tuning_cfg[i].value[ISP_DENOISE_BLACK_GAIN] = 256 * 1.65 * black_gain_mul[i];
						dynamic_denoise_cfg_isp520->tuning_cfg[i].value[ISP_DENOISE_BLACK_OFFSET] = 0;
						dynamic_denoise_cfg_isp520->tuning_cfg[i].value[ISP_DENOISE_WHITE_GAIN] = 256 * 1.65 * white_gain_mul[i];
						dynamic_denoise_cfg_isp520->tuning_cfg[i].value[ISP_DENOISE_WHITE_OFFSET] = 0;
						isp520_convert_tuning_cfg_to_network(GroupId, CfgIds, (unsigned char *)dynamic_denoise_cfg_isp520);
						ret = ispHandler->SetIspCfg(ispSel, GroupId, CfgIds, (void*)dynamic_denoise_cfg_isp520, sizeof(struct isp520_dynamic_denoise_cfg));
						if (ret) {
							errInfo.Format(_T("Failed to set enable_cfg(0x01)"));
							goto Calc_Denoise_FAILED;
						}
						ret = ispHandler->UpdateIspCfg(ispSel);
						if (ret) {
							errInfo.Format(_T("Failed to set enable_cfg(0x02)"));
							goto Calc_Denoise_FAILED;
						}
						break;
					case 521:
						ret = ispHandler->GetIspCfg(ispSel, GroupId, CfgIds, (void*)dynamic_denoise_cfg_isp521);
						if (ret < sizeof(struct isp521_dynamic_denoise_cfg)) {
							errInfo.Format(_T("Failed to get enable_cfg(%d/%d)", ret, sizeof(struct isp521_dynamic_denoise_cfg)));
							goto Calc_Denoise_FAILED;
						}
						isp521_convert_tuning_cfg_to_local(GroupId, CfgIds, (unsigned char *)dynamic_denoise_cfg_isp521);
						Sleep(60);
						dynamic_denoise_cfg_isp521->tuning_cfg[i].value[ISP_DENOISE_BLACK_GAIN] = 256 * 1.65 * black_gain_mul[i];
						dynamic_denoise_cfg_isp521->tuning_cfg[i].value[ISP_DENOISE_BLACK_OFFSET] = 0;
						dynamic_denoise_cfg_isp521->tuning_cfg[i].value[ISP_DENOISE_WHITE_GAIN] = 256 * 1.65 * white_gain_mul[i];
						dynamic_denoise_cfg_isp521->tuning_cfg[i].value[ISP_DENOISE_WHITE_OFFSET] = 0;
						isp521_convert_tuning_cfg_to_network(GroupId, CfgIds, (unsigned char *)dynamic_denoise_cfg_isp521);
						ret = ispHandler->SetIspCfg(ispSel, GroupId, CfgIds, (void*)dynamic_denoise_cfg_isp521, sizeof(struct isp521_dynamic_denoise_cfg));
						if (ret) {
							errInfo.Format(_T("Failed to set enable_cfg(0x01)"));
							goto Calc_Denoise_FAILED;
						}
						ret = ispHandler->UpdateIspCfg(ispSel);
						if (ret) {
							errInfo.Format(_T("Failed to set enable_cfg(0x02)"));
							goto Calc_Denoise_FAILED;
						}
						break;
					case 522:
						ret = isp522Handler->GetIspCfg(ispSel, GroupId, CfgIds, (void*)dynamic_denoise_cfg_isp522);
						if (ret < sizeof(struct isp522_dynamic_denoise_cfg)) {
							errInfo.Format(_T("Failed to get enable_cfg(%d/%d)", ret, sizeof(struct isp522_dynamic_denoise_cfg)));
							goto Calc_Denoise_FAILED;
						}
						isp522_convert_tuning_cfg_to_local(GroupId, CfgIds, (unsigned char *)dynamic_denoise_cfg_isp522);
						Sleep(60);
						dynamic_denoise_cfg_isp522->tuning_cfg[i].value[ISP_DENOISE_BLACK_GAIN] = 256 * 1.65 * black_gain_mul[i];
						dynamic_denoise_cfg_isp522->tuning_cfg[i].value[ISP_DENOISE_BLACK_OFFSET] = 0;
						dynamic_denoise_cfg_isp522->tuning_cfg[i].value[ISP_DENOISE_WHITE_GAIN] = 256 * 1.65 * white_gain_mul[i];
						dynamic_denoise_cfg_isp522->tuning_cfg[i].value[ISP_DENOISE_WHITE_OFFSET] = 0;
						isp522_convert_tuning_cfg_to_network(GroupId, CfgIds, (unsigned char *)dynamic_denoise_cfg_isp522);
						ret = isp522Handler->SetIspCfg(ispSel, GroupId, CfgIds, (void*)dynamic_denoise_cfg_isp522, sizeof(struct isp522_dynamic_denoise_cfg));
						if (ret) {
							errInfo.Format(_T("Failed to set enable_cfg(0x01)"));
							goto Calc_Denoise_FAILED;
						}
						ret = isp522Handler->UpdateIspCfg(ispSel);
						if (ret) {
							errInfo.Format(_T("Failed to set enable_cfg(0x02)"));
							goto Calc_Denoise_FAILED;
						}
						break;
					default:
						break;
				}
		}
	}


	switch(g_IspVersion)
	{
		case 500:
			SAFE_DELETE_ARRAY(cap_fmt.buffer);
			if (test_enable_cfg_isp500 != NULL)
				free(test_enable_cfg_isp500);
			if (dynamic_denoise_cfg_isp500 != NULL)
				free(dynamic_denoise_cfg_isp500);
			break;
		case 520:
			SAFE_DELETE_ARRAY(cap_fmt.buffer);
			if (test_enable_cfg_isp520 != NULL)
				free(test_enable_cfg_isp520);
			if (dynamic_denoise_cfg_isp520 != NULL)
				free(dynamic_denoise_cfg_isp520);
			break;
		case 521:
			SAFE_DELETE_ARRAY(cap_fmt.buffer);
			if (test_enable_cfg_isp521 != NULL)
				free(test_enable_cfg_isp521);
			if (dynamic_denoise_cfg_isp521 != NULL)
				free(dynamic_denoise_cfg_isp521);
			break;
		case 522:
			SAFE_DELETE_ARRAY(cap_fmt_isp522.buffer);
			if (test_enable_cfg_isp522 != NULL)
				free(test_enable_cfg_isp522);
			if (dynamic_denoise_cfg_isp522 != NULL)
				free(dynamic_denoise_cfg_isp522);
			break;
		default:
			break;
	}
	if (tuning_bdnf_table_cfg != NULL)
		free(tuning_bdnf_table_cfg);
	SAFE_DELETE_ARRAY(curve_rate);
	SAFE_DELETE_ARRAY(black_gain_mul);
	SAFE_DELETE_ARRAY(white_gain_mul);

	//owner->SendMessage(WM_CALC_FUNC_DONE, (WPARAM)TRUE, (LPARAM)&errInfo);
	return TRUE;

Calc_Denoise_FAILED:
	switch(g_IspVersion)
	{
		case 500:
			SAFE_DELETE_ARRAY(cap_fmt.buffer);
			if (test_enable_cfg_isp500 != NULL)
				free(test_enable_cfg_isp500);
			if (dynamic_denoise_cfg_isp500 != NULL)
				free(dynamic_denoise_cfg_isp500);
			break;
		case 520:
			SAFE_DELETE_ARRAY(cap_fmt.buffer);
			if (test_enable_cfg_isp520 != NULL)
				free(test_enable_cfg_isp520);
			if (dynamic_denoise_cfg_isp520 != NULL)
				free(dynamic_denoise_cfg_isp520);
			break;
		case 521:
			SAFE_DELETE_ARRAY(cap_fmt.buffer);
			if (test_enable_cfg_isp521 != NULL)
				free(test_enable_cfg_isp521);
			if (dynamic_denoise_cfg_isp521 != NULL)
				free(dynamic_denoise_cfg_isp521);
			break;
		case 522:
			SAFE_DELETE_ARRAY(cap_fmt_isp522.buffer);
			if (test_enable_cfg_isp522 != NULL)
				free(test_enable_cfg_isp522);
			if (dynamic_denoise_cfg_isp522 != NULL)
				free(dynamic_denoise_cfg_isp522);
			break;
		default:
			break;
	}
	if (tuning_bdnf_table_cfg != NULL)
		free(tuning_bdnf_table_cfg);
	SAFE_DELETE_ARRAY(curve_rate);
	SAFE_DELETE_ARRAY(black_gain_mul);
	SAFE_DELETE_ARRAY(white_gain_mul);
	
	owner->SendMessage(WM_CALC_FUNC_DONE, (WPARAM)FALSE, (LPARAM)&errInfo);
	return FALSE;

}
