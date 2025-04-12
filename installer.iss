#define MyAppName "RayTracerNG"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Nischaya Garg"
#define MyAppURL "https://raytracerng.vercel.app"
#define MyAppExeName "RayTracerNG.exe"
#define MyAppIconName "RayTracer_Icon.ico"

[Setup]
AppId={{C89A7F7E-3B3F-4B68-8E4A-9E0D12A1A34F}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=LICENSE
OutputDir=installer
OutputBaseFilename=RayTracerNG_Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
SetupIconFile=assets\{#MyAppIconName}
DisableWelcomePage=no
DisableDirPage=no
WizardImageFile=assets\RayTracer_Welcome.bmp
WizardSmallImageFile=assets\RayTracer_Header.bmp

[CustomMessages]
WebsiteLabel=🌐 Visit Official Website
GithubLabel=📦 View Source on GitHub
AuthorLabel=👨‍💻 Made by Nischaya Garg - Stay Innovated, Keep Coding, Think BIG!
LinkedInLabel=💼 Connect on LinkedIn
NetworkSpeedMsg=⚠️ Note: Installation speed depends on your network connection. Please be patient.
SupportLabel=📧 For any issues encountered, mail at:
SupportEmail=nischayagarg008@gmail.com
ProjectDesc=RayTracerNG is a high-performance, real-time 2D ray tracing application built with C++17 and modern OpenGL. It simulates realistic light behavior and reflections in an interactive scene, allowing users to manipulate light sources and obstacles dynamically.%n%nFeaturing an ImGui-powered UI, real-time performance monitoring, and a lightweight, cross-platform build system, RayTracerNG is perfect for learning, showcasing graphics skills, or exploring ray physics in 2D environments.

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Source files for building
Source: "src\*"; DestDir: "{app}\src"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "CMakeLists.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "build.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "script.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "build_installer.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "SDL3.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "assets\RayTracer_Welcome.bmp"; Flags: dontcopy
Source: "assets\RayTracer_Header.bmp"; Flags: dontcopy
Source: "assets\{#MyAppIconName}"; DestDir: "{app}\assets"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\build\{#MyAppExeName}"; IconFilename: "{app}\assets\{#MyAppIconName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\build\{#MyAppExeName}"; IconFilename: "{app}\assets\{#MyAppIconName}"; Tasks: desktopicon

[Run]
; Create necessary directories first
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\cmake"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\mingw64"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\python"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\glfw\glfw-3.3.8"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\glm\glm-0.9.9.8"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\imgui\imgui-docking"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\glad-gen"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\glad-gen\include"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\glad-gen\include\glad"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\glad-gen\src"" 2>nul"; Flags: runhidden

; Download and extract dependencies using cmd.exe instead of PowerShell
Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{tmp}\cmake.zip"" https://github.com/Kitware/CMake/releases/download/v3.28.1/cmake-3.28.1-windows-x86_64.zip"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c tar -xf ""{tmp}\cmake.zip"" -C ""{app}\libs\cmake"" --strip-components=1"; Flags: runhidden

; Download MinGW
Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{tmp}\mingw.7z"" https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev1/x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev1.7z"; Flags: runhidden

; Download 7-Zip standalone executable
Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{tmp}\7zr.exe"" https://www.7-zip.org/a/7zr.exe"; Flags: runhidden

; Extract MinGW directly to the target directory
Filename: "{tmp}\7zr.exe"; Parameters: "x ""{tmp}\mingw.7z"" -o""{tmp}\mingw_temp"" -y"; Flags: runhidden waituntilterminated

; Move contents from the nested directory to the final location
Filename: "cmd.exe"; Parameters: "/c xcopy /E /I /Y ""{tmp}\mingw_temp\mingw64\*"" ""{app}\libs\mingw64"""; Flags: runhidden waituntilterminated

; Clean up temp directory
Filename: "cmd.exe"; Parameters: "/c rmdir /S /Q ""{tmp}\mingw_temp"""; Flags: runhidden waituntilterminated

; Download and extract Python
Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{tmp}\python.zip"" https://www.python.org/ftp/python/3.11.7/python-3.11.7-embed-amd64.zip"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c tar -xf ""{tmp}\python.zip"" -C ""{app}\libs\python"""; Flags: runhidden

; Create Python site-packages directory
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\python\Lib"" 2>nul"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c mkdir ""{app}\libs\python\Lib\site-packages"" 2>nul"; Flags: runhidden

; Download and extract get-pip.py
Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{app}\libs\python\get-pip.py"" https://bootstrap.pypa.io/get-pip.py"; Flags: runhidden

; Download GLAD files directly
Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{app}\libs\glad-gen\include\glad\glad.h"" https://raw.githubusercontent.com/Dav1dde/glad/c/include/glad/glad.h"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{app}\libs\glad-gen\include\glad\khrplatform.h"" https://raw.githubusercontent.com/Dav1dde/glad/c/include/KHR/khrplatform.h"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{app}\libs\glad-gen\src\glad.c"" https://raw.githubusercontent.com/Dav1dde/glad/c/src/glad.c"; Flags: runhidden

; Download and extract other dependencies
Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{tmp}\glfw.zip"" https://github.com/glfw/glfw/archive/refs/tags/3.3.8.zip"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c tar -xf ""{tmp}\glfw.zip"" -C ""{app}\libs\glfw\glfw-3.3.8"" --strip-components=1"; Flags: runhidden

Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{tmp}\glm.zip"" https://github.com/g-truc/glm/archive/refs/tags/0.9.9.8.zip"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c tar -xf ""{tmp}\glm.zip"" -C ""{app}\libs\glm\glm-0.9.9.8"" --strip-components=1"; Flags: runhidden

Filename: "cmd.exe"; Parameters: "/c curl -L -o ""{tmp}\imgui.zip"" https://github.com/ocornut/imgui/archive/refs/heads/docking.zip"; Flags: runhidden
Filename: "cmd.exe"; Parameters: "/c tar -xf ""{tmp}\imgui.zip"" -C ""{app}\libs\imgui\imgui-docking"" --strip-components=1"; Flags: runhidden

; Set environment variables using cmd.exe - use set instead of setx for immediate effect
Filename: "cmd.exe"; Parameters: "/c set ""PATH=%PATH%;{app}\libs\cmake\bin;{app}\libs\mingw64\bin;{app}\libs\python"" && ""{app}\build_installer.bat"" ""{app}"""; Flags: runhidden

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Code]
var
  NetworkLabel: TNewStaticText;  // Declare this globally
  SupportLabel, SupportEmailLink: TNewStaticText;  // Add these global variables
  ProjectDescLabel: TNewStaticText;  // Add this with other global variables

// Add email click handler first
procedure EmailLinkClick(Sender: TObject);
var
  ErrorCode: Integer;
begin
  ShellExec('open', 'mailto:' + ExpandConstant('{cm:SupportEmail}'), '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

// Then declare the URLLabelClick procedure
procedure URLLabelClick(Sender: TObject);
var
  ErrorCode: Integer;
  Text: String;
begin
  Text := TNewStaticText(Sender).Caption;
  // Extract the URL from the second line
  Text := Copy(Text, Pos(#13#10, Text) + 2, Length(Text));
  ShellExec('open', Text, '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

// Then the InitializeSetup function
function InitializeSetup(): Boolean;
var
  ResultCode: Integer;
begin
  Result := True;
  
  // Check if PowerShell is available by trying to run a simple command
  if not Exec('cmd.exe', '/c powershell -Command "exit"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
  begin
    MsgBox('PowerShell is not available on your system. This installer requires PowerShell to function properly.' + #13#10 + #13#10 +
           'Please install PowerShell and try again. You can download it from:' + #13#10 +
           'https://github.com/PowerShell/PowerShell/releases', mbError, MB_OK);
    Result := False;
    Exit;
  end;
end;

// Modify CreateNetworkSpeedLabel procedure
procedure CreateNetworkSpeedLabel;
begin
  NetworkLabel := TNewStaticText.Create(WizardForm);
  NetworkLabel.Caption := ExpandConstant('{cm:NetworkSpeedMsg}');
  NetworkLabel.Parent := WizardForm.InstallingPage;
  NetworkLabel.Font.Name := 'Segoe UI';
  NetworkLabel.Font.Size := 8;
  NetworkLabel.Font.Style := [fsItalic];
  NetworkLabel.Font.Color := $404040;
  
  // Adjust positioning to be properly aligned under the progress bar
  NetworkLabel.Top := WizardForm.ProgressGauge.Top + WizardForm.ProgressGauge.Height + 12;
  NetworkLabel.Left := WizardForm.ProgressGauge.Left;
  NetworkLabel.Width := WizardForm.ProgressGauge.Width;
  NetworkLabel.AutoSize := False;  // Disable autosize to ensure proper width
  NetworkLabel.WordWrap := True;
  NetworkLabel.Height := 32;  // Give enough height for two lines if needed
  NetworkLabel.Caption := ExpandConstant('{cm:NetworkSpeedMsg}');  // Use the custom message directly
  NetworkLabel.Visible := False;  // Hide it initially
end;
  
// Add this new procedure to handle page changes
procedure CurPageChanged(CurPageID: Integer);
begin
  if NetworkLabel <> nil then
    NetworkLabel.Visible := CurPageID = wpInstalling;  // Only show on installing page
    
  // Support info visibility
  if (SupportLabel <> nil) and (SupportEmailLink <> nil) then
  begin
    SupportLabel.Visible := CurPageID = wpFinished;
    SupportEmailLink.Visible := CurPageID = wpFinished;
  end;
end;

// Move CreateSupportInfo procedure before InitializeWizard
procedure CreateSupportInfo;
var
  LinkFont: TFont;
begin
  // Create and configure the font
  LinkFont := TFont.Create;
  LinkFont.Name := 'Segoe UI';
  LinkFont.Size := 9;
  LinkFont.Style := [fsBold];
  
  // Create support label
  SupportLabel := TNewStaticText.Create(WizardForm);
  SupportLabel.Caption := ExpandConstant('{cm:SupportLabel}');
  SupportLabel.Parent := WizardForm.FinishedPage;
  SupportLabel.Font.Assign(LinkFont);
  SupportLabel.Font.Color := $404040;  // Dark gray
  SupportLabel.Top := WizardForm.FinishedLabel.Top + WizardForm.FinishedLabel.Height + 20;
  SupportLabel.Left := WizardForm.FinishedLabel.Left;
  
  // Create email link
  SupportEmailLink := TNewStaticText.Create(WizardForm);
  SupportEmailLink.Caption := ExpandConstant('{cm:SupportEmail}');
  SupportEmailLink.Cursor := crHand;
  SupportEmailLink.Parent := WizardForm.FinishedPage;
  LinkFont.Style := [fsBold, fsUnderline];  // Add underline for the email
  SupportEmailLink.Font.Assign(LinkFont);
  SupportEmailLink.Font.Color := $404040;  // Dark gray
  SupportEmailLink.Top := SupportLabel.Top + SupportLabel.Height + 4;
  SupportEmailLink.Left := WizardForm.FinishedLabel.Left;
  SupportEmailLink.OnClick := @EmailLinkClick;
  
  // Initially hide both controls
  SupportLabel.Visible := False;
  SupportEmailLink.Visible := False;
  
  // Free the font
  LinkFont.Free;
end;

procedure InitializeWizard;
var
  WebsiteLink, GithubLink, AuthorLabel, LinkedInLink: TNewStaticText;
  LinkFont: TFont;
begin
  // Create and configure the font for links
  LinkFont := TFont.Create;
  LinkFont.Name := 'Segoe UI';
  LinkFont.Size := 9;
  LinkFont.Style := [fsBold, fsUnderline];  // Always show underline
  
  // Create website link with improved styling
  WebsiteLink := TNewStaticText.Create(WizardForm);
  WebsiteLink.Caption := ExpandConstant('{cm:WebsiteLabel}') + #13#10 + '{#MyAppURL}';
  WebsiteLink.Cursor := crHand;
  WebsiteLink.Font.Assign(LinkFont);
  WebsiteLink.Font.Color := $404040;  // Dark gray color
  WebsiteLink.Parent := WizardForm.SelectDirPage;
  WebsiteLink.Top := WizardForm.DirEdit.Top + WizardForm.DirEdit.Height + 25;
  WebsiteLink.Left := WizardForm.DirEdit.Left;
  WebsiteLink.OnClick := @URLLabelClick;
  
  // Create Github link with improved styling
  GithubLink := TNewStaticText.Create(WizardForm);
  GithubLink.Caption := ExpandConstant('{cm:GithubLabel}') + #13#10 + 'https://github.com/Nischaya008/RayTracerNG';
  GithubLink.Cursor := crHand;
  GithubLink.Font.Assign(LinkFont);
  GithubLink.Font.Color := $404040;  // Dark gray color
  GithubLink.Parent := WizardForm.SelectDirPage;
  GithubLink.Top := WebsiteLink.Top + WebsiteLink.Height + 15;
  GithubLink.Left := WizardForm.DirEdit.Left;
  GithubLink.OnClick := @URLLabelClick;
  
  // Create author label with improved styling
  AuthorLabel := TNewStaticText.Create(WizardForm);
  AuthorLabel.Caption := ExpandConstant('{cm:AuthorLabel}');
  AuthorLabel.Parent := WizardForm.SelectDirPage;
  AuthorLabel.Font.Name := 'Segoe UI';
  AuthorLabel.Font.Size := 9;
  AuthorLabel.Font.Style := [fsBold];
  AuthorLabel.Font.Color := $404040;  // Dark gray
  AuthorLabel.Top := GithubLink.Top + GithubLink.Height + 20;
  AuthorLabel.Left := WizardForm.DirEdit.Left;
  
  // Create LinkedIn link with improved styling
  LinkedInLink := TNewStaticText.Create(WizardForm);
  LinkedInLink.Caption := ExpandConstant('{cm:LinkedInLabel}') + #13#10 + 'https://www.linkedin.com/in/nischaya008/';
  LinkedInLink.Cursor := crHand;
  LinkedInLink.Font.Assign(LinkFont);
  LinkedInLink.Font.Color := $404040;  // Dark gray color
  LinkedInLink.Parent := WizardForm.SelectDirPage;
  LinkedInLink.Top := AuthorLabel.Top + AuthorLabel.Height + 15;
  LinkedInLink.Left := WizardForm.DirEdit.Left;
  LinkedInLink.OnClick := @URLLabelClick;
  
  // Free the font object
  LinkFont.Free;

  // Add the network speed disclaimer
  CreateNetworkSpeedLabel;
  
  // Add the support information
  CreateSupportInfo;

  // Add project description to welcome page
  ProjectDescLabel := TNewStaticText.Create(WizardForm);
  ProjectDescLabel.Parent := WizardForm.WelcomePage;
  ProjectDescLabel.Caption := 'RayTracerNG is a high-performance, real-time 2D ray tracing application built with C++17 and modern OpenGL. It simulates realistic light behavior and reflections in an interactive scene, allowing users to manipulate light sources and obstacles dynamically.' + #13#10 + #13#10 +
                            'Featuring an ImGui-powered UI, real-time performance monitoring, and a lightweight, cross-platform build system, RayTracerNG is perfect for learning, showcasing graphics skills, or exploring ray physics in 2D environments.';
  ProjectDescLabel.Font.Name := 'Segoe UI';
  ProjectDescLabel.Font.Size := 9;
  ProjectDescLabel.Font.Color := $202020;  // Darker gray
  ProjectDescLabel.WordWrap := True;
  ProjectDescLabel.Width := ScaleX(245);  // Adjusted width to fit the right panel
  ProjectDescLabel.Left := WizardForm.WelcomePage.Width - ProjectDescLabel.Width - ScaleX(35);  // Position from right side
  ProjectDescLabel.Top := WizardForm.WelcomeLabel1.Top + WizardForm.WelcomeLabel1.Height + ScaleY(20);  // Adjusted spacing
  ProjectDescLabel.AutoSize := False;
  ProjectDescLabel.Height := ScaleY(200);  // Increased height significantly
end;

// Add this new procedure to manage progress bar updates
procedure UpdateProgressBar(CurStep: Integer; StepCount: Integer);
begin
  WizardForm.ProgressGauge.Position := (CurStep * 100) div StepCount;
  WizardForm.ProgressGauge.Refresh;
  // Add a small delay to make the progress bar movement more visible
  Sleep(100);
end;

// Modify CurStepChanged to include progress updates
procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  ErrorMsg: AnsiString;
  BuildLog: AnsiString;
  BuildLogFile: string;
begin
  if CurStep = ssInstall then
  begin
    // Update progress messages for better user feedback
    WizardForm.StatusLabel.Caption := 'Preparing installation...';
    UpdateProgressBar(1, 10);
    
    // Show progress for different stages
    WizardForm.StatusLabel.Caption := 'Downloading dependencies...';
    UpdateProgressBar(3, 10);
    
    WizardForm.StatusLabel.Caption := 'Installing build tools...';
    UpdateProgressBar(5, 10);
    
    WizardForm.StatusLabel.Caption := 'Setting up development environment...';
    UpdateProgressBar(7, 10);
  end;
  
  if CurStep = ssPostInstall then
  begin
    WizardForm.StatusLabel.Caption := 'Finalizing installation...';
    UpdateProgressBar(9, 10);
    
    ErrorMsg := '';
    
    // Verify all required files are present
    if not FileExists(ExpandConstant('{app}\libs\cmake\bin\cmake.exe')) then
      ErrorMsg := ErrorMsg + 'CMake installation failed.' + #13#10;
    
    if not FileExists(ExpandConstant('{app}\libs\mingw64\bin\gcc.exe')) then
      ErrorMsg := ErrorMsg + 'MinGW installation failed.' + #13#10;
    
    if not FileExists(ExpandConstant('{app}\libs\python\python.exe')) then
      ErrorMsg := ErrorMsg + 'Python installation failed.' + #13#10;
    
    // Check for GLAD files
    if not FileExists(ExpandConstant('{app}\libs\glad-gen\include\glad\glad.h')) then
      ErrorMsg := ErrorMsg + 'GLAD installation failed.' + #13#10;
    
    if ErrorMsg <> '' then
    begin
      MsgBox('The following components failed to install:' + #13#10 + #13#10 + ErrorMsg + #13#10 +
             'Please check the installation logs for details.', mbError, MB_OK);
      Exit;
    end;

    // Check if build failed and show build log if it exists
    BuildLogFile := ExpandConstant('{app}\build_log.txt');
    if FileExists(BuildLogFile) then
    begin
      if LoadStringFromFile(BuildLogFile, BuildLog) then
      begin
        if Pos('CMake Error', BuildLog) > 0 then
        begin
          MsgBox('Build failed during CMake configuration. Build log:' + #13#10 + #13#10 + BuildLog, mbError, MB_OK);
          Exit;
        end;
        if Pos('error:', BuildLog) > 0 then
        begin
          MsgBox('Build failed during compilation. Build log:' + #13#10 + #13#10 + BuildLog, mbError, MB_OK);
          Exit;
        end;
      end;
    end;

    // At the end of successful installation
    WizardForm.StatusLabel.Caption := 'Installation completed!';
    UpdateProgressBar(10, 10);
  end;
end;

// Add new procedure for uninstaller completion
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  ResultCode: Integer;
begin
  if CurUninstallStep = usPostUninstall then
  begin
    // Redirect to GitHub repository after uninstallation
    ShellExec('open', 'https://github.com/Nischaya008/RayTracerNG', '', '', SW_SHOWNORMAL, ewNoWait, ResultCode);
  end;
end; 