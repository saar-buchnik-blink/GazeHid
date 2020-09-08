/*++

Copyright (C) Microsoft Corporation, All Rights Reserved.

Module Name:

    vhidmini.cpp

Abstract:

    This module contains the implementation of the driver

Environment:

    Windows Driver Framework (WDF)

--*/

#include "driver.h"
#include "hidconstants.h"
#include "Tracker.h"
#include <strsafe.h>
#include <SetupAPI.h>

HID_REPORT_DESCRIPTOR EyeTrackerReportDescriptor[] = 
{
    HID_USAGE_PAGE(HID_USAGE_PAGE_EYE_HEAD_TRACKER),
    HID_USAGE(HID_USAGE_EYE_TRACKER),
    HID_BEGIN_APPLICATION_COLLECTION(),
#pragma region HID_USAGE_TRACKING_DATA
        HID_USAGE(HID_USAGE_TRACKING_DATA),
        HID_BEGIN_LOGICAL_COLLECTION(),
            HID_REPORT_ID(HID_USAGE_TRACKING_DATA),

            HID_LOGICAL_MINIMUM_BYTE(0x00),
            HID_LOGICAL_MAXIMUM_WORD(0x00FF),
            HID_REPORT_SIZE_UINT8(),
            HID_REPORT_COUNT(7),
            HID_INPUT_STATIC_VALUE(),

            HID_USAGE(HID_USAGE_TIMESTAMP),
            HID_UNIT_WORD(0x1001),                      // SI Linear
            HID_UNIT_EXPONENT_BYTE(0x0A),               // -6
            HID_REPORT_COUNT(8),
            HID_INPUT_DYNAMIC_VALUE(),

            HID_REPORT_COUNT(1),
            HID_REPORT_SIZE_UINT32(),
            HID_UNIT_BYTE(0x11),                        // Centimeter
            HID_UNIT_EXPONENT_BYTE(0x0C),               // -4, micrometers
            HID_LOGICAL_MINIMUM_DWORD(0xFFE17B80),      // -2000000
            HID_LOGICAL_MAXIMUM_DWORD(0x001E8480),      //  2000000

            HID_USAGE(HID_USAGE_GAZE_LOCATION),
            HID_BEGIN_PHYSICAL_COLLECTION(),
                HID_REPORT_COUNT(2),
                HID_USAGE(HID_USAGE_POSITION_X),
                HID_USAGE(HID_USAGE_POSITION_Y),
                HID_INPUT_STATIC_VALUE(),
            HID_END_COLLECTION_EX(),

            HID_USAGE(HID_USAGE_LEFT_EYE_POSITION),
            HID_BEGIN_PHYSICAL_COLLECTION(),
                HID_REPORT_COUNT(3),
                HID_USAGE(HID_USAGE_POSITION_X),
                HID_USAGE(HID_USAGE_POSITION_Y),
                HID_USAGE(HID_USAGE_POSITION_Z),
                HID_INPUT_STATIC_VALUE(),
            HID_END_COLLECTION_EX(),

            HID_USAGE(HID_USAGE_RIGHT_EYE_POSITION),
            HID_BEGIN_PHYSICAL_COLLECTION(),
            //HID_REPORT_COUNT(3),
            HID_USAGE(HID_USAGE_POSITION_X),
            HID_USAGE(HID_USAGE_POSITION_Y),
            HID_USAGE(HID_USAGE_POSITION_Z),
            HID_INPUT_STATIC_VALUE(),
        HID_END_COLLECTION_EX(),
    HID_END_COLLECTION_EX(),
#pragma endregion
#pragma region HID_USAGE_CAPABILITIES
        HID_USAGE(HID_USAGE_CAPABILITIES),
        HID_BEGIN_LOGICAL_COLLECTION(),
            HID_REPORT_ID(HID_USAGE_CAPABILITIES),

            HID_REPORT_SIZE_UINT8(),
            HID_REPORT_COUNT(1),
            HID_USAGE_WORD(HID_USAGE_TRACKER_QUALITY),
            HID_LOGICAL_MINIMUM_BYTE(0x00),
            HID_LOGICAL_MAXIMUM_BYTE(0x01),
            HID_UNIT_BYTE(0x00),                        // None
            HID_UNIT_EXPONENT_BYTE(0x00),               // 0
            HID_FEATURE_STATIC_VALUE(),

            HID_REPORT_COUNT(1),
            HID_REPORT_SIZE_UINT16(),
            //HID_LOGICAL_MINIMUM_BYTE(0x00),
            HID_LOGICAL_MAXIMUM_DWORD(0x0000FFFF),
            HID_FEATURE_STATIC_VALUE(),

            HID_BEGIN_PHYSICAL_COLLECTION(),
                HID_REPORT_COUNT(5),
                HID_REPORT_SIZE_UINT32(),
                HID_LOGICAL_MINIMUM_DWORD(0xFFE17B80),      // -2000000
                HID_LOGICAL_MAXIMUM_DWORD(0x001E8480),      //  2000000
                HID_UNIT_BYTE(0x11),                        // Centimeter
                HID_UNIT_EXPONENT_BYTE(0x0C),               // -4, micrometers
                HID_USAGE_WORD(HID_USAGE_MINIMUM_TRACKING_DISTANCE),
                HID_USAGE_WORD(HID_USAGE_OPTIMUM_TRACKING_DISTANCE),
                HID_USAGE_WORD(HID_USAGE_MAXIMUM_TRACKING_DISTANCE),
                HID_USAGE_WORD(HID_USAGE_MAXIMUM_SCREEN_PLANE_WIDTH),
                HID_USAGE_WORD(HID_USAGE_MAXIMUM_SCREEN_PLANE_HEIGHT),
                HID_FEATURE_STATIC_VALUE(),
            HID_END_COLLECTION_EX(),
        HID_END_COLLECTION_EX(),
#pragma endregion
#pragma region HID_USAGE_CONFIGURATION
        HID_USAGE(HID_USAGE_CONFIGURATION),
        HID_BEGIN_LOGICAL_COLLECTION(),
            HID_REPORT_ID(HID_USAGE_CONFIGURATION),

            HID_REPORT_SIZE_UINT8(),
            HID_LOGICAL_MINIMUM_BYTE(0x00),
            HID_LOGICAL_MAXIMUM_WORD(0x00FF),
            HID_REPORT_COUNT(1),
            HID_FEATURE_STATIC_VALUE(),

            HID_REPORT_SIZE_UINT16(),
            HID_LOGICAL_MAXIMUM_DWORD(0x0000FFFF),
            HID_UNIT_BYTE(0x00),                        // None
            HID_UNIT_EXPONENT_BYTE(0x00),               // 0
            HID_USAGE_WORD(HID_USAGE_DISPLAY_MANUFACTURER_ID),
            HID_FEATURE_STATIC_VALUE(),

            HID_USAGE_WORD(HID_USAGE_DISPLAY_PRODUCT_ID),
            HID_FEATURE_STATIC_VALUE(),

            HID_REPORT_SIZE_UINT32(),
            HID_LOGICAL_MAXIMUM_DWORD(0x7FFFFFFF),
            HID_USAGE_WORD(HID_USAGE_DISPLAY_SERIAL_NUMBER),
            HID_FEATURE_STATIC_VALUE(),

            HID_REPORT_SIZE_UINT16(),
            //HID_LOGICAL_MINIMUM_BYTE(0x00),
            HID_LOGICAL_MAXIMUM_DWORD(0x0000FFFF),
            HID_USAGE_WORD(HID_USAGE_DISPLAY_MANUFACTURER_DATE),
            HID_FEATURE_STATIC_VALUE(),

            HID_BEGIN_PHYSICAL_COLLECTION(),
                HID_UNIT_BYTE(0x11),                        // Centimeter
                HID_UNIT_EXPONENT_BYTE(0x0C),               // -4, micrometers
                HID_LOGICAL_MAXIMUM_DWORD(0x7FFFFFFF),
                HID_REPORT_SIZE_UINT32(),
                HID_USAGE_WORD(HID_USAGE_CALIBRATED_SCREEN_WIDTH),
                HID_FEATURE_STATIC_VALUE(),

                HID_USAGE_WORD(HID_USAGE_CALIBRATED_SCREEN_HEIGHT),
                HID_FEATURE_STATIC_VALUE(),
            HID_END_COLLECTION_EX(),

        HID_END_COLLECTION_EX(),
#pragma endregion
#pragma region HID_USAGE_TRACKER_STATUS (Feature)
        HID_USAGE(HID_USAGE_TRACKER_STATUS),
        HID_BEGIN_LOGICAL_COLLECTION(),
            HID_REPORT_ID(HID_USAGE_TRACKER_STATUS),

            HID_REPORT_SIZE_UINT8(),
            HID_UNIT_BYTE(0x00),                        // None
            HID_UNIT_EXPONENT_BYTE(0x00),               // 0
            HID_LOGICAL_MAXIMUM_BYTE(0x04),
            HID_USAGE_WORD(HID_USAGE_CONFIGURATION_STATUS),
            HID_FEATURE_DYNAMIC_VALUE(),

            HID_REPORT_SIZE_UINT16(),
            HID_LOGICAL_MAXIMUM_DWORD(0x0000FFFF),
            HID_UNIT_WORD(0xF001),                      // SI Linear
            HID_UNIT_EXPONENT_BYTE(0x00),               // 0
            HID_USAGE_WORD(HID_USAGE_SAMPLING_FREQUENCY),
            HID_FEATURE_DYNAMIC_VALUE(),
        HID_END_COLLECTION_EX(),
#pragma endregion
#pragma region HID_USAGE_TRACKER_STATUS (Input)
        HID_USAGE(HID_USAGE_TRACKER_STATUS),
        HID_BEGIN_LOGICAL_COLLECTION(),
            HID_REPORT_ID(HID_USAGE_TRACKER_STATUS),

            HID_REPORT_SIZE_UINT8(),
            //HID_UNIT_BYTE(0x00),                        // None
            //HID_UNIT_EXPONENT_BYTE(0x00),               // 0
            HID_LOGICAL_MAXIMUM_BYTE(0x04),
            HID_USAGE_WORD(HID_USAGE_CONFIGURATION_STATUS),
            HID_INPUT_DYNAMIC_VALUE(),

            HID_REPORT_SIZE_UINT16(),
            HID_LOGICAL_MAXIMUM_DWORD(0x0000FFFF),
            HID_UNIT_WORD(0xF001),                      // SI Linear
            HID_UNIT_EXPONENT_BYTE(0x00),               // 0
            HID_USAGE_WORD(HID_USAGE_SAMPLING_FREQUENCY),
            HID_INPUT_DYNAMIC_VALUE(),
        HID_END_COLLECTION_EX(),
#pragma endregion
#pragma region HID_USAGE_TRACKER_CONTROL
        HID_USAGE(HID_USAGE_TRACKER_CONTROL),
        HID_BEGIN_LOGICAL_COLLECTION(),
            HID_REPORT_ID(HID_USAGE_TRACKER_CONTROL),

            HID_REPORT_SIZE_UINT8(),
            HID_LOGICAL_MAXIMUM_BYTE(0x07),
            HID_UNIT_BYTE(0x00),                        // None
            HID_UNIT_EXPONENT_BYTE(0x00),               // 0
            HID_USAGE_WORD(HID_USAGE_MODE_REQUEST),
            HID_FEATURE_DYNAMIC_VALUE(),
        HID_END_COLLECTION_EX(),
#pragma endregion
    HID_END_COLLECTION_EX()
};


