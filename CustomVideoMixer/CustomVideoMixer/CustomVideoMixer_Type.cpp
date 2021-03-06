//----------------------------------------------------------------------------------------------
// CustomVideoMixer_Type.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

HRESULT CCustomVideoMixer::GetOutputType(IMFMediaType** ppType) {

	TRACE_TRANSFORM((L"CustomVideoMixer::GetOutputType"));

	HRESULT hr = S_OK;
	IMFMediaType* pOutputType = NULL;

	try {

		IF_FAILED_THROW(hr = MFCreateMediaType(&pOutputType));

		IF_FAILED_THROW(hr = pOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
		// MFVideoFormat_ARGB32 MFVideoFormat_RGB32
		IF_FAILED_THROW(hr = pOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));
		IF_FAILED_THROW(hr = pOutputType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE));
		IF_FAILED_THROW(hr = pOutputType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE));
		IF_FAILED_THROW(hr = pOutputType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
		IF_FAILED_THROW(hr = MFSetAttributeRatio(pOutputType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1));

		*ppType = pOutputType;
		(*ppType)->AddRef();
	}
	catch (HRESULT) {}

	SAFE_RELEASE(pOutputType);
	return hr;
}

HRESULT CCustomVideoMixer::OnCheckInputType(IMFMediaType** ppInptuType, IMFMediaType* pmt, const GUID gFormatSubType) {

	TRACE_TRANSFORM((L"CustomVideoMixer::OnCheckInputType"));

	HRESULT hr = S_OK;

	if (ppInptuType && *ppInptuType) {

		DWORD dwFlags = 0;

		if ((*ppInptuType)->IsEqual(pmt, &dwFlags) == S_OK) {
			return hr;
		}
		else {
			IF_FAILED_RETURN(hr = MF_E_INVALIDTYPE);
			// Todo
			//SAFE_RELEASE(pInptuType);
			//SAFE_RELEASE(m_pOutputType);
			//Flush();
		}
	}

	GUID majortype = { 0 };
	GUID subtype = { 0 };
	UINT32 width = 0;
	UINT32 height = 0;

	IF_FAILED_RETURN(hr = pmt->GetMajorType(&majortype));
	IF_FAILED_RETURN(hr = pmt->GetGUID(MF_MT_SUBTYPE, &subtype));
	IF_FAILED_RETURN(hr = MFGetAttributeSize(pmt, MF_MT_FRAME_SIZE, &width, &height));

	IF_FAILED_RETURN(hr = (majortype != MFMediaType_Video ? MF_E_INVALIDTYPE : S_OK));
	IF_FAILED_RETURN(hr = (subtype != gFormatSubType ? MF_E_INVALIDTYPE : S_OK));

	// Todo : check width = 0/height = 0
	IF_FAILED_RETURN(hr = ((width > MAX_VIDEO_WIDTH_HEIGHT || height > MAX_VIDEO_WIDTH_HEIGHT) ? MF_E_INVALIDTYPE : S_OK));

	return hr;
}

HRESULT CCustomVideoMixer::OnSetInputType(IMFMediaType** ppInptuType, IMFMediaType* pType) {

	TRACE_TRANSFORM((L"CustomVideoMixer::OnSetInputType"));

	HRESULT hr = S_OK;
	GUID subtype = { 0 };

	SAFE_RELEASE(*ppInptuType);

	IF_FAILED_RETURN(hr = pType->GetGUID(MF_MT_SUBTYPE, &subtype));

	*ppInptuType = pType;
	(*ppInptuType)->AddRef();

	return hr;
}

HRESULT CCustomVideoMixer::OnCheckOutputType(IMFMediaType* pType) {

	TRACE_TRANSFORM((L"CustomVideoMixer::OnCheckOutputType"));

	LogMediaType(pType);

	HRESULT hr = S_OK;

	if (m_pOutputType) {

		DWORD dwFlags = 0;

		if (m_pOutputType->IsEqual(pType, &dwFlags) == S_OK) {
			return hr;
		}
		else {
			IF_FAILED_RETURN(hr = MF_E_INVALIDTYPE);
			// Todo
			//SAFE_RELEASE(m_pInputType);
			//Flush();
		}
	}

	// Todo : not here, but before call to OnCheckOutputType
	if (m_pRefInputType == NULL) {
		return MF_E_TRANSFORM_TYPE_NOT_SET;
	}

	IMFMediaType* pOurType = NULL;
	BOOL bMatch = FALSE;

	try {

		IF_FAILED_THROW(hr = GetOutputType(&pOurType));

		IF_FAILED_THROW(hr = pOurType->Compare(pType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &bMatch));

		IF_FAILED_THROW(hr = (!bMatch ? MF_E_INVALIDTYPE : S_OK));
	}
	catch (HRESULT) {}

	SAFE_RELEASE(pOurType);
	return hr;
}
