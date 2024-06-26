// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

#include "ClientTests.h"
#include <memory>
#include <windows_version_helpers.h>


void Validate_Numeric_In_ReturnByRef();
void Validate_Float_In_ReturnAndUpdateByRef();
void Validate_Double_In_ReturnAndUpdateByRef();
void Validate_LCID_Marshaled();
void Validate_Enumerator();
void Validate_ParamCoerce();

template<COINIT TM>
struct ComInit
{
    const HRESULT Result;

    ComInit()
        : Result{ ::CoInitializeEx(nullptr, TM) }
    { }

    ~ComInit()
    {
        if (SUCCEEDED(Result))
            ::CoUninitialize();
    }
};

using ComMTA = ComInit<COINIT_MULTITHREADED>;

int __cdecl main()
{
    if (is_windows_nano() == S_OK)
    {
        ::puts("RegFree COM is not supported on Windows Nano. Auto-passing this test.\n");
        return 100;
    }
    ComMTA init;
    if (FAILED(init.Result))
        return -1;

    try
    {
        Validate_Numeric_In_ReturnByRef();
        Validate_Float_In_ReturnAndUpdateByRef();
        Validate_Double_In_ReturnAndUpdateByRef();
        Validate_LCID_Marshaled();
        Validate_Enumerator();
        Validate_ParamCoerce();
    }
    catch (HRESULT hr)
    {
        ::printf("Test Failure: 0x%08x\n", hr);
        return 101;
    }

    return 100;
}

void Validate_Numeric_In_ReturnByRef()
{
    HRESULT hr;

    CoreShimComActivation csact{ W("NETServer"), W("DispatchTesting") };

    ComSmartPtr<IDispatchTesting> dispatchTesting;
    THROW_IF_FAILED(::CoCreateInstance(CLSID_DispatchTesting, nullptr, CLSCTX_INPROC, IID_IDispatchTesting, (void**)&dispatchTesting));

    LPOLESTR numericMethodName = (LPOLESTR)W("DoubleNumeric_ReturnByRef");
    LCID lcid = MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT);
    DISPID methodId;

    ::wprintf(W("Invoke %s\n"), numericMethodName);
    THROW_IF_FAILED(dispatchTesting->GetIDsOfNames(
        IID_NULL,
        &numericMethodName,
        1,
        lcid,
        &methodId));

    uint8_t b1 = 24;
    uint8_t b2;
    int16_t s1 = 53;
    int16_t s2;
    uint16_t us1 = 74;
    uint16_t us2;
    LONG i1 = 34;
    LONG i2;
    ULONG ui1 = 854;
    ULONG ui2;
    int64_t l1 = 894;
    int64_t l2;
    uint64_t ul1 = 4168;
    uint64_t ul2;

    {
        DISPPARAMS params;
        params.cArgs = 14;
        params.rgvarg = new VARIANTARG[params.cArgs];
        params.cNamedArgs = 0;
        params.rgdispidNamedArgs = nullptr;

        V_VT(&params.rgvarg[13]) = VT_UI1;
        V_UI1(&params.rgvarg[13]) = b1;
        V_VT(&params.rgvarg[12]) = VT_BYREF | VT_UI1;
        V_UI1REF(&params.rgvarg[12]) = &b2;
        V_VT(&params.rgvarg[11]) = VT_I2;
        V_I2(&params.rgvarg[11]) = s1;
        V_VT(&params.rgvarg[10]) = VT_BYREF | VT_I2;
        V_I2REF(&params.rgvarg[10]) = &s2;
        V_VT(&params.rgvarg[9]) = VT_UI2;
        V_UI2(&params.rgvarg[9]) = us1;
        V_VT(&params.rgvarg[8]) = VT_BYREF | VT_UI2;
        V_UI2REF(&params.rgvarg[8]) = &us2;
        V_VT(&params.rgvarg[7]) = VT_I4;
        V_I4(&params.rgvarg[7]) = i1;
        V_VT(&params.rgvarg[6]) = VT_BYREF | VT_I4;
        V_I4REF(&params.rgvarg[6]) = &i2;
        V_VT(&params.rgvarg[5]) = VT_UI4;
        V_UI4(&params.rgvarg[5]) = ui1;
        V_VT(&params.rgvarg[4]) = VT_BYREF | VT_UI4;
        V_UI4REF(&params.rgvarg[4]) = &ui2;
        V_VT(&params.rgvarg[3]) = VT_I8;
        V_I8(&params.rgvarg[3]) = l1;
        V_VT(&params.rgvarg[2]) = VT_BYREF | VT_I8;
        V_I8REF(&params.rgvarg[2]) = &l2;
        V_VT(&params.rgvarg[1]) = VT_UI8;
        V_UI8(&params.rgvarg[1]) = ul1;
        V_VT(&params.rgvarg[0]) = VT_BYREF | VT_UI8;
        V_UI8REF(&params.rgvarg[0]) = &ul2;

        THROW_IF_FAILED(dispatchTesting->Invoke(
            methodId,
            IID_NULL,
            lcid,
            DISPATCH_METHOD,
            &params,
            nullptr,
            nullptr,
            nullptr
        ));

        THROW_FAIL_IF_FALSE(b2 == b1 * 2);
        THROW_FAIL_IF_FALSE(s2 == s1 * 2);
        THROW_FAIL_IF_FALSE(us2 == us1 * 2);
        THROW_FAIL_IF_FALSE(i2 == i1 * 2);
        THROW_FAIL_IF_FALSE(ui2 == ui1 * 2);
        THROW_FAIL_IF_FALSE(l2 == l1 * 2);
        THROW_FAIL_IF_FALSE(ul2 == ul1 * 2);
    }

    {
        b2 = 0;
        s2 = 0;
        us2 = 0;
        i2 = 0;
        ui2 = 0;
        l2 = 0;
        ul2 = 0;

        THROW_IF_FAILED(dispatchTesting->DoubleNumeric_ReturnByRef(b1, &b2, s1, &s2, us1, &us2, i1, (int32_t*)&i2, ui1, (uint32_t*)&ui2, l1, &l2, ul1, &ul2));

        THROW_FAIL_IF_FALSE(b2 == b1 * 2);
        THROW_FAIL_IF_FALSE(s2 == s1 * 2);
        THROW_FAIL_IF_FALSE(us2 == us1 * 2);
        THROW_FAIL_IF_FALSE(i2 == i1 * 2);
        THROW_FAIL_IF_FALSE(ui2 == ui1 * 2);
        THROW_FAIL_IF_FALSE(l2 == l1 * 2);
        THROW_FAIL_IF_FALSE(ul2 == ul1 * 2);
    }
}