//
// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of G_DefaultReportDescriptor.
//

HID_DESCRIPTOR              EyeTrackerHidDescriptor = {
    0x09,   // length of HID descriptor
    0x21,   // descriptor type == HID  0x21
    0x0100, // hid spec release
    0x00,   // country code == Not Specified
    0x01,   // number of HID class descriptors
    {                                       //DescriptorList[0]
        0x22,                               //report descriptor type 0x22
        sizeof(EyeTrackerReportDescriptor)   //total length of report descriptor
    }
};

NTSTATUS
DriverEntry(
    _In_  PDRIVER_OBJECT    DriverObject,
    _In_  PUNICODE_STRING   RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS, or another status value for which NT_SUCCESS(status) equals
                    TRUE if successful,

    STATUS_UNSUCCESSFUL, or another status for which NT_SUCCESS(status) equals
                    FALSE otherwise.

--*/
{
    WDF_DRIVER_CONFIG       config;
    NTSTATUS                status;

    KdPrint(("DriverEntry for VHidMini\n"));

    WDF_DRIVER_CONFIG_INIT(&config, EvtDeviceAdd);

    status = WdfDriverCreate(DriverObject,
                            RegistryPath,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &config,
                            WDF_NO_HANDLE);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Error: WdfDriverCreate failed 0x%x\n", status));
        return status;
    }

    return status;
}

