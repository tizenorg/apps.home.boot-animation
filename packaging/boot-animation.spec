#sbs-git:slp/pkgs/b/boot-animation boot-animation 0.2 d1bbca3948e4cdb6b2f9e75f176500f452fe6a33
Name: boot-animation
Version: 0.3.7
Release: 7
Summary: Boot animation
URL: http://slp-source.sec.samsung.net
Source: %{name}-%{version}.tar.gz
Source1: boot-animation.service
Source2: shutdown-animation.service
Source3: silent-animation.service
Source4: late-tizen-system.service
License: Apache
Group: Samsung/Application
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires: cmake
BuildRequires: edje, edje-bin, embryo, embryo-bin
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(x11)
BuildRequires: pkgconfig(libexif)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(utilX)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(mm-bootsound)

Requires(post): /usr/bin/vconftool

%description
Shows an animation and plays a sound when the device is booted or shutdown.

%prep
%setup -q
#%patch0 -p1


%build
%ifarch %{arm}
%define ARCH arm
%else
%define ARCH emulator
%endif


%if "%{?tizen_profile_name}" == "wearable"
	%define micro_ani ON
%else
	%define micro_ani OFF
%endif

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DUSE_MICRO_ANI=%{micro_ani} -DARCH=%{ARCH} -DTIZEN_PROFILE_NAME=%{tizen_profile_name}

make %{?jobs:-j%jobs}

%install  
rm -rf %{buildroot}  
mkdir -p %{buildroot}/usr/share/license
cp -f LICENSE %{buildroot}/usr/share/license/%{name}
%make_install

mkdir -p %{buildroot}%{_libdir}/systemd/system/tizen-system.target.wants
install -m 0644 %SOURCE1 %{buildroot}%{_libdir}/systemd/system/boot-animation.service
ln -s ../boot-animation.service %{buildroot}%{_libdir}/systemd/system/tizen-system.target.wants/
#mkdir -p %{buildroot}%{_libdir}/systemd/system/shutdown.target.wants
install -m 0644 %SOURCE2 %{buildroot}%{_libdir}/systemd/system/shutdown-animation.service
#ln -s ../shutdown-animation.service %{buildroot}%{_libdir}/systemd/system/shutdown.target.wants/
install -m 0644 %SOURCE3 %{buildroot}%{_libdir}/systemd/system/silent-animation.service
ln -s ../silent-animation.service %{buildroot}%{_libdir}/systemd/system/tizen-system.target.wants/
install -m 0644 %SOURCE4 %{buildroot}%{_libdir}/systemd/system/late-tizen-system.service
ln -s ../late-tizen-system.service %{buildroot}%{_libdir}/systemd/system/tizen-system.target.wants/

%clean
make clean

%docs_package

%post
vconftool  -i set -t int memory/boot_animation/finished 0  -u 5000 -f -s system::vconf_inhouse


%files
%manifest boot-animation.manifest
/usr/share/edje/poweroff.edj
/usr/share/edje/poweron.edj
%if %{?ARCH} == arm
/usr/share/keysound/poweron.ogg
%else
/usr/share/keysound/poweron.wav
%endif
%if %{?ARCH} == emulator
/usr/share/edje/emul/1X1_poweron.edj
/usr/share/edje/emul/1X1_poweroff.edj
/usr/share/edje/emul/3X4_poweron.edj
/usr/share/edje/emul/3X4_poweroff.edj
%endif
/usr/share/license/%{name}
%{_bindir}/boot-animation
%{_libdir}/systemd/system/boot-animation.service
%{_libdir}/systemd/system/tizen-system.target.wants/boot-animation.service
%{_libdir}/systemd/system/shutdown-animation.service
%{_libdir}/systemd/system/silent-animation.service
%{_libdir}/systemd/system/tizen-system.target.wants/silent-animation.service
#%{_libdir}/systemd/system/shutdown.target.wants/shutdown-animation.service
%{_libdir}/systemd/system/late-tizen-system.service
%{_libdir}/systemd/system/tizen-system.target.wants/late-tizen-system.service