namespace
{
    bool EqualByBound(float expected, float actual)
    {
        float low = expected - 0.0001f;
        float high = expected + 0.0001f;
        float eps = abs(expected - actual);
        return eps < std::numeric_limits<float>::epsilon() || (low < actual && actual < high);
    }

    bool EqualByBound(double expected, double actual)
    {
        double low = expected - 0.00001;
        double high = expected + 0.00001;
        double eps = abs(expected - actual);
        return eps < std::numeric_limits<double>::epsilon() || (low < actual && actual < high);
    }
}

void Validate_Float_In_ReturnAndUpdateByRef()
{
    HRESULT hr;

    CoreShimComActivation csact{ W("NETServer"), W("DispatchTesting") };

    ComSmartPtr<IDispatchTesting> dispatchTesting;
    THROW_IF_FAILED(::CoCreateInstance(CLSID_DispatchTesting, nullptr, CLSCTX_INPROC, IID_IDispatchTesting, (void**)&dispatchTesting));

    LPOLESTR numericMethodName = (LPOLESTR)W("Add_Float_ReturnAndUpdateByRef");
    LCID lcid = MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT);
    DISPID methodId;

    ::wprintf(W("Invoke %s\n"), numericMethodName);
    THROW_IF_FAILED(dispatchTesting->GetIDsOfNames(
        IID_NULL,
        &numericMethodName,
        1,
        lcid,
        &methodId));

    const float a = 12.34f;
    const float b_orig = 1.234f;
    const float expected = b_orig + a;

    float b = b_orig;

    {
        DISPPARAMS params;
        params.cArgs = 2;
        params.rgvarg = new VARIANTARG[params.cArgs];
        params.cNamedArgs = 0;
        params.rgdispidNamedArgs = nullptr;

        VARIANT result;

        V_VT(&params.rgvarg[1]) = VT_R4;
        V_R4(&params.rgvarg[1]) = a;
        V_VT(&params.rgvarg[0]) = VT_BYREF | VT_R4;
        V_R4REF(&params.rgvarg[0]) = &b;


        THROW_IF_FAILED(dispatchTesting->Invoke(
            methodId,
            IID_NULL,
            lcid,
            DISPATCH_METHOD,
            &params,
            &result,
            nullptr,
            nullptr
        ));

        THROW_FAIL_IF_FALSE(EqualByBound(expected, V_R4(&result)));
        THROW_FAIL_IF_FALSE(EqualByBound(expected, b));
    }

    {
        b = b_orig;
        float result;
        THROW_IF_FAILED(dispatchTesting->Add_Float_ReturnAndUpdateByRef(a, &b, &result));

        THROW_FAIL_IF_FALSE(EqualByBound(expected, result));
        THROW_FAIL_IF_FALSE(EqualByBound(expected, b));
    }
}