NTSTATUS
EvtDeviceAdd(
    _In_  WDFDRIVER         Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                status;
    WDF_OBJECT_ATTRIBUTES   deviceAttributes;
    WDFDEVICE               device;
    PDEVICE_CONTEXT         deviceContext;
    PHID_DEVICE_ATTRIBUTES  hidAttributes;
    UNREFERENCED_PARAMETER  (Driver);

    KdPrint(("Enter EvtDeviceAdd\n"));

    //
    // Mark ourselves as a filter, which also relinquishes power policy ownership
    //
    WdfFdoInitSetFilter(DeviceInit);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                            &deviceAttributes,
                            DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit,
                            &deviceAttributes,
                            &device);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Error: WdfDeviceCreate failed 0x%x\n", status));
        return status;
    }

    deviceContext = GetDeviceContext(device);
    deviceContext->Device       = device;
    deviceContext->DeviceData   = 0;

    hidAttributes = &deviceContext->HidDeviceAttributes;
    RtlZeroMemory(hidAttributes, sizeof(HID_DEVICE_ATTRIBUTES));
    hidAttributes->Size         = sizeof(HID_DEVICE_ATTRIBUTES);
    hidAttributes->VendorID     = HIDMINI_VID;
    hidAttributes->ProductID    = HIDMINI_PID;
    hidAttributes->VersionNumber = HIDMINI_VERSION;

    status = QueueCreate(device,
                         &deviceContext->DefaultQueue);
    if( !NT_SUCCESS(status) ) {
        return status;
    }

    status = ManualQueueCreate(device,
                        &deviceContext->ManualQueue);
    if( !NT_SUCCESS(status) ) {
        return status;
    }

    //
    // Use default "HID Descriptor" (hardcoded). We will set the
    // wReportLength memeber of HID descriptor when we read the
    // the report descriptor either from registry or the hard-coded
    // one.
    //
    deviceContext->HidDescriptor = EyeTrackerHidDescriptor;

    //
    // Check to see if we need to read the Report Descriptor from
    // registry. If the "ReadFromRegistry" flag in the registry is set
    // then we will read the descriptor from registry using routine
    // ReadDescriptorFromRegistry(). Otherwise, we will use the
    // hard-coded default report descriptor.
    //

    status = CheckRegistryForDescriptor(device);
    if (NT_SUCCESS(status)){
        //
        // We need to read read descriptor from registry
        //
        status = ReadDescriptorFromRegistry(device);
        if (!NT_SUCCESS(status)){
            KdPrint(("Failed to read descriptor from registry\n"));
        }
    }

    //
    // We will use hard-coded report descriptor if registry one is not used.
    //
    if (!NT_SUCCESS(status)){
        deviceContext->ReportDescriptor = EyeTrackerReportDescriptor;
        KdPrint(("Using Hard-coded Report descriptor\n"));
        status = STATUS_SUCCESS;
    }

    // Filling in generic values here. These should be customized for the specific tracker
    // When InitializeEyeTracker is called, the tracker can customize its own specific values

    PCAPABILITIES_REPORT caps = &deviceContext->CapabilitiesReport;
    caps->ReportId = HID_USAGE_CAPABILITIES;
    caps->TrackerQuality = TRACKER_QUALITY_FINE_GAZE;
    caps->MinimumTrackingDistance = 50000;
    caps->OptimumTrackingDistance = 65000;
    caps->MaximumTrackingDistance = 90000;

    GetPrimaryMonitorInfo(deviceContext);

    PTRACKER_STATUS_REPORT trackerStatus = &deviceContext->TrackerStatusReport;
    trackerStatus->ReportId = HID_USAGE_TRACKER_STATUS;
    trackerStatus->ConfigurationStatus = TRACKER_STATUS_READY;
    trackerStatus->SamplingFrequency = 100;

    if (!InitializeEyeTracker(deviceContext))
    {
        KdPrint(("Error: Failed to initialize eye tracker\n"));
        return STATUS_UNSUCCESSFUL;
    }

    return status;
}

#ifdef _KERNEL_MODE
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL EvtIoDeviceControl;
#else
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL          EvtIoDeviceControl;
#endif

NTSTATUS
QueueCreate(
    _In_  WDFDEVICE         Device,
    _Out_ WDFQUEUE          *Queue
    )
