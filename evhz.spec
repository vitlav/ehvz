Name: evhz
Version: 2006
Release: alt1

Summary: Measure mouse polling rate

Group: System/Configuration/Hardware
License: Public domain
Url: https://wiki.archlinux.org/index.php/Mouse_Polling_Rate

Packager: Vitaly Lipatov <lav@altlinux.ru>

# http://web.archive.org/web/20060623094750/http://homepages.nildram.co.uk/~kial/evhz.c
# Source-git: https://gist.github.com/993351
Source: %name-%version.tar

%description
A tool named evhz that can display the current mouse refresh rate -- useful
when checking that your customised polling settings have been applied.

You will have root permissions to /dev/input/event*

Full credit goes to the original author, Alan Kivlin.

%prep
%setup

%build
gcc -o %name %name.c

%install
install %name -D %buildroot%_sbindir/%name

%files
%_sbindir/%name

%changelog
* Fri Apr 06 2012 Vitaly Lipatov <lav@altlinux.ru> 2006-alt1
- initial build for ALT Linux Sisyphus