void Validate_Double_In_ReturnAndUpdateByRef()
{
    HRESULT hr;

    CoreShimComActivation csact{ W("NETServer"), W("DispatchTesting") };

    ComSmartPtr<IDispatchTesting> dispatchTesting;
    THROW_IF_FAILED(::CoCreateInstance(CLSID_DispatchTesting, nullptr, CLSCTX_INPROC, IID_IDispatchTesting, (void**)&dispatchTesting));

    LPOLESTR numericMethodName = (LPOLESTR)W("Add_Double_ReturnAndUpdateByRef");
    LCID lcid = MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT);
    DISPID methodId;

    ::wprintf(W("Invoke %s\n"), numericMethodName);
    THROW_IF_FAILED(dispatchTesting->GetIDsOfNames(
        IID_NULL,
        &numericMethodName,
        1,
        lcid,
        &methodId));

    const double a = 1856.5634;
    const double b_orig = 587867.757;
    const double expected = a + b_orig;

    double b = b_orig;

    {
        DISPPARAMS params;
        params.cArgs = 2;
        params.rgvarg = new VARIANTARG[params.cArgs];
        params.cNamedArgs = 0;
        params.rgdispidNamedArgs = nullptr;

        VARIANT result;

        V_VT(&params.rgvarg[1]) = VT_R8;
        V_R8(&params.rgvarg[1]) = a;
        V_VT(&params.rgvarg[0]) = VT_BYREF | VT_R8;
        V_R8REF(&params.rgvarg[0]) = &b;


        THROW_IF_FAILED(dispatchTesting->Invoke(
            methodId,
            IID_NULL,
            lcid,
            DISPATCH_METHOD,
            &params,
            &result,
            nullptr,
            nullptr
        ));

        THROW_FAIL_IF_FALSE(EqualByBound(expected, V_R8(&result)));
        THROW_FAIL_IF_FALSE(EqualByBound(expected, b));
    }

    {
        b = b_orig;
        double result;
        THROW_IF_FAILED(dispatchTesting->Add_Double_ReturnAndUpdateByRef(a, &b, &result));

        THROW_FAIL_IF_FALSE(EqualByBound(expected, result));
        THROW_FAIL_IF_FALSE(EqualByBound(expected, b));
    }
}

void Validate_LCID_Marshaled()
{
    HRESULT hr;

    CoreShimComActivation csact{ W("NETServer"), W("DispatchTesting") };

    ComSmartPtr<IDispatchTesting> dispatchTesting;
    THROW_IF_FAILED(::CoCreateInstance(CLSID_DispatchTesting, nullptr, CLSCTX_INPROC, IID_IDispatchTesting, (void**)&dispatchTesting));

    LPOLESTR numericMethodName = (LPOLESTR)W("PassThroughLCID");
    LCID lcid = MAKELCID(MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_CHILE), SORT_DEFAULT);
    DISPID methodId;

    ::wprintf(W("Invoke %s\n"), numericMethodName);
    THROW_IF_FAILED(dispatchTesting->GetIDsOfNames(
        IID_NULL,
        &numericMethodName,
        1,
        lcid,
        &methodId));

    DISPPARAMS params;
    params.cArgs = 0;
    params.rgvarg = nullptr;
    params.cNamedArgs = 0;
    params.rgdispidNamedArgs = nullptr;

    VARIANT result;

    THROW_IF_FAILED(dispatchTesting->Invoke(
        methodId,
        IID_NULL,
        lcid,
        DISPATCH_METHOD,
        &params,
        &result,
        nullptr,
        nullptr
    ));

    THROW_FAIL_IF_FALSE(lcid == (LCID)V_UI4(&result));
}