/*++
Routine Description:

    This function creates a default, parallel I/O queue to proces IOCTLs
    from hidclass.sys.

Arguments:

    Device - Handle to a framework device object.

    Queue - Output pointer to a framework I/O queue handle, on success.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                status;
    WDF_IO_QUEUE_CONFIG     queueConfig;
    WDF_OBJECT_ATTRIBUTES   queueAttributes;
    WDFQUEUE                queue;
    PQUEUE_CONTEXT          queueContext;

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
                            &queueConfig,
                            WdfIoQueueDispatchParallel);

#ifdef _KERNEL_MODE
    queueConfig.EvtIoInternalDeviceControl  = EvtIoDeviceControl;
#else
    //
    // HIDclass uses INTERNAL_IOCTL which is not supported by UMDF. Therefore
    // the hidumdf.sys changes the IOCTL type to DEVICE_CONTROL for next stack
    // and sends it down
    //
    queueConfig.EvtIoDeviceControl          = EvtIoDeviceControl;
#endif

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                            &queueAttributes,
                            QUEUE_CONTEXT);

    status = WdfIoQueueCreate(
                            Device,
                            &queueConfig,
                            &queueAttributes,
                            &queue);

    if( !NT_SUCCESS(status) ) {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n",status));
        return status;
    }

    queueContext = GetQueueContext(queue);
    queueContext->Queue         = queue;
    queueContext->DeviceContext = GetDeviceContext(Device);
    queueContext->OutputReport  = 0;

    *Queue = queue;
    return status;
}


NTSTATUS
ManualQueueCreate(
    _In_  WDFDEVICE         Device,
    _Out_ WDFQUEUE          *Queue
)
/*++
Routine Description:

    This function creates a manual I/O queue to receive IOCTL_HID_READ_REPORT
    forwarded from the device's default queue handler.
    It also creates a periodic timer to check the queue and complete any pending
    request with data from the device. Here timer expiring is used to simulate
    a hardware event that new data is ready.
    The workflow is like this:
        - Hidclass.sys sends an ioctl to the miniport to read input report.
        - The request reaches the driver's default queue. As data may not be avaiable
          yet, the request is forwarded to a second manual queue temporarily.
        - Later when data is ready (as simulated by timer expiring), the driver
          checks for any pending request in the manual queue, and then completes it.
        - Hidclass gets notified for the read request completion and return data to
          the caller.
    On the other hand, for IOCTL_HID_WRITE_REPORT request, the driver simply
    sends the request to the hardware (as simulated by storing the data at
    DeviceContext->DeviceData) and completes the request immediately. There is
    no need to use another queue for write operation.

Arguments:

    Device - Handle to a framework device object.
    Queue - Output pointer to a framework I/O queue handle, on success.

Return Value:
    NTSTATUS

--*/ 
{
    NTSTATUS                status;
    WDF_IO_QUEUE_CONFIG     queueConfig;
    WDF_OBJECT_ATTRIBUTES   queueAttributes;
    WDFQUEUE                queue;
    PMANUAL_QUEUE_CONTEXT   queueContext;

    WDF_IO_QUEUE_CONFIG_INIT(
        &queueConfig,
        WdfIoQueueDispatchManual);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
        &queueAttributes,
        MANUAL_QUEUE_CONTEXT);

    status = WdfIoQueueCreate(
        Device,
        &queueConfig,
        &queueAttributes,
        &queue);

    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }

    queueContext = GetManualQueueContext(queue);
    queueContext->Queue = queue;
    queueContext->DeviceContext = GetDeviceContext(Device);
    queueContext->Timer = NULL;

    *Queue = queue;
    return status;
}


VOID
EvtIoDeviceControl(
    _In_  WDFQUEUE          Queue,
    _In_  WDFREQUEST        Request,
    _In_  size_t            OutputBufferLength,
    _In_  size_t            InputBufferLength,
    _In_  ULONG             IoControlCode
    )
