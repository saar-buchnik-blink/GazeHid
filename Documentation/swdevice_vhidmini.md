# HID Minidriver Eye Tracker Sample (UMDF V2)

This *HID minidriver* sample demonstrates how to write a HID minidriver using User-Mode Driver Framework (UMDF) for eye trackers. This is a reference implementation to match the HID specification for eye trackers.

## Technical Notes

The UMDF driver relies on the driver obtaining data from other sources. The [GhostHid](../drivers/ghost/GhostHid.c) driver generates data itself, while the various other examples ([Tobii](../drivers/tobii/tobii.c), [EyeTech DS](../drivers/eyetech/eyetech.c), [GazePoint](../drivers/gazepoint/gazepoint.c)) query their associated proprietary API to obtain data.

## Compiling

To compile, you must install the [Windows Driver Kit](https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) and you must [install Spectre mitigation libraries](https://devblogs.microsoft.com/cppblog/spectre-mitigations-in-msvc/).

Next, open and build *GazeHid.sln*. Be sure to build for the appropriate platform, such as x64 or x86.

## Driver Installation

After compiling one of the drivers, you must navigate to to the binary output directory of the built drivers. Right click on the `.inf` file and select install, then follow the prompts.

![Driver Install](assets/images/Driver_Install.png)

Note: You will need to turn on TESTSIGNING (*Bcdedit.exe -set TESTSIGNING ON* from an administrative prompt) to use this driver in test scenarios.

## Running

The sofware device driver (swdevice.exe) needs no arguments to run. Also note that `swdevice.exe` needs to be run from an administrative prompt or from Visual Studio running in administrator mode.

Once running, you will see a prompt similar to this:

![swdevice running](assets/images/Running_Swdevice.png)

Once the swdevice is running, change the driver to the one you installed previously. First, open the device manager and open the `Software Devices` section. There you should see an entry for a `HID Eye Tracker Device`. Right click on it and select properties.

![HID Eye Tracker Device](assets/images/HID_Eye_Tracker_Device.png)

On the driver tab, select `Update Driver`.

![Update Driver](assets/images/Update_Driver.png)

In the subsequent dialog, first select `Browse my computer for driver software`.

![Browse my computer for driver software](assets/images/Browse_for_Driver.png)

On the following dialog, you should see the UMDF driver that was installed in the prior section. Select it and hit `Next`.

![Select Driver](assets/images/Select_Driver.png)

If everything worked correctly, you should now be able to find your device has moved to the `Human Interface Device` section and has been renamed to match your selected driver.

![Driver Installed](assets/images/Driver_Installed.png)

## Testing

An easy way to see the driver working is to install the [Windows Community Toolkit Sample App](https://www.microsoft.com/en-us/p/windows-community-toolkit-sample-app/9nblggh4tlcq). Once 
installed, open the app and navitage to Gaze->Gaze Tracing. If prompted, be sure to authorize eye gaze for the application. If the everything is working properly
you should see a series of dots bouncing around the screen. The dots represent the gaze data being sent from [GhostHidFrameProc](drivers/ghost/GhostHid.c).

![Windows Community Toolkit - Test](assets/images/Windows_Community_Toolkit_Test.png)

*textvhid* is a tool which dumps lots of information about the driver - raw report data, structure, results of HID queries, etc. 

## Related Topics

[Creating UMDF-based HID Minidrivers](http://msdn.microsoft.com/en-us/library/windows/hardware/hh439579)

[Human Input Devices Design Guide](http://msdn.microsoft.com/en-us/library/windows/hardware/ff539952)

[Human Input Devices Reference](http://msdn.microsoft.com/en-us/library/windows/hardware/ff539956)

[UMDF HID Minidriver IOCTLs](http://msdn.microsoft.com/en-us/library/windows/hardware/hh463977)

## Contributing

This project welcomes contributions and suggestions. Most contributions require you to
agree to a Contributor License Agreement (CLA) declaring that you have the right to,
and actually do, grant us the rights to use your contribution. For details, visit
https://cla.microsoft.com.

When you submit a pull request, a CLA-bot will automatically determine whether you need
to provide a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the
instructions provided by the bot. You will only need to do this once across all repositories using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/)
or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
