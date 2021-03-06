; Script generated by the Inno Script Studio Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "HISE"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Hart Instruments"
#define MyAppURL "http://hise.audio"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId= {{01F93846-3497-4390-BB80-6456826EABA0}

AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile="..\..\license.txt"
ArchitecturesInstallIn64BitMode=x64
SetupLogging=yes
OutputBaseFilename=RenameInstaller
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Components]
Name: "x86Plugin"; Description: "32bit VST Plugin Stereo Out"; Types: compact custom full
Name: "x64Plugin"; Description: "64bit VST Plugin Stereo Out"; Types: compact custom full
Name: "x86PluginMC"; Description: "32bit VST Plugin 16 Out"; Types: compact custom full
Name: "x64PluginMC"; Description: "64bit VST Plugin 16 Out"; Types: compact custom full
Name: "Standalone64bit"; Description: "64bit Standalone application"; ExtraDiskSpaceRequired: 9000; Types: full compact custom
Name: "Standalone32bit"; Description: "32bit Standalone application"; ExtraDiskSpaceRequired: 9000

[Files]

Source: "..\..\..\Installer\Windows\files\HISE.exe"; DestDir: "{app}"; Flags: 64bit; Components: Standalone64bit
Source: "..\..\..\Installer\Windows\files\HISE x86.exe"; DestDir: "{app}"; Flags: 32bit; Components: Standalone32bit
Source: "..\..\..\Installer\Windows\files\HISE x64.dll"; DestDir: "{code:Getx64bitDir}"; Flags: 64bit; Components: x64Plugin
Source: "..\..\..\Installer\Windows\files\HISE x64 16 Out.dll"; DestDir: "{code:Getx64bitDir}"; Flags: 64bit; Components: x64PluginMC
Source: "..\..\..\Installer\Windows\files\HISE x86.dll"; DestDir: "{code:Getx86bitDir}"; Flags: 32bit; Components: x86Plugin
Source: "..\..\..\Installer\Windows\files\HISE x86 16 Out.dll"; DestDir: "{code:Getx86bitDir}"; Flags: 32bit; Components: x86PluginMC

[Icons]
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"; 
Name: "{group}\HISE x86"; Filename: "{app}\HISE x86.exe"; Components: Standalone32bit
Name: "{group}\HISE"; Filename: "{app}\HISE.exe"; Components: Standalone64bit

[Dirs]

[Code]
var x86Page: TInputDirWizardPage;
var x64Page: TInputDirWizardPage;

procedure InitializeWizard;
begin
  // Create the page



  x86Page := CreateInputDirPage(wpSelectComponents,
    'Select VST 32bit Plugin Folder', 'Where should the 32bit version of the VST Plugin be installed?',
    'Select your VST 32bit Folder',
    False, '');
  x86Page.Add('');

  x86Page.Values[0] := GetPreviousData('x86', '');

  x64Page := CreateInputDirPage(wpSelectComponents,
    'Select VST 64bit Plugin Folder', 'Where should the 64bit version of the VST Plugin be installed?',
    'Select your VST 64bit Folder',
    False, '');
  x64Page.Add('');

  x64Page.Values[0] := GetPreviousData('x64', '');

end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin

  if PageFromID(PageID) = x64Page then Result := not isComponentSelected('x64Plugin');
  if PageFromID(PageID) = x86Page then Result := not isComponentSelected('x86Plugin');

end;


function NextButtonClick(CurPageID: Integer): Boolean;
begin



  // Set default folder if empty
  if x86Page.Values[0] = '' then
     x86Page.Values[0] := ExpandConstant('{pf32}\VSTPlugins');
  Result := True;

  if (IsWin64) and (x64Page.Values[0] = '') then
     x64Page.Values[0] := ExpandConstant('{pf64}\VSTPlugins');
  Result := True;
end;

function Getx86bitDir(Param: String): String;
begin
  { Return the selected DataDir }
  Result := x86Page.Values[0];
end;

function Getx64bitDir(Param: String): String;
begin
  { Return the selected DataDir }
  Result := x64Page.Values[0];
end;