/*++
Routine Description:

    This event callback function is called when the driver receives an

    (KMDF) IOCTL_HID_Xxx code when handlng IRP_MJ_INTERNAL_DEVICE_CONTROL
    (UMDF) IOCTL_HID_Xxx, IOCTL_UMDF_HID_Xxx when handling IRP_MJ_DEVICE_CONTROL

Arguments:

    Queue - A handle to the queue object that is associated with the I/O request

    Request - A handle to a framework request object.

    OutputBufferLength - The length, in bytes, of the request's output buffer,
            if an output buffer is available.

    InputBufferLength - The length, in bytes, of the request's input buffer, if
            an input buffer is available.

    IoControlCode - The driver or system defined IOCTL associated with the request

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                status;
    BOOLEAN                 completeRequest = TRUE;
    WDFDEVICE               device = WdfIoQueueGetDevice(Queue);
    PDEVICE_CONTEXT         deviceContext = NULL;
    PQUEUE_CONTEXT          queueContext = GetQueueContext(Queue);
    UNREFERENCED_PARAMETER  (OutputBufferLength);
    UNREFERENCED_PARAMETER  (InputBufferLength);

    deviceContext = GetDeviceContext(device);

    switch (IoControlCode)
    {
    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:   // METHOD_NEITHER
        //
        // Retrieves the device's HID descriptor.
        //
        _Analysis_assume_(deviceContext->HidDescriptor.bLength != 0);
        status = RequestCopyFromBuffer(Request,
                            &deviceContext->HidDescriptor,
                            deviceContext->HidDescriptor.bLength);
        break;

    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:   // METHOD_NEITHER
        //
        //Retrieves a device's attributes in a HID_DEVICE_ATTRIBUTES structure.
        //
        status = RequestCopyFromBuffer(Request,
                            &queueContext->DeviceContext->HidDeviceAttributes,
                            sizeof(HID_DEVICE_ATTRIBUTES));
        break;

    case IOCTL_HID_GET_REPORT_DESCRIPTOR:   // METHOD_NEITHER
        //
        //Obtains the report descriptor for the HID device.
        //
        status = RequestCopyFromBuffer(Request,
                            deviceContext->ReportDescriptor,
                            deviceContext->HidDescriptor.DescriptorList[0].wReportLength);
        break;

    case IOCTL_HID_READ_REPORT:             // METHOD_NEITHER
        //
        // Returns a report from the device into a class driver-supplied
        // buffer.
        //
        status = ReadReport(queueContext, Request, &completeRequest);
        break;

    case IOCTL_HID_WRITE_REPORT:            // METHOD_NEITHER
        //
        // Transmits a class driver-supplied report to the device.
        //
        status = WriteReport(queueContext, Request);
        break;

#ifdef _KERNEL_MODE

    case IOCTL_HID_GET_FEATURE:             // METHOD_OUT_DIRECT

        status = GetFeature(queueContext, Request);
        break;

    case IOCTL_HID_SET_FEATURE:             // METHOD_IN_DIRECT

        status = SetFeature(queueContext, Request);
        break;

    case IOCTL_HID_GET_INPUT_REPORT:        // METHOD_OUT_DIRECT

        status = GetInputReport(queueContext, Request);
        break;

    case IOCTL_HID_SET_OUTPUT_REPORT:       // METHOD_IN_DIRECT

        status = SetOutputReport(queueContext, Request);
        break;

#else // UMDF specific

    //
    // HID minidriver IOCTL uses HID_XFER_PACKET which contains an embedded pointer.
    //
    //   typedef struct _HID_XFER_PACKET {
    //     PUCHAR reportBuffer;
    //     ULONG  reportBufferLen;
    //     UCHAR  reportId;
    //   } HID_XFER_PACKET, *PHID_XFER_PACKET;
    //
    // UMDF cannot handle embedded pointers when marshalling buffers between processes.
    // Therefore a special driver mshidumdf.sys is introduced to convert such IRPs to
    // new IRPs (with new IOCTL name like IOCTL_UMDF_HID_Xxxx) where:
    //
    //   reportBuffer - passed as one buffer inside the IRP
    //   reportId     - passed as a second buffer inside the IRP
    //
    // The new IRP is then passed to UMDF host and driver for further processing.
    //

    case IOCTL_UMDF_HID_GET_FEATURE:        // METHOD_NEITHER

        status = GetFeature(queueContext, Request);
        break;

    case IOCTL_UMDF_HID_SET_FEATURE:        // METHOD_NEITHER

        status = SetFeature(queueContext, Request);
        break;

    case IOCTL_UMDF_HID_GET_INPUT_REPORT:  // METHOD_NEITHER

        status = ReadReport(queueContext, Request, &completeRequest);
        break;

    case IOCTL_UMDF_HID_SET_OUTPUT_REPORT: // METHOD_NEITHER

        status = SetOutputReport(queueContext, Request);
        break;

#endif // _KERNEL_MODE

    case IOCTL_HID_GET_STRING:                      // METHOD_NEITHER

        status = GetString(Request);
        break;

    case IOCTL_HID_GET_INDEXED_STRING:              // METHOD_OUT_DIRECT

        status = GetIndexedString(Request);
        break;

    case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:  // METHOD_NEITHER
        //
        // This has the USBSS Idle notification callback. If the lower driver
        // can handle it (e.g. USB stack can handle it) then pass it down
        // otherwise complete it here as not inplemented. For a virtual
        // device, idling is not needed.
        //
        // Not implemented. fall through...
        //
    case IOCTL_HID_ACTIVATE_DEVICE:                 // METHOD_NEITHER
    case IOCTL_HID_DEACTIVATE_DEVICE:               // METHOD_NEITHER
    case IOCTL_GET_PHYSICAL_DESCRIPTOR:             // METHOD_OUT_DIRECT
        //
        // We don't do anything for these IOCTLs but some minidrivers might.
        //
        // Not implemented. fall through...
        //
    default:
        status = STATUS_NOT_IMPLEMENTED;
        break;
    }

    //
    // Complete the request. Information value has already been set by request
    // handlers.
    //
    if (completeRequest) {
        WdfRequestComplete(Request, status);
    }
}

NTSTATUS
RequestCopyFromBuffer(
    _In_  WDFREQUEST        Request,
    _In_  PVOID             SourceBuffer,
    _When_(NumBytesToCopyFrom == 0, __drv_reportError(NumBytesToCopyFrom cannot be zero))
    _In_  size_t            NumBytesToCopyFrom
    )
/*++

Routine Description:

    A helper function to copy specified bytes to the request's output memory

Arguments:

    Request - A handle to a framework request object.

    SourceBuffer - The buffer to copy data from.

    NumBytesToCopyFrom - The length, in bytes, of data to be copied.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                status;
    WDFMEMORY               memory;
    size_t                  outputBufferLength;

    status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if( !NT_SUCCESS(status) ) {
        KdPrint(("WdfRequestRetrieveOutputMemory failed 0x%x\n",status));
        return status;
    }

    WdfMemoryGetBuffer(memory, &outputBufferLength);
    if (outputBufferLength < NumBytesToCopyFrom) {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("RequestCopyFromBuffer: buffer too small. Size %d, expect %d\n",
                (int)outputBufferLength, (int)NumBytesToCopyFrom));
        return status;
    }

    status = WdfMemoryCopyFromBuffer(memory,
                                    0,
                                    SourceBuffer,
                                    NumBytesToCopyFrom);
    if( !NT_SUCCESS(status) ) {
        KdPrint(("WdfMemoryCopyFromBuffer failed 0x%x\n",status));
        return status;
    }

    WdfRequestSetInformation(Request, NumBytesToCopyFrom);
    return status;
}

NTSTATUS
ReadReport(
    _In_  PQUEUE_CONTEXT    QueueContext,
    _In_  WDFREQUEST        Request,
    _Always_(_Out_)
          BOOLEAN*          CompleteRequest
    )
/*++

Routine Description:

    Handles IOCTL_HID_READ_REPORT for the HID collection. Normally the request
    will be forwarded to a manual queue for further process. In that case, the
    caller should not try to complete the request at this time, as the request
    will later be retrieved back from the manually queue and completed there.
    However, if for some reason the forwarding fails, the caller still need
    to complete the request with proper error code immediately.

Arguments:

    QueueContext - The object context associated with the queue

    Request - Pointer to  Request Packet.

    CompleteRequest - A boolean output value, indicating whether the caller
            should complete the request or not

Return Value:

    NT status code.

--*/
{
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
    //GAZE_REPORT             gazeReport;

    //KdPrint(("ReadReport\n"));

    //
    // forward the request to manual queue
    //
    status = WdfRequestForwardToIoQueue(
        Request,
        QueueContext->DeviceContext->ManualQueue);

    if( !NT_SUCCESS(status) ) {
        KdPrint(("WdfRequestForwardToIoQueue failed with 0x%x\n", status));
        *CompleteRequest = TRUE;
        }
    else {
        *CompleteRequest = FALSE;
    }

    return status;
}

NTSTATUS
WriteReport(
    _In_  PQUEUE_CONTEXT    QueueContext,
    _In_  WDFREQUEST        Request
    )
/*++

Routine Description:

    Handles IOCTL_HID_WRITE_REPORT all the collection.

Arguments:

    QueueContext - The object context associated with the queue

    Request - Pointer to  Request Packet.

Return Value:

    NT status code.

--*/

{
    NTSTATUS                status;
    HID_XFER_PACKET         packet;
    ULONG                   reportSize;
    PHIDMINI_OUTPUT_REPORT  outputReport;

    KdPrint(("WriteReport\n"));

    status = RequestGetHidXferPacket_ToWriteToDevice(
                            Request,
                            &packet);
    if( !NT_SUCCESS(status) ) {
        return status;
    }

    if (packet.reportId != CONTROL_COLLECTION_REPORT_ID) {
        //
        // Return error for unknown collection
        //
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("WriteReport: unkown report id %d\n", packet.reportId));
        return status;
    }

    //
    // before touching buffer make sure buffer is big enough.
    //
    reportSize = sizeof(HIDMINI_OUTPUT_REPORT);

    if (packet.reportBufferLen < reportSize) {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("WriteReport: invalid input buffer. size %d, expect %d\n",
                            packet.reportBufferLen, reportSize));
        return status;
    }

    outputReport = (PHIDMINI_OUTPUT_REPORT)packet.reportBuffer;

    //
    // Store the device data in device extension.
    //
    QueueContext->DeviceContext->DeviceData = outputReport->Data;

    //
    // set status and information
    //
    WdfRequestSetInformation(Request, reportSize);
    return status;
}


