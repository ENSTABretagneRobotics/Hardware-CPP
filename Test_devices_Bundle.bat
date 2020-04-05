cd /d "%~dp0"
7z a -y "..\Test_Devices_Bundle.zip" "..\Test_Devices" -xr!"Debug" -xr!"Release" -xr!".vs" -xr!"Test_Devices.ncb" -xr!"Test_Devices.sdf" -xr!"Test_Devices.opensdf" -xr!"Test_Devices.VC.db" -xr!"*.git*"
pause
exit