namespace
{
    void ValidateExpectedEnumVariant(IEnumVARIANT *enumVariant, int expectedStart, int expectedCount)
    {
        HRESULT hr;
        VARIANT element;
        ULONG numFetched;
        for(int i = expectedStart; i < expectedStart + expectedCount; ++i)
        {
            THROW_IF_FAILED(enumVariant->Next(1, &element, &numFetched));
            THROW_FAIL_IF_FALSE(numFetched == 1);
            THROW_FAIL_IF_FALSE(V_I4(&element) == i)
            ::VariantClear(&element);
        }

        hr = enumVariant->Next(1, &element, &numFetched);
        THROW_FAIL_IF_FALSE(hr == S_FALSE && numFetched == 0);
    }

    void ValidateReturnedEnumerator(VARIANT *toValidate)
    {
        HRESULT hr;
        THROW_FAIL_IF_FALSE(V_VT(toValidate) == VT_UNKNOWN || V_VT(toValidate) == VT_DISPATCH);

        ComSmartPtr<IEnumVARIANT> enumVariant;
        THROW_IF_FAILED(V_UNKNOWN(toValidate)->QueryInterface<IEnumVARIANT>(&enumVariant));

        // Implementation of IDispatchTesting should return [0,9]
        ValidateExpectedEnumVariant(enumVariant, 0, 10);

        THROW_IF_FAILED(enumVariant->Reset());
        ValidateExpectedEnumVariant(enumVariant, 0, 10);

        THROW_IF_FAILED(enumVariant->Reset());
        THROW_IF_FAILED(enumVariant->Skip(3));
        ValidateExpectedEnumVariant(enumVariant, 3, 7);
    }
}

void Validate_Enumerator()
{
    HRESULT hr;

    CoreShimComActivation csact{ W("NETServer"), W("DispatchTesting") };

    ComSmartPtr<IDispatchTesting> dispatchTesting;
    THROW_IF_FAILED(::CoCreateInstance(CLSID_DispatchTesting, nullptr, CLSCTX_INPROC, IID_IDispatchTesting, (void**)&dispatchTesting));
    LCID lcid = MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT);

    ::printf("Invoke GetEnumerator (DISPID_NEWENUM)\n");
    DISPPARAMS params {};
    VARIANT result;
    THROW_IF_FAILED(dispatchTesting->Invoke(
        DISPID_NEWENUM,
        IID_NULL,
        lcid,
        DISPATCH_METHOD,
        &params,
        &result,
        nullptr,
        nullptr
    ));

    ::printf(" -- Validate returned IEnumVARIANT\n");
    ValidateReturnedEnumerator(&result);

    LPOLESTR methodName = (LPOLESTR)W("ExplicitGetEnumerator");

    ::wprintf(W("Invoke %s\n"), methodName);
    DISPID methodId;
    THROW_IF_FAILED(dispatchTesting->GetIDsOfNames(
        IID_NULL,
        &methodName,
        1,
        lcid,
        &methodId));

    ::VariantClear(&result);
    THROW_IF_FAILED(dispatchTesting->Invoke(
        methodId,
        IID_NULL,
        lcid,
        DISPATCH_METHOD,
        &params,
        &result,
        nullptr,
        nullptr
    ));

    ::printf(" -- Validate returned IEnumVARIANT\n");
    ValidateReturnedEnumerator(&result);
}

void Validate_ParamCoerce_Success(ComSmartPtr<IDispatchCoerceTesting>& dispatchCoerceTesting, int lcid, DISPID methodId, VARIANT arg, int expected)
{
    HRESULT hr;

    DISPPARAMS params;
    VARIANTARG args;
    params.cArgs = 1;
    params.rgvarg = &args;
    params.cNamedArgs = 0;
    params.rgdispidNamedArgs = nullptr;

    args = arg;
    VARIANT result;

    THROW_IF_FAILED(dispatchCoerceTesting->Invoke(
        methodId,
        IID_NULL,
        lcid,
        DISPATCH_METHOD,
        &params,
        &result,
        nullptr,
        nullptr
    ));

    THROW_FAIL_IF_FALSE(V_I4(&result) == expected);
}