HRESULT
GetFeature(
    _In_  PQUEUE_CONTEXT    QueueContext,
    _In_  WDFREQUEST        Request
    )
/*++

Routine Description:

    Handles IOCTL_HID_GET_FEATURE for all the collection.

Arguments:

    QueueContext - The object context associated with the queue

    Request - Pointer to  Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                status;
    HID_XFER_PACKET         packet;
    ULONG                   reportSize;
    PUCHAR                  reportData;
	
	UNREFERENCED_PARAMETER(QueueContext);
    // PHID_DEVICE_ATTRIBUTES  hidAttributes = &QueueContext->DeviceContext->HidDeviceAttributes;

    KdPrint(("GetFeature\n"));

    status = RequestGetHidXferPacket_ToReadFromDevice(
                            Request,
                            &packet);
    if( !NT_SUCCESS(status) ) {
        return status;
    }

    switch (packet.reportId)
    {
    case HID_USAGE_CAPABILITIES:
        reportSize = sizeof(CAPABILITIES_REPORT);
        reportData = (PUCHAR)&QueueContext->DeviceContext->CapabilitiesReport;
        break;
    case HID_USAGE_CONFIGURATION:
        reportSize = sizeof(CONFIGURATION_REPORT);
        reportData = (PUCHAR)&QueueContext->DeviceContext->ConfigurationReport;
        break;
    case HID_USAGE_TRACKER_STATUS:
        reportSize = sizeof(TRACKER_STATUS_REPORT);
        reportData = (PUCHAR)&QueueContext->DeviceContext->TrackerStatusReport;
        break;
    default:
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("GetFeature: invalid report id %d\n", packet.reportId));
        return status;
    }

    //
    // Since output buffer is for write only (no read allowed by UMDF in output
    // buffer), any read from output buffer would be reading garbage), so don't
    // let app embed custom control code in output buffer. The minidriver can
    // support multiple features using separate report ID instead of using
    // custom control code. Since this is targeted at report ID 1, we know it
    // is a request for getting attributes.
    //
    // While KMDF does not enforce the rule (disallow read from output buffer),
    // it is good practice to not do so.
    //

    if (packet.reportBufferLen < reportSize) {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("GetFeature: output buffer too small. Size %d, expect %d\n",
                            packet.reportBufferLen, reportSize));
        return status;
    }

    //
    // Since this device has one report ID, hidclass would pass on the report
    // ID in the buffer (it wouldn't if report descriptor did not have any report
    // ID). However, since UMDF allows only writes to an output buffer, we can't
    // "read" the report ID from "output" buffer. There is no need to read the
    // report ID since we get it other way as shown above, however this is
    // something to keep in mind.
    //

    memcpy(packet.reportBuffer, reportData, reportSize);

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, reportSize);
    return status;
}

NTSTATUS
SetFeature(
    _In_  PQUEUE_CONTEXT    QueueContext,
    _In_  WDFREQUEST        Request
    )
/*++

Routine Description:

    Handles IOCTL_HID_SET_FEATURE for all the collection.
    For control collection (custom defined collection) it handles
    the user-defined control codes for sideband communication

Arguments:

    QueueContext - The object context associated with the queue

    Request - Pointer to Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                status;
    HID_XFER_PACKET         packet;
    ULONG                   reportSize;
    PTRACKER_CONTROL_REPORT trackerControl;

    UNREFERENCED_PARAMETER(QueueContext);

    KdPrint(("SetFeature\n"));

    status = RequestGetHidXferPacket_ToWriteToDevice(
                            Request,
                            &packet);
    if( !NT_SUCCESS(status) ) {
        return status;
    }

    if (packet.reportId != HID_USAGE_TRACKER_CONTROL) {
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("SetFeature: invalid report id %d\n", packet.reportId));
        return status;
    }

    //
    // before touching control code make sure buffer is big enough.
    //
    reportSize = sizeof(TRACKER_CONTROL_REPORT);

    if (packet.reportBufferLen < reportSize) {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("SetFeature: invalid input buffer. size %d, expect %d\n",
                            packet.reportBufferLen, reportSize));
        return status;
    }

    trackerControl = (PTRACKER_CONTROL_REPORT)packet.reportBuffer;

    // TODO: Handle mode request

    return STATUS_SUCCESS;
}


NTSTATUS
SetOutputReport(
    _In_  PQUEUE_CONTEXT    QueueContext,
    _In_  WDFREQUEST        Request
    )
/*++

Routine Description:

    Handles IOCTL_HID_SET_OUTPUT_REPORT for all the collection.

Arguments:

    QueueContext - The object context associated with the queue

    Request - Pointer to Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                status;
    HID_XFER_PACKET         packet;
    ULONG                   reportSize;
    PHIDMINI_OUTPUT_REPORT  reportBuffer;

    KdPrint(("SetOutputReport\n"));

    status = RequestGetHidXferPacket_ToWriteToDevice(
                            Request,
                            &packet);
    if( !NT_SUCCESS(status) ) {
        return status;
    }

    if (packet.reportId != CONTROL_COLLECTION_REPORT_ID) {
        //
        // If collection ID is not for control collection then handle
        // this request just as you would for a regular collection.
        //
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("SetOutputReport: unkown report id %d\n", packet.reportId));
        return status;
    }

    //
    // before touching buffer make sure buffer is big enough.
    //
    reportSize = sizeof(HIDMINI_OUTPUT_REPORT);

    if (packet.reportBufferLen < reportSize) {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("SetOutputReport: invalid input buffer. size %d, expect %d\n",
                            packet.reportBufferLen, reportSize));
        return status;
    }

    reportBuffer = (PHIDMINI_OUTPUT_REPORT)packet.reportBuffer;

    QueueContext->OutputReport = reportBuffer->Data;

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, reportSize);
    return status;
}


NTSTATUS
GetStringId(
    _In_  WDFREQUEST        Request,
    _Out_ ULONG            *StringId,
    _Out_ ULONG            *LanguageId
    )
/*++

Routine Description:

    Helper routine to decode IOCTL_HID_GET_INDEXED_STRING and IOCTL_HID_GET_STRING.

Arguments:

    Request - Pointer to Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                status;
    ULONG                   inputValue;

#ifdef _KERNEL_MODE

    WDF_REQUEST_PARAMETERS  requestParameters;

    //
    // IOCTL_HID_GET_STRING:                      // METHOD_NEITHER
    // IOCTL_HID_GET_INDEXED_STRING:              // METHOD_OUT_DIRECT
    //
    // The string id (or string index) is passed in Parameters.DeviceIoControl.
    // Type3InputBuffer. However, Parameters.DeviceIoControl.InputBufferLength
    // was not initialized by hidclass.sys, therefore trying to access the
    // buffer with WdfRequestRetrieveInputMemory will fail
    //
    // Another problem with IOCTL_HID_GET_INDEXED_STRING is that METHOD_OUT_DIRECT
    // expects the input buffer to be Irp->AssociatedIrp.SystemBuffer instead of
    // Type3InputBuffer. That will also fail WdfRequestRetrieveInputMemory.
    //
    // The solution to the above two problems is to get Type3InputBuffer directly
    //
    // Also note that instead of the buffer's content, it is the buffer address
    // that was used to store the string id (or index)
    //

    WDF_REQUEST_PARAMETERS_INIT(&requestParameters);
    WdfRequestGetParameters(Request, &requestParameters);

    inputValue = (ULONG)requestParameters.Parameters.DeviceIoControl.Type3InputBuffer;

    status = STATUS_SUCCESS;

#else

    WDFMEMORY               inputMemory;
    size_t                  inputBufferLength;
    PVOID                   inputBuffer;

    //
    // mshidumdf.sys updates the IRP and passes the string id (or index) through
    // the input buffer correctly based on the IOCTL buffer type
    //

    status = WdfRequestRetrieveInputMemory(Request, &inputMemory);
    if( !NT_SUCCESS(status) ) {
        KdPrint(("WdfRequestRetrieveInputMemory failed 0x%x\n",status));
        return status;
    }
    inputBuffer = WdfMemoryGetBuffer(inputMemory, &inputBufferLength);

    //
    // make sure buffer is big enough.
    //
    if (inputBufferLength < sizeof(ULONG))
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("GetStringId: invalid input buffer. size %d, expect %d\n",
                            (int)inputBufferLength, (int)sizeof(ULONG)));
        return status;
    }

    inputValue = (*(PULONG)inputBuffer);

#endif

    //
    // The least significant two bytes of the INT value contain the string id.
    //
    *StringId = (inputValue & 0x0ffff);

    //
    // The most significant two bytes of the INT value contain the language
    // ID (for example, a value of 1033 indicates English).
    //
    *LanguageId = (inputValue >> 16);

    return status;
}


NTSTATUS
GetIndexedString(
    _In_  WDFREQUEST        Request
    )
/*++

Routine Description:

    Handles IOCTL_HID_GET_INDEXED_STRING

Arguments:

    Request - Pointer to Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                status;
    ULONG                   languageId, stringIndex;

    status = GetStringId(Request, &stringIndex, &languageId);

    // While we don't use the language id, some minidrivers might.
    //
    UNREFERENCED_PARAMETER(languageId);

    if (NT_SUCCESS(status)) {

        if (stringIndex != VHIDMINI_DEVICE_STRING_INDEX)
        {
            status = STATUS_INVALID_PARAMETER;
            KdPrint(("GetString: unkown string index %d\n", stringIndex));
            return status;
        }

        status = RequestCopyFromBuffer(Request, VHIDMINI_DEVICE_STRING, sizeof(VHIDMINI_DEVICE_STRING));
    }
    return status;
}


NTSTATUS
GetString(
    _In_  WDFREQUEST        Request
    )
/*++

Routine Description:

    Handles IOCTL_HID_GET_STRING.

Arguments:

    Request - Pointer to Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                status;
    ULONG                   languageId, stringId;
    size_t                  stringSizeCb;
    PWSTR                   string;

    status = GetStringId(Request, &stringId, &languageId);

    // While we don't use the language id, some minidrivers might.
    //
    UNREFERENCED_PARAMETER(languageId);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    switch (stringId){
    case HID_STRING_ID_IMANUFACTURER:
        stringSizeCb = sizeof(VHIDMINI_MANUFACTURER_STRING);
        string = VHIDMINI_MANUFACTURER_STRING;
        break;
    case HID_STRING_ID_IPRODUCT:
        stringSizeCb = sizeof(VHIDMINI_PRODUCT_STRING);
        string = VHIDMINI_PRODUCT_STRING;
        break;
    case HID_STRING_ID_ISERIALNUMBER:
        stringSizeCb = sizeof(VHIDMINI_SERIAL_NUMBER_STRING);
        string = VHIDMINI_SERIAL_NUMBER_STRING;
        break;
    default:
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("GetString: unkown string id %d\n", stringId));
        return status;
    }

    status = RequestCopyFromBuffer(Request, string, stringSizeCb);
    return status;
}


NTSTATUS
CheckRegistryForDescriptor(
        _In_ WDFDEVICE Device
        )
/*++

Routine Description:

    Read "ReadFromRegistry" key value from device parameters in the registry.

Arguments:

    device - pointer to a device object.

Return Value:

    NT status code.

--*/

