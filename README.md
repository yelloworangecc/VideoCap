# Table of Contents

1.  [Change logs](#orgec50f4c)
    1.  [Version 1.2](#orgd87854e)
    2.  [Version 1.1](#org7c6ded8)
2.  [Overview](#org66c1b02)
3.  [Run VideoCap](#orgde15965)
4.  [Change Format](#org20755bc)
5.  [Change Resolution](#org726bc7b)
6.  [Snap and Capture](#org22502d6)
7.  [Configuration](#org8a313b2)
8.  [CPU and Memory usage](#org57491c1)

<a id="orgec50f4c"></a>

# Change logs

<a id="orgd87854e"></a>

## Version 1.2

1.  fixed crash when no device available
2.  refined UI
3.  supported Chinese language
4.  fixed the displaying stucked problem happened somtimes after switching device

<a id="org7c6ded8"></a>

## Version 1.1

1.  added a json config file
2.  removed all command line options
3.  used MP4 insteaded of AVI
4.  supported hardware snap button
5.  solved memory leaking problem
6.  solved window painting problem

<a id="org66c1b02"></a>

# Overview

This software captures video from any video device which can be recognized by Windows 10. User also can save image or video files. If there are more than two video devices, user can switch between devices. Besides, user can also configure the camera output format and resolution.

<a id="orgde15965"></a>

# Run VideoCap

Just double click "VideoCap.exe", the default video device will be opened. If has 2 or more video devices, user can switch between these devices.

<a id="org20755bc"></a>

# Change Format

Many cameras support several kinds of output format. For example, Compressed format like MJPG and raw format like YUY2 and RGB24. Change the format setting can influence the fluency and quality a little bit.

<a id="org726bc7b"></a>

# Change Resolution

Many cameras can output image data with different resolutons. Usually High resoluton result in low frame rate.

By default the width of image area of the Window is 570 pixels. Images with width less than 530 pixel will be stretched to fit the Window area. If the width is more than 570, then the Window size will be changed to fit the image resolution.

<a id="org22502d6"></a>

# Snap and Capture

-   Click "Snapshot" button to save a JPG file.
-   Press a hardware snapshot button on the video device to save a BMP file.
-   Click "Record" to start recording a MP4 file. After that, the button text change to "Stop". When the recording is finished, click "Stop" then you will get a MP4 file.

If the "SavePath" keyword is not configured in the config file, all JPG, BMP and MP4 files are saved to the same direcotry of "VideoCap.exe" 

<a id="org8a313b2"></a>

# Configuration

When first running, the application will generate a json-formated configure file with default settings. The name of the file is "VideoCap.json". Blow is an example of the file content.

    {
      "DeviceName": "Integrated Camera",
      "Format": "YUY2",
      "Resolution": "640 x 480",
      "SavePath": "",
      "LanguageCode": "en"
    }

When the application is running, if user changing the selection, config file will be updated when application is exiting. As a result, user don't need to change the most of the content except the items listed in blow list.

-   SavePath: For changing the save path of files. Please note the format of the path string. Here is an example `D:\\OE\\captures\\`.
-   LanguageCode: For changing the UI language. The default code is "en", Chinese is supported with code "zh".

<a id="org57491c1"></a>

# CPU and Memory usage

If using compressed format like MPEG, more CPU resources is needed but less memory resources. Blow is two statistics based on experiment.

<table border="2" cellspacing="0" cellpadding="6" rules="groups" frame="hsides">
<colgroup>
<col  class="org-left" />

<col  class="org-left" />

<col  class="org-left" />

<col  class="org-right" />

<col  class="org-left" />
</colgroup>
<tbody>
<tr>
<td class="org-left">Device Name</td>
<td class="org-left">Format</td>
<td class="org-left">Resolution</td>
<td class="org-right">AVG CPU</td>
<td class="org-left">AVG Memory</td>
</tr>

<tr>
<td class="org-left">Intergrated Camera</td>
<td class="org-left">MJPG</td>
<td class="org-left">640 x 480</td>
<td class="org-right">3%-4%</td>
<td class="org-left">25 MB</td>
</tr>

<tr>
<td class="org-left">Intergrated Camera</td>
<td class="org-left">YUY2</td>
<td class="org-left">640 x 480</td>
<td class="org-right">2%-3%</td>
<td class="org-left">31 MB</td>
</tr>
</tbody>
</table>