void Validate_ParamCoerce_Exception(ComSmartPtr<IDispatchCoerceTesting>& dispatchCoerceTesting, int lcid, DISPID methodId, VARIANT arg, HRESULT expected)
{
    HRESULT hr;

    DISPPARAMS params;
    VARIANTARG args;
    params.cArgs = 1;
    params.rgvarg = &args;
    params.cNamedArgs = 0;
    params.rgdispidNamedArgs = nullptr;

    args = arg;
    VARIANT result;

    hr = dispatchCoerceTesting->Invoke(
        methodId,
        IID_NULL,
        lcid,
        DISPATCH_METHOD,
        &params,
        &result,
        nullptr,
        nullptr
    );

    THROW_FAIL_IF_FALSE(hr == expected);
}

void Validate_ParamCoerce()
{
    HRESULT hr;

    CoreShimComActivation csact{ W("NETServer"), W("DispatchCoerceTesting") };

    ComSmartPtr<IDispatchCoerceTesting> dispatchCoerceTesting;
    THROW_IF_FAILED(::CoCreateInstance(CLSID_DispatchCoerceTesting, nullptr, CLSCTX_INPROC, IID_IDispatchCoerceTesting, (void**)&dispatchCoerceTesting));

    LPOLESTR numericMethodName = (LPOLESTR)W("ManagedArgument");
    LCID lcid = MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT);
    DISPID methodId;

    ::wprintf(W("Invoke %s\n"), numericMethodName);
    THROW_IF_FAILED(dispatchCoerceTesting->GetIDsOfNames(
        IID_NULL,
        &numericMethodName,
        1,
        lcid,
        &methodId));
    
    VARIANT arg;

    ::wprintf(W("Validating VT_UI4\n"));
    V_VT(&arg) = VT_UI4;
    V_UI4(&arg) = 0x1234ABCD;
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, 0x1234ABCD);
    
    ::wprintf(W("Validating VT_I2\n"));
    V_VT(&arg) = VT_I2;
    V_I2(&arg) = 123;
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, 123);
    
    ::wprintf(W("Validating VT_I8\n"));
    V_VT(&arg) = VT_I8;
    V_I8(&arg) = int64_t(1) << 32;
    Validate_ParamCoerce_Exception(dispatchCoerceTesting, lcid, methodId, arg, DISP_E_OVERFLOW);

    ::wprintf(W("Validating VT_R8\n"));
    V_VT(&arg) = VT_R8;
    V_R8(&arg) = 123.45;
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, 123);

    ::wprintf(W("Validating VT_CY\n"));
    V_VT(&arg) = VT_CY;
    V_I8(&arg) = 123456;
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, 12);

    ::wprintf(W("Validating VT_BSTR\n"));
    V_VT(&arg) = VT_BSTR;
    V_BSTR(&arg) = ::SysAllocString(L"123");
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, 123);

    ::wprintf(W("Validating VT_BOOL\n"));
    V_VT(&arg) = VT_BOOL;
    V_BOOL(&arg) = VARIANT_TRUE;
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, -1);
    V_VT(&arg) = VT_BOOL;
    V_I4(&arg) = 123;
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, -1);
    V_VT(&arg) = VT_BOOL;
    V_BOOL(&arg) = VARIANT_FALSE;
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, 0);

    ::wprintf(W("Validating VT_DATE\n"));
    V_VT(&arg) = VT_DATE;
    V_R8(&arg) = -657434.0;
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, -657434);
    V_VT(&arg) = VT_DATE;
    V_R8(&arg) = -657435.0;
    Validate_ParamCoerce_Exception(dispatchCoerceTesting, lcid, methodId, arg, E_INVALIDARG);
    V_VT(&arg) = VT_DATE;
    V_R8(&arg) = 2958465.0;
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, 2958465);
    V_VT(&arg) = VT_DATE;
    V_R8(&arg) = 2958466.0;
    Validate_ParamCoerce_Exception(dispatchCoerceTesting, lcid, methodId, arg, E_INVALIDARG);

    ::wprintf(W("Validating VT_DECIMAL\n"));
    V_VT(&arg) = VT_DECIMAL;
    VarDecFromI4(123, &V_DECIMAL(&arg));
    Validate_ParamCoerce_Success(dispatchCoerceTesting, lcid, methodId, arg, 123);
    V_VT(&arg) = VT_DECIMAL;
    VarDecFromI8(int64_t(1) << 32, &V_DECIMAL(&arg));
    Validate_ParamCoerce_Exception(dispatchCoerceTesting, lcid, methodId, arg, DISP_E_OVERFLOW);
}