{
    WDFKEY          hKey = NULL;
    NTSTATUS        status;
    UNICODE_STRING  valueName;
    ULONG           value;

    status = WdfDeviceOpenRegistryKey(Device,
                                  PLUGPLAY_REGKEY_DEVICE,
                                  KEY_READ,
                                  WDF_NO_OBJECT_ATTRIBUTES,
                                  &hKey);
    if (NT_SUCCESS(status)) {

        RtlInitUnicodeString(&valueName, L"ReadFromRegistry");

        status = WdfRegistryQueryULong (hKey,
                                  &valueName,
                                  &value);

        if (NT_SUCCESS (status)) {
            if (value == 0) {
                status = STATUS_UNSUCCESSFUL;
            }
        }

        WdfRegistryClose(hKey);
    }

    return status;
}

NTSTATUS
ReadDescriptorFromRegistry(
        _In_ WDFDEVICE Device
        )
/*++

Routine Description:

    Read HID report descriptor from registry

Arguments:

    device - pointer to a device object.

Return Value:

    NT status code.

--*/
{
    WDFKEY          hKey = NULL;
    NTSTATUS        status;
    UNICODE_STRING  valueName;
    WDFMEMORY       memory;
    size_t          bufferSize;
    PVOID           reportDescriptor;
    PDEVICE_CONTEXT deviceContext;
    WDF_OBJECT_ATTRIBUTES   attributes;

    deviceContext = GetDeviceContext(Device);

    status = WdfDeviceOpenRegistryKey(Device,
                                  PLUGPLAY_REGKEY_DEVICE,
                                  KEY_READ,
                                  WDF_NO_OBJECT_ATTRIBUTES,
                                  &hKey);

    if (NT_SUCCESS(status)) {

        RtlInitUnicodeString(&valueName, L"MyReportDescriptor");

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = Device;

        status = WdfRegistryQueryMemory (hKey,
                                  &valueName,
                                  NonPagedPool,
                                  &attributes,
                                  &memory,
                                  NULL);

        if (NT_SUCCESS (status)) {

            reportDescriptor = WdfMemoryGetBuffer(memory, &bufferSize);

            KdPrint(("No. of report descriptor bytes copied: %d\n", (INT) bufferSize));

            //
            // Store the registry report descriptor in the device extension
            //
            deviceContext->ReadReportDescFromRegistry = TRUE;
            deviceContext->ReportDescriptor = reportDescriptor;
            deviceContext->HidDescriptor.DescriptorList[0].wReportLength = (USHORT)bufferSize;
        }

        WdfRegistryClose(hKey);
    }

    return status;
}

#include <initguid.h>
#include <cfgmgr32.h>
DEFINE_GUID(GUID_CLASS_MONITOR, 0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18);

void
GetPrimaryMonitorInfo(
    _In_ PDEVICE_CONTEXT DeviceContext
)
{
    HDEVINFO devInfo = SetupDiGetClassDevsEx(&GUID_CLASS_MONITOR, NULL, NULL, DIGCF_PRESENT | DIGCF_PROFILE, NULL, NULL, NULL);

    if (NULL == devInfo)
    {
        return;
    }

    for (ULONG i = 0; ERROR_NO_MORE_ITEMS != GetLastError(); ++i)
    {
        SP_DEVINFO_DATA devInfoData;

        memset(&devInfoData, 0, sizeof(devInfoData));
        devInfoData.cbSize = sizeof(devInfoData);

        if (!SetupDiEnumDeviceInfo(devInfo, i, &devInfoData))
        {
            return;
        }
        TCHAR Instance[MAX_DEVICE_ID_LEN];
        if (!SetupDiGetDeviceInstanceId(devInfo, &devInfoData, Instance, MAX_PATH, NULL))
        {
            return;
        }

        HKEY hEDIDRegKey = SetupDiOpenDevRegKey(devInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

        if (!hEDIDRegKey || (hEDIDRegKey == INVALID_HANDLE_VALUE))
        {
            continue;
        }

        BYTE EDIDdata[1024];
        DWORD edidsize = sizeof(EDIDdata);

        if (ERROR_SUCCESS != RegQueryValueEx(hEDIDRegKey, L"EDID", NULL, NULL, EDIDdata, &edidsize))
        {
            RegCloseKey(hEDIDRegKey);
            continue;
        }

        DeviceContext->ConfigurationReport.DisplayManufacturerId = (EDIDdata[0x08] << 8) + EDIDdata[0x09];
        DeviceContext->ConfigurationReport.DisplayProductId = (EDIDdata[0x0A] << 8) + EDIDdata[0x0B];
        DeviceContext->ConfigurationReport.DisplaySerialNumber = (EDIDdata[0x0C] << 24) + (EDIDdata[0x0D] << 16) + (EDIDdata[0x0E] << 8) + EDIDdata[0x0F];
        DeviceContext->ConfigurationReport.DisplayManufacturerDate = (EDIDdata[0x11] << 8) + EDIDdata[0x10];
        DeviceContext->ConfigurationReport.CalibratedScreenWidth = (((EDIDdata[68] & 0xF0) << 4) + EDIDdata[66]) * 1000; // width in micrometers
        DeviceContext->ConfigurationReport.CalibratedScreenHeight = (((EDIDdata[68] & 0x0F) << 8) + EDIDdata[67]) * 1000; // height in micrometers

        KdPrint(("VHIDMINI - Calibrated Screen W %dum H %dum\n", DeviceContext->ConfigurationReport.CalibratedScreenWidth, 
                                                             DeviceContext->ConfigurationReport.CalibratedScreenHeight));

        RegCloseKey(hEDIDRegKey);

        // this only handles the case of the primary monitor
        break;
    }
    SetupDiDestroyDeviceInfoList(devInfo);
}

void
SendGazeReport(
    _In_ PDEVICE_CONTEXT DeviceContext,
    _In_ PGAZE_REPORT    GazeReport
)
{
    NTSTATUS status;
    PMANUAL_QUEUE_CONTEXT queueContext;
    WDFREQUEST request;

    queueContext = GetManualQueueContext(DeviceContext->ManualQueue);
    status = WdfIoQueueRetrieveNextRequest(queueContext->Queue, &request);
    if (NT_SUCCESS(status))
    {
        status = RequestCopyFromBuffer(request, GazeReport, sizeof(*GazeReport));
        WdfRequestComplete(request, status);
    }
}

void
SendTrackerStatusReport(
    _In_ PDEVICE_CONTEXT DeviceContext,
    _In_ uint8_t         TrackerStatus
    )
{
    NTSTATUS status;
    PMANUAL_QUEUE_CONTEXT queueContext;
    WDFREQUEST request;

    DeviceContext->TrackerStatusReport.ConfigurationStatus = TrackerStatus;

    queueContext = GetManualQueueContext(DeviceContext->ManualQueue);
    status = WdfIoQueueRetrieveNextRequest(queueContext->Queue, &request);
    if (NT_SUCCESS(status))
    {
        status = RequestCopyFromBuffer(request, &DeviceContext->TrackerStatusReport, sizeof(TRACKER_STATUS_REPORT));
        WdfRequestComplete(request, status);
    }
}